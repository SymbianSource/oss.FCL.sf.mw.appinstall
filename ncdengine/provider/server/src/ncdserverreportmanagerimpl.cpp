/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdServerReportManager class
*
*/


#include <e32err.h>
#include <e32base.h>
#include <s32mem.h>

#include "ncdserverreportmanagerimpl.h"
#include "ncdreportmanager.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeimpl.h"
#include "ncdnodemetadataimpl.h"
#include "ncdproviderimpl.h"
#include "ncdpurchasehistorydbimpl.h"
#include "ncdpurchasehistoryutils.h"
#include "ncdutils.h"
#include "ncdnodefunctionids.h"
#include "catalogssession.h"
#include "catalogsbasemessage.h"
#include "catalogscontext.h"
#include "catalogsaccesspointmanager.h"
#include "catalogshttpsession.h"
#include "catalogsdebug.h"


CNcdServerReportManager* CNcdServerReportManager::NewL( CNcdProvider& aProvider )
    {
    CNcdServerReportManager* self = 
        CNcdServerReportManager::NewLC( aProvider );
    CleanupStack::Pop( self );
    return self;        
    }

CNcdServerReportManager* CNcdServerReportManager::NewLC( CNcdProvider& aProvider )
    {
    CNcdServerReportManager* self = 
        new( ELeave ) CNcdServerReportManager( aProvider);
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;        
    }
    
    
CNcdServerReportManager::CNcdServerReportManager( CNcdProvider& aProvider )
: CCatalogsCommunicable(),
  iProvider( aProvider )
    {
    }


void CNcdServerReportManager::ConstructL()
    {
    DLTRACEIN((""));
    }


CNcdServerReportManager::~CNcdServerReportManager()
    {
    DLTRACEIN((""));
    }


