/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the implementation of CSWInstDocument
*                class member functions.
*
*/


// INCLUDE FILES

#include "SWInstDocument.h"
#include "SWInstAppUi.h"
#include "SWInstDefs.h"

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSWInstDocument::CSWInstDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSWInstDocument::CSWInstDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

// -----------------------------------------------------------------------------
// CSWInstDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSWInstDocument* CSWInstDocument::NewL( CEikApplication& aApp )
    {
    return new(ELeave) CSWInstDocument( aApp );
    }

// Destructor
CSWInstDocument::~CSWInstDocument()
    {
    }

// -----------------------------------------------------------------------------
// CSWInstDocument::OpenFileL
// Called by the framework to open a file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CFileStore* CSWInstDocument::OpenFileL( TBool /*aDoOpen*/,
                                        const TDesC& aFilename,
                                        RFs& /*aFs*/ )
    {
    TRAPD( err, static_cast<CSWInstAppUi*>(iAppUi)->InstallL( aFilename ) );
    if ( err != KErrNone && err != KSWInstErrBusy )
        {
        static_cast<CSWInstAppUi*>(iAppUi)->RequestCompleted( err );        
        }
    
    return NULL;    
    }

void CSWInstDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
    {
    aFileStore = NULL;    
    TRAPD( err, static_cast<CSWInstAppUi*>(iAppUi)->InstallL( aFile ) );
    if ( err != KErrNone && err != KSWInstErrBusy )
        {
        static_cast<CSWInstAppUi*>(iAppUi)->RequestCompleted( err );        
        }
    }

// -----------------------------------------------------------------------------
// CSWInstDocument::CreateAppUiL
// Creates CSWInstAppUi object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEikAppUi* CSWInstDocument::CreateAppUiL()
    {
    return new( ELeave ) CSWInstAppUi;
    }


//  End of File  
