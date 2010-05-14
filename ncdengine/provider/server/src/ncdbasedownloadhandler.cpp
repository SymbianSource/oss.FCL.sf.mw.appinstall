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
* Description:   CNcdBaseDownloadHandler implementation
*
*/


#include <f32file.h>

#include "ncdnode.h"
#include "ncdnodemanager.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogshttpsession.h"
#include "catalogsdebug.h"
#include "ncdbasedownloadhandler.h"
#include "ncdnodeidentifier.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdBaseDownloadHandler::~CNcdBaseDownloadHandler()
    {
    DLTRACEIN((""));
    delete iNodeId;
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// CurrentDownload
// ---------------------------------------------------------------------------
//    
TInt CNcdBaseDownloadHandler::CurrentDownload() const
    {
    return iCurrentDownload;
    }


// ---------------------------------------------------------------------------
// DownloadCount
// ---------------------------------------------------------------------------
//    
TInt CNcdBaseDownloadHandler::DownloadCount() const
    {
    return iDownloads;
    }



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdBaseDownloadHandler::CNcdBaseDownloadHandler( 
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession ) :
    iNodeManager( aNodeManager ), iHttpSession( aHttpSession ), iDownloads( 1 )
    {
    }
    
    
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CNcdBaseDownloadHandler::ConstructL( const CNcdNodeIdentifier& aNodeId )
    {
    DLTRACEIN((""));
    iNodeId = CNcdNodeIdentifier::NewL( aNodeId );
    DLTRACEOUT((""));
    }
    
    
// ---------------------------------------------------------------------------
// Node getter
// ---------------------------------------------------------------------------
//
CNcdNode& CNcdBaseDownloadHandler::NodeL() const
    {
    return iNodeManager.NodeL( *iNodeId );
    }
    

// ---------------------------------------------------------------------------
// Node ID getter
// ---------------------------------------------------------------------------
//
const CNcdNodeIdentifier& CNcdBaseDownloadHandler::NodeId() const
    {
    DASSERT( iNodeId );
    return *iNodeId;
    }
    
// ---------------------------------------------------------------------------
// HTTP session getter
// ---------------------------------------------------------------------------
//
MCatalogsHttpSession& CNcdBaseDownloadHandler::HttpSession() const
    {
    return iHttpSession;
    }


// ---------------------------------------------------------------------------
// Node manager getter
// ---------------------------------------------------------------------------
//
CNcdNodeManager& CNcdBaseDownloadHandler::NodeManager() const
    {
    return iNodeManager;
    }

// ---------------------------------------------------------------------------
// Download count setter
// ---------------------------------------------------------------------------
//

void CNcdBaseDownloadHandler::SetTotalDownloads( TInt aDownloads )
    {
    iDownloads = aDownloads;
    }
    

// ---------------------------------------------------------------------------
// Current download setter
// ---------------------------------------------------------------------------
//    
void CNcdBaseDownloadHandler::SetCurrentDownload( TInt aCurrentDownload )
    {
    iCurrentDownload = aCurrentDownload;
    }
    
