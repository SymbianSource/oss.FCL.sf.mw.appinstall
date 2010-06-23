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


#ifndef M_CATALOGSHTTPTRANSACTIONMANAGER_H
#define M_CATALOGSHTTPTRANSACTIONMANAGER_H

#include "catalogshttpmanager.h"

class MCatalogsHttpOperation;
class CCatalogsHttpStack;
class MCatalogsHttpStackObserver;

class MCatalogsHttpTransactionManager : public MCatalogsHttpManager
    {
    public:
        

        /**
        * Session ID getter
        *
        * @return Session id
        */
        virtual TInt32 SessionId() const = 0;
    
        /**
        * Removes the transaction from the manager 
        * @param aTransaction Transaction to remove from the manager
        */
        virtual void RemoveOperation( MCatalogsHttpOperation* 
            aOperation ) = 0;
    
    
        /**
         * Gets an usable HTTP stack
         */    
        virtual CCatalogsHttpStack* HttpStackL( 
            MCatalogsHttpStackObserver& aObserver ) = 0;
        
        /**
         * Releases an HTTP stack for re-use
         */
        virtual void ReleaseHttpStackL( CCatalogsHttpStack* aStack ) = 0;
                
    };


#endif // M_CATALOGSHTTPTRANSACTIONMANAGER_H