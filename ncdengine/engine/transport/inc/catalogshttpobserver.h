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


#ifndef M_CATALOGSHTTPOBSERVER_H
#define M_CATALOGSHTTPOBSERVER_H

#include "catalogshttptypes.h"

class MCatalogsHttpOperation;   // HTTP operation


/**
* Interface for HTTP operation observers
*/
class MCatalogsHttpObserver
    {
    public:

        /**
         * Handles events from HTTP operations
	     * @param aOperation Operation that sent the event
	     * @param aEvent The event
	     */
        virtual void HandleHttpEventL( 
            MCatalogsHttpOperation& aOperation, 
            TCatalogsHttpEvent aEvent ) = 0;
            
        /**
         * Handles HTTP errors
         *
         * @param aOperation Operation that sent the event
         * @param aError Error information
         * @return ETrue if aOperation was released/cancelled in the handler
         * @note HandleHttpEventL will not be called in error situations
         */
        virtual TBool HandleHttpError(
            MCatalogsHttpOperation& aOperation,
            TCatalogsHttpError aError ) = 0;
    };


#endif // M_CATALOGSHTTPOBSERVER_H
