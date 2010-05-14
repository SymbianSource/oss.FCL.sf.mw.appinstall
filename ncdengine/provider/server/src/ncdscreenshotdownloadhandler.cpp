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
* Description:  
*
*/


#include <f32file.h>

#include "ncdnode.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogshttpsession.h"
#include "catalogsdebug.h"
#include "ncdscreenshotdownloadhandler.h"
#include "ncdnodescreenshotimpl.h"
#include "ncd_pp_download.h"
#include "ncdproviderutils.h"
#include "catalogsconstants.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeidentifier.h"
#include "ncdpanics.h"
#include "ncdfilehandler.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdScreenshotDownloadHandler* CNcdScreenshotDownloadHandler::NewL( 
    const CNcdNodeIdentifier& aNodeId,
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession )
    {
    CNcdScreenshotDownloadHandler* self = new( ELeave ) 
        CNcdScreenshotDownloadHandler( aNodeManager, aHttpSession );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeId );
    CleanupStack::Pop( self );
    
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdScreenshotDownloadHandler::~CNcdScreenshotDownloadHandler()
    {
    }


// ---------------------------------------------------------------------------
// DownloadL
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CNcdScreenshotDownloadHandler::DownloadL()
    {
    DLTRACEIN(( "" ));
    
    CNcdNode& node( NodeL() );
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
    const CNcdNodeScreenshot& screenshot( metadata.ScreenshotL() );

    NCD_ASSERT_ALWAYS( CurrentDownload() < 
        screenshot.ScreenshotDownloadCount(), ENcdPanicNoData );

    // Update total download count
    SetTotalDownloads( screenshot.ScreenshotDownloadCount() );

    const TDesC& uri( 
        screenshot.ScreenshotDownloadUri( CurrentDownload() ) );   
    
    DLINFO(( _L("Screenshot URI: %S"), &uri ));
    MCatalogsHttpOperation* dl = HttpSession().CreateDownloadL( 
        uri,
        EFalse );
    CleanupReleasePushL( *dl );

    HBufC* tempPath = CNcdProviderUtils::TempPathLC( 
        NodeId().ClientUid().Name() );
    
    // Use provider's temp path    
    dl->Config().SetDirectoryL( 
        *tempPath );
    CleanupStack::PopAndDestroy( tempPath );
    
    SetCurrentDownload( CurrentDownload() + 1 );
    CleanupStack::Pop( dl );
    DLTRACEOUT(( "" ));
    return dl;
    }
    

// ---------------------------------------------------------------------------
// FinishDownloadL
// ---------------------------------------------------------------------------
//    
void CNcdScreenshotDownloadHandler::FinishDownloadL( 
    MCatalogsHttpOperation& aDownload, 
    MNcdFileHandler* aFileHandler)
    {
    DLTRACEIN(( "filehandler: %X", aFileHandler ));    
    
    HBufC* filename = aDownload.Config().FullPathLC();

    // Get node
    CNcdNode& node( NodeL() );
    
    // Get node metadata
    CNcdNodeMetaData& metadata = node.NodeMetaDataL();
    
    const CNcdNodeScreenshot& screenshot( metadata.ScreenshotL() );
    const TDesC& uri( 
        screenshot.ScreenshotDownloadUri( CurrentDownload() - 1 ) );   

            
    DLTRACE(( _L("Moving the file from %S"), filename ));

    // Moves the file to target location. 
    // Using metadata's namespace
    aFileHandler->
        MoveFileL( *filename, 
                   metadata.Identifier().NodeNameSpace(),
                   uri );
    CleanupStack::PopAndDestroy( filename );
    
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdScreenshotDownloadHandler::CNcdScreenshotDownloadHandler( 
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession ) :
    CNcdBaseDownloadHandler( aNodeManager, aHttpSession )
    {
    }
