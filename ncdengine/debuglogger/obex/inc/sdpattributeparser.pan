/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BTPointToPoint application panic codes
*
*/


#ifndef __SDP_ATTRIBUTE_PARSER_PAN__
#define __SDP_ATTRIBUTE_PARSER_PAN__

/** BTPointToPoint application panic codes */
enum TSdpAttributeParserPanics 
    {
    ESdpAttributeParserInvalidCommand = 1,
    ESdpAttributeParserNoValue,
    ESdpAttributeParserValueIsList,
    ESdpAttributeParserValueTypeUnsupported
    };


/** Panic Category */
_LIT(KPanicSAP, "SAP");


inline void Panic(TSdpAttributeParserPanics aReason)
    { 
    User::Panic(KPanicSAP, aReason);
    }


#endif // __SDP_ATTRIBUTE_PARSER_PAN__
