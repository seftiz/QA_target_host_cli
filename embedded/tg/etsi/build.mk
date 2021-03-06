$(eval $(call def_asn1c,ITS-Container.asn CAM.asn DENM.asn,etsi,-gen-PER -fnative-types))

obj-y += AccelerationConfidence.o
obj-y += AccelerationControl.o
obj-y += AccidentSubCauseCode.o
obj-y += ActionID.o
obj-y += AdverseWeatherCondition-AdhesionSubCauseCode.o
obj-y += AdverseWeatherCondition-ExtremeWeatherConditionSubCauseCode.o
obj-y += AdverseWeatherCondition-PrecipitationSubCauseCode.o
obj-y += AdverseWeatherCondition-VisibilitySubCauseCode.o
obj-y += AlacarteContainer.o
obj-y += Altitude.o
obj-y += AltitudeConfidence.o
obj-y += AltitudeValue.o
obj-y += asn_codecs_prim.o
obj-y += asn_SEQUENCE_OF.o
obj-y += asn_SET_OF.o
obj-y += BasicContainer.o
obj-y += BasicVehicleContainerHighFrequency.o
obj-y += BasicVehicleContainerLowFrequency.o
obj-y += ber_decoder.o
obj-y += ber_tlv_length.o
obj-y += ber_tlv_tag.o
obj-y += BIT_STRING.o
obj-y += BOOLEAN.o
obj-y += CAM.o
obj-y += CamParameters.o
obj-y += CauseCode.o
obj-y += CauseCodeType.o
obj-y += ClosedLanes.o
obj-y += CollisionRiskSubCauseCode.o
obj-y += constraints.o
obj-y += constr_CHOICE.o
obj-y += constr_SEQUENCE.o
obj-y += constr_SEQUENCE_OF.o
obj-y += constr_SET_OF.o
obj-y += constr_TYPE.o
obj-y += CoopAwareness.o
obj-y += Curvature.o
obj-y += CurvatureCalculationMode.o
obj-y += CurvatureConfidence.o
obj-y += CurvatureValue.o
obj-y += DangerousEndOfQueueSubCauseCode.o
obj-y += DangerousGoodsBasic.o
obj-y += DangerousGoodsContainer.o
obj-y += DangerousGoodsExtended.o
obj-y += DangerousSituationSubCauseCode.o
obj-y += DecentralizedEnvironmentalNotificationMessage.o
obj-y += DeltaAltitude.o
obj-y += DeltaLatitude.o
obj-y += DeltaLongitude.o
obj-y += DeltaReferencePosition.o
obj-y += DENM.o
obj-y += der_encoder.o
obj-y += DriveDirection.o
obj-y += DrivingLaneStatus.o
obj-y += EmbarkationStatus.o
obj-y += EmergencyContainer.o
obj-y += EmergencyPriority.o
obj-y += EmergencyVehicleApproachingSubCauseCode.o
obj-y += EmptyRSUContainerHighFrequency.o
obj-y += EnergyStorageType.o
obj-y += ExteriorLights.o
obj-y += GenerationDeltaTime.o
obj-y += HardShoulderStatus.o
obj-y += HazardousLocation-AnimalOnTheRoadSubCauseCode.o
obj-y += HazardousLocation-DangerousCurveSubCauseCode.o
obj-y += HazardousLocation-ObstacleOnTheRoadSubCauseCode.o
obj-y += HazardousLocation-SurfaceConditionSubCauseCode.o
obj-y += Heading.o
obj-y += HeadingConfidence.o
obj-y += HeadingValue.o
obj-y += HeightLonCarr.o
obj-y += HighFrequencyContainer.o
obj-y += HumanPresenceOnTheRoadSubCauseCode.o
obj-y += HumanProblemSubCauseCode.o
obj-y += IA5String.o
obj-y += ImpactReductionContainer.o
obj-y += InformationQuality.o
obj-y += INTEGER.o
obj-y += ItineraryPath.o
obj-y += ItsPduHeader.o
obj-y += LaneNumber.o
obj-y += LateralAcceleration.o
obj-y += LateralAccelerationValue.o
obj-y += Latitude.o
obj-y += LightBarSirenInUse.o
obj-y += LocationContainer.o
obj-y += Longitude.o
obj-y += LongitudinalAcceleration.o
obj-y += LongitudinalAccelerationValue.o
obj-y += LowFrequencyContainer.o
obj-y += ManagementContainer.o
obj-y += NativeEnumerated.o
obj-y += NativeInteger.o
obj-y += NULL.o
obj-y += NumberOfOccupants.o
obj-y += OCTET_STRING.o
obj-y += PathDeltaTime.o
obj-y += PathHistory.o
obj-y += PathPoint.o
obj-y += per_decoder.o
obj-y += per_encoder.o
obj-y += PerformanceClass.o
obj-y += per_opentype.o
obj-y += per_support.o
obj-y += PosCentMass.o
obj-y += PosConfidenceEllipse.o
obj-y += PosFrontAx.o
obj-y += PositioningSolutionType.o
obj-y += PositionOfOccupants.o
obj-y += PositionOfPillars.o
obj-y += PosLonCarr.o
obj-y += PosPillar.o
obj-y += PostCrashSubCauseCode.o
obj-y += PtActivation.o
obj-y += PtActivationData.o
obj-y += PtActivationType.o
obj-y += PublicTransportContainer.o
obj-y += ReferencePosition.o
obj-y += RelevanceDistance.o
obj-y += RelevanceTrafficDirection.o
obj-y += RequestResponseIndication.o
obj-y += RescueAndRecoveryWorkInProgressSubCauseCode.o
obj-y += RescueContainer.o
obj-y += RestrictedTypes.o
obj-y += RoadType.o
obj-y += RoadWorksContainerBasic.o
obj-y += RoadWorksContainerExtended.o
obj-y += RoadworksSubCauseCode.o
obj-y += SafetyCarContainer.o
obj-y += SemiAxisLength.o
obj-y += SequenceNumber.o
obj-y += SignalViolationSubCauseCode.o
obj-y += SituationContainer.o
obj-y += SlowVehicleSubCauseCode.o
obj-y += SpecialTransportContainer.o
obj-y += SpecialTransportType.o
obj-y += SpecialVehicleContainer.o
obj-y += Speed.o
obj-y += SpeedConfidence.o
obj-y += SpeedLimit.o
obj-y += SpeedValue.o
obj-y += StationarySince.o
obj-y += StationaryVehicleContainer.o
obj-y += StationaryVehicleSubCauseCode.o
obj-y += StationID.o
obj-y += StationType.o
obj-y += SteeringWheelAngle.o
obj-y += SteeringWheelAngleValue.o
obj-y += SteeringWheelConfidence.o
obj-y += SubCauseCodeType.o
obj-y += Temperature.o
obj-y += TimestampIts.o
obj-y += Traces.o
obj-y += TrafficConditionSubCauseCode.o
obj-y += TrafficRule.o
obj-y += TransmissionInterval.o
obj-y += TurningRadius.o
obj-y += UTF8String.o
obj-y += ValidityDuration.o
obj-y += VDS.o
obj-y += VehicleBreakdownSubCauseCode.o
obj-y += VehicleIdentification.o
obj-y += VehicleLength.o
obj-y += VehicleLengthConfidenceIndication.o
obj-y += VehicleLengthValue.o
obj-y += VehicleMass.o
obj-y += VehicleRole.o
obj-y += VehicleWidth.o
obj-y += VerticalAcceleration.o
obj-y += VerticalAccelerationValue.o
obj-y += WheelBaseVehicle.o
obj-y += WMInumber.o
obj-y += WrongWayDrivingSubCauseCode.o
obj-y += xer_decoder.o
obj-y += xer_encoder.o
obj-y += xer_support.o
obj-y += YawRate.o
obj-y += YawRateConfidence.o
obj-y += YawRateValue.o

