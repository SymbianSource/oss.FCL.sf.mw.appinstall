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
* Description:   Contains CNcdNodeInstallProxy class implementation
*
*/


#include <bamdesca.h>
#include <bautils.h>
#include <s32mem.h>

#include "ncdnodeinstallproxy.h"
#include "ncdnodeproxy.h"
#include "ncdnodemetadataproxy.h"
#include "ncdproviderproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdserverreportmanagerproxy.h"
#include "ncdoperationimpl.h"
#include "ncdinstalloperationproxy.h"
#include "ncdsilentinstalloperationproxy.h"
#include "ncdoperationmanagerproxy.h"
#include "ncdoperationdatatypes.h"
#include "catalogsclientserver.h"
#include "ncdnodeidentifier.h"
#include "ncdnodefunctionids.h"
#include "ncdnodeclassids.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdpanics.h"
#include "catalogsconstants.h"
#include "ncdinstallinfo.h"
#include "ncdfileinfo.h"
#include "ncdinstallationservice.h"
#include "ncddeviceinteractionfactory.h"
#include "ncdinstalledapplicationimpl.h"
#include "ncdinstalledthemeimpl.h"
#include "ncdinstalledfileimpl.h"
#include "ncdinstalledcontent.h"
#include "ncderrors.h"
#include "ncdextendedinstallinfo.h"
#include "ncdnodedownloadproxy.h"

// ======== PUBLIC MEMBER FUNCTIONS ========

CNcdNodeInstallProxy::CNcdNodeInstallProxy(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
: CNcdInterfaceBaseProxy( aSession, aHandle, &aMetadata ),
  iMetadata( aMetadata )
    {
    }


void CNcdNodeInstallProxy::ConstructL()
    {
    // Register the interface
    MNcdNodeInstall* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this,
            MNcdNodeInstall::KInterfaceUid ) );
    
    iInstallationService = 
        &Metadata().Node().OperationManager().InstallationServiceL(); 
    
    InternalizeL();                    
    }


CNcdNodeInstallProxy* CNcdNodeInstallProxy::NewL(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeInstallProxy* self = 
        CNcdNodeInstallProxy::NewLC( aSession, aHandle, aMetadata );
    CleanupStack::Pop( self );
    return self;
    }

CNcdNodeInstallProxy* CNcdNodeInstallProxy::NewLC(
    MCatalogsClientServer& aSession,
    TInt aHandle,
    CNcdNodeMetadataProxy& aMetadata )
    {
    CNcdNodeInstallProxy* self = 
        new( ELeave ) CNcdNodeInstallProxy( aSession, aHandle, aMetadata );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CNcdNodeInstallProxy::~CNcdNodeInstallProxy()
    {
    DLTRACEIN((""));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdNodeInstall::KInterfaceUid );
   
    // Delete member variables here
    // Do not delete installation service, node and operation manager because
    // this object does not own them.    
    
    // Release all content objects
    iContent.ResetAndDestroy();
    }


CNcdNodeMetadataProxy& CNcdNodeInstallProxy::Metadata() const
    {
    return iMetadata;
    }


void CNcdNodeInstallProxy::InternalizeL()
    {
    DLTRACEIN((""));

    HBufC8* data( NULL );
        
    // Because we do not know the exact size of the data, use
    // the alloc method, which creates the buffer of the right size
    // and sets the pointer to point to the created buffer.
    // Get all the data that is necessary to internalize this object
    // from the server side.
    User::LeaveIfError(
        ClientServerSession().
        SendSyncAlloc( NcdNodeFunctionIds::ENcdInternalize,
                       KNullDesC8,
                       data,
                       Handle(),
                       0 ) );

    if ( data == NULL )
        {
        DLERROR((""));
        User::Leave(  KErrNotFound );
        }

    CleanupStack::PushL( data );

    // Read the data from the stream and insert it to the member variables
    RDesReadStream stream( *data );
    CleanupClosePushL( stream );
    
    InternalizeDataL( stream );
    
    // Closes the stream
    CleanupStack::PopAndDestroy( &stream ); 
    CleanupStack::PopAndDestroy( data );

    DLTRACEOUT((""));    
    }


