/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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

	
#ifndef C_CATALOGSHTTPMESSAGECONSTANTS_H
#define C_CATALOGSHTTPMESSAGECONSTANTS_H

#include <e32base.h>

namespace CatalogsHttpMessageConstants
{

const TChar KCr = 0x0d;
const TChar KLf = 0x0a;
const TChar KHorizTab = 0x09;
_LIT8( KColon, ":");
    
_LIT8( KHttpVersion, "HTTP/1.1" );
_LIT8( KDefaultStatus, "OK" );
_LIT8( KSpace, " " );
_LIT8( KCrLf, "\r\n" );
    
}

#endif // C_CATALOGSHTTPMESSAGECONSTANTS_H