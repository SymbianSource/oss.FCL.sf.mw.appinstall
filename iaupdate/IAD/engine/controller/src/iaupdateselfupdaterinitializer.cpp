/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/




// NCD headers:
// Purchase history contains items
#include <ncdpurchasehistory.h>
// Contains CBase-class headers of the ncd purchase classes.
#include <ncdutils.h>
#include <ncdprovider.h>

#include "iaupdateselfupdaterinitializer.h"
#include "iaupdatenodeimpl.h"
#include "iaupdateselfupdaterinitobserver.h"
#include "iaupdatecontrollerimpl.h"
#include "iaupdateselfupdaterctrl.h"
#include "iaupdaterfilelist.h"
#include "iaupdaterfileinfo.h"
#include "iaupdateridentifier.h"
#include "iaupdatedebug.h"


CIAUpdateSelfUpdaterInitializer* CIAUpdateSelfUpdaterInitializer::NewL(
    CIAUpdateNode& aNode,
    MIAUpdateSelfUpdaterInitObserver& aObserver )
    {
    CIAUpdateSelfUpdaterInitializer* self =
        CIAUpdateSelfUpdaterInitializer::NewLC( aNode, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateSelfUpdaterInitializer* CIAUpdateSelfUpdaterInitializer::NewLC(
    CIAUpdateNode& aNode,
    MIAUpdateSelfUpdaterInitObserver& aObserver )
    {
    CIAUpdateSelfUpdaterInitializer* self =
        new( ELeave ) CIAUpdateSelfUpdaterInitializer( aNode, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateSelfUpdaterInitializer::CIAUpdateSelfUpdaterInitializer( 
    CIAUpdateNode& aNode,
    MIAUpdateSelfUpdaterInitObserver& aObserver )
: CActive( CActive::EPriorityStandard ),
  iNode( aNode ),
  iObserver( aObserver ) 
    {

    }


void CIAUpdateSelfUpdaterInitializer::ConstructL()
    {
    User::LeaveIfError( FileServer().Connect() );
    CActiveScheduler::Add( this );
    }


CIAUpdateSelfUpdaterInitializer::~CIAUpdateSelfUpdaterInitializer()
    {
    Cancel();

    FileServer().Close();

    // Notice, that file list can be also NULL if
    // the ownership has been transferred to else where.
    delete iFileList;
    }
    

void CIAUpdateSelfUpdaterInitializer::StartL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::Start() begin");
    
    if ( !IsActive()
         && iState == ENotStarted )
        {
        // Start the active loop, which will
        // set the content files into the file list.
        iState = EInitialize;            
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* ptrStatus = &iStatus;
        User::RequestComplete( ptrStatus, KErrNone );        
        }
    else
        {
        User::Leave( KErrInUse );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::Start() end");
    }


void CIAUpdateSelfUpdaterInitializer::DoCancel()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::DoCancel() begin");
    
    iState = ENotStarted;

    // The ownership has not been transferred yet.
    // So, delete filelist here.    
    delete iFileList;
    iFileList = NULL;
    
    // Also inform observer that the cancellation happened.
    Observer().SelfUpdateInitComplete( KErrCancel );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::DoCancel() end");
    }


void CIAUpdateSelfUpdaterInitializer::RunL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::RunL() begin");
    
    switch( State() )
        {
        case EInitialize:
            // Initialize things for the self update and set file list info.
            // Create the file list here. If an error occurs below, RunError will
            // delete the file list. If everything goes ok, then ownership is
            // transferred to other object.
            iFileList = CIAUpdaterFileList::NewL();
            SetContentFileListL();
            // Inform observer that setup is complete.
            // The ownership of the file list will be transferred here.
            InitCompleteL();
            break;
                
        default:
            // Unknown state.
            // Let RunError handle this.
            User::Leave( KErrArgument );
            break;
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::RunL() end");
    }


TInt CIAUpdateSelfUpdaterInitializer::RunError( TInt aError )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::RunError() begin");
    
    iState = ENotStarted;
    
    // Delete file list because error occurred.
    delete iFileList;
    iFileList = NULL;

    // Inform observer about the error.
    Observer().SelfUpdateInitComplete( aError );

    // We do not continue the operation anymore.

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::RunError() end");

    return KErrNone;
    }


const CIAUpdateSelfUpdaterInitializer::TUpdaterState& CIAUpdateSelfUpdaterInitializer::State() const
    {
    return iState;
    }


CIAUpdateNode& CIAUpdateSelfUpdaterInitializer::Node()
    {
    return iNode;
    }


MIAUpdateSelfUpdaterInitObserver& CIAUpdateSelfUpdaterInitializer::Observer()
    {
    return iObserver;
    }


RFs& CIAUpdateSelfUpdaterInitializer::FileServer()
    {
    return iFs;
    }


void CIAUpdateSelfUpdaterInitializer::SetContentFileListL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::SetContentFileListL() begin");
    
    // The file list will have one common name for all the files if the content
    // is given as a bundle.
    iFileList->SetBundleNameL( Node().Name() );
    iFileList->SetHidden( Node().Hidden() );
    CIAUpdaterIdentifier& identifier( iFileList->Identifier() );
    identifier.SetIdL( Node().MetaId() );
    identifier.SetNamespaceL( Node().MetaNamespace() );
    identifier.SetUid( Node().Uid() );

    MNcdProvider& provider =
        Node().Controller().ProviderL();

    MNcdPurchaseHistory* history( provider.PurchaseHistoryL() );
    CleanupReleasePushL( *history );

    // Create filter. So, we will get
    // all the purchase history items.
    CNcdPurchaseHistoryFilter* filter =
        CNcdPurchaseHistoryFilter::NewLC();

    // Add family uid to the filter
    RArray< TUid > uids;
    CleanupClosePushL( uids );
    uids.AppendL( Node().Controller().FamilyUid() );
    filter->SetClientUids( uids.Array() );
    CleanupStack::PopAndDestroy( &uids );

    // Add other filter values.
    // We only want content information for this node.
    filter->SetNamespaceL( Node().MetaNamespace() );
    filter->SetEntityIdL(  Node().MetaId() );
    
    // Get the ids. So, we can next get all the corresponding
    // details.
    RArray< TUint > ids =
        history->PurchaseIdsL( *filter );
    CleanupStack::PopAndDestroy( filter );
    CleanupClosePushL( ids );
    
    if ( ids.Count() > 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] Ids found.");
    
        // If there are any details, then the latest should be the first
        // one in the array. So, use that.
        CNcdPurchaseDetails* details( 
            history->PurchaseDetailsL( ids[ 0 ] , EFalse ) );        

        if ( details )
            {
            IAUPDATE_TRACE("[IAUPDATE] Details found.");
        
            // We got the newest details. Get its file information.
            CleanupStack::PushL( details );

            const MDesCArray& files = details->DownloadedFiles();        
            TInt count( files.MdcaCount() );
            IAUPDATE_TRACE_1("[IAUPDATE] Files count: %d", count);    
            for( TInt i = 0; i < count; ++i )
                {
                CIAUpdaterFileInfo* info( CIAUpdaterFileInfo::NewLC() );
                info->SetFilePathL( files.MdcaPoint( i ) );
                iFileList->AddFileInfoL( info );
                CleanupStack::Pop( info );
                info = NULL;                    
                IAUPDATE_TRACE("[IAUPDATE] Info added to file list.");
                }
            
            CleanupStack::PopAndDestroy( details );
            details = NULL;
            }
        }

    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::PopAndDestroy( history );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::SetContentFileListL() end");
    }


void CIAUpdateSelfUpdaterInitializer::InitCompleteL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::InitComplete() begin");

    // Everything has been done.
    // Set the state.
    iState = ENotStarted;

    if ( iFileList->FileInfos().Count() > 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] Add filelist into the controller list.");
        // Insert the file list for the use.
        // If this leaves, then let RunError handle the error situation.
        // It will also inform observer.
        Node().Controller().SelfUpdaterCtrl().AddFileListL( iFileList );
        // Ownership was transferred.
        iFileList = NULL;
        Observer().SelfUpdateInitComplete( KErrNone );
        }
    else
        {
        IAUPDATE_TRACE("[IAUPDATE] No files found. Error?");
        // No files to install. Think this as an error.
        // And, do not start the self updater because there is no need for that.
        // Let RunError handle the error situation.
        User::Leave( KErrNotFound );
        }

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterInitializer::InitComplete() end");
    }