// ---------------------------------------------------------------------------
// Checks if this item has the purpose
// ---------------------------------------------------------------------------
// 
TBool CNcdNodeInstallProxy::IsPurpose( TNcdItemPurpose aPurpose ) const
    {
    return iPurpose & aPurpose;    
    }
    

// MNcdNodeInstall functions
    
// ---------------------------------------------------------------------------
// Creates an install operation
// ---------------------------------------------------------------------------
//
MNcdInstallOperation* CNcdNodeInstallProxy::InstallL( 
    MNcdInstallOperationObserver& aObserver )
    {
    DLTRACEIN((""));

    // Create operation
    
    // Using proxy as the observer. Forwards callbacks to the real operation
    // observer
    CNcdInstallOperationProxy* op = 
        Metadata().Node().OperationManager().CreateInstallOperationL(
        Metadata().Node(), this );
    iOperationObserver = &aObserver;
        
    DLTRACEOUT((""));    

    return op;
    }

// ---------------------------------------------------------------------------
// Creates an install operation
// ---------------------------------------------------------------------------
//
MNcdInstallOperation* CNcdNodeInstallProxy::SilentInstallL( 
    MNcdInstallOperationObserver& aObserver,
    Usif::COpaqueNamedParams* aInstallOptions )
    {
    DLTRACEIN((""));

    // Create operation
    
    // Using proxy as the observer. Forwards callbacks to the real operation
    // observer.
    // Notice that TrustedUI capabilites are checked when silent install operation is
    // created in the server side.
    
    CNcdInstallOperationProxy* op = 
        Metadata().Node().OperationManager().CreateSilentInstallOperationL(
        Metadata().Node(), this, aInstallOptions );
    
    
    iOperationObserver = &aObserver;
       
    DLTRACEOUT((""));    

    return op;
    }


// ---------------------------------------------------------------------------
// Returns installed status
// ---------------------------------------------------------------------------
//
TBool CNcdNodeInstallProxy::IsInstalledL() const
    {
    DLTRACEIN((""));
    if ( !iInstalled || !iContent.Count() )
        {
        DLTRACEOUT(("Not installed"));
        return EFalse;
        }
        
    for ( TInt i = 0; i < iContent.Count(); ++i )
        {
        if ( !iContent[i]->IsInstalledL() )
            {
            DLTRACEOUT(("Not installed (entirely)"));
            return EFalse;
            }
        }
    DLTRACEOUT(("Installed"));
    return ETrue;
    }


// ---------------------------------------------------------------------------
// IsLaunchableL
// ---------------------------------------------------------------------------
//
TBool CNcdNodeInstallProxy::IsLaunchable() const
    {        
    DLTRACEIN((""));
    for ( TInt i = 0; i < iContent.Count(); ++i )
        {
        if ( iContent[i]->IsLaunchable() )
            {
            DLTRACEOUT(("Item contains something launchable"));
            return ETrue;
            }
        }
    
    DLTRACEOUT(("Item doesn't contain anything launchable"));
    return EFalse;
    }
    

// ---------------------------------------------------------------------------
// InstalledContentL
// ---------------------------------------------------------------------------
//
RCatalogsArray<MNcdInstalledContent> 
    CNcdNodeInstallProxy::InstalledContentL()
    {
    DLTRACEIN(("this-ptr: %x, refcount: %d", this, TotalRefCount() ));
    RCatalogsArray<MNcdInstalledContent> array;
    CleanupResetAndDestroyPushL( array );
    
    array.ReserveL( iContent.Count() );
    MNcdInstalledContent* content = NULL;
    for ( TInt i = 0; i < iContent.Count(); ++i )
        {
        content = iContent[i];
        array.AppendL( content );
        content->AddRef();
        }
    CleanupStack::Pop( &array );
    DLTRACEOUT(("this-ptr: %x, refcount: %d", this, TotalRefCount() ));
    return array;
    }


