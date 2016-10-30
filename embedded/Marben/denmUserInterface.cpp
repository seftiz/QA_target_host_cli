#include "V2XSession.hpp"
#include "V2XIpcTcpNx.hpp"
#include "DENMServiceCodec.hpp"
#include "asn1.hpp"
#include <string>
#include <iostream>

static const uint64_t RefTimestamp = 300907872765;/* in milliseconds since 2004 (ETSI) */
static const long double lat0 = 485519.20L; /* in 100 microdegrees, + is North */
static const long double lon0 = 19248.24L;/* in 100 microdegrees, + is East */
static const unsigned int idAppl = 42;
const char itsaid[] = {char( 0x80 ), 0x02};

void denmUserInterface()
{
  std::string error;
  MarbenV2X::Application::V2XIpcTcpNx ipc; /* IPC : Autotalks' TCP */
  MarbenV2X::Application::V2XSession session( ipc ); /* Session object hiding the IPC mechanism */
  MarbenV2X::Facilities::DENMServiceCodec denmcodec; /* ServiceCodec object that serializes/deserializes the DENM trigger/update/terminate requests */
  std::string denm( "denm" );
  session.addSAP( denm, &denmcodec ); /* Associate "denm" string to the DENM ServiceCodec */
  int fail = session.openItsaid( std::string( itsaid, sizeof( itsaid ) ) ); /* Open the session with the ITS Stack */

  if( !fail )
  {
    /* Fills information related to the DENM to be sent */
    EtsiActionID actionId;
    //
    Sleipnir::Interaction * interactionptr;
    EtsiReferencePosition eventPosition;
    EtsiTimestampIts detectionTime;
    MarbenV2X::Facilities::DENMRepetition repetition;
    //
    eventPosition.latitude = lat0 * 1000; // in decimicrodegree
    eventPosition.longitude = lon0 * 1000; // in decimicrodegree
    eventPosition.altitude.altitudeValue = casn_EtsiAltitudeValue__unavailable;
    eventPosition.altitude.altitudeConfidence = casn_EtsiAltitudeConfidence__unavailable;
    eventPosition.positionConfidenceEllipse.semiMajorConfidence = 3096;// in centimeters, upto 4094
    eventPosition.positionConfidenceEllipse.semiMajorOrientation.headingValue = casn_Etsiwgs84West;// in decidegree from North(0), clockwise
    eventPosition.positionConfidenceEllipse.semiMajorOrientation.headingConfidence = casn_EtsiHeadingConfidence__unavailable;
    eventPosition.positionConfidenceEllipse.semiMinorConfidence = 2048;// in centimeters, upto 4094
    //
    detectionTime = RefTimestamp;
    //
    repetition.repetitionDuration = 360;// in seconds: six minutes (must be less than default validity duration, as it is not provided)
    repetition.transmissionInterval = 1000;// in milliseconds: every second

    /* Build the interaction to be sent to the DENM Unit */
    interactionptr = denmcodec.build_AppDENM_trigger( idAppl, eventPosition, detectionTime, nullptr, &repetition, nullptr, nullptr, nullptr, casn_Etsiover10km,
                                     casn_EtsiRelevanceTrafficDirection__unavailable, 0, nullptr, false );

    /* Send the interaction to the ITS-Stack */
    std::cerr << "write #1" << std::endl;
    session.write( denm, interactionptr );
    interactionptr = nullptr;
    //
    size_t counter = 0;
    std::cerr << "waiting at most 10 seconds for reply" << std::endl;

    while( ( !session.poll() ) && counter < 10 )
    {
      std::this_thread::sleep_for( std::chrono::nanoseconds(1000000000) );
      ++counter;
    }

    if( counter < 10 )
    {
      /* Something has been received from the ITS-Stack. Get the interaction */
      std::string from;
      Sleipnir::Interaction * receivedInteraction;
      std::cerr << "read #1" << std::endl;
      receivedInteraction = session.read( from );

      if( receivedInteraction )
      {
        if( !denm.compare( from ) )
        {
          /* The interaction comes from the DENM Unit */
          if( MarbenV2X::Facilities::TransmissionServiceCode::TriggerAckCode == receivedInteraction->code )
          {
            unsigned int idApplication;
            MarbenV2X::Facilities::FailureNotif failNotif;
            denmcodec.read_AppDENM_trigger_ack( receivedInteraction, idApplication, actionId, failNotif );

            /* Let the DENM Unit sends the DENM during 40s */
            std::cerr << "sleep 40s" << std::endl;
            std::this_thread::sleep_for( std::chrono::nanoseconds(40000000000) );
   
            /* Send a request to the DENM Unit to stop sending this DENM */
            detectionTime += 40 * 1000;
            interactionptr = denmcodec.build_AppDENM_terminate( actionId, detectionTime , &repetition, 0, false );
            std::cerr << "write #2" << std::endl;
            session.write( denm, interactionptr );
            interactionptr = nullptr;
            //
            std::cerr << "read #2" << std::endl;
            receivedInteraction = session.read( from );

            if( receivedInteraction )
            {
              if( !denm.compare( from ) )
              {
                if( MarbenV2X::Facilities::TransmissionServiceCode::TerminationAckCode == receivedInteraction->code )
                {
                  EtsiActionID actionId2;
                  MarbenV2X::Facilities::FailureNotif failNotif2;
                  denmcodec.read_AppDENM_terminate_ack( receivedInteraction, actionId2, failNotif2 );

                  if( actionId2.originatorStationID != actionId.originatorStationID )
                  {
                    error = "The termination is about a different station ID.";
                  }
                  else if( actionId2.sequenceNumber != actionId.sequenceNumber )
                  {
                    error = "The termination is about a different sequence number.";
                  }
                }
                else
                {
                  error = "unexpected code of Interaction on second read.";
                }
              }
              else
              {
                error = "reading from unexpected service on second read.";
              }
            }
            else
            {
              error = "second read returned nullptr, session has been closed by server.";
            }
          }
          else
          {
            error = "unexpected code of Interaction on first read.";
          }
        }
        else
        {
          error = "reading from unexpected service on first read.";
        }
      }
      else
      {
        error = "first read returned nullptr, session has been closed by server.";
      }
    }
    else
    {
      error = "Unanswered first write.";
    }
  }
  else
  {
    error = "Unable to open session. The whole collection of test has failed.";
  }

  if( error.empty() )
  {
    std::cerr << "Overall success" << std::endl;
  }
  else
  {
    std::cerr << "Overall failure : " << error << std::endl;
  }

  session.close();
}
