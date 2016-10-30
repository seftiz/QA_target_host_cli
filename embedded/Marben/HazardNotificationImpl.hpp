/**
 * \file HazardNotificationImpl.hpp
 */

#ifndef HAZARD_NOTIFICATION_IMPL_HPP_
#define HAZARD_NOTIFICATION_IMPL_HPP_

#include "HazardNotification.hpp"


namespace User
{

/**
 * \Class HazardNotificationImpl
 * \brief Class in charge of processing the hazard notification.
 */
class HazardNotificationImpl: public MarbenV2X::Application::HazardNotification
{
public:
  /** Usual constructor */
  HazardNotificationImpl( );
  /** Usual destructor */
  virtual ~HazardNotificationImpl( );

  /** Processing the hazard notification */
  virtual void notificationIndication(
               MarbenV2X::Application::UseCaseType useCase, 
               MarbenV2X::Application::EventID eventID, 
               MarbenV2X::Facilities::TimeType timeToEvent, 
               int32_t distanceToEvent, 
               MarbenV2X::Facilities::DataUnitPtrVector & dataUnitList, 
               bool end ) ;
};

}

#endif /*HAZARD_NOTIFICATION_IMPL_HPP_*/