// ---------------------------------------------------------------------------
// SetAsInstalledL
// ---------------------------------------------------------------------------
//
void CNcdNodeInstallProxy::SetApplicationInstalledL( TInt aErrorCode )
    {
    DLTRACEIN(("error code: %d", aErrorCode));

    // This function can only be called if download operation has already been done.
    CNcdNodeDownloadProxy* download( Metadata().Download() );
    if ( download == NULL 
         || !download->IsDownloadedL() )
        {
        DLINFO(("download has not been done"));
        User::Leave( KErrAbort );
        }

    // Handle the information in the server side.
    // This will also delete the content install files if they
    // still exist after succesfull installation.
    CBufBase* buf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    stream.WriteInt32L( aErrorCode );

    CleanupStack::PopAndDestroy( &stream );
    TPtrC8 ptr = buf->Ptr( 0 );    
        
    TInt tmp( 0 );
    TInt error =
    ClientServerSession().
        SendSync( NcdNodeFunctionIds::ENcdSetApplicationInstalled,
                  ptr,
                  tmp,
                  Handle() );
    User::LeaveIfError( error ); 

    CleanupStack::PopAndDestroy( buf );


    CNcdNodeManagerProxy& nodeManager( 
        Metadata().Node().NodeManager() );
        
   CNcdProviderProxy& provider(
        nodeManager.Provider() );

    // Send an error report.
    // Report manager is checking if the install report list already contains the
    // corresonding report and replaces an old one if necessary. But, in all the
    // cases set this report. If install has been set as success or failure,
    // always send the information. Even if the node state has been installed before
    // this operation, send new report because this may be a new try to install
    // the content for some reason. So, success and failure information is necessary
    // then also.
    provider.ServerReportManager().NodeSetAsInstalledL( Metadata().Node(), 
                                                        aErrorCode );

    // Internalize node and all corresponding nodes.
    // This way, all their information will be up-to-date.
    // For example, the installation state may have changed because of the
    // settings above.
    nodeManager.InternalizeRelatedNodesL( Metadata().Node() );
    }


// ---------------------------------------------------------------------------
// Installationservice getter
// ---------------------------------------------------------------------------
//
MNcdInstallationService& CNcdNodeInstallProxy::InstallationService()
    {
    return *iInstallationService;
    }


// ---------------------------------------------------------------------------
// File session getter
// ---------------------------------------------------------------------------
//
RFs& CNcdNodeInstallProxy::FileSession()
    {
    return iInstallationService->FileServerSession();
    }


// ---------------------------------------------------------------------------
// File opener
// ---------------------------------------------------------------------------
//
RFile CNcdNodeInstallProxy::OpenFileL( TInt aFileIndex )
    {
    DLTRACEIN((""));

    // Convert index to a descriptor
    HBufC8* filenumber = IntToDes8LC( aFileIndex );
    
    RFile fileHandle = ClientServerSession().SendSyncFileOpenL( 
        NcdNodeFunctionIds::ENcdInstallOpenFile,
        *filenumber,        
        Handle() );
        
    CleanupStack::PopAndDestroy( filenumber );
        
    DLTRACEOUT((""));
    return fileHandle;
    }

