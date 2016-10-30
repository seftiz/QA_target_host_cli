/**
 * \file ssuUpdate.hpp
 */

#ifndef SSU_UPDATE_HPP_
#define SSU_UPDATE_HPP_

#include "EtsiFeederInterface.hpp"

enum can_gm_msgs_id {
  CAN_MSG_ID_CHASSIS          = 0x1E9,
  CAN_MSG_ID_BRAKE_APPLY      = 0x0F1,
  CAN_MSG_ID_TRANS            = 0x1F5,
  CAN_MSG_ID_BODY_INFO        = 0x12A,
  CAN_MSG_ID_SPEED_DIST       = 0x3E9,
  CAN_MSG_ID_PLATFORM         = 0x1F1,
  CAN_MSG_ID_LIGHTS           = 0x140,
  CAN_MSG_ID_ABS_TC_STAT      = 0x17D,
  CAN_MSG_ID_ETEI_ENGINE_ST   = 0x1A1,
  CAN_MSG_ID_PPEI_ENGINE_ST1  = 0x0C9,
  CAN_MSG_ID_PPEI_ENGINE_ST2  = 0x3D1,
  CAN_MSG_ID_ODOMETER         = 0x120,
  CAN_MSG_ID_STEERING_ANGLE   = 0x1E5
};

#define BITMASK(nbits) ((1 << nbits) - 1)


void ssuUpdate( MarbenV2X::Common::Etsi::FeederInterface & FeederInterface );
void ssuUpdateGPS( MarbenV2X::Common::Etsi::FeederInterface & FeederInterface );
void ssu_update_can( MarbenV2X::Common::Etsi::FeederInterface & FeederInterface );

#endif /* SSU_UPDATE_HPP_ */
