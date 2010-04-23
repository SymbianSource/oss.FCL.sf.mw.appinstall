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


#ifndef M_CATALOGSHTTPMANAGER_H
#define M_CATALOGSHTTPMANAGER_H

#include <e32base.h>

class MCatalogsHttpOperation;
class MCatalogsHttpConfig;
class CCatalogsHttpConfig;
class MCatalogsHttpObserver;
class CCatalogsHttpConnectionCreator;
class CCatalogsHttpConnectionManager;

class MCatalogsHttpManager
    {
    public:
    
        /** 
         * Add reference
         */
        virtual void AddRef() = 0;
        
        /**
         * Release reference
         * Deletes the object when reference count reaches 0
         * @return Remaining reference count
         */
        virtual TInt Release() = 0;
        
        /**
        * Starts the operation if there are enough free connections. Otherwise
        * the operation is queued
        * @param aOperation The operation
        * @return 
        */
        virtual TInt StartOperation( MCatalogsHttpOperation* aOperation ) = 0;
        
        /**
         * Completes the operation by removing it from any queues it is in
         * and may start some queued operations
         */
        virtual TInt CompleteOperation( MCatalogsHttpOperation* aOperation ) = 0;
        
        /**
         * Reports changes in connection activity
         */
        virtual void ReportConnectionStatus( TBool aActive ) = 0;
        
        
        /**
        * The operations priority has changed
        * @param aOperation The operation         
        * @param Symbian error code
        */
        virtual TInt OperationPriorityChanged( MCatalogsHttpOperation* aOperation ) = 0;
        
        /**
        * Default configuration getter
        *
        * @return Default configuration
        */
        virtual MCatalogsHttpConfig& DefaultConfig() const = 0;


        /**
         * Creates a transaction for getting HTTP headers
         */      
        virtual MCatalogsHttpOperation* CreateDlTransactionL( 
            const TDesC8& aUrl,
            MCatalogsHttpObserver& aObserver, 
            const CCatalogsHttpConfig& aConfig ) = 0;
              
        
        /**
         * Connection creator getter
         * Connection creator is created if necessary
         */      
        virtual CCatalogsHttpConnectionCreator& ConnectionCreatorL() = 0;
        
        /**
         * Connection manager getter
         * Connection manager is created if necessary
         */         
        virtual CCatalogsHttpConnectionManager& ConnectionManager() = 0;
        
protected:

    virtual ~MCatalogsHttpManager()
        {
        }
    };


#endif // M_CATALOGSHTTPSESSIONMANAGER_H