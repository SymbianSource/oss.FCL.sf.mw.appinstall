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


#include "ncdhttputils.h"
#include "ncdconnectionmethod.h"

#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CNcdHttpUtils::CNcdHttpUtils(         
    MCatalogsAccessPointManager& aAccessPointManager ) : 
    iAccessPointManager( aAccessPointManager )
    {    
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::CreateTransactionL( 
    MCatalogsHttpSession& aSession,
    MCatalogsHttpOperation*& aOperation,
    const TDesC& aUri,
    MCatalogsHttpObserver& aObserver,
    const TDesC8& aBody )
    {
    DLTRACEIN((""));
    if ( aOperation ) 
        {
        aOperation->Release();
        aOperation = NULL;
        }
    
    aOperation = aSession.CreateTransactionL( 
        aUri,
        &aObserver );
                
    aOperation->SetBodyL( aBody );    
    
    // Try to get and set fixed AP
    TUint32 ap = 0;
    iAccessPointManager.GetFixedApL( ap );
    if ( ap ) 
        {
        SetAccessPoint( *aOperation, KErrNone, ap );
        }
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::CreateTransactionL( 
    MCatalogsHttpSession& aSession,
    MCatalogsHttpOperation*& aOperation,
    const TDesC& aUri,
    MCatalogsHttpObserver& aObserver,
    const TDesC8& aBody,
    const TDesC& aNamespace,
    const MCatalogsAccessPointManager::TAction& aAction, 
    const TUid& aClientUid )
    {
    DLTRACEIN((""));
    CreateTransactionL( aSession, aOperation, aUri, aObserver, aBody );
    
    TUint32 ap = 0;
    TInt error = iAccessPointManager.AccessPointIdL(
        aNamespace, 
        aAction, 
        aClientUid, 
        ap );

    SetAccessPoint( *aOperation, error, ap );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::CreateTransactionL( 
    MCatalogsHttpSession& aSession,
    MCatalogsHttpOperation*& aOperation,
    const TDesC& aUri,
    MCatalogsHttpObserver& aObserver,
    const TDesC8& aBody,
    const TDesC& aNamespace,
    const TDesC& aCatalogId,
    const MCatalogsAccessPointManager::TAction& aAction, 
    const TUid& aClientUid )
    {
    DLTRACEIN((""));
    CreateTransactionL( aSession, aOperation, aUri, aObserver, aBody );
    
    TUint32 ap = 0;
    TInt error = iAccessPointManager.AccessPointIdL(
        aNamespace, 
        aCatalogId,
        aAction, 
        aClientUid, 
        ap );

    SetAccessPoint( *aOperation, error, ap );

    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::CreateTransactionL( 
    MCatalogsHttpSession& aSession,
    MCatalogsHttpOperation*& aOperation,
    const TDesC& aUri,
    MCatalogsHttpObserver& aObserver,
    const TDesC8& aBody,
    const CNcdNodeIdentifier& aNodeIdentifier,
    const MCatalogsAccessPointManager::TAction& aAction, 
    const TUid& aClientUid )
    {
    DLTRACEIN((""));
    CreateTransactionL( aSession, aOperation, aUri, aObserver, aBody );
    
    TUint32 ap = 0;
    TInt error = iAccessPointManager.AccessPointIdL(
        aNodeIdentifier, 
        aAction, 
        aClientUid, 
        ap );

    SetAccessPoint( *aOperation, error, ap );
    }


// ---------------------------------------------------------------------------
// Set access point
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::CreateTransactionL( 
    MCatalogsHttpSession& aSession,
    MCatalogsHttpOperation*& aOperation,
    const TDesC& aUri,
    MCatalogsHttpObserver& aObserver,
    const TDesC8& aBody,
    const CNcdNodeIdentifier& aNodeIdentifier,
    const CNcdNodeIdentifier& aBackupNodeIdentifier,
    const MCatalogsAccessPointManager::TAction& aAction, 
    const TUid& aClientUid )
    {
    DLTRACEIN((""));
    CreateTransactionL( aSession, aOperation, aUri, aObserver, aBody );
    
    TUint32 ap = 0;
    TInt error = iAccessPointManager.AccessPointIdL(
        aNodeIdentifier, 
        aAction, 
        aClientUid, 
        ap );

    if ( error != KErrNone ) 
        {
        DLTRACE(("First node id failed, try backup"));
        ap = 0;
        error = iAccessPointManager.AccessPointIdL(
            aBackupNodeIdentifier, 
            aAction, 
            aClientUid, 
            ap );
        }
    SetAccessPoint( *aOperation, error, ap );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::ConvertConnectionMethod(
    const TNcdConnectionMethod& aSource,
    TCatalogsConnectionMethod& aTarget ) const
    {
    aTarget = TCatalogsConnectionMethod( 
        aSource.iId,
        static_cast<TCatalogsConnectionMethodType>( aSource.iType ) );
    }


// ---------------------------------------------------------------------------
// Set access point
// ---------------------------------------------------------------------------
//
void CNcdHttpUtils::SetAccessPoint(
    MCatalogsHttpOperation& aOperation,
    TInt aError,
    TUint32 aAp ) 
    {
    if ( aError == KErrNone ) 
        {
        DLTRACE(("Setting accesspoint id"));        
        TCatalogsConnectionMethod method( 
            aAp, 
            ECatalogsConnectionMethodTypeAccessPoint );
    
        aOperation.Config().SetConnectionMethod( method );
        }    
    }


