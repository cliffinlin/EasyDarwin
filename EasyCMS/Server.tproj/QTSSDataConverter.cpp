/*
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright (c) 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 *
 */
 /*
     Copyleft (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
     Github: https://github.com/EasyDarwin
     WEChat: EasyDarwin
     Website: http://www.EasyDarwin.org
 */
 /*
     File:       QTSSDataConverter.cpp

     Contains:   Utility routines for converting to and from
                 QTSS_AttrDataTypes and text

     Written By: Denis Serenyi

     Change History (most recent first):

 */

#include "QTSSDataConverter.h"
#include "StrPtrLen.h"
#include <string.h>
#include <stdio.h>

static const StrPtrLen kEnabledStr("true");
static const StrPtrLen kDisabledStr("false");

static char* kDataTypeStrings[] =
{
    "Unknown",
    "CharArray",
    "bool",
    "SInt16",
    "UInt16",
    "SInt32",
    "UInt32",
    "SInt64",
    "UInt64",
    "QTSS_Object",
    "QTSS_StreamRef",
    "Float32",
    "Float64",
    "VoidPointer",
    "QTSS_TimeVal"
};

static const char* kHEXChars = { "0123456789ABCDEF" };

static const UInt8 sCharToNums[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0-9 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //10-19 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //20-29
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //30-39 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //40-49 - 48 = '0'
    2, 3, 4, 5, 6, 7, 8, 9, 0, 0, //50-59
    0, 0, 0, 0, 0, 10, 11, 12, 13, 14, //60-69 A-F
    15, 0, 0, 0, 0, 0, 0, 0, 0, 0, //70-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //80-89
    0, 0, 0, 0, 0, 0, 0, 10, 11, 12, //90-99 a-f
    13, 14, 15, 0, 0, 0, 0, 0, 0, 0, //100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //110-119
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //120-129
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //130-139
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //140-149
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //150-159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //160-169
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //170-179
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //180-189
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //190-199
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //200-209
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //210-219
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //220-229
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //230-239
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //240-249
    0, 0, 0, 0, 0, 0             //250-255
};

char* QTSSDataConverter::TypeToTypeString(QTSS_AttrDataType inType)
{
    if (inType < qtssAttrDataTypeNumTypes)
        return kDataTypeStrings[inType];
    return kDataTypeStrings[qtssAttrDataTypeUnknown];
}

QTSS_AttrDataType QTSSDataConverter::TypeStringToType(char* inTypeString)
{
    for (UInt32 x = 0; x < qtssAttrDataTypeNumTypes; x++)
    {
        StrPtrLen theTypeStrPtr(inTypeString);
        if (theTypeStrPtr.EqualIgnoreCase(kDataTypeStrings[x], ::strlen(kDataTypeStrings[x])))
            return x;
    }
    return qtssAttrDataTypeUnknown;
}

QTSS_Error QTSSDataConverter::StringToValue(char* inValueAsString,
    QTSS_AttrDataType inType,
    void* ioBuffer,
    UInt32* ioBufSize)
{
    if (inValueAsString == nullptr || ioBufSize == nullptr)
        return QTSS_BadArgument;

    if (inType == qtssAttrDataTypeCharArray)
    {
        //
        // If this data type is a string, copy the string into
        // the destination buffer
        UInt32 theLen = ::strlen(inValueAsString);

        //
        // First check to see if the destination is big enough
        if ((ioBuffer == nullptr) || (*ioBufSize < theLen))
        {
            *ioBufSize = theLen;
            return QTSS_NotEnoughSpace;
        }

        //
        // Do the string copy. Use memcpy for speed.
        ::memcpy(ioBuffer, inValueAsString, theLen);
        *ioBufSize = theLen;
        return QTSS_NoErr;
    }

    if (inType == qtssAttrDataTypeBool16)
    {
        //
        // The text "enabled" means true, anything else means false
        if (*ioBufSize < sizeof(bool))
        {
            *ioBufSize = sizeof(bool);
            return QTSS_NotEnoughSpace;
        }

        bool* it = static_cast<bool*>(ioBuffer);
        StrPtrLen theValuePtr(inValueAsString);
        if (kEnabledStr.EqualIgnoreCase(inValueAsString, ::strlen(inValueAsString)))
            *it = true;
        else
            *it = false;

        *ioBufSize = sizeof(bool);
        return QTSS_NoErr;
    }

    UInt32 theBufSize;
    char* theFormat;
    //
    // If this is another type, format the string into that type
    switch (inType)
    {
    case qtssAttrDataTypeUInt16:
        {
            theBufSize = sizeof(UInt16);
            theFormat = "%hu";
        }
        break;

    case qtssAttrDataTypeSInt16:
        {
            theBufSize = sizeof(SInt16);
            theFormat = "%hd";
        }
        break;

    case qtssAttrDataTypeSInt32:
        {
            theBufSize = sizeof(SInt32);
            theFormat = "%d";
        }
        break;

    case qtssAttrDataTypeUInt32:
        {
            theBufSize = sizeof(UInt32);
            theFormat = "%u";
        }
        break;

    case qtssAttrDataTypeSInt64:
        {
            theBufSize = sizeof(SInt64);
            theFormat = "%" _64BITARG_ "d";
        }
        break;

    case qtssAttrDataTypeUInt64:
        {
            theBufSize = sizeof(UInt64);
            theFormat = "%" _64BITARG_ "u";
        }
        break;

    case qtssAttrDataTypeFloat32:
        {
            theBufSize = sizeof(Float32);
            theFormat = "%f";
        }
        break;

    case qtssAttrDataTypeFloat64:
        {
            theBufSize = sizeof(Float64);
            theFormat = "%f";
        }
        break;

    case qtssAttrDataTypeTimeVal:
        {
            theBufSize = sizeof(SInt64);
            theFormat = "%" _64BITARG_ "d";
        }
        break;

    default:
        return ConvertCHexStringToBytes(inValueAsString, ioBuffer, ioBufSize);
    }

    if ((ioBuffer == nullptr) || (*ioBufSize < theBufSize))
    {
        *ioBufSize = theBufSize;
        return QTSS_NotEnoughSpace;
    }
    *ioBufSize = theBufSize;
    ::sscanf(inValueAsString, theFormat, ioBuffer);

    return QTSS_NoErr;
}

