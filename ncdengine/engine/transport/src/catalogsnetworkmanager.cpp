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


#include "catalogsnetworkmanager.h"
#include "catalogshttputils.h"

#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CCatalogsNetworkManager* CCatalogsNetworkManager::NewL()
    {
    CCatalogsNetworkManager* self = new (ELeave) CCatalogsNetworkManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CCatalogsNetworkManager::~CCatalogsNetworkManager()
    {
    DLTRACEIN((""));
    
    iObservers.Close();
    iConnection.Close();
    iSs.Close();
    }
     
       
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::UpdateConnectionsL()
    {
    DLTRACEIN((""));    
    
    User::LeaveIfError( iConnection.EnumerateConnections( iConnectionCount ) );
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TBool CCatalogsNetworkManager::IsAccessPointOpen( 
    const TCatalogsConnectionMethod& aAp )
    {
    DLTRACEIN((""));
    if ( iConnectionCount == 0 )
        {
        DLTRACEOUT(("No connections"));
        return EFalse;
        }
    else
        {               
        TConnectionInfoBuf connectionInfo;
        return CatalogsHttpUtils::IsConnectionMethodOpen( 
            iConnection,
            iConnectionCount,
            aAp,
            connectionInfo );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::OpenAccessPointL( 
    const TCatalogsConnectionMethod& aAp )
    {
    DLTRACEIN((""));
    NotifyAccessPointObserversL( aAp, ECatalogsAccessPointOpened );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::CloseAccessPointL( 
    const TCatalogsConnectionMethod& aAp )
    {
    DLTRACEIN((""));
    NotifyAccessPointObserversL( aAp, ECatalogsAccessPointClosed );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::NotifyAccessPointReallyClosedL( 
    const TCatalogsConnectionMethod& aAp )
    {
    DLTRACEIN((""));
    NotifyAccessPointObserversL( aAp, ECatalogsAccessPointReallyClosed );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::AddObserverL( 
    MCatalogsAccessPointObserver& aObserver )
    {
    DLTRACEIN(("observer: %x", &aObserver ));
    iObservers.InsertInAddressOrderL( &aObserver );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::RemoveObserver( 
    MCatalogsAccessPointObserver& aObserver )
    {
    DLTRACEIN(("observer: %x", &aObserver ));
    TInt index = iObservers.FindInAddressOrder( &aObserver );
    if ( index != KErrNotFound ) 
        {
        DLTRACE(("Observer found, removing"));
        iObservers.Remove( index );
        }
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::NotifyAccessPointObserversL( 
    const TCatalogsConnectionMethod& aAp,
    const TCatalogsAccessPointEvent& aEvent )
    {
    DLTRACEIN((""));
    TInt i = iObservers.Count();
    while ( i-- ) 
        {
        iObservers[i]->HandleAccessPointEventL( aAp, aEvent );
        }
    DLTRACEOUT(("Observers handled"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CCatalogsNetworkManager::ConstructL()
    {
    DLTRACEIN((""));
    User::LeaveIfError( iSs.Connect() );
    User::LeaveIfError( iConnection.Open( iSs ) );
    }
