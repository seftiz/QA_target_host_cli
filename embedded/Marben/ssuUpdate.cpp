

#include <atlk/sdk.h>
#include <atlk/can.h>
#include <atlk/can_service.h>

#include "ssuUpdate.hpp"
#include <iostream>
#include "asn1.hpp"

#include <craton/can_hw.h>


int can_parser_rx(can_id_t can_id, size_t data_size, uint8_t data[CAN_DATA_SIZE_MAX], MarbenV2X::Common::Etsi::FeederInterface & FeederInterface );

void ssuUpdate( MarbenV2X::Common::Etsi::FeederInterface & FeederInterface )
{
  MarbenV2X::Common::Etsi::SSUData * ssuData = new MarbenV2X::Common::Etsi::SSUData[2]();

  /* Wait 10 s and changed the exterior lights and the light bar */
  std::this_thread::sleep_for( std::chrono::nanoseconds(10000000000) );

  /* Modify the exterior lights */
  ssuData[0].alt = MarbenV2X::Common::Etsi::SSUData::ExteriorLights;
  ssuData[0].exteriorLights = MarbenV2X::Common::Etsi::ExteriorLightsFlag::LeftTurnSignalOn; 

  /* Modify the light bar */
  ssuData[1].alt = MarbenV2X::Common::Etsi::SSUData::LightBarSiren;
  ssuData[1].lightBarSirenInUse = MarbenV2X::Common::Etsi::LightBarSirenInUseFlag::SirenActivated;

  /* Set the modification in the SSU */
  FeederInterface.feed( 2, ssuData );
}

void ssu_update_can( MarbenV2X::Common::Etsi::FeederInterface & FeederInterface )
{
  atlk_rc_t 						rc;
	
  can_service_t *can_service = NULL;
  can_socket_t *can_socket = NULL;
	
	can_socket_config_t socket_config = { .if_index = 1,
																				.filter_array_ptr = NULL, 
																				.filter_array_size = 0
																			};

  const can_id_filter_t filter_array[] = { { .can_id = 0, .can_id_mask = 0 } };


  socket_config.filter_array_ptr = filter_array;
  socket_config.filter_array_size = 1;

  rc = can_hw_service_get(&can_service);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_hw_service_get: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  /* Create CAN socket */
	socket_config.if_index = 1;
  rc = can_socket_create(can_service, &can_socket, &socket_config);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_socket_create: %s\n", atlk_rc_to_str(rc));
    goto error;
  }

  while (1) {
    uint8_t data[CAN_DATA_SIZE_MAX];
    size_t data_size = sizeof(data);
    can_id_t can_id;

  	rc = can_receive(can_socket, data, &data_size, &can_id, &atlk_wait_forever);
  	if (atlk_error(rc)) {
  	  fprintf(stderr, "can_receive: %s\n", atlk_rc_to_str(rc));
      continue;
  	}
    
    rc = can_parser_rx( can_id, data_size, data, FeederInterface );
    

    printf("CAN RX: ID = 0x%x, DLC = %d, Data[0:4] = %d, %d, %d, %d, %d\n",
           (unsigned int)can_id, data_size,
           data[0], data[1], data[2], data[3], data[4]);
  }

  return;
  
error:
  
  can_socket_delete(can_socket);
  can_socket = NULL;
  can_service_delete(can_service);
  can_service = NULL;

  
}

/**
 * Parse incoming CAN frames based on GM protocol
 * @param[in] frame incoming CAN frame
 * @param[out] msg pointer to the output structure to be filled with
 *             the CAN frame relative data
 * @return 0 upon success, negative value otherwise
 */

#define BIT(_bit_val_) (1 << _bit_val_)
#define DATA(_val_) ( _val <= data_size) ? data[_val_] : 0;

