

#include "../general/general.h"
#include "../v2x_cli/v2x_cli.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "mng_api.h"

/* MIB service */
mib_service_t 	*mib_service;


int cli_v2x_managment_init( void )
{
	atlk_rc_t 	rc        = ATLK_OK;

/* Get default MIB service */
  rc = mib_default_service_get(&mib_service);
  if (atlk_error(rc)) {
    return (-1);
  }

#ifdef __linux__
	struct cli_def *cli;
	/* Create the remote MIB service */
	rc = mib_remote_service_create(get_active_cli_transport(), NULL, &mib_service);
	if (atlk_error(rc)) {
		cli_print( cli, "Remote MIB service create: %s\n", atlk_rc_to_str(rc));
		/* Clean-up resources */
		//clean_up();
		return atlk_error(rc);
	}
#endif

	return 0;
}

int cli_v2x_managment_service_create( struct cli_def *cli, const char *command, char *argv[], int argc  ) 
{
  atlk_rc_t             rc    = ATLK_OK;
  char                  str_data[256] = "";

  (void) command;
  
  IS_HELP_ARG("mng create -type hw|remote")
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_STRING("-type", str_data, 0, "Specify service type, local or remote");
	
  if ( strcmp( (char*) str_data,  "hw") == 0 ) {

    if ( mib_service != NULL ) {
      cli_print ( cli, "NOTE : Local service is active" );
    }
    else {
		  rc = mib_default_service_get(&mib_service);
		  if (atlk_error(rc)) {
	      cli_print ( cli, "ERROR : mib_default_service_get: %s\n", atlk_rc_to_str(rc) );
    	  goto error;
  	  }	
    }
  } 
  else if ( strcmp( (char*) str_data, "remote") == 0 ) {
#ifdef __linux__
		
		/* Create the remote V2X service */
		rc = mib_remote_service_create( get_active_cli_transport(), NULL, &mib_service);
		if (atlk_error(rc)) {
			cli_print( cli, "Remote Managment service create: %s\n", atlk_rc_to_str(rc));
			/* Clean-up resources */
			goto error;
		}
#else 
		cli_print( cli, "Remote Managment service is not avaliable" );
#endif		
  } 
  else {
    cli_print( cli, "ERROR : unknown mode of link api");
  }
	syslog( LOG_DEBUG, "mib_service pointer is %p", (void*) mib_service );
	
error:
  return atlk_error(rc);
}
int cli_v2x_managment_service_delete( struct cli_def *cli, const char *command, char *argv[], int argc  )
{
	  atlk_rc_t             rc    = ATLK_OK;
  (void) command;
  
	IS_HELP_ARG("mng delete")
  CHECK_NUM_ARGS /* make sure all parameter are there */

  if ( mib_service == NULL ) {
    cli_print ( cli, "ERROR : Service not created, please create first\n" );
    return CLI_ERROR;
  }
	
	syslog( LOG_DEBUG, "mib_service_delete pointer is %p", (void*) mib_service );

	rc = mib_service_delete(mib_service);
  if ( atlk_error(rc) ) {
    cli_print ( cli, "ERROR : mib_service_delete: %d, %s\n", rc, atlk_rc_to_str(rc) );
    goto error;
  }	

error:
  mib_service= NULL;
  return atlk_error(rc);
  
}

