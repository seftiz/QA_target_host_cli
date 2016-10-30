/**
 * \file main.cpp
 * \brief ITS stack configuration
 */

#include "MarbenV2XInit.hpp"
#include "ldmUserInterface.hpp"
#include "denmUserInterface.hpp"
#include "ssuUpdate.hpp"


#include "MibV2X.hpp"
#include "MibITSFac.hpp"
#include "MibITSGn.hpp"
#include "MibKey.hpp"
#include "Mib.hpp"

#include "LogTools.hpp"

#include <atlk/sdk.h>
#include <atlk/mibs/wlan-mib.h>
// #include <atlk/mibs/core.h>
#include <atlk/mibs/if-mib.h>
#include <atlk/eui48.h>

#include <nx_api.h>
#include <net.h>

using namespace MarbenV2X::Management;
using namespace MarbenV2X::Management::Mib;
using namespace MarbenV2X::Common;

NX_IP *trusted_ip_instance;
mib_service_t *mib_service = NULL;


#define RF_FREQ_1		5890
#define RF_FREQ_2		5920
#define RF_POWER		-15


int craton_rf_init( void )
{
	atlk_rc_t 	rc        = ATLK_OK;
	int 				i					= 0;
	int					rf_freq[2] = { RF_FREQ_1 , RF_FREQ_2 };  

  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
    return (-1);
  }

	
	for ( i = 1; i <= 2; i++) {
	
		printf("\nV2X-CLI setting RF %d to Frequency %d with power %d\n", i, rf_freq[i-1], RF_POWER);
		rc = mib_set_wlanDefaultTxPower(mib_service, i, RF_POWER ); 
		
		if (atlk_error(rc)) {
			printf( "ERROR :  mib_set_wlanDefaultTxPower  failed for if %d and value %d: %s\n", i, RF_POWER, atlk_rc_to_str(rc));
      
		}
		
		rc = mib_set_wlanFrequency (mib_service, i, RF_FREQ_1); 
		if (atlk_error(rc)) {
			printf( "ERROR :  wlanFrequency  failed for if %d and value %d: %s\n", i, rf_freq[i-1], atlk_rc_to_str(rc));
      return (-1);
		}
		
	}

  return ATLK_OK;

}

void ssuModify()
{
  // ssuUpdate( getFeederUnit() );
	ssu_update_can( getFeederInterface() );
	
}

/**
 * classical main entry point
 */
