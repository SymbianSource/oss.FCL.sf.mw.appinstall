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
* Description:   Implements CNcdPurchaseHistoryProxy class
*
*/


#include <e32err.h>
#include <s32mem.h>

#include "catalogsinterfaceidentifier.h"
#include "catalogsclientserver.h"
#include "ncdpurchasehistoryproxy.h"
#include "ncdnodefunctionids.h"
#include "ncdpurchasedetails.h"
#include "ncdutils.h"
#include "ncdpurchasehistoryfilter.h"
#include "catalogsutils.h"
#include "catalogsconstants.h"
#include "catalogsdebug.h"


CNcdPurchaseHistoryProxy::CNcdPurchaseHistoryProxy(
    MCatalogsClientServer& aSession, 
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdOperationManagerProxy& aOperationManager )
        : CNcdInterfaceBaseProxy( aSession, aHandle, aParent ),
        iOperationManager( aOperationManager )
    {
    }

void CNcdPurchaseHistoryProxy::ConstructL()
    {
    MNcdPurchaseHistory* history( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL(
            history,
            this,
            MNcdPurchaseHistory::KInterfaceUid ) );
    }


CNcdPurchaseHistoryProxy* CNcdPurchaseHistoryProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdOperationManagerProxy& aOperationManager )
    {
    CNcdPurchaseHistoryProxy* self = 
        CNcdPurchaseHistoryProxy::NewLC(
            aSession,
            aHandle,
            aParent,
            aOperationManager );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdPurchaseHistoryProxy* CNcdPurchaseHistoryProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CCatalogsInterfaceBase* aParent,
    CNcdOperationManagerProxy& aOperationManager )
    {
    CNcdPurchaseHistoryProxy* self = 
        new( ELeave ) CNcdPurchaseHistoryProxy(
            aSession,
            aHandle,
            aParent,
            aOperationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;        
    }

CNcdPurchaseHistoryProxy::~CNcdPurchaseHistoryProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdPurchaseHistory::KInterfaceUid );
    }

void CNcdPurchaseHistoryProxy::SavePurchaseL(
    const MNcdPurchaseDetails& aDetails,
    TBool aSaveIcon )
    {
    DLTRACEIN((""));
    
    DLINFO(("Save purchase details to purchase history"));
    
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );        
    
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );
    
    static_cast<const CNcdPurchaseDetails*>(&aDetails)->ExternalizeL( stream );
    
    CleanupStack::PopAndDestroy( &stream );    
    
    TInt funcId( NcdNodeFunctionIds::ENcdPurchaseHistorySavePurchase );
    if ( !aSaveIcon )
        {
        funcId = 
            NcdNodeFunctionIds::ENcdPurchaseHistorySavePurchaseWithOldIcon;
        }
        
    HBufC8* dummyBuf = NULL;

    User::LeaveIfError( ClientServerSession().SendSyncAlloc(
        funcId,
        buf->Ptr( 0 ),
        dummyBuf,
        Handle(),
        0 ) );
    delete dummyBuf;

    CleanupStack::PopAndDestroy( buf );
    
    DLTRACEOUT((""));
    }

