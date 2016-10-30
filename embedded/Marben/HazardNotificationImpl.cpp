/**
 * \file HazardNotificationImpl.cpp
 */

#include "HazardNotificationImpl.hpp"


namespace User
{

/**
 * Usual constructor
 */
HazardNotificationImpl::HazardNotificationImpl( )
{
  // Implement here the specific code
}

/**
 * Usual destructor
 */
HazardNotificationImpl::~HazardNotificationImpl( )
{
  // Implement here the specific code
}

/**
 * Processing the hazard notification.
 */
void HazardNotificationImpl::notificationIndication(
  MarbenV2X::Application::UseCaseType, MarbenV2X::Application::EventID, MarbenV2X::Facilities::TimeType, 
  int32_t, MarbenV2X::Facilities::DataUnitPtrVector &, bool )
{
  // Implement here the specific code
}

}
