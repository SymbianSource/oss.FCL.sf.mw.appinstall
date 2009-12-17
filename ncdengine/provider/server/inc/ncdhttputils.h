/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_NCDHTTPUTILS_H
#define C_NCDHTTPUTILS_H

#include <e32base.h>

#include "catalogshttpincludes.h"
#include "catalogsaccesspointmanager.h"

class CNcdNodeIdentifier;
class TNcdConnectionMethod;

/**
 * Utils for creating HTTP transactions
 */
class CNcdHttpUtils : public CBase
    {
public:

    CNcdHttpUtils(         
        MCatalogsAccessPointManager& aAccessPointManager );
        

    void CreateTransactionL( 
        MCatalogsHttpSession& aSession,
        MCatalogsHttpOperation*& aOperation,
        const TDesC& aUri,
        MCatalogsHttpObserver& aObserver,
        const TDesC8& aBody );

    void CreateTransactionL( 
        MCatalogsHttpSession& aSession,
        MCatalogsHttpOperation*& aOperation,
        const TDesC& aUri,
        MCatalogsHttpObserver& aObserver,
        const TDesC8& aBody,
        const TDesC& aNamespace,
        const MCatalogsAccessPointManager::TAction& aAction, 
        const TUid& aClientUid );

    void CreateTransactionL( 
        MCatalogsHttpSession& aSession,
        MCatalogsHttpOperation*& aOperation,
        const TDesC& aUri,
        MCatalogsHttpObserver& aObserver,
        const TDesC8& aBody,
        const TDesC& aNamespace,
        const TDesC& aCatalogId,
        const MCatalogsAccessPointManager::TAction& aAction, 
        const TUid& aClientUid );


    void CreateTransactionL( 
        MCatalogsHttpSession& aSession,
        MCatalogsHttpOperation*& aOperation,
        const TDesC& aUri,
        MCatalogsHttpObserver& aObserver,
        const TDesC8& aBody,
        const CNcdNodeIdentifier& aNodeIdentifier,
        const MCatalogsAccessPointManager::TAction& aAction, 
        const TUid& aClientUid );


    void CreateTransactionL( 
        MCatalogsHttpSession& aSession,
        MCatalogsHttpOperation*& aOperation,
        const TDesC& aUri,
        MCatalogsHttpObserver& aObserver,
        const TDesC8& aBody,
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdNodeIdentifier& aBackupNodeIdentifier,
        const MCatalogsAccessPointManager::TAction& aAction, 
        const TUid& aClientUid );


    void ConvertConnectionMethod(
        const TNcdConnectionMethod& aSource,
        TCatalogsConnectionMethod& aTarget ) const;

private:

    void SetAccessPoint(
        MCatalogsHttpOperation& aOperation,
        TInt aError,
        TUint32 aAp );
 
        
private:

    MCatalogsAccessPointManager& iAccessPointManager;    
            
    };

#endif // C_NCDTRANSACTIONUTILS_H

