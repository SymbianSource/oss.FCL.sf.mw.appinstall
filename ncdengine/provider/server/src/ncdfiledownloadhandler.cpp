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
* Description:   CNcdFileDownloadHandler implementation
*
*/


#include <f32file.h>

#include "ncdnode.h"
#include "catalogshttpoperation.h"
#include "catalogshttpconfig.h"
#include "catalogshttpsession.h"
#include "catalogsdebug.h"
#include "ncdfiledownloadhandler.h"
#include "ncdnodeidentifier.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdFileDownloadHandler* CNcdFileDownloadHandler::NewL( 
    const CNcdNodeIdentifier& aNodeId,
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession )
    {
    CNcdFileDownloadHandler* self = new( ELeave ) 
        CNcdFileDownloadHandler( aNodeManager, aHttpSession );
    CleanupStack::PushL( self );
    self->ConstructL( aNodeId );
    CleanupStack::Pop( self );    
    
    return self;
    }
    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CNcdFileDownloadHandler::~CNcdFileDownloadHandler()
    {
    }


// ---------------------------------------------------------------------------
// DownloadL
// ---------------------------------------------------------------------------
//    
MCatalogsHttpOperation* CNcdFileDownloadHandler::DownloadL()
    {
    DLTRACEIN(( _L("Creating download for URI: %S"), 
        &NodeId().NodeNameSpace() ));

    
    MCatalogsHttpOperation* dl = HttpSession().CreateDownloadL( 
        NodeId().NodeNameSpace(),
        EFalse );
    CleanupStack::PushL( dl );

    DLTRACE(( _L("Parsing path: %S"), &NodeId().NodeId() ));
    TParsePtrC path( NodeId().NodeId() );
    
    dl->Config().SetDirectoryL( path.DriveAndPath() );  
    dl->Config().SetFilenameL( path.NameAndExt() );
    
    SetCurrentDownload( CurrentDownload() + 1 );
    CleanupStack::Pop( dl );
    DLTRACEOUT(( "" ));
    return dl;
    }
    

// ---------------------------------------------------------------------------
// FinishDownloadL
// ---------------------------------------------------------------------------
//    
void CNcdFileDownloadHandler::FinishDownloadL( 
    MCatalogsHttpOperation& /*aDownload*/, 
    MNcdFileHandler* /*aFileHandler*/)
    {
    DLTRACEIN((""));
    /**
     * Transport takes care of this
     */    
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdFileDownloadHandler::CNcdFileDownloadHandler( 
    CNcdNodeManager& aNodeManager,
    MCatalogsHttpSession& aHttpSession ) :
    CNcdBaseDownloadHandler( aNodeManager, aHttpSession )
    {
    }