RArray<TUint> CNcdPurchaseHistoryProxy::PurchaseIdsL(
    const MNcdPurchaseHistoryFilter& aFilter )
    {
    DLTRACEIN((""));
    
    DLINFO(("Get purchase IDs from purchase history"));
    
    CBufBase* filterBuf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( filterBuf );
    
    RBufWriteStream stream( *filterBuf );
    CleanupClosePushL( stream );
    
    TArray<TUid> clientUids = aFilter.ClientUids();
    TUint count = clientUids.Count();
    stream.WriteUint32L( count );
    for ( TInt i = 0; i < count; i++ )
        {
        stream.WriteInt32L( clientUids[i].iUid );
        }
    ExternalizeDesL( aFilter.Namespace(), stream );
    ExternalizeDesL( aFilter.EntityId(), stream );
    stream.WriteUint32L( aFilter.EventId() );
    
    CleanupStack::PopAndDestroy( &stream );

    HBufC8* idBuf = NULL;
    User::LeaveIfError( ClientServerSession().SendSyncAlloc(
        NcdNodeFunctionIds::ENcdPurchaseHistoryGetPurchaseIds,
        filterBuf->Ptr( 0 ),
        idBuf,
        Handle(),
        0 ) );
        
    CleanupStack::PopAndDestroy( filterBuf );

    CleanupStack::PushL( idBuf );

    TInt size = sizeof(TUint);
    count = Des8ToUint( idBuf->Mid( 0, size ) );

    RArray<TUint> purchaseIds;    
    CleanupClosePushL( purchaseIds );
    
    for ( TInt i = 0; i < count; i++ )
        {
        purchaseIds.Append(
            Des8ToUint( idBuf->Mid( ( i + 1 ) * size, size ) ) );
        }

    CleanupStack::Pop( &purchaseIds );
    CleanupStack::PopAndDestroy( idBuf );
    
    DLTRACEOUT((""));

    return purchaseIds;
    }

CNcdPurchaseDetails* CNcdPurchaseHistoryProxy::PurchaseDetailsL(
    TUint aPurchaseId,
    TBool aLoadIcon )
    {
    DLTRACEIN(("Load with icon: %d", aLoadIcon));
    
    DLINFO(("Get purchase details from purchase history"));
    
    HBufC8* id = UintToDes8LC( aPurchaseId );

    HBufC8* detailsBuf = NULL;
    
    
    if ( aLoadIcon ) 
        {        
        User::LeaveIfError( ClientServerSession().SendSyncAlloc(
            NcdNodeFunctionIds::ENcdPurchaseHistoryGetPurchase,
            *id,
            detailsBuf,
            Handle(),
            0 ) );
        }
    else
        {
        User::LeaveIfError( ClientServerSession().SendSyncAlloc(
            NcdNodeFunctionIds::ENcdPurchaseHistoryGetPurchaseNoIcon,
            *id,
            detailsBuf,
            Handle(),
            0 ) );
        }
    CleanupStack::PopAndDestroy( id );
    CleanupStack::PushL( detailsBuf );

    CNcdPurchaseDetails* details = CNcdPurchaseDetails::NewLC();

    RDesReadStream stream( *detailsBuf );
    CleanupClosePushL( stream );
    
    details->InternalizeL( stream );
    
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( details );
    CleanupStack::PopAndDestroy( detailsBuf );
    
    DLTRACEOUT((""));
    
    return details;
    }

void CNcdPurchaseHistoryProxy::RemovePurchaseL( TUint aPurchaseId )
    {
    DLTRACEIN((""));
    
    DLINFO(("Remove purchase from purchase history"));
    
    HBufC8* id = UintToDes8LC( aPurchaseId );

    HBufC8* dummyBuf = NULL;
    User::LeaveIfError( ClientServerSession().SendSyncAlloc(
        NcdNodeFunctionIds::ENcdPurchaseHistoryRemovePurchase,
        *id,
        dummyBuf,
        Handle(),
        0 ) );
    delete dummyBuf;
    
    CleanupStack::PopAndDestroy( id );

    DLTRACEOUT((""));
    }

TUint CNcdPurchaseHistoryProxy::EventCountL()
    {
    DLTRACEIN((""));
    
    DLINFO(("Get event count from purchase history"));
    
    HBufC8* countBuf = NULL;
    User::LeaveIfError( ClientServerSession().SendSyncAlloc(
        NcdNodeFunctionIds::ENcdPurchaseHistoryEventCount,
        KNullDesC8,
        countBuf,
        Handle(),
        0 ) );
    CleanupStack::PushL( countBuf );

    TUint count = Des8ToUint( *countBuf );
    
    CleanupStack::PopAndDestroy( countBuf );
    
    DLTRACEOUT((""));
    
    return count;
    }