# This is only CAM modules
obj-z += AccelerationConfidence.o
obj-z += AccelerationControl.o
obj-z += AccidentSubCauseCode.o
obj-z += AdverseWeatherCondition-AdhesionSubCauseCode.o
obj-z += AdverseWeatherCondition-ExtremeWeatherConditionSubCauseCode.o
obj-z += AdverseWeatherCondition-PrecipitationSubCauseCode.o
obj-z += AdverseWeatherCondition-VisibilitySubCauseCode.o
obj-z += Altitude.o
obj-z += AltitudeConfidence.o
obj-z += AltitudeValue.o
obj-z += asn_codecs_prim.o
obj-z += asn_SEQUENCE_OF.o
obj-z += asn_SET_OF.o
obj-z += BasicContainer.o
obj-z += BasicVehicleContainerHighFrequency.o
obj-z += BasicVehicleContainerLowFrequency.o
obj-z += ber_decoder.o
obj-z += ber_tlv_length.o
obj-z += ber_tlv_tag.o
obj-z += BIT_STRING.o
obj-z += BOOLEAN.o
obj-z += CAM.o
obj-z += CamParameters.o
obj-z += CauseCode.o
obj-z += CauseCodeType.o
obj-z += ClosedLanes.o
obj-z += CollisionRiskSubCauseCode.o
obj-z += constraints.o
obj-z += constr_CHOICE.o
obj-z += constr_SEQUENCE.o
obj-z += constr_SEQUENCE_OF.o
obj-z += constr_SET_OF.o
obj-z += constr_TYPE.o
obj-z += CoopAwareness.o
obj-z += Curvature.o
obj-z += CurvatureCalculationMode.o
obj-z += CurvatureConfidence.o
obj-z += CurvatureValue.o
obj-z += DangerousEndOfQueueSubCauseCode.o
obj-z += DangerousGoodsBasic.o
obj-z += DangerousGoodsContainer.o
obj-z += DangerousGoodsExtended.o
obj-z += DangerousSituationSubCauseCode.o
obj-z += DeltaAltitude.o
obj-z += DeltaLatitude.o
obj-z += DeltaLongitude.o
obj-z += DeltaReferencePosition.o
obj-z += der_encoder.o
obj-z += DriveDirection.o
obj-z += DrivingLaneStatus.o
obj-z += EmbarkationStatus.o
obj-z += EmergencyContainer.o
obj-z += EmergencyPriority.o
obj-z += EmergencyVehicleApproachingSubCauseCode.o
obj-z += EmptyRSUContainerHighFrequency.o
obj-z += EnergyStorageType.o
obj-z += ExteriorLights.o
obj-z += GenerationDeltaTime.o
obj-z += HardShoulderStatus.o
obj-z += HazardousLocation-AnimalOnTheRoadSubCauseCode.o
obj-z += HazardousLocation-DangerousCurveSubCauseCode.o
obj-z += HazardousLocation-ObstacleOnTheRoadSubCauseCode.o
obj-z += HazardousLocation-SurfaceConditionSubCauseCode.o
obj-z += Heading.o
obj-z += HeadingConfidence.o
obj-z += HeadingValue.o
obj-z += HeightLonCarr.o
obj-z += HighFrequencyContainer.o
obj-z += HumanPresenceOnTheRoadSubCauseCode.o
obj-z += HumanProblemSubCauseCode.o
obj-z += IA5String.o
obj-z += InformationQuality.o
obj-z += INTEGER.o
obj-z += ItsPduHeader.o
obj-z += LaneNumber.o
obj-z += LateralAcceleration.o
obj-z += LateralAccelerationValue.o
obj-z += Latitude.o
obj-z += LightBarSirenInUse.o
obj-z += Longitude.o
obj-z += LongitudinalAcceleration.o
obj-z += LongitudinalAccelerationValue.o
obj-z += LowFrequencyContainer.o
obj-z += NativeEnumerated.o
obj-z += NativeInteger.o
obj-z += NULL.o
obj-z += OCTET_STRING.o
obj-z += PathDeltaTime.o
obj-z += PathHistory.o
obj-z += PathPoint.o
obj-z += per_decoder.o
obj-z += per_encoder.o
obj-z += PerformanceClass.o
obj-z += per_opentype.o
obj-z += per_support.o
obj-z += PosCentMass.o
obj-z += PosConfidenceEllipse.o
obj-z += PosFrontAx.o
obj-z += PositioningSolutionType.o
obj-z += PositionOfOccupants.o
obj-z += PosLonCarr.o
obj-z += PosPillar.o
obj-z += PostCrashSubCauseCode.o
obj-z += PtActivation.o
obj-z += PtActivationData.o
obj-z += PtActivationType.o
obj-z += PublicTransportContainer.o
obj-z += ReferencePosition.o
obj-z += RequestResponseIndication.o
obj-z += RescueAndRecoveryWorkInProgressSubCauseCode.o
obj-z += RescueContainer.o
obj-z += RoadType.o
obj-z += RoadWorksContainerBasic.o
obj-z += RoadworksSubCauseCode.o
obj-z += SafetyCarContainer.o
obj-z += SemiAxisLength.o
obj-z += SignalViolationSubCauseCode.o
obj-z += SlowVehicleSubCauseCode.o
obj-z += SpecialTransportContainer.o
obj-z += SpecialTransportType.o
obj-z += SpecialVehicleContainer.o
obj-z += Speed.o
obj-z += SpeedConfidence.o
obj-z += SpeedLimit.o
obj-z += SpeedValue.o
obj-z += StationarySince.o
obj-z += StationaryVehicleSubCauseCode.o
obj-z += StationID.o
obj-z += StationType.o
obj-z += SteeringWheelAngle.o
obj-z += SteeringWheelAngleValue.o
obj-z += SteeringWheelConfidence.o
obj-z += SubCauseCodeType.o
obj-z += Temperature.o
obj-z += TimestampIts.o
obj-z += TrafficConditionSubCauseCode.o
obj-z += TrafficRule.o
obj-z += TurningRadius.o
obj-z += UTF8String.o
obj-z += VDS.o
obj-z += VehicleBreakdownSubCauseCode.o
obj-z += VehicleIdentification.o
obj-z += VehicleLength.o
obj-z += VehicleLengthConfidenceIndication.o
obj-z += VehicleLengthValue.o
obj-z += VehicleMass.o
obj-z += VehicleRole.o
obj-z += VehicleWidth.o
obj-z += VerticalAcceleration.o
obj-z += VerticalAccelerationValue.o
obj-z += WheelBaseVehicle.o
obj-z += WMInumber.o
obj-z += WrongWayDrivingSubCauseCode.o
obj-z += xer_decoder.o
obj-z += xer_encoder.o
obj-z += xer_support.o
obj-z += YawRate.o
obj-z += YawRateConfidence.o
obj-z += YawRateValue.o

x := $(TARGET)/lib/libasn1-etsi.a
$x-obj-y := $(obj-y)
$x-cflags-y := -w -O2
build-y += $x

x := $(HOST)/lib/libasn1-etsi.a
$x-obj-y := $(obj-y)
$x-cflags-y := -w -O2
build-y += $x
