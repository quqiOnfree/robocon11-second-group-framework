///\file

/******************************************************************************
The MIT License(MIT)
Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com
Copyright(c) 2024 John Wellbelove
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef GDUT_BASE64_INCLUDED
#define GDUT_BASE64_INCLUDED

#include "platform.hpp"
#include "static_assert.hpp"
#include "exception.hpp"
#include "error_handler.hpp"
#include "type_traits.hpp"
#include "enum_type.hpp"
#include "integral_limits.hpp"

#include <stdint.h>

/**************************************************************************************************************************************************************************
* See https://en.wikipedia.org/wiki/Base64
* 
* Encoding	                                                Encoding characters	     Separate encoding of lines	                          Decoding non-encoding characters
*                                                           62nd	63rd	Pad	         Separators	Length	                        Checksum
* RFC 1421 : Base64 for Privacy - Enhanced Mail(deprecated)   +    /    = mandatory 	CR + LF	    64, or lower for the last line	No	    No
* RFC 2045 : Base64 transfer encoding for MIME                +    /    = mandatory 	CR + LF	    At most 76	No	                        Discarded
* RFC 2152 : Base64 for UTF - 7                               +    /    No	         No	                                                  No
* RFC 3501 : Base64 encoding for IMAP mailbox names           +    ,    No	         No	                                                  No
* RFC 4648 : base64(standard)[a]                              +    /    = optional 	 No	                                                  No
* RFC 4648 : base64url(URL - and filename - safe standard)    -    _    = optional   No	                                                  No
**************************************************************************************************************************************************************************/

namespace gdut
{
  //***************************************************************************
  /// Exception base for base64
  //***************************************************************************
  class base64_exception : public gdut::exception
  {
  public:

    base64_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// buffer overflow exception.
  //***************************************************************************
  class base64_overflow : public base64_exception
  {
  public:

    base64_overflow(string_type file_name_, numeric_type line_number_)
      : base64_exception(GDUT_ERROR_TEXT("base64:overflow", GDUT_BASE64_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Illegal character exception.
  //***************************************************************************
  class base64_invalid_data : public base64_exception
  {
  public:

    base64_invalid_data(string_type file_name_, numeric_type line_number_)
      : base64_exception(GDUT_ERROR_TEXT("base64:invalid data", GDUT_BASE64_FILE_ID"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Invalid decode input length exception.
  //***************************************************************************
  class base64_invalid_decode_input_length : public base64_exception
  {
  public:

    base64_invalid_decode_input_length(string_type file_name_, numeric_type line_number_)
      : base64_exception(GDUT_ERROR_TEXT("base64:invalid decode input length", GDUT_BASE64_FILE_ID"C"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Common Base64 definitions
  //***************************************************************************
  class base64
  {
  public:

    struct Encoding
    {
      enum enum_type
      {
        //RFC_1421, // Not implemented
        //RFC_2045, // Not implemented
        RFC_2152,
        RFC_3501,
        RFC_4648,
        RFC_4648_PADDING,
        RFC_4648_URL,
        RFC_4648_URL_PADDING,
      };

      GDUT_DECLARE_ENUM_TYPE(Encoding, int)
      //GDUT_ENUM_TYPE(RFC_1421, "RFC_1421") // Not implemented
      //GDUT_ENUM_TYPE(RFC_2045, "RFC_2045") // Not implemented
      GDUT_ENUM_TYPE(RFC_2152,             "RFC_2152")
      GDUT_ENUM_TYPE(RFC_3501,             "RFC_3501")
      GDUT_ENUM_TYPE(RFC_4648,             "RFC_4648")
      GDUT_ENUM_TYPE(RFC_4648_PADDING,     "RFC_4648_PADDING")
      GDUT_ENUM_TYPE(RFC_4648_URL,         "RFC_4648_URL")
      GDUT_ENUM_TYPE(RFC_4648_URL_PADDING, "RFC_4648_URL_PADDING")
      GDUT_END_ENUM_TYPE
    };

    struct Padding
    {
      enum enum_type
      {
        No_Padding  = 0,
        Use_Padding = 1
      };

      GDUT_DECLARE_ENUM_TYPE(Padding, bool)
      GDUT_ENUM_TYPE(No_Padding,  "No_Padding")
      GDUT_ENUM_TYPE(Use_Padding, "Use_Padding")
      GDUT_END_ENUM_TYPE
    };

    struct Non_Coding_Characters
    {
      enum enum_type
      {
        Ignore = 0,
        Reject = 1
      };

      GDUT_DECLARE_ENUM_TYPE(Non_Coding_Characters, bool)
      GDUT_ENUM_TYPE(Ignore, "Ignore")
      GDUT_ENUM_TYPE(Reject, "Reject")
      GDUT_END_ENUM_TYPE
    };

    enum
    {
      Invalid_Data = gdut::integral_limits<int>::max,
      Min_Encode_Buffer_Size = 4,
      Min_Decode_Buffer_Size = 3
    };

  protected:

    GDUT_CONSTEXPR14
    base64(const char* encoder_table_,
           bool        use_padding_)
      : encoder_table(encoder_table_)
      , use_padding(use_padding_)
    {
    }

    //*************************************************************************
    // Character set for RFC-1421, RFC-2045, RFC-2152 and RFC-4648
    //*************************************************************************
    static
    GDUT_CONSTEXPR14
    const char* character_set_1()
    {
      return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    }

    //*************************************************************************
    // Character set for RFC-4648-URL
    //*************************************************************************
    static
    GDUT_CONSTEXPR14
    const char* character_set_2()
    {
      return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    }

    //*************************************************************************
    // Character set for RFC-3501-URL
    //*************************************************************************
    static
    GDUT_CONSTEXPR14
    const char* character_set_3()
    {
      return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,";
    }

    const char* encoder_table;
    const bool  use_padding;
  };
}
#endif