void CNcdServerReportManager::ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                              TInt aFunctionNumber )
    {
    DLTRACEIN(("handle: %d, function: %d", 
               aMessage->Handle(), 
               aFunctionNumber));

    DASSERT( aMessage );
    
    // Now, we can be sure that rest of the time iMessage exists.
    // This member variable is set for the CounterPartLost function.
    iMessage = aMessage;
        
    TInt trapError( KErrNone );
    
    switch( aFunctionNumber )
        {
        case NcdNodeFunctionIds::ENcdServerReportManagerSetReportingMethod:
            TRAP( trapError, SetReportingMethodRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdServerReportManagerReportingMethod:
            TRAP( trapError, ReportingMethodRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdServerReportManagerSetReportingStyle:
            TRAP( trapError, SetReportingStyleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdServerReportManagerReportingStyle:
            TRAP( trapError, ReportingStyleRequestL( *aMessage ) );
            break;

        case NcdNodeFunctionIds::ENcdServerReportManagerNodeSetAsInstalled:
            TRAP( trapError, NodeSetAsInstalledRequestL( *aMessage ) );
            break;        

        case NcdNodeFunctionIds::ENcdRelease:
            ReleaseRequest( *aMessage );
            break;

        default:
            DLERROR(("Unidentified function request"));
            DASSERT( EFalse );
            break;
        }

    if ( trapError != KErrNone )
        {
        // Because something went wrong the complete has not been
        // yet called for the message.
        // So, inform the client about the error.
        DLTRACEIN(("Complete with error message"));
        aMessage->CompleteAndRelease( trapError );
        }

    // Because the message should not be used after this, set it NULL.
    // So, CounterPartLost function will know that no messages are
    // waiting the response at the moment.
    iMessage = NULL;
            
    DLTRACEOUT((""));
    }


void CNcdServerReportManager::CounterPartLost( const MCatalogsSession& aSession )
    {
    // This function may be called whenever -- when the message is waiting
    // response or when the message does not exist.
    // iMessage may be NULL here, because in the end of the
    // ReceiveMessage it is set to NULL. The life time of the message
    // ends shortly after CompleteAndRelease is called.
    if ( iMessage != NULL )
        {
        iMessage->CounterPartLost( aSession );
        }    
    }


CNcdProvider& CNcdServerReportManager::Provider()
    {
    return iProvider;
    }


CNcdReportManager& CNcdServerReportManager::ReportManagerL( MCatalogsBaseMessage& aMessage )
    {
    // Get current context
    MCatalogsContext& context( aMessage.Session().Context() );

    // All the clients have their own report manager.
    TNcdProviderContext providerContext;
    Provider().GetProviderContextL( context, providerContext );
    CNcdReportManager& reportManager( *providerContext.iReportManager );
    
    return reportManager;
    }


void CNcdServerReportManager::SetReportingMethodRequestL( MCatalogsBaseMessage& aMessage )
    {
    HBufC8* input = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 inputPtr = input->Des();
    aMessage.ReadInput( inputPtr );
    RDesReadStream inputStream( *input );
    CleanupClosePushL( inputStream );

    MNcdServerReportManager::TReportingMethod method(
        static_cast<MNcdServerReportManager::TReportingMethod>( inputStream.ReadInt32L() ) );

    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( input );

    ReportManagerL( aMessage ).SetReportingMethod( method );

    // If this leaves, ReceiveMessge will complete the message.
    aMessage.CompleteAndRelease( KErrNone );
    }


void CNcdServerReportManager::ReportingMethodRequestL( MCatalogsBaseMessage& aMessage )
    {
    TInt method( 
        ReportManagerL( aMessage ).ReportingMethod() );
    aMessage.CompleteAndReleaseL( method, KErrNone );    
    }


void CNcdServerReportManager::SetReportingStyleRequestL( MCatalogsBaseMessage& aMessage )
    {
    HBufC8* input = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 inputPtr = input->Des();
    aMessage.ReadInput( inputPtr );
    RDesReadStream inputStream( *input );
    CleanupClosePushL( inputStream );

    MNcdServerReportManager::TReportingStyle style(
        static_cast<MNcdServerReportManager::TReportingStyle>( inputStream.ReadInt32L() ) );

    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( input );

    ReportManagerL( aMessage ).SetReportingStyle( style );

    // If this leaves, ReceiveMessge will complete the message.
    aMessage.CompleteAndRelease( KErrNone );
    }


void CNcdServerReportManager::ReportingStyleRequestL( MCatalogsBaseMessage& aMessage )
    {
    TInt style( 
        ReportManagerL( aMessage ).ReportingStyle() );
    aMessage.CompleteAndReleaseL( style, KErrNone );    
    }


void CNcdServerReportManager::NodeSetAsInstalledRequestL( MCatalogsBaseMessage& aMessage )
    {
    HBufC8* input = HBufC8::NewLC( aMessage.InputLength() );
    TPtr8 inputPtr = input->Des();
    aMessage.ReadInput( inputPtr );
    RDesReadStream inputStream( *input );
    CleanupClosePushL( inputStream );

    TInt errorCode( inputStream.ReadInt32L() );
    CNcdNodeIdentifier* identifier( CNcdNodeIdentifier::NewLC( inputStream ) );

    CNcdReportManager& reportManager( ReportManagerL( aMessage ) );
    CNcdNode& node( Provider().NodeManager().NodeL( *identifier ) ); 
    CNcdNodeMetaData& metaData( node.NodeMetaDataL() );
    
    TNcdReportStatusInfo info( ENcdReportCreate, errorCode );
    // Use the node identifier to identify the content in install report.
    // Node id uniquely identifies the node that contains contents
    // that will be installed. One node may contains multiple contents but
    // they are all thought as one bundle, in one operation. Also, notice that 
    // multiple nodes can contain same metadata and same content.
    TNcdReportId reportId = 
        reportManager.RegisterInstallL( 
            identifier->NodeId(),
            metaData.Identifier(),
            info,
            metaData.Identifier().ServerUri(),
            metaData.Identifier().NodeNameSpace() );

    // Set access point for report.
    UpdateInstallReportAccessPointL( aMessage.Session().Context().FamilyId(),
                                     reportId,
                                     node,
                                     metaData,
                                     reportManager,
                                     HttpSessionL( aMessage.Session().Context() ) );    

    // Set the final success information directly into the report instead of
    // reporting other install statuses here.
    TNcdReportStatus status( ENcdReportSuccess );
    if ( errorCode == KErrNone )
        {
        status = ENcdReportSuccess;
        }
    else if ( errorCode == KErrCancel )
        {
        status = ENcdReportCancel;
        }
    else 
        {
        status = ENcdReportFail;
        }
    
    // Create the status info object with the given info.
    info.iStatus = status;
    info.iErrorCode = errorCode;
    
    reportManager.ReportInstallStatusL(
            reportId,
            info );
    
    CleanupStack::PopAndDestroy( identifier );
    CleanupStack::PopAndDestroy( &inputStream );
    CleanupStack::PopAndDestroy( input );

    aMessage.CompleteAndRelease( KErrNone );    
    }


void CNcdServerReportManager::ReleaseRequest( MCatalogsBaseMessage& aMessage ) const
    {
    DLTRACEIN((""));

    // Decrease the reference count for this object.
    // When the reference count reaches zero, this object will be destroyed
    // and removed from the session.
    MCatalogsSession& requestSession( aMessage.Session() );
    TInt handle( aMessage.Handle() );
    aMessage.CompleteAndRelease( KErrNone );
    requestSession.RemoveObject( handle );

    DLTRACEOUT((""));
    }


void CNcdServerReportManager::UpdateInstallReportAccessPointL( const TUid& aClientUid,
                                                               TInt aReportId,
                                                               CNcdNode& aNode,
                                                               CNcdNodeMetaData& aMetaData,
                                                               CNcdReportManager& aReportManager,
                                                               MCatalogsHttpSession& aHttpSession )
    {
    DLTRACEIN((""));

    CNcdPurchaseHistoryDb& db = Provider().NodeManager().PurchaseHistory();    
    
    CNcdPurchaseDetails* purchase = 
        NcdPurchaseHistoryUtils::PurchaseDetailsLC(
            db,
            aClientUid,
            aMetaData.Identifier(),
            EFalse );
                
    // Create origin identifier
    CNcdNodeIdentifier* originIdentifier = 
        CNcdNodeIdentifier::NewL(
            aNode.Identifier().NodeNameSpace(), 
            purchase->OriginNodeId(), 
            aNode.Identifier().ClientUid() );

    CleanupStack::PopAndDestroy( purchase );
    
    CleanupStack::PushL( originIdentifier );
    
    // Get report ap    
    TUint32 apId( 0 );

    TInt error = 
        Provider().AccessPointManager().AccessPointIdL(
            *originIdentifier, 
            MCatalogsAccessPointManager::EBrowse, 
            aClientUid, 
            apId );
        
    TCatalogsConnectionMethod reportAp;
    if ( error == KErrNone ) 
        {
        DLTRACE(( "Setting access point %d for reports", apId ))   
        reportAp = 
            TCatalogsConnectionMethod( 
                apId, 
                ECatalogsConnectionMethodTypeAccessPoint );
        }
    
    if ( reportAp.iId == 0 ) 
        {
        reportAp = aHttpSession.ConnectionManager().DefaultConnectionMethod();
        }

    CleanupStack::PopAndDestroy( originIdentifier );

    aReportManager.SetInstallReportAccessPoint( 
        aReportId,
        reportAp );
    }


MCatalogsHttpSession& CNcdServerReportManager::HttpSessionL( MCatalogsContext& aContext )
    {
    TNcdProviderContext providerContext;
    Provider().GetProviderContextL( aContext, providerContext );
    return *providerContext.iHttpSession;
    }

