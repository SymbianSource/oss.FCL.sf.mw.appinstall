/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateDocument class 
*                member functions.
*
*/


//INCLUDES
#include <e32property.h>
#include "iaupdateappui.h"
#include "iaupdatedocument.h"
#include "iaupdatefwnsmlpskeys.h"
#include "iaupdatedebug.h"
#include "iaupdateuids.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CIAUpdateDocument::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIAUpdateDocument* CIAUpdateDocument::NewL(CEikApplication& aApp)
    {
    CIAUpdateDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CIAUpdateDocument::NewLC
// 
// ---------------------------------------------------------------------------
//
CIAUpdateDocument* CIAUpdateDocument::NewLC(CEikApplication& aApp)
    {
    CIAUpdateDocument* self = new (ELeave) CIAUpdateDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// void CCIAUpdateDocument::ConstructL()
// ---------------------------------------------------------------------------
//
void CIAUpdateDocument::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDocument::ConstructL() begin");
    TInt err  = RProperty::Define( KPSUidNSmlDMSyncApp,
            KFotaDLStatus,
            RProperty::EInt); //Policies removed as FOTA to write     
    
    err =  RProperty::Set( KPSUidNSmlDMSyncApp,
            KFotaDLStatus,KErrNotFound );  
    
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateDocument::ConstructL() end");
    }    


// ---------------------------------------------------------------------------
// CIAUpdateDocument::CIAUpdateDocument(CEikApplication& aApp)
// constructor
// ---------------------------------------------------------------------------
//
CIAUpdateDocument::CIAUpdateDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    }

// ---------------------------------------------------------------------------
// CIAUpdateDocument::~CIAUpdateDocument()
// Destructor
// ---------------------------------------------------------------------------
//
CIAUpdateDocument::~CIAUpdateDocument()
    {
    TInt err = RProperty::Delete(KPSUidNSmlDMSyncApp, KFotaDLStatus);
    }

// ----------------------------------------------------
// CIAUpdateDocument::CreateAppUiL()
// constructs CIAUpdateAppUi
// ----------------------------------------------------
//
CEikAppUi* CIAUpdateDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CIAUpdateAppUi;
    return appUi;
    }
 
// End of File  
