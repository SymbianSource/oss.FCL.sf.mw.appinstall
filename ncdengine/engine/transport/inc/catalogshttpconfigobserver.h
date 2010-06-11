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


#ifndef M_CATALOGSHTTPCONFIGOBSERVER_H
#define M_CATALOGSHTTPCONFIGOBSERVER_H

#include "catalogshttptypes.h"

class MCatalogsHttpOperation;   // HTTP operation


/**
* Interface for HTTP configuration observers
*/
class MCatalogsHttpConfigObserver
    {
    public:

        /**
        * Handles events from HTTP configurations
	    * @param aConfig Configuration that sent the event
	    * @param aEvent The event
	    */
        virtual TInt HandleHttpConfigEvent( 
            MCatalogsHttpConfig* aConfig, 
            const TCatalogsHttpConfigEvent& aEvent ) = 0;
    };


#endif // M_CATALOGSHTTPCONFIGOBSERVER_H