QTSS_Error QTSSDataConverter::ConvertCHexStringToBytes(char* inValueAsString,
    void* ioBuffer,
    UInt32* ioBufSize)
{
    UInt32 stringLen = ::strlen(inValueAsString);
    UInt32 dataLen = (stringLen + (stringLen & 1 ? 1 : 0)) / 2;

    // First check to see if the destination is big enough
    if ((ioBuffer == nullptr) || (*ioBufSize < dataLen))
    {
        *ioBufSize = dataLen;
        return QTSS_NotEnoughSpace;
    }

    UInt8* dataPtr = static_cast<UInt8*>(ioBuffer);
    UInt8 char1, char2;
    while (*inValueAsString)
    {
        char1 = sCharToNums[static_cast<UInt8>(*inValueAsString++)] * 16;
        if (*inValueAsString != 0)
            char2 = sCharToNums[static_cast<UInt8>(*inValueAsString++)];
        else
            char2 = 0;
        *dataPtr++ = char1 + char2;
    }

    *ioBufSize = dataLen;
    return QTSS_NoErr;
}

char* QTSSDataConverter::ConvertBytesToCHexString(void* inValue, const UInt32 inValueLen)
{
    UInt8* theDataPtr = static_cast<UInt8*>(inValue);
    UInt32 len = inValueLen * 2;

    char *theString = new char[len + 1];
    char *resultStr = theString;
    if (theString != nullptr)
    {
        UInt8 temp;
        for (UInt32 count = 0; count < inValueLen; ++count)
        {
            temp = *theDataPtr++;
            *theString++ = kHEXChars[temp >> 4];
            *theString++ = kHEXChars[temp & 0xF];
        }
        *theString = 0;
    }
    return resultStr;
}

char* QTSSDataConverter::ValueToString(void* inValue,
    const UInt32 inValueLen,
    const QTSS_AttrDataType inType)
{
    if (inValue == nullptr)
        return nullptr;

    if (inType == qtssAttrDataTypeCharArray)
    {
        StrPtrLen theStringPtr(static_cast<char*>(inValue), inValueLen);
        return theStringPtr.GetAsCString();
    }
    if (inType == qtssAttrDataTypeBool16)
    {
        bool* theBoolPtr = static_cast<bool*>(inValue);
        if (*theBoolPtr)
            return kEnabledStr.GetAsCString();
        else
            return kDisabledStr.GetAsCString();
    }

    //
    // With these other types, its impossible to tell how big they'll
    // be, so just allocate some buffer and hope we fit.
    char* theString = new char[128];

    //
    // If this is another type, format the string into that type
    switch (inType)
    {
    case qtssAttrDataTypeUInt16:
        qtss_sprintf(theString, "%hu", *static_cast<UInt16*>(inValue));
        break;

    case qtssAttrDataTypeSInt16:
        qtss_sprintf(theString, "%hd", *static_cast<SInt16*>(inValue));
        break;

    case qtssAttrDataTypeSInt32:
        qtss_sprintf(theString, "%" _U32BITARG_ , *static_cast<SInt32*>(inValue));
        break;

    case qtssAttrDataTypeUInt32:
        qtss_sprintf(theString, "%" _U32BITARG_ , *static_cast<UInt32*>(inValue));
        break;

    case qtssAttrDataTypeSInt64:
        qtss_sprintf(theString, "%" _64BITARG_ "d", *static_cast<SInt64*>(inValue));
        break;

    case qtssAttrDataTypeUInt64:
        qtss_sprintf(theString, "%" _64BITARG_ "u", *static_cast<UInt64*>(inValue));
        break;

    case qtssAttrDataTypeFloat32:
        qtss_sprintf(theString, "%f", *static_cast<Float32*>(inValue));
        break;

    case qtssAttrDataTypeFloat64:
        qtss_sprintf(theString, "%f", *static_cast<Float64*>(inValue));
        break;

    case qtssAttrDataTypeTimeVal:
        qtss_sprintf(theString, "%" _64BITARG_ "d", *static_cast<SInt64*>(inValue));
        break;

    default:
        delete[] theString;
        theString = ConvertBytesToCHexString(inValue, inValueLen);
    }

    return theString;
}