// ---------------------------------------------------------------------------
// Handles progress information
// ---------------------------------------------------------------------------
//
void CNcdNodeInstallProxy::InstallProgress( MNcdInstallOperation& aOperation,
    TNcdProgress aProgress )
    {
    DLTRACEIN((""));
    DASSERT( iOperationObserver );
    
    iOperationObserver->InstallProgress( aOperation, aProgress );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Handles querys
// ---------------------------------------------------------------------------
//
void CNcdNodeInstallProxy::QueryReceived( MNcdInstallOperation& aOperation,
    MNcdQuery* aQuery )
    {
    DLTRACEIN((""));
    DASSERT( iOperationObserver );
    iOperationObserver->QueryReceived( aOperation, aQuery );
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Handles operation complete callbacks
// ---------------------------------------------------------------------------
//
void CNcdNodeInstallProxy::OperationComplete( 
    MNcdInstallOperation& aOperation, TInt aError )
    {
    DLTRACEIN(( "Error: %d", aError ));
    DASSERT( iOperationObserver );


    if ( aError == KErrNone || aError == KNcdThemeReinstalled ) 
        {
        // update proxy's status from the server       
        TRAPD( err, iMetadata.InternalizeL() );
        if ( err != KErrNone ) 
            {
            DLERROR(("Error during internalization: %d", err));
            aError = err;
            }            
        }
     
    iOperationObserver->OperationComplete( aOperation, aError );
    DLTRACEOUT((""));
    }
    
    

// Other functions

void CNcdNodeInstallProxy::InternalizeDataL( RReadStream& aStream )
    {
    DLTRACEIN((""));

    // Use catalogsutils.h functions to internalize
    // memebervariables according to the data received
    // from the server.
    // Make sure that the variables are set here in the same
    // order as they are externalized in the server side.
    // Small mistake here messes up everything!

    // First read the class id. Because, it is the first thing in the stream.
    TInt classId( aStream.ReadInt32L() );
    
    if ( classId != NcdNodeClassIds::ENcdNodeInstallClassId )
        {
        // classId is not recognized
        DLERROR(("Class id was not recognized!"));
        // For testing purposes assert here
        DASSERT( EFalse );
        
        // Otherwise leave is adequate
        User::Leave( KErrCorrupt );
        }

    iPurpose = aStream.ReadInt32L();
    DLINFO(("Purpose: %u", iPurpose));
    
    iInstalled = aStream.ReadInt32L();
    DLINFO(("Installed: %d", iInstalled));
    
    iLaunchable = aStream.ReadInt32L();
    DLINFO(("Launchable: %d", iLaunchable));
    
    iContent.ResetAndDestroy();
    
    if ( iInstalled )
        {
        DLTRACE(("Reading infos"));
        TInt infos = aStream.ReadInt32L();
        TInt fileIndex = 0;        
        
        CNcdExtendedInstallInfo* tempInfo = NULL;
        while ( infos )
            {
            tempInfo = CNcdExtendedInstallInfo::NewLC();
            tempInfo->InternalizeL( aStream );
            CreateInstalledContentL( *tempInfo, fileIndex );
            CleanupStack::PopAndDestroy( tempInfo );
            tempInfo = NULL;
            --infos;
            ++fileIndex;
            }
        }    

    DLTRACEOUT((""));
    }
    
    
void CNcdNodeInstallProxy::CreateInstalledContentL(
    const CNcdExtendedInstallInfo& aInfo, 
    TInt aFileIndex )
    {
    DLTRACEIN((""));
    CNcdInstalledContent* content = NULL;
    if ( aInfo.Filename() != KNullDesC() ) 
        {
        DLTRACE(( _L("Installed file: %S"), &aInfo.Filename() ));

        CNcdFileInfo* fileInfo = CNcdFileInfo::NewLC( 
            aInfo.Filename(), aInfo.MimeType() );
        CleanupStack::Pop( fileInfo );
        
        // fileInfo is deleted in CNcdInstalledFile if NewLC leaves            
        CNcdInstalledFile* file = 
            CNcdInstalledFile::NewLC( *this, fileInfo, aFileIndex );
        content = file;
        }
    else if ( aInfo.ApplicationUid() != TUid::Null() ) 
        {
        DLTRACE(("Application"));
        CNcdInstalledApplication* app = 
            CNcdInstalledApplication::NewLC( 
                *this, 
                aInfo.ApplicationUid(),
                aInfo.ApplicationVersion(),
                aInfo.Parameter(),
                aInfo.ContentType(),
                aInfo.UriExists() );
        content = app;
        }
    else // assume theme
        {
        DLTRACE(( _L("Theme: %S"), &aInfo.ThemeName() ));
        CNcdInstalledTheme* theme = CNcdInstalledTheme::NewLC(
            *this, aInfo.ThemeName().AllocL() );
        content = theme;
        }

    DASSERT( content );
    content->AddRef();
    content->SetLaunchable( aInfo.IsLaunchable() );
    
    DLTRACE(("Content refcount: %d", content->RefCount() ));
    iContent.AppendL( content );
    
    CleanupStack::Pop(); // file, app or theme

    DLTRACEOUT(("Content added successfully"));
    }