int can_parser_rx(can_id_t can_id, size_t data_size, uint8_t data[CAN_DATA_SIZE_MAX], MarbenV2X::Common::Etsi::FeederInterface & FeederInterface )
{
  uint32_t var;
  uint32_t ssu_count = 0;
  MarbenV2X::Common::Etsi::SSUData *ssuData = new MarbenV2X::Common::Etsi::SSUData[3]();

  var = data_size;
  
  if (!data) {
    printf( "can_parser_rx has invalid input at msg or data");
    return (-1);
  }

  switch(can_id) {

  case CAN_MSG_ID_CHASSIS:

    ssuData[0].alt = MarbenV2X::Common::Etsi::SSUData::VehicleBreakdownCase;
    ssu_count++;

    ssuData[1].alt = MarbenV2X::Common::Etsi::SSUData::LateralAcceleration;
    ssu_count++;

    /* pedal_pressure_detected */
    if ( (data[0] & BIT(6)) ) {
      
      ssuData[0].vehicleBreakdownSubCauseCode = MarbenV2X::Common::Etsi::AccelerationControlFlag::GasPedalEngaged;
    }

    
    /* accelerate */
    var = ( ((data[0] & BITMASK(4)) << 8) | data[1] );
    /* var is signed 12 bits */
    ssuData[1].latAcc.lateralAccConfidence = 102;  /* 	In 0.1 meters per seconds squared, (102) unavailable.  */
    /* In 0.1 meters per seconds squared, (161) unavailable.  */
    ssuData[1].latAcc.lateralAccValue = ((var >> 11) == 0) ? var : (-1 ^ 0xFFF) | var;
    

    /* abs_active */
    if ( (data[3] & BIT(6)) ) {
      ssuData[0].vehicleBreakdownSubCauseCode = MarbenV2X::Common::Etsi::DangerousSituationFlag::AbsEngaged;
    }

    /* traction_control_active or stability control*/
    if ( (data[3] & BIT(4)) | (data[3] & BIT(0)) ) {
      ssuData[0].vehicleBreakdownSubCauseCode = MarbenV2X::Common::Etsi::DangerousSituationFlag::EspEngaged;
    }
    
    ssuData[2].alt = MarbenV2X::Common::Etsi::SSUData::YawRate;
    
    var = ((data[4] & BITMASK(4)) << 8) | data[5];
    ssuData[2].yawRate.yawRateValue = (var >> 11) == 0 ? var : (-1 ^ 0xFFF) | var;
    ssuData[2].yawRate.yawRateConfidence = 8;

    /* Set the modification in the SSU */
    FeederInterface.feed( 2, ssuData );
    break;


  case CAN_MSG_ID_BRAKE_APPLY:
    /*
    msg->body.brakes.pedal_moderate_travel = data[0] & BIT(6) ? TRUE : FALSE;
    msg->body.brakes.pedal_initial_tavel = data[0] & BIT(1) ? TRUE : FALSE;
    msg->body.brakes.pedal_postition = data[1];
    */
    break;

  case CAN_MSG_ID_TRANS:
    /*
    msg->body.transmission.transmission_gear = data[0] & BITMASK(4);
    */
    break;

  case CAN_MSG_ID_BODY_INFO:
    /*
    msg->body.body_info.battery_voltage = data[3];
    */
    break;

  case CAN_MSG_ID_SPEED_DIST:
    /*
    msg->body.speed_dist.speed_avg = (data[4] & BITMASK(7)) << 8 | data[5];
    */
    break;

  case CAN_MSG_ID_PLATFORM:
    /*
    msg->body.platform_status.airbag_deployed = data[7] & BIT(2) ? TRUE : FALSE;
    */
    break;

  case CAN_MSG_ID_LIGHTS:
    /*
    msg->body.ext_lights.brake_light_active = data[0] & BIT(6) ? TRUE : FALSE;
    msg->body.ext_lights.headligh_beam = (data[1] & 0x06) >> 1;
    */
    break;

  case CAN_MSG_ID_ABS_TC_STAT:
    /*
    var = ((data[4] & BITMASK(4)) << 8) | data[5];
    msg->body.abs_tc.vehicle_acc = (var >> 11) == 0 ? var : (-1 ^ 0xFFF) | var;
    */
    
    break;

  case CAN_MSG_ID_ETEI_ENGINE_ST:
    /*
    msg->body.etei_eng_stat.acc_pedal_pos = data[6];
    */
    break;

  case CAN_MSG_ID_PPEI_ENGINE_ST1:
    /*
    msg->body.engine_stat1.cruise_control_active = data[3] & BIT(6) ? TRUE : FALSE;
    msg->body.engine_stat1.driver_throttle_override = data[3] & BIT(4) ? TRUE : FALSE;
    */
    break;

  case CAN_MSG_ID_PPEI_ENGINE_ST2:
    /*
    msg->body.engine_stat2.cruise_speed_limiter = ((data[2] & BITMASK(4)) << 8) | data[3];
    */
    break;

  case CAN_MSG_ID_ODOMETER:
    /*
    msg->body.odometer_hs.odometer = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    */
    break;

  case CAN_MSG_ID_STEERING_ANGLE:
    /*
    msg->body.steering.steering_wheel_angle = data[1] << 8 | data[2];
    */
    break;

  default:
    /* Unknown CAN ID */
    return (-1);
  }
  return 0;
}
