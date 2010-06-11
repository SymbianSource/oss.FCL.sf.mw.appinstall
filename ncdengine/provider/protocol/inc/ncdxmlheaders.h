/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NCD_XML_HEADERS_H
#define NCD_XML_HEADERS_H


#ifdef RD_XML_ENGINE_API_CHANGE
    // Comment this if you compile for 3.2 wk12 or older
    #define S60_3_2_WK14_OR_NEWER
#endif    

// All necessary XML DOM parser headers are included here
#ifdef S60_3_2_WK14_OR_NEWER
// Filenames in 3.2 from week 14 onwards
    #include <xml/dom/xmlengdom.h>
    #include <xml/dom/xmlengdomimplementation.h>
    #include <xml/dom/xmlengdocument.h>
    #include <xml/dom/xmlengserializationoptions.h>
    #include <xml/utils/xmlengxestrings.h>
    #include <xml/utils/xmlengutils.h>
#else

// Filenames in 3.1
    #include <xmlengddom.h>
    #include <xmlengddomimplementation.h>
    #include <xmlengddocument.h>
    #include <xmlengdserializationoptions.h>
    #include <xmlenguxestrings.h> //Added because week 2 SDK 3.2 XmlEngDDOM.h -> xmlengdnodefilter.h -> xmlenguxestd.h had removed it.
    #include <xmlenguutils.h>
#endif


#endif // NCD_XML_HEADERS_H