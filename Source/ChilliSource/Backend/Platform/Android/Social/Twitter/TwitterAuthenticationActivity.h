/*
 *  TwitterAuthenticationActivity.h
 *  moFlow
 *
 *  Created by Robert Henning on 14/05/2012.
 *  Copyright (c) 2012 Tag Games. All rights reserved.
 *
 */

#ifndef _MO_FLO_PLATFORM_ANDROID_SOCIAL_TWITTER_TWITTER_AUTHENTICATION_ACTIVITY_H_
#define _MO_FLO_PLATFORM_ANDROID_SOCIAL_TWITTER_TWITTER_AUTHENTICATION_ACTIVITY_H_

#include <ChilliSource/Social/Twitter/TwitterAuthenticationActivity.h>
#include <ChilliSource/Core/Math/UnifiedCoordinates.h>

//@class UIView;
//@class UILabel;
//@class UIActivityIndicatorView;
//@class UIButton;
//@class UITwitterAuthenticationWebDelegate;
//@class UITwitterTextFieldDelegate;
//@class UITwitterAlertDelegate;
//@protocol UIWebViewDelegate;
//@protocol UITextFieldDelegate;
//@protocol UIAlertViewDelegate;

namespace ChilliSource
{
	namespace Android
	{
		class CTwitterAuthenticationActivity : public Social::ITwitterAuthenticationActivity
		{
		public:
			CTwitterAuthenticationActivity();
			//-----------------------------------------------
			/// Present
			/// 
			/// Starts the activity by presenting a view with
			/// a 'busy, please wait' message
			//-----------------------------------------------
			void Present();
			//-----------------------------------------------
			/// Presents the authorise URL
			/// 
			/// Set the URL received from Twitter to visit
			/// in order to see PIN
			///
			/// @param URL
			//-----------------------------------------------
			void PresentURL(const std::string& instrURL);
			//-----------------------------------------------
			/// Save PIN
			/// 
			/// Saves the PIN entered by the user
			///
			/// @param PIN number entered by the user
			//-----------------------------------------------
			bool SavePIN(const std::string& instrPIN);
			//-----------------------------------------------
			/// Dismiss
			/// 
			/// Dismiss the web view interface
			//-----------------------------------------------
			void Dismiss();
			//-----------------------------------------------
			/// On PIN Complete
			///
			/// Called when the user has entered a valid PIN
			//-----------------------------------------------
			void OnPINComplete();
			
			moFlo::UnifiedVector2					mvUnifiedSize;
			moFlo::Core::CVector2					mvAbsoluteSize;
			moFlo::Core::CVector2					mvAbsolutePositionTopLeft;
			moFlo::Core::CVector2					mvAbsolutePositionCentre;
		};
	}
}

#endif
