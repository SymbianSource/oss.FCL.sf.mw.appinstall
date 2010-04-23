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


#ifndef M_CATALOGSSMSOBSERVER_H
#define M_CATALOGSSMSOBSERVER_H

#include "catalogssmsoperation.h"
#include "catalogssmstypes.h"

/**
* Interface for SMS observers
*/
class MCatalogsSmsObserver
    {
    public:
	
	    /**
	    * Handles SMS events.
	    *
	    * @param aOperation The operation that caused the event
	    * @param aEvent The event
	    * @return Symbian error code
	    */ 	    
        virtual TInt HandleSmsEvent( 
            MCatalogsSmsOperation& aOperation, 
            TCatalogsSmsEvent aEvent ) = 0;
    };


#endif // M_CATALOGSSMSOBSERVER_H
