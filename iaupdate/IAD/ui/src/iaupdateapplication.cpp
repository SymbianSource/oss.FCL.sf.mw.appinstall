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
* Description:   This module contains the implementation of CIAUpdateApplication
*                class member functions.
*
*/



#include <eikstart.h>
#include "iaupdatedocument.h"
#include "iaupdateapplication.h"
#include "iaupdateserver.h"


// ---------------------------------------------------------
// CAppMngrApp::CreateDocumentL()
// Creates CIAUpdateDocumentt object
// ---------------------------------------------------------
//
CApaDocument* CIAUpdateApplication::CreateDocumentL()
    {  
    // Create an IAUpdate document, and return a pointer to it
    CApaDocument* document = CIAUpdateDocument::NewL(*this);
    return document;
    }


// ---------------------------------------------------------
// CIAUpdateApplication::AppDllUid
// Returns application UID
// ---------------------------------------------------------
//
TUid CIAUpdateApplication::AppDllUid() const
    {
    // Return the UID for the IAUpdate application
    return KUidIAUpdateApp;
    }

// ---------------------------------------------------------
// CIAUpdateApplication::NewAppServerL
// Cretaes an instance of applicayion server class
// ---------------------------------------------------------
//
void CIAUpdateApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = CIAUpdateServer::NewL();    
    }
    
	
// ================= OTHER EXPORTED FUNCTIONS ==============
//
// Create an application, and return a pointer to it
LOCAL_C CApaApplication* NewApplication()
    {
    return new CIAUpdateApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File  
