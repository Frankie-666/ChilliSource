//
//  WindowwsStringUtils.h
//  Chilli Source
//
//  Created by Ian Copland on 03/03/2014.
//  Copyright 2014 Tag Games. All rights reserved.
//

#include <codecvt>
#include <locale>
#include <string>

namespace ChilliSource
{
	namespace Windows
	{
		//------------------------------------------------------------
		/// A collection of utility methods for using strings on windows.
		/// This is mostly methods for working with wstrings.
		//------------------------------------------------------------
		namespace WindowsStringUtils
		{
			//------------------------------------------------------------
			/// Converts from a UTF-8 encoded string to a UTF-16 encoded
			/// wstring.
			///
			/// @author I Copland
			///
			/// @param The UTF-8 string.
			/// @return The UTF-16 wstring.
			//------------------------------------------------------------
			std::wstring UTF8ToUTF16(const std::string& in_utf8String);
			//------------------------------------------------------------
			/// Converts from a UTF-16 encoded wstring to a UTF-8 encoded
			/// string.
			///
			/// @author I Copland
			///
			/// @param The UTF-16 wstring.
			/// @return The UTF-8 string.
			//------------------------------------------------------------
			std::string UTF16ToUTF8(const std::wstring& in_utf16String);
		}
	}
}