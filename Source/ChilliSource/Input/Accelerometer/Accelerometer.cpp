//
//  Accelerometer.cpp
//  moFlow
//
// Created by Ian Copland on 13/06/2013
// Copyright 2013 Tag Games Limited. All rights reserved
//

#include <ChilliSource/Input/Accelerometer/Accelerometer.h>

#include <ChilliSource/Core/Base/FastDelegate.h>

namespace ChilliSource
{
	namespace Input
	{
		DEFINE_NAMED_INTERFACE(Accelerometer);
		//----------------------------------------------------
		/// Constructor
		//----------------------------------------------------
		Accelerometer::Accelerometer()
		{
		}
		//----------------------------------------------------
		/// Get Acceleration Updated Event
		//----------------------------------------------------
		Core::IEvent<Accelerometer::AccelerationUpdatedDelegate>& Accelerometer::GetAccelerationUpdatedEvent()
		{
			return mAccelerationUpdatedEvent;
		}
		//----------------------------------------------------
		/// Destructor
		//----------------------------------------------------
		Accelerometer::~Accelerometer()
		{
		}
	}
}
