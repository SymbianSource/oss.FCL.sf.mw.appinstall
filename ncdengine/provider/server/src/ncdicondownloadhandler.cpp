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
#include <s32file.h>

#include "ncdnode.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogshttpsession.h"
#include "ncdicondownloadhandler.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeiconimpl.h"
#include "ncdfilehandler.h"
#include "ncdnodeidentifier.h"
#include "ncdproviderutils.h"

#include "catalogsdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdIconDownloadHandler* CNcdIconDownloadHandler::NewL( 
    const CNcdNodeIdentifier& aNodeId,
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession )
    {
    CNcdIconDownloadHandler* self = new( ELeave ) 
        CNcdIconDownloadHandler( aNodeManager, aHttpSession );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeId );
    CleanupStack::Pop( self );
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdIconDownloadHandler::~CNcdIconDownloadHandler()
    {
    DLTRACE(("--><--"));
    }



// ---------------------------------------------------------------------------
// DownloadL
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CNcdIconDownloadHandler::DownloadL()
    {
    DLTRACEIN(( "" ));
    
    CNcdNode& node( NodeL() );
    
    DLTRACE(("Getting metadata"));
    // Get node metadata
    CNcdNodeMetaData& metadata = node.NodeMetaDataL();
    
    DLTRACE(("Getting icon"));
    // Get icon
    const CNcdNodeIcon& icon = metadata.IconL();

    DLINFO(( _L("Creating download for Uri: %S"), &icon.Uri() ));
    // Create download    
    MCatalogsHttpOperation* dl = HttpSession().CreateDownloadL(
        icon.Uri(), EFalse );
    CleanupStack::PushL( dl );
    
    // ensure that transactions are not used to get HTTP headers
    dl->SetHeaderMode( ECatalogsHttpHeaderModeNoHead );
    
    // Icon downloads are low priority
    dl->Config().SetPriority( ECatalogsPriorityLow );
    
    // Prevent filename extension update because it's not needed
    dl->Config().SetOptions( 
        dl->Config().Options() & ~ECatalogsHttpDisableHeadRequest );
    
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
void CNcdIconDownloadHandler::FinishDownloadL( 
    MCatalogsHttpOperation& aDownload, 
    MNcdFileHandler* aFileHandler)
    {    
    DLTRACEIN(( "filehandler: %X", aFileHandler ));
    HBufC* filename = aDownload.Config().FullPathLC();

    // Get node
    CNcdNode& node( NodeL() );
    
    // Get node metadata
    CNcdNodeMetaData& metadata = node.NodeMetaDataL();
    
    // Get icon
    const CNcdNodeIcon& icon = metadata.IconL();
      
    DLINFO(( _L("Moving the file from %S"), filename ));

    // Moves the file to target location. 
    // Using metadata's namespace
    aFileHandler->
        MoveFileL( *filename, 
                   metadata.Identifier().NodeNameSpace(),
                   icon.IconId() );
    CleanupStack::PopAndDestroy( filename );
      
    DLTRACEOUT(( "" ));
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdIconDownloadHandler::CNcdIconDownloadHandler( 
    CNcdNodeManager& aNodeManager, 
    MCatalogsHttpSession& aHttpSession ) :
    CNcdBaseDownloadHandler( aNodeManager, aHttpSession )
    {
    }
    
