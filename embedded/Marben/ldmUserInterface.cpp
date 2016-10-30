#include "V2XSession.hpp"
#include "V2XIpcTcpNx.hpp"
#include "LDMSubscriptionAndNotificationServiceCodec.hpp"
#include "EtsiNearbyVehicleDescriptor.hpp"
#include "asn1.hpp"
#include "CommonTypes.hpp"
#include <string>
#include <iostream>
#include <unistd.h>

const char itsaid[] = {char( 0xE1 ), 0x02, 0x03, 0x04};
MarbenV2X::Facilities::ItsAid itsaid_as_int = 0xE1020304;

void ldmUserInterface()
{
  std::string error;
  MarbenV2X::Application::V2XIpcTcpNx ipc; /* IPC : Autotalks' TCP */
  MarbenV2X::Application::V2XSession session( ipc ); /* Session object hiding the IPC mechanism */
  MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec ldmcodec; /* ServiceCodec object that serializes/deserializes the exchanges with the LDM Unit */
  std::string ldm( "ldm" );
  session.addSAP( ldm, &ldmcodec ); /* Associate "ldm" string to the LDM ServiceCodec */
  int fail = session.openItsaid( std::string( itsaid, sizeof( itsaid ) ) ); /* Open the session with the ITS Stack */

  if( !fail )
  {
    MarbenV2X::Facilities::SubsId subscriptionId;
    //
    Sleipnir::Interaction * interactionptr;
    //
    {
      MarbenV2X::Facilities::DataUnitType dataUnitType;
      MarbenV2X::Facilities::Filter filter;
      MarbenV2X::Facilities::ElementaryFilteringConditionList elementaryConditionList;
      MarbenV2X::Facilities::OptionalCompareFieldType noise;
      //
      dataUnitType = MarbenV2X::Facilities::DataUnitType::EtsiNearbyVehicleDescriptorUnit;
      //
      // any value would go
      MarbenV2X::Facilities::Etsi::VehicleType dummy(casn_Etsipedestrian, casn_EtsiPerformanceClass__unavailable); 
      // 
      // as we are to ignore them anyway
      dummy.setCompareFieldCheckStatus(MarbenV2X::Facilities::Etsi::StationTypeForVTToCheck,false);
      dummy.setCompareFieldCheckStatus(MarbenV2X::Facilities::Etsi::PerfoClassForVTToCheck,false);
      //
      MarbenV2X::Facilities::ElementaryFilteringCondition catchAllVehicleType( dummy, MarbenV2X::Facilities::IsEqual, noise);
      elementaryConditionList.push_back(catchAllVehicleType);
      //
      MarbenV2X::Facilities::SubFilter subfilter(elementaryConditionList);
      filter.addSubFilter(subfilter);

      /* Build the interaction (for LDM Subscription) to be sent to the LDM Unit */
      interactionptr = ldmcodec.encodeSubscribeReq( itsaid_as_int, dataUnitType, filter, false, false );
    }

    /* Send the interaction to the ITS-Stack */
    std::cerr << "write #1" << std::endl;
    session.write( ldm, interactionptr );
    interactionptr = nullptr;
    //
    size_t number = 0;
    std::cerr << "waiting at most 10 seconds for reply" << std::endl;

    while( error.empty() && ( number < 100 ) )
    {
      size_t counter = 0;

      while( ( !session.poll() ) && counter < 10 )
      {
        std::this_thread::sleep_for( std::chrono::nanoseconds(1000000000) );
        ++counter;
      }

      if( counter < 10 )
      {
        /* Something has been received from the ITS-Stack. Get the interaction */
        ++number;
        std::string from;
        Sleipnir::Interaction * receivedInteraction;
        std::cerr << "read #" << number << std::endl;
        receivedInteraction = session.read( from );

        if( receivedInteraction )
        {
          if( !ldm.compare( from ) )
          {
            /* The interaction comes from the LDM Unit */
            switch( ldmcodec.whatInteraction( *receivedInteraction ) )
            {
            case MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::InteractionKind::SubscriptionResp :
            {
              MarbenV2X::Facilities::SubscribeResult result;
              MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::ReqAndRespRet status;
              status = ldmcodec.decodeSubscribeResp( receivedInteraction, result );

              if( status == MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::Success )
              {
                if( result.result == MarbenV2X::Facilities::SimpleResult::Accepted )
                {
                  subscriptionId = result.subscriptionId;
                  std::cerr << "Subscription was accepted: id=" << subscriptionId << std::endl;
                }
                else
                {
                  error = "Subscription was not accepted.";
                }
              }
              else
              {
                error = "failure to decode Subscription Response.";
              }
            }
            break;

            case MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::InteractionKind::UnsubscriptionResp :
            {
              MarbenV2X::Facilities::SimpleResult result;
              MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::ReqAndRespRet status;
              status = ldmcodec.decodeUnsubscribeResp( receivedInteraction, result );

              if( status == MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::Success )
              {
                if( result != MarbenV2X::Facilities::SimpleResult::Accepted )
                {
                  error = "Unsubscription was not accepted.";
                }
              }
              else
              {
                error = "failure to decode Unsubscription Response.";
              }
            }
            break;

            case MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::InteractionKind::AddNotificationReq :
            case MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::InteractionKind::DeleteNotificationReq :
            case MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::InteractionKind::UpdateNotificationReq :
            {
              MarbenV2X::Facilities::DataUnitPtrVector * duv = nullptr;
              MarbenV2X::Facilities::SubsId sid;
              MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::ReqAndRespRet status;
              status = ldmcodec.decodeNotifyReq( receivedInteraction, duv, sid );

              if( status == MarbenV2X::Facilities::LDMSubscriptionAndNotificationServiceCodec::Success )
              {
                if( subscriptionId != sid )
                {
                  error = "Notify was performed on different id.";
                }
              }
              else
              {
                error = "failure to decode Notify Request.";
              }

              MarbenV2X::Facilities::SimpleResult result = MarbenV2X::Facilities::SimpleResult::Accepted ;
              interactionptr = ldmcodec.encodeNotifyResp( result );

              if( interactionptr )
              {
                if( session.write( ldm, interactionptr ) )
                {
                  error += "failure to write the Notify Response.";
                }

                std::cerr << "write Notify Response" << std::endl;
                interactionptr = nullptr;
              }
              else
              {
                error += "failure to encode Notify Response.";
              }

              // after 98 notify, unsubscribe
              if ( number == 99 )
              {
                interactionptr = ldmcodec.encodeUnsubscribeReq( subscriptionId );

                if( interactionptr )
                {
                  if( session.write( ldm, interactionptr ) )
                  {
                    error += "failure to write the unsubscription.";
                  }

                  interactionptr = nullptr;
                }
                else
                {
                  error += "failure to encode Unsubscribe Request.";
                }
	      }

              if (duv)
              {
                std::cerr << "Number of notified data unit: " << duv->size() << std::endl;
                // look at data unit in duv
                for( auto ptr : *duv )
                {
                  switch( ptr->getTypeId() )
                  {
                  case MarbenV2X::Facilities::DataUnitType::EtsiNearbyVehicleDescriptorUnit:
                  {
                    MarbenV2X::Facilities::Etsi::NearbyVehicleDescriptor * nvd = 
                        dynamic_cast<MarbenV2X::Facilities::Etsi::NearbyVehicleDescriptor *>( ptr );
                    std::cout << "One EtsiNearbyVehicleDescriptorUnit" << std::endl;
                    std::cout << "Latitude = " << nvd->getLocationRef().getReferencePosition().latitude << std::endl;
                    std::cout << "Longitude = " << nvd->getLocationRef().getReferencePosition().longitude << std::endl;
                    std::cout << "Altitude = " << nvd->getLocationRef().getReferencePosition().altitude.altitudeValue << std::endl;
                    std::cout << "ExteriorLights = " << static_cast<unsigned>(nvd->getExternalLightsOnRef().getExteriorLights().value[0]) << std::endl;
                    std::cout << "LightBar = " << static_cast<unsigned>(nvd->getActingAsSpecialTransportVehicleRef().getSpecialTransportContainer().lightBarSirenInUse.value[0]) << std::endl;
                  }
                  break;

                  default:
                    std::cerr << "Unexpected data unit type: " << ptr->getTypeId() << std::endl;
                    break;
                  }
                }

                // release all data unit in duv
                for( auto ptr : *duv )
                {
                  delete ptr;
                }
              }
            }
            break;
            break;

            default:
              error = "unexpected code of Interaction on read.";
              break;
            }
          }
          else
          {
            error = "reading from unexpected service on read.";
          }

          delete receivedInteraction;
        }
        else
        {
          error = "read returned nullptr, session has been closed by server.";
        }
      }
      else
      {
        error = "timeout on read.";
      }
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
