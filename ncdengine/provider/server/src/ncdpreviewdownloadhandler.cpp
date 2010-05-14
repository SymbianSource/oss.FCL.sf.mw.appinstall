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
#include "ncdpreviewdownloadhandler.h"
#include "ncdnodepreviewimpl.h"
#include "ncdnodemetadataimpl.h"
#include "ncdfilehandler.h"
#include "ncdpanics.h"
#include "ncdproviderutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdPreviewDownloadHandler* CNcdPreviewDownloadHandler::NewL( 
    const CNcdNodeIdentifier& aNodeId,
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession )
    {
    CNcdPreviewDownloadHandler* self = new( ELeave ) 
        CNcdPreviewDownloadHandler( aNodeManager, aHttpSession );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeId );
    CleanupStack::Pop( self );
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdPreviewDownloadHandler::~CNcdPreviewDownloadHandler()
    {
    }



// ---------------------------------------------------------------------------
// DownloadL
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CNcdPreviewDownloadHandler::DownloadL()
    {
    DLTRACEIN(( "" ));
    
    CNcdNode& node( NodeL() );
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
    CNcdNodePreview& preview( metadata.PreviewL() );

    NCD_ASSERT_ALWAYS( CurrentDownload() < preview.UriCount(),
        ENcdPanicIndexOutOfRange ); 
                

    DLTRACE(( _L("Create preview download, URI: %S"),
        &preview.Uri( CurrentDownload() ) ));        
        
    MCatalogsHttpOperation* dl = HttpSession().CreateDownloadL( 
        preview.Uri( CurrentDownload() ),
        EFalse );
    CleanupReleasePushL( *dl );

    HBufC* tempPath = CNcdProviderUtils::TempPathLC( 
        NodeId().ClientUid().Name() );
    
    // Use provider's temp path    
    dl->Config().SetDirectoryL( 
        *tempPath );
    CleanupStack::PopAndDestroy( tempPath );

    dl->SetHeaderMode( ECatalogsHttpHeaderModeForceHead );
    
    SetCurrentDownload( CurrentDownload() + 1 );
    CleanupStack::Pop( dl );
    DLTRACEOUT( ( "" ) );
    return dl;
    }
    

// ---------------------------------------------------------------------------
// FinishDownloadL
// ---------------------------------------------------------------------------
//    
void CNcdPreviewDownloadHandler::FinishDownloadL( 
    MCatalogsHttpOperation& aDownload, 
    MNcdFileHandler* /* aFileHandler */)
    {
    DLTRACEIN( ( "" ) );
    HBufC* filename = aDownload.Config().FullPathLC();
    
    DLTRACE(("Call preview handler"));
    
    // CurrentDownload-1 since it was increased after the download was
    // created
    DASSERT( CurrentDownload() - 1 >= 0 );
    HBufC* mime = Des8ToDes16LC( aDownload.ContentType() );
    //        
    NodeManager().PreviewHandlerL( 
        NodeId(), 
        *filename, 
        CurrentDownload() - 1,
        *mime );
        
    CleanupStack::PopAndDestroy( 2, filename );  // mime, filename
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdPreviewDownloadHandler::CNcdPreviewDownloadHandler( 
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession ) :
    CNcdBaseDownloadHandler( aNodeManager, aHttpSession )
    {
    }
