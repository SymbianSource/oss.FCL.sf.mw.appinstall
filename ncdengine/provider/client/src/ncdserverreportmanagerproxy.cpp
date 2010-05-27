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
* Description:   Contains MNcdNode interface
*
*/


#include "ncdserverreportmanagerproxy.h"

#include <s32mem.h>

#include "catalogsinterfaceidentifier.h"
#include "catalogsclientserver.h"
#include "catalogsconstants.h"
#include "ncdproviderproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodeidentifier.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdserverreportoperationproxy.h"
#include "catalogsdebug.h"


CNcdServerReportManagerProxy* CNcdServerReportManagerProxy::NewL( MCatalogsClientServer& aSession, 
                                                                 TInt aHandle,
                                                                 CNcdProviderProxy& aProvider )
    {
    CNcdServerReportManagerProxy* self = 
        CNcdServerReportManagerProxy::NewLC( aSession, aHandle, aProvider );
    CleanupStack::Pop( self );
    return self;    
    }


CNcdServerReportManagerProxy* CNcdServerReportManagerProxy::NewLC( MCatalogsClientServer& aSession, 
                                                                  TInt aHandle,
                                                                  CNcdProviderProxy& aProvider )
    {
    CNcdServerReportManagerProxy* self = 
        new( ELeave ) CNcdServerReportManagerProxy( aSession, aHandle, aProvider );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }
    

CNcdServerReportManagerProxy::CNcdServerReportManagerProxy( MCatalogsClientServer& aSession, 
                                                            TInt aHandle,
                                                            CNcdProviderProxy& aProvider )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aProvider ),
  iProvider( aProvider )
    {
    
    }


void CNcdServerReportManagerProxy::ConstructL()
    {
    DLTRACEIN((""));

    // Register the interface
    MNcdServerReportManager* mgr( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( mgr, 
                                            this, 
                                            MNcdServerReportManager::KInterfaceUid ) );
    }


CNcdServerReportManagerProxy::~CNcdServerReportManagerProxy()
    {
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdServerReportManager::KInterfaceUid );
    }


void CNcdServerReportManagerProxy::NodeSetAsInstalledL( CNcdNodeProxy& aNode, TInt aErrorCode )
    {
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    stream.WriteInt32L( aErrorCode );
    aNode.NodeIdentifier().ExternalizeL( stream );

    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    TInt tmp( 0 );
    TInt error =
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdServerReportManagerNodeSetAsInstalled,
                  ptr,
                  tmp,
                  Handle() );
    User::LeaveIfError( error ); 

    CleanupStack::PopAndDestroy( buf );
    }    


void CNcdServerReportManagerProxy::SetReportingMethodL( const MNcdServerReportManager::TReportingMethod& aMethod )
    {
    DLTRACEIN((""));

    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    stream.WriteInt32L( aMethod );

    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    TInt tmp( 0 );
    TInt error =
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdServerReportManagerSetReportingMethod,
                  ptr,
                  tmp,
                  Handle() );
    User::LeaveIfError( error ); 

    CleanupStack::PopAndDestroy( buf );
    }


MNcdServerReportManager::TReportingMethod CNcdServerReportManagerProxy::ReportingMethodL() const
    {
    DLTRACEIN((""));

    TInt method( MNcdServerReportManager::EReportingBackground );
    
    TInt error =
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdServerReportManagerReportingMethod,
                  KNullDesC,
                  method,
                  Handle() );
    User::LeaveIfError( error );
    
    MNcdServerReportManager::TReportingMethod retMethod( 
        static_cast<MNcdServerReportManager::TReportingMethod>( method ) );
    
    return retMethod;
    }


void CNcdServerReportManagerProxy::SetReportingStyleL( const MNcdServerReportManager::TReportingStyle& aStyle )
    {
    DLTRACEIN((""));

    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    stream.WriteInt32L( aStyle );

    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    TInt tmp( 0 );
    TInt error =
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdServerReportManagerSetReportingStyle,
                  ptr,
                  tmp,
                  Handle() );
    User::LeaveIfError( error ); 

    CleanupStack::PopAndDestroy( buf );
    }


MNcdServerReportManager::TReportingStyle CNcdServerReportManagerProxy::ReportingStyleL() const
    {
    DLTRACEIN((""));

    TInt style( MNcdServerReportManager::EReportingStyleGeneral );
    
    TInt error =
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdServerReportManagerReportingStyle,
                  KNullDesC,
                  style,
                  Handle() );
    User::LeaveIfError( error );
    
    MNcdServerReportManager::TReportingStyle retStyle( 
        static_cast<MNcdServerReportManager::TReportingStyle>( style ) );
    
    return retStyle;
    }


MNcdServerReportOperation* CNcdServerReportManagerProxy::SendL( MNcdServerReportOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    // Create operation
    
    // This class does not need to observe the operation itself.
    // It is up to the called to observe the progress. 
    // So, just set the given observer directly for the operation.
    CNcdServerReportOperationProxy* op( NULL );
    
    if ( ReportingMethodL() == MNcdServerReportManager::EReportingManaged )
        {
        DLINFO(("Reporting method managed, create operation."));
        op = Provider().OperationManager().CreateServerReportOperationL( aObserver );       
        }
        
    DLTRACEOUT((""));    

    return op;
    }


CNcdProviderProxy& CNcdServerReportManagerProxy::Provider()
    {
    return iProvider;
    }
    
