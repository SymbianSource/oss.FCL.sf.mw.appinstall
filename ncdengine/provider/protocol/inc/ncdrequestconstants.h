/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef NCD_REQUEST_CONSTANTS_HH
#define NCD_REQUEST_CONSTANTS_HH

#include "ncdprotocoltypes.h"

#ifndef RD_XML_ENGINE_API_CHANGE
typedef TSerializationOptions TXmlEngSerializationOptions;
static const char* KSerializationOptionUtf8 = "UTF-8";
#else
_LIT8( KSerializationOptionUtf8, "UTF-8" );
#endif

_LIT8( KDefaultNamespaceUri, "http://nokia.com/preminet/protocol/v/2/0" );

_LIT8( KCdpNamespacePrefix, "cp" );
_LIT8( KCdpNamespaceUri, "http://nokia.com/preminet/protocol/configuration/v/1/0" );

_LIT8( KXsNamespacePrefix, "xs" );
_LIT8( KXsNamespaceUri, "http://www.w3.org/2001/XMLSchema" );

_LIT8( KXsiNamespacePrefix, "xsi");
_LIT8( KXsiNamespaceUri, "http://www.w3.org/2001/XMLSchema-instance");

_LIT8( KPreminetReqVersion, "2.0");
_LIT8( KConfigurationReqVersion, "1.0");
_LIT8( KDefaultRequestType, "preminet" );

#endif //NCD_REQUEST_CONSTANTS_HH