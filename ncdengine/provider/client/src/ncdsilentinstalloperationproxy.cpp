/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements CNcdSilentInstallOperationProxy
*
*/


#include <s32strm.h>
#include "ncdsilentinstalloperationproxy.h"
#include "ncdnodemanagerproxy.h"
#include "ncdinstallationservice.h"
#include "ncdfileinfo.h"
#include "ncdinstallinfo.h"
#include "catalogsdebug.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdSilentInstallOperationProxy* CNcdSilentInstallOperationProxy::NewLC( 
    MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdInstallOperationObserver* aObserver,
    MNcdInstallationService& aInstallationService,
    Usif::COpaqueNamedParams* aInstallOptions )
    {
    CNcdSilentInstallOperationProxy* self =
        new( ELeave ) CNcdSilentInstallOperationProxy( aInstallationService, 
                                                       aInstallOptions );
    
    self->AddRef();
    CleanupReleasePushL( *self );    
    self->ConstructL( aSession, aHandle, aRemoveHandler, aNode, aNodeManager,
        aObserver );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdSilentInstallOperationProxy::CNcdSilentInstallOperationProxy( 
    MNcdInstallationService& aInstallationService,
    /*const*/ Usif::COpaqueNamedParams* aInstallOptions )
: CNcdInstallOperationProxy( aInstallationService )
    {
    // Copy all the install options into the buffer that will be used
    // when silent installation is delegated to the installer.
    iInstallOptions = aInstallOptions;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdSilentInstallOperationProxy::~CNcdSilentInstallOperationProxy()
    {
    DLTRACEIN(( "" ));
    
    delete iInstallOptions;
    
    // Cancel must be called here because base class will only call its own
    // DoCancel()
    Cancel();
    DLTRACEOUT(( "" ));    
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdSilentInstallOperationProxy::ConstructL( MCatalogsClientServer& aSession,
    TInt aHandle,
    MNcdOperationProxyRemoveHandler* aRemoveHandler,
    CNcdNodeProxy* aNode,
    CNcdNodeManagerProxy* aNodeManager,
    MNcdInstallOperationObserver* aObserver )
    {
    DLTRACEIN(( "" ));

    CNcdInstallOperationProxy::ConstructL( aSession, aHandle, aRemoveHandler, 
                                           aNode, aNodeManager,
                                           aObserver );
        
    DLTRACEOUT(( "" ));    
    }


// ---------------------------------------------------------------------------
// From CActive
// ?implementation_description
// ---------------------------------------------------------------------------
//

void CNcdSilentInstallOperationProxy::DoCancel()
    {
    DLTRACEIN((""));
    
    // These variables will contain the possible information
    // about the cancelled installation.
    HBufC* fileName( NULL );
    TUid appUid( TUid::Null() );
    TInt errorCode( KErrCancel );

    // Because the operation is going on, we have to stop the
    // silent installation of the installer.
    Installer().CancelSilentInstall( fileName, appUid, errorCode );

    // Only update succesfull info to the server.
    if ( errorCode == KErrNone )
        {
        // Cancellation did not have effect and the installation
        // was success for the current item.
        // Notice, that UpdateInfoToServerL will also delete the
        // used install file of the content.
        if ( fileName != NULL )
            {
            TRAP_IGNORE( 
                UpdateInfoToServerL( *fileName, 
                                     appUid, 
                                     errorCode ) );            
            }
        else
            {
            TRAP_IGNORE( 
                UpdateInfoToServerL( KNullDesC, 
                                     appUid, 
                                     errorCode ) );                        
            }
        }

    // No need for the fileName anymore.
    delete fileName;
    fileName = NULL;

    // Notice that if the errorCode is KErrNone, then UpdateInfoToServerL above
    // deleted the content install file and increased the current file count by one. 
    // So, we can directly compare CurrentFile() with FileCount() without adding 1 
    // for the current file index.
    if ( errorCode == KErrNone && CurrentFile() < FileCount() )
        {
        // Error code should be cancel because not all of the files
        // in the bundle where installed.
        errorCode = KErrCancel;
        }
        
    TRAP_IGNORE( UpdateOperationInfoToPurchaseHistoryL( errorCode ) );
    
    TRAP_IGNORE( NodeManager()->InternalizeRelatedNodesL( *NodeProxy() ) );

    // By setting the error code here, the parent class may inform the
    // report manager with the correct value.
    SetInstallationErrorCode( errorCode );

    // Let the parent handle other cancellations
    CNcdInstallOperationProxy::DoCancel();
    }    


// ---------------------------------------------------------------------------
// Calls the functions of the installer. Child classes may call different functions.
// ---------------------------------------------------------------------------
//
void CNcdSilentInstallOperationProxy::UseInstallServiceL( const CNcdFileInfo& aFile )
    {
    DLTRACEIN((""));
    // Choose correct installation type
    switch( CurrentInfo().InstallType() )
        {
        case CNcdInstallInfo::ENcdInstallNormal:
            {            
            Installer().SilentInstallL(
                iFileHandle, 
                aFile.MimeType(), 
                aFile.Purpose(), 
                SilentInstallOptions() ); 
            break;
            }
        
        case CNcdInstallInfo::ENcdInstallJar: // flow through
        case CNcdInstallInfo::ENcdInstallJad:
            {
            Installer().SilentInstallJavaL( 
                iFileHandle, 
                aFile.MimeType(), 
                aFile.Data(),
                SilentInstallOptions() );             
            break;
            }            

        // Call the corresponding installing function according to the type value.
        // MimeType info is missing here but SWI will figure out the mimetype via file handler
        // and call the corresponding plugin.
        case CNcdInstallInfo::ENcdInstallWidget:
            {
            Installer().SilentInstallWidgetL(
                    iFileHandle, 
                    SilentInstallOptions() );
   
            break;
            }
        
        default:
            {
            // All cases should be handled
            DASSERT( 0 );
            break;
            }
        }
    DLTRACEOUT((""));   
    }

const Usif::COpaqueNamedParams* CNcdSilentInstallOperationProxy::SilentInstallOptions() const
    {
    DLTRACEIN((""));
    return iInstallOptions;
    }
