/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_CATALOGSHTTPSESSIONMANAGER_H
#define M_CATALOGSHTTPSESSIONMANAGER_H

#include <e32base.h>

class MCatalogsHttpOperation;
class MCatalogsHttpConfig;
class CCatalogsHttpConfig;
class MCatalogsHttpObserver;
class CCatalogsHttpConnectionCreator;
class CCatalogsHttpConnectionManager;

class MCatalogsHttpSessionManager
    {
    public:
    
        virtual void AddRef() = 0;
        virtual TInt Release() = 0;
        
        /**
        * Starts the operation if there are enough free connections. Otherwise
        * the operation is queued
        * @param aOperation The operation
        * @return 
        */
        virtual TInt StartOperation( MCatalogsHttpOperation* aOperation, TBool aConnect ) = 0;
        virtual TInt PauseOperation( MCatalogsHttpOperation* aOperation ) = 0;
        virtual TInt CompleteOperation( MCatalogsHttpOperation* aOperation ) = 0;
        
        /**
        * The operations priority has changed
        * @param aOperation The operation         
        * @param Symbian error code
        */
        virtual TInt OperationPriorityChanged( MCatalogsHttpOperation* aOperation ) = 0;
        

        /**
         * Connection creator getter
         * Connection creator is created if necessary
         */                          
        virtual CCatalogsHttpConnectionCreator& ConnectionCreatorL() = 0;
        
        /**
         * Sets resume mode
         * If resume mode is true, then an operation is resumed when some operation is canceled.
         * If resume mode is false, then nothing is resumed when some operation is canceled.
         * This is used when all operations are canceled at once
         */
        virtual void SetResumeMode( TBool aResumeQueued ) = 0;
        
        /**
         * Disconnects network connection if it is not used by any
         * client anymore
         * 
         * @return ETrue if the connection was up
         */
        virtual TBool DisconnectL() = 0; 
                
protected:

    virtual ~MCatalogsHttpSessionManager()
        {
        }
    };


#endif // M_CATALOGSHTTPSESSIONMANAGER_H
