/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*     Main application class
*
*
*/



// INCLUDE FILES
#include "ADM.hrh"
#include "ADMDocument.h"
#include "ADMApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CADMApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CADMApplication::CreateDocumentL()
	{
	// Create an ADM document, and return a pointer to it
	return CADMDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CADMApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CADMApplication::AppDllUid() const
	{
	// Return the UID for the ADM application
	return KUidADMApp;
	}

// End of File
