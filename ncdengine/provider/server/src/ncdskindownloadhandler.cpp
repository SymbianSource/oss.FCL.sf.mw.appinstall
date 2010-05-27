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


#include "ncdnode.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogshttpsession.h"
#include "catalogsdebug.h"
#include "ncdskindownloadhandler.h"
#include "ncdnodemetadataimpl.h"
#include "ncdnodeskinimpl.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdSkinDownloadHandler* CNcdSkinDownloadHandler::NewL( 
    const CNcdNodeIdentifier& aNodeId,
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession )
    {
    CNcdSkinDownloadHandler* self = new( ELeave ) 
        CNcdSkinDownloadHandler( aNodeManager, aHttpSession );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeId );
    CleanupStack::Pop( self );
    
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdSkinDownloadHandler::~CNcdSkinDownloadHandler()
    {
    }


// ---------------------------------------------------------------------------
// DownloadL
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CNcdSkinDownloadHandler::DownloadL()
    {
    DLTRACEIN( ( "" ) );
    
    // Get node
    CNcdNode& node( NodeL() );
    CNcdNodeMetaData& metadata( node.NodeMetaDataL() );
    const CNcdNodeSkin& skin( metadata.SkinL() );    
    
    DLTRACE(( _L("Download skin id: %S"), &skin.Uri() ));
    MCatalogsHttpOperation* dl = HttpSession().CreateDownloadL( 
        skin.Uri(),
        EFalse );
    CleanupStack::PushL( dl );
    
    SetCurrentDownload( CurrentDownload() + 1 );
    CleanupStack::Pop( dl );
    DLTRACEOUT( ( "" ) );
    return dl;
    }
    

// ---------------------------------------------------------------------------
// FinishDownloadL
// ---------------------------------------------------------------------------
//    
void CNcdSkinDownloadHandler::FinishDownloadL( 
    MCatalogsHttpOperation& /* aDownload */, 
    MNcdFileHandler* /* aFileHandler */)
    {
    DLTRACEIN( ( "" ) );
    // Nothing to do since Transport takes care of moving the file
    DLTRACEOUT( ( "" ) );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdSkinDownloadHandler::CNcdSkinDownloadHandler( 
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession ) :
    CNcdBaseDownloadHandler( aNodeManager, aHttpSession )
    {
    }
