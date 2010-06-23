/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file declares panic functions for the server purposes.
*
*/


#ifndef SWINSTSERVERPANIC_H
#define SWINSTSERVERPANIC_H

//  INCLUDES
#include <e32base.h>
#include "SWInstCommon.h"

namespace SwiUI
{

//FORWARD DECLARATIONS


// FUNCTION PROTOTYPES

/**
* Panics the server.
* @since 3.0
* @param aPanic - Reason for the panic.
*/
GLDEF_C void PanicServer( TSWInstPanic aPanic );
          
/**
* Panics the client.
* @since 3.0
* @param aMessage
* @param aPanic - Reason for the panic.
*/
GLDEF_C void PanicClient( const RMessage2& aMessage, TSWInstPanic aPanic );  

}

#endif

// End of File