int cli_v2x_mibApi_testGet( struct cli_def *cli, UNUSED(const char *command), char *argv[], UNUSED(int argc) )
{
	atlk_rc_t rc       = ATLK_OK;
	
	char func_name[128];
	
	char type[128];/*string*/
	int if_idx = ERR_VALUE;	
	
	char save_val[]="-value1";

	char save_str[]="-type1";
	int index = 2;
	int int_value = 0;
	int32_t int32_value = 0;
	uint32_t uint32_value = 0;
	uint8_t uint8_value = 0;
	size_t sizet_value = 0;
	int enum_value = 0;
	
	char char_value[128];
	int special_values[60]= {
 	6,108,-30,33,1,255,-240,264,0,255,
	0,65535,0,255,0,0,255,
	0,4,1,1000,-96,-35,0,100,-200,200,
	0,255,740,5920,0,30,5180,5930,5180,5930,
	-60,60,-100,100,0,4,-1280,1270,-1280,1270,
	0,256,-60,60,-100,100,0,65535,-240,320 } ;

	IS_HELP_ARG("mibApi testGet -type1 fIndex -value1 1|2");

       	
	GET_STRING("-type1",type,0,"First parameter type");

	int i=2;
	do{
				
		if (strcmp(type,"fIndex")==0 && if_idx == ERR_VALUE)
		{
			GET_INT(save_val,if_idx,i,"Specify interface index");
		}
		else if (strcmp(type,"int")==0 && int_value == 0)
		{
			GET_INT(save_val,int_value,i,"get value");
		}
		else if (strcmp(type,"int32")==0 && int32_value == 0)
		{												
			GET_INT(save_val,int32_value,i,"get value");
		}
		else if (strcmp(type,"uint32")==0 && uint32_value == 0)
		{
			GET_UINT32(save_val,uint32_value,i,"get value");
		}
		else if (strcmp(type,"uint8")==0 && uint8_value == 0)
		{	
			GET_UINT8(save_val,uint8_value,i,"get value");
		}
		else if (strcmp(type,"char")==0 && char_value[0] == 0)			
		{	
			GET_STRING(save_val,char_value,i,"get value");
		}
		else if (strcmp(type,"eui48")==0 && uint8_value == 0)
		{
			GET_UINT8(save_val,uint8_value,i,"get value");
		}
		else if (strcmp(type,"size_t")==0 && sizet_value == 0)
		{
			GET_INT(save_val,sizet_value,i,"get value");
		}		
		else if ((strcmp(type,"enum")==0 || strcmp(type,"mib")>0)  && enum_value == 0)		
		{	
			GET_INT(save_val,enum_value,i,"get value");
		}
		strcpy(type,"");
				
		save_str[5]= index+ '0';
	
		save_val[6]= index + '0';
		i+=2;
               	GET_STRING(save_str,type,i,"another parameter type");
		
		i+=2;
		index++;
	}while (strcmp(type, ""));

	if (index==6)
	{
			strcpy(func_name,"mib_get_wlanConfigSaveStatus");
			rc = mib_get_wlanConfigSaveStatus(mib_service,&enum_value);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_get_wlanRxDuplicateFrameFilteringEnabled");
			rc = mib_get_wlanRxDuplicateFrameFilteringEnabled(mib_service,&int_value);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_get_wlanEpdEnabled");

			rc = mib_get_wlanEpdEnabled(mib_service,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxCsd");
			
			rc = mib_get_wlanTxCsd(mib_service,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxDiversityEnabled");

			rc = mib_get_wlanTxDiversityEnabled(mib_service,&int_value);
			if (atlk_error(rc))
				goto finally;	
			strcpy(func_name,"mib_get_wlanRxDiversityEnabled");
			
			rc = mib_get_wlanRxDiversityEnabled(mib_service,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxDiversityCnt");

			rc = mib_get_wlanRxDiversityCnt(mib_service,&uint32_value);
			if (atlk_error(rc))
				goto finally;
	}
	else if (index==11)
	{
			strcpy(func_name,"mib_get_wlanDefaultTxDataRate");
			rc = mib_get_wlanDefaultTxDataRate(mib_service,if_idx,&special_values[0]);
			if (atlk_error(rc))
				goto finally;
			eui48_t eui48_t_value = EUI48_ZERO_INIT;
			strcpy(func_name,"mib_get_wlanBssid");
			rc = mib_get_wlanBssid(mib_service,if_idx,&eui48_t_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanDefaultTxPower");
			rc = mib_get_wlanDefaultTxPower(mib_service,if_idx,&special_values[2]);
			if (atlk_error(rc))
				goto finally;
				
			strcpy(func_name,"mib_get_wlanRandomBackoffEnabled");

			rc = mib_get_wlanRandomBackoffEnabled(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanMacAddress");

			rc = mib_get_wlanMacAddress(mib_service,if_idx,&uint8_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxSaOverrideEnabled");

			rc = mib_get_wlanTxSaOverrideEnabled(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxUcastDaFilterEnabled");

			rc = mib_get_wlanRxUcastDaFilterEnabled(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanShortRetryLimit");

			rc = mib_get_wlanShortRetryLimit(mib_service,if_idx,&special_values[4]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanDefaultTxPowerDbm8");
			
			rc = mib_get_wlanDefaultTxPowerDbm8(mib_service,if_idx,&special_values[6]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanQosDataEnabled");

			rc = mib_get_wlanQosDataEnabled(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanFrameTxCnt");

			rc = mib_get_wlanFrameTxCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanFrameRxCnt");

			rc = mib_get_wlanFrameRxCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxFailCnt");

			rc = mib_get_wlanTxFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxAllocFailCnt");

			rc = mib_get_wlanTxAllocFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxQueueFailCnt");

			rc = mib_get_wlanTxQueueFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxFailCnt");

			rc = mib_get_wlanRxFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxAllocFailCnt");

			rc = mib_get_wlanRxAllocFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxQueueFailCnt");

			rc = mib_get_wlanRxQueueFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxCrcFailCnt");

			rc = mib_get_wlanRxCrcFailCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanEdcaCWmin");

			rc = mib_get_wlanEdcaCWmin(mib_service,if_idx,&special_values[8]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanEdcaCWmax");

			rc = mib_get_wlanEdcaCWmax(mib_service,if_idx,&special_values[10]);
			if (atlk_error(rc))
				goto finally;
	
			strcpy(func_name,"mib_get_wlanCsIntervalA");

			rc = mib_get_wlanCsIntervalA(mib_service,if_idx,&special_values[12]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanCsIntervalB");

			rc = mib_get_wlanCsIntervalB(mib_service,if_idx,&special_values[14]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanCsSyncTolerance");

			rc = mib_get_wlanCsSyncTolerance(mib_service,if_idx,&special_values[16]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanChannelProbingInterval");

			rc = mib_get_wlanChannelProbingInterval(mib_service,if_idx,&special_values[20]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanChannelLoadThreshold");

			rc = mib_get_wlanChannelLoadThreshold(mib_service,if_idx,&special_values[22]);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanChannelBusyRatio");

			rc = mib_get_wlanChannelBusyRatio(mib_service,if_idx,&special_values[24]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanPhyHeaderErrCnt");

			rc = mib_get_wlanPhyHeaderErrCnt(mib_service,if_idx,&uint32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanDcocEnabled");

			rc = mib_get_wlanDcocEnabled(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRssiLatestFrame");

			rc = mib_get_wlanRssiLatestFrame(mib_service,if_idx,&int32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanDefaultTxPower");

			rc = mib_get_wlanRficTemperature(mib_service,if_idx,&special_values[26]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanRcpiLatestFrame");

			rc = mib_get_wlanRcpiLatestFrame(mib_service,if_idx,&special_values[28]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_get_wlanAntennaStatus");

			rc = mib_get_wlanAntennaStatus(mib_service,if_idx,&enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanFrequency");

			rc = mib_get_wlanFrequency(mib_service,if_idx,&special_values[30]);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanDcocStatus");

			rc = mib_get_wlanDcocStatus(mib_service,if_idx,&enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRfFrontEndConnected");

			rc = mib_get_wlanRfFrontEndConnected(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRfEnabled");

			rc = mib_get_wlanRfEnabled(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRfFrontEndOffset");

			rc = mib_get_wlanRfFrontEndOffset(mib_service,if_idx,&special_values[32]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanPhyOFDMChannelWidth");

			rc = mib_get_wlanPhyOFDMChannelWidth(mib_service,if_idx,&enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanPresetFrequency0");

			rc = mib_get_wlanPresetFrequency0(mib_service,if_idx,&special_values[34]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanPresetFrequency1");

			rc = mib_get_wlanPresetFrequency1(mib_service,if_idx,&special_values[36]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanRfTestMode");

			rc = mib_get_wlanRfTestMode(mib_service,if_idx,&enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTxIqImbalanceAmplitude");

			rc = mib_get_wlanTxIqImbalanceAmplitude(mib_service,if_idx,&special_values[38]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanTxIqImbalancePhase");

			rc = mib_get_wlanTxIqImbalancePhase(mib_service,if_idx,&special_values[40]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanPantLutIndex");

			rc = mib_get_wlanPantLutIndex(mib_service,if_idx,&special_values[42]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanTssiDetectorReading");

			rc = mib_get_wlanTssiDetectorReading(mib_service,if_idx,&int32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRfCalibrationRequired");

			rc = mib_get_wlanRfCalibrationRequired(mib_service,if_idx,&int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanTssiInterval");

			rc = mib_get_wlanTssiInterval(mib_service,if_idx,&int32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_get_wlanRxSampleGainLow");

			rc = mib_get_wlanRxSampleGainLow(mib_service,if_idx,&special_values[44]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanRxSampleGainMid");

			rc = mib_get_wlanRxSampleGainMid(mib_service,if_idx,&special_values[46]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanRxSampleGainHigh");

			rc = mib_get_wlanRxSampleGainHigh(mib_service,if_idx,&special_values[48]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanGrfiSignalDelayResolution");

			rc = mib_get_wlanGrfiSignalDelayResolution(mib_service,if_idx,&special_values[50]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanRxIqImbalanceAmplitude");

			rc = mib_get_wlanRxIqImbalanceAmplitude(mib_service,if_idx,&special_values[52]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanRxIqImbalancePhase");

			rc = mib_get_wlanRxIqImbalancePhase(mib_service,if_idx,&special_values[54]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanLoLeakage");

			rc = mib_get_wlanLoLeakage(mib_service,if_idx,&special_values[56]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_get_wlanPantLutDbm8");

			rc = mib_get_wlanPantLutDbm8(mib_service,if_idx,&special_values[58]);
			if (atlk_error(rc))
				goto finally;
			}
	else 
	{
		strcpy(func_name,"mib_get_wlanPantLut");

		rc = mib_get_wlanPantLut(mib_service,if_idx,char_value,&sizet_value);
		if (atlk_error(rc))
			goto finally;
	}

finally:
	if (atlk_error(rc))
		cli_print( cli, "ERROR : wlanMib Failed for func %s :%d %s\n", func_name,rc, atlk_rc_to_str(rc));
	else
		cli_print( cli, "PASS: wlanMib Succeeded for func %s :%d %s\n", func_name,rc, atlk_rc_to_str(rc));	
  	return atlk_error(rc);
}
int cli_v2x_mibApi_testSet( struct cli_def *cli, UNUSED(const char *command), char *argv[], UNUSED(int argc) )
{
	atlk_rc_t rc       = ATLK_OK;
	int index = 2;
	char func_name[128];

	char type[128];
	int if_idx = ERR_VALUE;	
	
	char save_val[]="-value1";
	
	char save_str[]="-type1";
	
	int int_value = 0;
	int32_t int32_value = 0;
	uint32_t uint32_value = 0;
	uint8_t uint8_value = 0;
	size_t sizet_value = 0;
	int enum_value = 0;
	
	int special_values[60]= {
	 6,108,-30,33,1,255,-240,264,0,255,
	0,65500,0,255,0,50,0,255,
	0,4,1,1000,-96,-35,0,100,-200,200,
	0,255,740,5920,0,30,5180,5930,5180,5930,
	-60,60,-100,100,0,4,-1280,1270,-1280,1270,
	0,256,-60,60,-100,100,0,65500,-240,320 } ;
	
	char char_value[128];
	
	IS_HELP_ARG("mibApi testGet -type1 fIndex -value1 1|2");

        GET_STRING("-type1",type,0,"First parameter type");
	
	int i=2;

	do{				
		if (strcmp(type,"fIndex")==0 && if_idx == ERR_VALUE)
		{
			GET_INT(save_val,if_idx,i,"Specify interface index");
		}
		else if (strcmp(type,"int")==0 && int_value == 0)
		{
			GET_INT(save_val,int_value,i,"get value");
		}
		else if (strcmp(type,"int32")==0 && int32_value == 0)
		{											
			GET_INT(save_val,int32_value,i,"get value");
		}
		else if (strcmp(type,"uint32")==0 && uint32_value == 0)
		{
			GET_UINT32(save_val,uint32_value,i,"get value");
		}
		else if (strcmp(type,"uint8")==0 && uint8_value == 0)
		{	
			GET_UINT8(save_val,uint8_value,i,"get value");
		}
		else if (strcmp(type,"char")==0 && char_value[0] == 0)			
		{	
			GET_STRING(save_val,char_value,i,"get value");
		}
		else if (strcmp(type,"eui48")==0 && uint8_value == 0)
		{
			GET_UINT8(save_val,uint8_value,i,"get value");
		}
		else if (strcmp(type,"size_t")==0 && sizet_value == 0)
		{
			GET_INT(save_val,sizet_value,i,"get value");
		}			
		else if ((strcmp(type,"enum")==0 || strcmp(type,"mib")>0)  && enum_value == 0)				
		{	
			GET_INT(save_val,enum_value,i,"get value");
		}
		strcpy(type,"");
		
		save_str[5]= index+ '0';
	
		save_val[6]= index + '0';
		i+=2;
               	GET_STRING(save_str,type,i,"another parameter type");
		
		i+=2;
		index++;

	     	} while (strcmp(type, ""));
	if (index==5)
	{
			strcpy(func_name,"mib_set_wlanConfigSaveStatus");
			rc = mib_set_wlanConfigSaveStatus(mib_service,enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanRxDuplicateFrameFilteringEnabled");
			rc = mib_set_wlanRxDuplicateFrameFilteringEnabled(mib_service,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanEpdEnabled");
			rc = mib_set_wlanEpdEnabled(mib_service,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanTxCsd");
			rc = mib_set_wlanTxCsd(mib_service,special_values[18]);
			if (atlk_error(rc))
				goto finally;
			rc = mib_set_wlanTxCsd(mib_service,special_values[19]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_set_wlanTxDiversityEnabled");
			rc = mib_set_wlanTxDiversityEnabled(mib_service,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanRxDiversityEnabled");					
			rc = mib_set_wlanRxDiversityEnabled(mib_service,int_value);
			if (atlk_error(rc))
				goto finally;
	}
	else if (index==11)
	{
			strcpy(func_name,"mib_set_wlanDefaultTxDataRate");
			rc = mib_set_wlanDefaultTxDataRate(mib_service,if_idx,special_values[0]);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanBssid");
			eui48_t eui48_t_value = EUI48_ZERO_INIT;
			rc = mib_set_wlanBssid(mib_service,if_idx,eui48_t_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanDefaultTxPower");
			rc = mib_set_wlanDefaultTxPower(mib_service,if_idx,special_values[3]);
			if (atlk_error(rc))
				goto finally;
						
			strcpy(func_name,"mib_set_wlanRandomBackoffEnabled");
			rc = mib_set_wlanRandomBackoffEnabled(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanMacAddress");
			
			rc = mib_set_wlanMacAddress(mib_service,if_idx,eui48_t_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanTxSaOverrideEnabled");
			rc = mib_set_wlanTxSaOverrideEnabled(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanRxUcastDaFilterEnabled");
			rc = mib_set_wlanRxUcastDaFilterEnabled(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanShortRetryLimit");
			rc = mib_set_wlanShortRetryLimit(mib_service,if_idx,special_values[4]);
			if (atlk_error(rc))
				goto finally;
			
			strcpy(func_name,"mib_set_wlanDefaultTxPowerDbm8");
			rc = mib_set_wlanDefaultTxPowerDbm8(mib_service,if_idx,special_values[6]);
			if (atlk_error(rc))
				goto finally;
			rc = mib_set_wlanDefaultTxPowerDbm8(mib_service,if_idx,special_values[7]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_set_wlanQosDataEnabled");
			rc = mib_set_wlanQosDataEnabled(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanEdcaCWmin");
			rc = mib_set_wlanEdcaCWmin(mib_service,if_idx,special_values[8]);
			if (atlk_error(rc))
				goto finally;
			rc = mib_set_wlanEdcaCWmin(mib_service,if_idx,special_values[9]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_set_wlanEdcaCWmax");
			rc = mib_set_wlanEdcaCWmax(mib_service,if_idx,special_values[10]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanCsIntervalA");
			rc = mib_set_wlanCsIntervalA(mib_service,if_idx,special_values[12]);
			if (atlk_error(rc))
				goto finally;
			
			strcpy(func_name,"mib_set_wlanCsIntervalB");
			rc = mib_set_wlanCsIntervalB(mib_service,if_idx,special_values[14]);
			if (atlk_error(rc))
				goto finally;
			rc = mib_set_wlanCsIntervalB(mib_service,if_idx,special_values[15]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_set_wlanCsSyncTolerance");
			rc = mib_set_wlanCsSyncTolerance(mib_service,if_idx,special_values[16]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanChannelProbingInterval");
			rc = mib_set_wlanChannelProbingInterval(mib_service,if_idx,special_values[20]);
			if (atlk_error(rc))
				goto finally;
			rc = mib_set_wlanChannelProbingInterval(mib_service,if_idx,special_values[21]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_set_wlanChannelLoadThreshold");
			rc = mib_set_wlanChannelLoadThreshold(mib_service,if_idx,special_values[22]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanDcocEnabled");
			rc = mib_set_wlanDcocEnabled(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanFrequency");
			rc = mib_set_wlanFrequency(mib_service,if_idx,special_values[30]);
			if (atlk_error(rc))
				goto finally;
			rc = mib_set_wlanFrequency(mib_service,if_idx,special_values[31]);
			if (atlk_error(rc))
				goto finally;

			strcpy(func_name,"mib_set_wlanRfFrontEndConnected");
			rc = mib_set_wlanRfFrontEndConnected(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanRfEnabled");
			rc = mib_set_wlanRfEnabled(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanRfFrontEndOffset");
			rc = mib_set_wlanRfFrontEndOffset(mib_service,if_idx,special_values[32]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanPhyOFDMChannelWidth");
			rc = mib_set_wlanPhyOFDMChannelWidth(mib_service,if_idx,enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanPresetFrequency0");
			rc = mib_set_wlanPresetFrequency0(mib_service,if_idx,special_values[34]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanPresetFrequency1");
			rc = mib_set_wlanPresetFrequency1(mib_service,if_idx,special_values[36]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRfTestMode");
			rc = mib_set_wlanRfTestMode(mib_service,if_idx,enum_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanTxIqImbalanceAmplitude");
			rc = mib_set_wlanTxIqImbalanceAmplitude(mib_service,if_idx,special_values[38]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRxDiversityEnabled");
			rc = mib_set_wlanTxIqImbalancePhase(mib_service,if_idx,special_values[40]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanPantLutIndex");
			rc = mib_set_wlanPantLutIndex(mib_service,if_idx,special_values[42]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRfCalibrationRequired");
			rc = mib_set_wlanRfCalibrationRequired(mib_service,if_idx,int_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanTssiInterval");
			rc = mib_set_wlanTssiInterval(mib_service,if_idx,int32_value);
			if (atlk_error(rc))
				goto finally;
			strcpy(func_name,"mib_set_wlanRxSampleGainLow");
			rc = mib_set_wlanRxSampleGainLow(mib_service,if_idx,special_values[44]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRxSampleGainMid");
			rc = mib_set_wlanRxSampleGainMid(mib_service,if_idx,special_values[46]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRxSampleGainHigh");
			rc = mib_set_wlanRxSampleGainHigh(mib_service,if_idx,special_values[48]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanGrfiSignalDelayResolution");
			rc = mib_set_wlanGrfiSignalDelayResolution(mib_service,if_idx,special_values[50]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRxIqImbalanceAmplitude");
			rc = mib_set_wlanRxIqImbalanceAmplitude(mib_service,if_idx,special_values[52]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanRxIqImbalancePhase");
			rc = mib_set_wlanRxIqImbalancePhase(mib_service,if_idx,special_values[54]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanLoLeakage");
			rc = mib_set_wlanLoLeakage(mib_service,if_idx,special_values[56]);
			if (atlk_error(rc))
				goto finally;
		
			strcpy(func_name,"mib_set_wlanPantLutDbm8");
			rc = mib_set_wlanPantLutDbm8(mib_service,if_idx,special_values[58]);
			if (atlk_error(rc))
				goto finally;
	}
	else 
	{
		strcpy(func_name,"mib_set_wlanPantLut");

		rc = mib_set_wlanPantLut(mib_service,if_idx,char_value,sizet_value);
		if (atlk_error(rc))
			goto finally;
	}

finally:
	if (atlk_error(rc))
		cli_print( cli, "ERROR : test_mib_api_set Failed for func %s :%d %s\n", func_name,rc, atlk_rc_to_str(rc));
	else
		cli_print( cli, "Pass: test_mib_api_set Succeeded for func %s :%d %s\n", func_name,rc, atlk_rc_to_str(rc));	
  	return atlk_error(rc);
}




mib_service_t 	*cli_v2x_managment_service_get( void )
{
	return mib_service;
}





int cli_v2x_set_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    = ERR_VALUE, 
            mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("set wlanDefaultTxDataRate -if_idx 1|2 -value 0-100");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }

  GET_INT("-value", mib_data, 2, "Tx datarate");
  if ( mib_data == ERR_VALUE ) {
    cli_print(cli, "ERROR : tx_rate is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc = mib_set_wlanDefaultTxDataRate(mib_service, if_idx, mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanDefaultTxDataRate( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int     if_idx    = ERR_VALUE, 
          mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("get wlanDefaultTxDataRate -if_idx 1|2");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc = mib_get_wlanDefaultTxDataRate(mib_service, if_idx, (int32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %d", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

  
int cli_v2x_set_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    = ERR_VALUE, 
            mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("set wlanDefaultTxPower -if_idx 1|2 -value -100 - 20");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }

  GET_INT("-value", mib_data, 2, "Tx power");
  if ( mib_data == ERR_VALUE ) {
    cli_print(cli, "ERROR : tx_rate/home/user/correct_version/ is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_set_wlanDefaultTxPower (mib_service, if_idx, mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR :  mib_set_wlanDefaultTxPower  failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanDefaultTxPower( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc        = ATLK_OK;
  
  int     if_idx    = ERR_VALUE, 
          mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("get wlanDefaultTxPower -if_idx 1|2");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanDefaultTxPower(mib_service, if_idx, (int32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %d", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_set_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t rc        = ATLK_OK;
  
  int       if_idx    = ERR_VALUE, 
            mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("set wlanFrequency -if_idx 1|2 -value 5800 - 6000");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }

  GET_INT("-value", mib_data, 2, "RX and TX frequency in MHz");
  if ( mib_data == ERR_VALUE ) {
    cli_print(cli, "ERROR : freq_mhz is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_set_wlanFrequency (mib_service, if_idx, mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR :  wlanFrequency  failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanFrequency( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t rc        = ATLK_OK;
  
  int     if_idx    = ERR_VALUE, 
          mib_data  = ERR_VALUE;
             
  // user_context *myctx = (user_context *) cli_get_context(cli);
  
  IS_HELP_ARG("get wlanFrequency -if_idx 1|2");
  
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanFrequency(mib_service, if_idx, (int32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_set_wlanDefaultTxDataRate failed for if %d and value %d: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %d", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanFrameRxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
 atlk_rc_t      rc        = ATLK_OK;
  
  int           if_idx    = ERR_VALUE;
  
  unsigned int  mib_data  = ERR_VALUE;
            
  
  IS_HELP_ARG("get wlanFrameRxCnt -if_idx 1|2");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanFrameRxCnt(mib_service, if_idx, (uint32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : wlanFrameRxCnt failed for if %d and value %u: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %u", mib_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_get_wlanFrameTxCnt( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t     rc        = ATLK_OK;
  
  int           if_idx    = ERR_VALUE;
  
  unsigned int  mib_data  = ERR_VALUE;
             
  
  IS_HELP_ARG("get wlanFrameTxCnt -if_idx 1|2");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanFrameTxCnt(mib_service, if_idx, (uint32_t*) &mib_data); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : wlanFrameTxCnt failed for if %d and value %u: %s\n", if_idx, mib_data, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
    cli_print( cli, "MIB_VALUE : %u", mib_data );
  }
   
exit:
  return atlk_error(rc);
}


int cli_v2x_get_wlanMacAddress( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t     rc        = ATLK_OK;
  
  int           if_idx    = ERR_VALUE;
  
	eui48_t				mac_addr = EUI48_ZERO_INIT;
             
  
  IS_HELP_ARG("get wlanMacAddress -if_idx 1|2");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
  
  rc =  mib_get_wlanMacAddress(mib_service, if_idx, &mac_addr); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR : mib_get_wlanMacAddress failed for if %d,  %s\n", if_idx, atlk_rc_to_str(rc));
    goto exit;
  }
  else {
		char      str_data[200] = "";

		sprintf(str_data,"%02x:%02x:%02x:%02x:%02x:%02x", 
					 mac_addr.octets[0], mac_addr.octets[1],
           mac_addr.octets[2], mac_addr.octets[3],
           mac_addr.octets[4], mac_addr.octets[5]);
    cli_print( cli, "MIB_VALUE : %s", str_data );
  }
   
exit:
  return atlk_error(rc);
}

int cli_v2x_set_wlanMacAddress( struct cli_def *cli, UNUSED(const char *command), char *argv[], int argc ) 
{
  atlk_rc_t rc            = ATLK_OK;
  
  int       if_idx    		= ERR_VALUE;
	char      str_data[200] = "";

	eui48_t		  mac_addr;
  char        *pos = NULL;
  unsigned int i = 0;

  IS_HELP_ARG("set wlanMacAddress -if_idx 1|2 -addr xx:xx:xx:xx:xx:xx");
  CHECK_NUM_ARGS /* make sure all parameter are there */

  GET_INT("-if_idx", if_idx, 0, "Specify interface index");
  if ( if_idx < IF_ID_MIN || if_idx > IF_ID_MAX) {
    cli_print(cli, "ERROR : if_idx is not optional and must be in range of 1-2");
    return CLI_ERROR;
  }
	
	GET_STRING_VALUE("-addr", str_data,"WLAN Mac address");

  // Convert mac address xx:xx:xx:xx:xx:xx to array of bytes
  if (strlen(str_data)) {
    pos = str_data;
    for (i = 0; i < sizeof(mac_addr); i++){
      mac_addr.octets[i] = hex_to_byte(pos);
      pos += 3;
    }
    /* For debug:
    printf("cli_v2x_set_wlanMacAddress : mac address = 0x");
    for (i = 0; i < sizeof(mac_addr); i++)
      printf("%02x", mac_addr.octets[i]);
    printf("\n");
    */

  }

  rc =  mib_set_wlanMacAddress (mib_service, if_idx, mac_addr); 
  if (atlk_error(rc)) {
    cli_print( cli, "ERROR :  wlanFrequency  failed for if %d, %s\n", if_idx, atlk_rc_to_str(rc));
    goto exit;
  }
   
exit:
  return atlk_error(rc);
}