extern "C" void craton_user_init( void )
{
	atlk_rc_t 	rc        = ATLK_OK;
	eui48_t 		mac_addr  = EUI48_ZERO_INIT;

	
	rc = net_ip_trusted_instance_get(&trusted_ip_instance);
  if ( atlk_error(rc) ) {
		printf("Error while getting ip instance: %s", atlk_rc_to_str(rc));
	}

	
	/* Call rf configuration */
	rc = craton_rf_init();
  if ( rc != ATLK_OK ) {
    printf( "ERROR : unable to set RF freqs, exiting all process" );
    return;
  }
	
  /* General Marben stack initialization */
  init ();

  /* 
   * Set traces on Units and SAPs.
   * The first parameter indicates the name of the Unit or SAP.
   * The second parameter indicates the level of traces.
   * The third parameter (for Units) is meaningful only for Units that encodes/decodes messages, i.e. CAM, DENM, MAP or SPAT.
   *  If non-null, it indicates the size of the traced messages (i.e. if message is longer, it will be truncated in the traces)
   *  If null, it indicates that the encoded/decoded messages are not traced.
   */

	/*
	setUnitLogConfiguration( "G5Nav", Sleipnir::Everything, 0 );
	setUnitLogConfiguration( "G5Can", Sleipnir::Everything, 0 );
	setSAPLogConfiguration( "Feeder", Sleipnir::Everything );
	*/
	
	
  //setUnitLogConfiguration( "Top", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "RoadSafety", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "SessionServer", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "Mng", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "Feeder", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "SSU", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "LDM", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "CAM", Sleipnir::Everything, 10000 );
  //setUnitLogConfiguration( "DENM", Sleipnir::Everything, 10000 );
  //setUnitLogConfiguration( "MAP", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "SPAT", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "BTP", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "Geonet", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "GeoFwd", Sleipnir::Everything_but_debug, 0 );
  //setUnitLogConfiguration( "GeoBuf", Sleipnir::Everything_but_debug, 0 );
  //setUnitLogConfiguration( "G5Acc", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "G5Nav", Sleipnir::Everything, 0 );
  //setUnitLogConfiguration( "G5Can", Sleipnir::Everything, 0 );
  
  //setSAPLogConfiguration( "RoadSafety", Sleipnir::Everything );
  //setSAPLogConfiguration( "RS2CAM", Sleipnir::Everything );
  //setSAPLogConfiguration( "RS2DENM", Sleipnir::Everything );
  //setSAPLogConfiguration( "RS2MAP", Sleipnir::Error_and_more_important );
  //setSAPLogConfiguration( "RS2SPAT", Sleipnir::Everything );
  //setSAPLogConfiguration( "LDM2RS", Sleipnir::Everything );
  //setSAPLogConfiguration( "RS2MNG", Sleipnir::Everything );
  //setSAPLogConfiguration( "IPC2MNG", Sleipnir::Everything );
  //setSAPLogConfiguration( "IPC2DENM", Sleipnir::Everything );
  //setSAPLogConfiguration( "IPC2LDM", Sleipnir::Everything );
  //setSAPLogConfiguration( "Feeder", Sleipnir::Everything );
  //setSAPLogConfiguration( "CAM2LDM", Sleipnir::Everything );
  //setSAPLogConfiguration( "DENM2LDM", Sleipnir::Everything );
  //setSAPLogConfiguration( "MAP2LDM", Sleipnir::Everything );
  //setSAPLogConfiguration( "SPAT2LDM", Sleipnir::Everything );
  //setSAPLogConfiguration( "BTP2CAM", Sleipnir::Everything );
  //setSAPLogConfiguration( "BTP2DENM", Sleipnir::Everything );
  //setSAPLogConfiguration( "BTP2MAP", Sleipnir::Everything );
  //setSAPLogConfiguration( "BTP2SPAT", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoMain", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoSHB", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoGF", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoFG", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoFB", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoBF", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoFF", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoBG", Sleipnir::Everything );
  //setSAPLogConfiguration( "GeoGB", Sleipnir::Everything );
  //setSAPLogConfiguration( "G5Acc", Sleipnir::Everything );

	rc = mib_get_ifPhysAddress(mib_service, 1, &mac_addr);
	if (atlk_error(rc)) {
		printf( "ERROR :  mib_get_ifPhysAddress failed for if %d.error  %s\n", 1, atlk_rc_to_str(rc));
	}
  /* Set a couple of Marben V2X MIB parameters. */
	
  /* Set the Geonet Address */
  const char gnAddr[8] = { 0x01, 0x01, mac_addr.octets[5], mac_addr.octets[4], mac_addr.octets[3], mac_addr.octets[2], mac_addr.octets[1], mac_addr.octets[0] };
  mib->set( MarbenV2X::Management::Mib::Key::itsGnLocalGnAddress, std::make_shared<MarbenV2X::Mib::ItsGn::LocalGnAddress>( 8, gnAddr ) );

  /* Set CAM Transmit to true */
  mib->set( MarbenV2X::Management::Mib::Key::itsFacCamTransmit, std::make_shared<MarbenV2X::Mib::ItsFacilities::CamTransmit>( true ) );

  /* Set a station ID */
  mib->set( MarbenV2X::Management::Mib::Key::itsFacStationID, std::make_shared<MarbenV2X::Mib::ItsFacilities::StationID>(  mac_addr.octets[5] ) );

  /* MarbenV2X is initialized and configured. Start it. */
  start ();

  /* User part */
  /* Examples of applications */
  new std::thread( ssuModify );
  new std::thread( ldmUserInterface );
  // new std::thread( denmUserInterface );
}
