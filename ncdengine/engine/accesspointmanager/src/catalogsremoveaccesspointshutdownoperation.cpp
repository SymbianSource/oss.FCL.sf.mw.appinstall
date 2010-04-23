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
* Description:   Class CCatalogsRemoveAccesspointShutdownOperation implementation
*
*/


#include "catalogsremoveaccesspointshutdownoperation.h"

#include "catalogsconnectionmethod.h"
#include "catalogsaccesspointmanagerimpl.h"
#include "catalogshttpsessionmanagerimpl.h"
#include "catalogsnetworkmanager.h"

// one minute
const TTimeIntervalMicroSeconds32 KTimerDelay( 60 * 1000000 );


TInt CCatalogsRemoveAccesspointShutdownOperation::CallbackRemoveAccessPoint( 
    TAny* aData )
    {
    static_cast< CCatalogsRemoveAccesspointShutdownOperation* >( 
        aData )->RemoveAccessPoint();
    return KErrNone;
    }

CCatalogsRemoveAccesspointShutdownOperation* 
    CCatalogsRemoveAccesspointShutdownOperation::NewL( 
        const TUid& aFamilyUid,
        const TUint32 aApnId )
    {
    CCatalogsRemoveAccesspointShutdownOperation* self = new( ELeave ) 
        CCatalogsRemoveAccesspointShutdownOperation( aFamilyUid, aApnId );
    return self;
    }


CCatalogsRemoveAccesspointShutdownOperation::CCatalogsRemoveAccesspointShutdownOperation(
    const TUid& aFamilyUid,
    const TUint32 aApnId ) 
    : CCatalogsShutdownOperation( aFamilyUid ),
      iApnId( aApnId )
    {    
    }

    
CCatalogsRemoveAccesspointShutdownOperation::~CCatalogsRemoveAccesspointShutdownOperation()
    {
    Cancel();
    }


void CCatalogsRemoveAccesspointShutdownOperation::HandleAccessPointEventL( 
    const TCatalogsConnectionMethod& aAp,
    const TCatalogsAccessPointEvent& aEvent )
    {
    DLTRACEIN((""));
    if ( aEvent == ECatalogsAccessPointReallyClosed &&
         aAp.iApnId == iApnId )
        {
        DLTRACE(("AP closed, delete it"));
        RemoveAccessPoint();
        }
    }


void CCatalogsRemoveAccesspointShutdownOperation::DoExecuteL()
    {
    DLTRACEIN((""));
    iNetworkManager = &CCatalogsHttpSessionManager::NetworkManagerL();
    iNetworkManager->AddObserverL( *this );
    
    iNetworkManager->UpdateConnectionsL();    
    
    TCatalogsConnectionMethod method( 
        iApnId, 
        ECatalogsConnectionMethodTypeAccessPoint );
    method.iId = iApnId;
    
    TBool apIsOpen = iNetworkManager->IsAccessPointOpen( method );
    if ( !apIsOpen ) 
        {
        // Try to delete the accesspoint again
        RemoveAccessPoint();
        }
    // this is the ultimate failsafe that ensures that we actually stop the 
    // engine at some point even if we don't get any connection closed events.
    else
        {
        iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        iTimer->Start( 
            KTimerDelay, 
            KTimerDelay, 
            TCallBack( &CallbackRemoveAccessPoint, this ) );
        }
    }


void CCatalogsRemoveAccesspointShutdownOperation::DoCancel()
    {
    delete iTimer;
    iTimer = NULL;

    if ( iNetworkManager )
        {
        iNetworkManager->RemoveObserver( *this );
        iNetworkManager = NULL;
        }
    }


void CCatalogsRemoveAccesspointShutdownOperation::RemoveAccessPoint()
    {
    DLTRACEIN((""));
    Cancel();
    
    // If this fails then it fails. We've tried hard enough
    TRAP_IGNORE( 
        CCatalogsAccessPointManager::RemoveApFromCommsDatabaseL( iApnId ) );
    
    // Notify owner which will delete this object
    NotifyObserver( KErrNone );
    }



