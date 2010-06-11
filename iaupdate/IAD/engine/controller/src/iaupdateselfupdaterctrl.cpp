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




#include <e32std.h>

#include "iaupdateselfupdaterctrl.h"
#include "iaupdatenode.h"
#include "iaupdatebasenode.h"
#include "iaupdatependingnodesfile.h"
#include "iaupdateridentifier.h"
#include "iaupdaterestartinfo.h"
#include "iaupdaterendezvous.h"
#include "iaupdateselfupdaterobserver.h"
#include "iaupdatedebug.h"

// Self updater file handling objects
#include "iaupdaterfilelistfile.h"

// Self updater header for udpater constants
#include "iaupdaterdefs.h"


CIAUpdateSelfUpdaterCtrl* CIAUpdateSelfUpdaterCtrl::NewL( 
    MIAUpdateSelfUpdaterObserver& aObserver )
    {
    CIAUpdateSelfUpdaterCtrl* self =
        CIAUpdateSelfUpdaterCtrl::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateSelfUpdaterCtrl* CIAUpdateSelfUpdaterCtrl::NewLC( 
    MIAUpdateSelfUpdaterObserver& aObserver )
    {
    CIAUpdateSelfUpdaterCtrl* self =
        new( ELeave ) CIAUpdateSelfUpdaterCtrl( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateSelfUpdaterCtrl::CIAUpdateSelfUpdaterCtrl( 
    MIAUpdateSelfUpdaterObserver& aObserver )
: CBase(),
  iObserver( aObserver )
    {

    }


void CIAUpdateSelfUpdaterCtrl::ConstructL()
    {
    iRendezvous = CIAUpdateRendezvous::NewL( *this );
    iFile = CIAUpdaterFileListFile::NewL();
    }


CIAUpdateSelfUpdaterCtrl::~CIAUpdateSelfUpdaterCtrl()
    {
    delete iRendezvous;
    delete iFile;
    }


TBool CIAUpdateSelfUpdaterCtrl::DataExists() const
    {
    TInt count( iFile->FileList().Count() );
    if ( count == 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::DataExists no data");
        return EFalse;
        }
    else
        {
        IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::DataExists count: %d", count );
        return ETrue;
        }    
    }    


TBool CIAUpdateSelfUpdaterCtrl::StartL( 
    TInt aIndex, 
    TInt aTotalCount,
    const RPointerArray< MIAUpdateNode >& aPendingNodes,
    TBool aSilent )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartL begin");
    
    if ( !DataExists() )
        {
        // Nothing to do.
        return EFalse;
        }


    // Set the information if the dialogs should be shown or
    // if the operation should be silent. Also,set the index 
    // and total count info.
    // Rest has been set before.
    File().SetSilent( aSilent );
    File().SetPckgStartIndex( aIndex );
    File().SetTotalPckgCount( aTotalCount );

    // Write data into the file for the self updater to use.
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartL write file");
    File().WriteDataL();
    
    // Now, that the data is written, reset filelist
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartL reset file");
    File().Reset();


    // Save the pending node infromation
    CIAUpdatePendingNodesFile* pendingNodesFile( 
        CIAUpdatePendingNodesFile::NewLC() );
    pendingNodesFile->SetIndex( aIndex );
    for ( TInt i = 0; i < aPendingNodes.Count(); ++i )
        {
        MIAUpdateNode* node( aPendingNodes[ i ] );
        MIAUpdateBaseNode& baseNode( node->Base() );
        CIAUpdaterIdentifier* identifier( CIAUpdaterIdentifier::NewLC() );
        identifier->SetUid( baseNode.Uid() );
        identifier->SetIdL( baseNode.MetaId() );
        identifier->SetNamespaceL( baseNode.MetaNamespace() );
        pendingNodesFile->PendingNodes().AppendL( identifier );
        CleanupStack::Pop( identifier );
        }
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartL write pending nodes file");
    pendingNodesFile->WriteDataL();
    CleanupStack::PopAndDestroy( pendingNodesFile );
    

    StartUpdaterProcessL();


    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartL end");

    return ETrue;
    }


void CIAUpdateSelfUpdaterCtrl::Reset()
    {
    File().Reset();    
    }


void CIAUpdateSelfUpdaterCtrl::AddFileListL( 
    CIAUpdaterFileList* aFileList )
    {
    // Notice, that the ownership is transferred here.
    File().FileList().AppendL( aFileList );
    }


CIAUpdateRestartInfo* CIAUpdateSelfUpdaterCtrl::SelfUpdateRestartInfo()
    {
    CIAUpdateRestartInfo* info( NULL );
    
    TRAP_IGNORE( info = CIAUpdateRestartInfo::NewL() );
    
    return info;
    }


void CIAUpdateSelfUpdaterCtrl::RendezvousComplete( 
    TInt aErrorCode )
    {
    // Self updater finished its job and informed about it by setting the
    // rendezvous complete.
    Observer().SelfUpdaterComplete( aErrorCode );
    }


MIAUpdateSelfUpdaterObserver& CIAUpdateSelfUpdaterCtrl::Observer()
    {
    return iObserver;
    }


CIAUpdaterFileListFile& CIAUpdateSelfUpdaterCtrl::File()
    {
    return *iFile;
    }


CIAUpdateRendezvous& CIAUpdateSelfUpdaterCtrl::RendezvousListener()
    {
    return *iRendezvous;
    }


// -----------------------------------------------------------------------------
// CIAUpdateSelfUpdaterCtrl::StartUpdaterProcessL()
// 
// -----------------------------------------------------------------------------
//	
void CIAUpdateSelfUpdaterCtrl::StartUpdaterProcessL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartUpdaterProcessL begin");

    // Start IAD Update
    // The application is started here by using process handle. 
    // This way we get the process id, which is required for the 
    // rendezvous listener.
    RProcess process;
    CleanupClosePushL( process );
    // Create the process
    User::LeaveIfError( 
        process.Create( IAUpdaterDefs::KIAUpdaterExe, KNullDesC ) );
    RendezvousListener().StartL( process.Id() );
    // Start executing the process.
    process.Resume();
    CleanupStack::PopAndDestroy( &process );

    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSelfUpdaterCtrl::StartUpdaterProcessL end");
    }
