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
*     CBootstrapApplication class implementation.
*
*
*/


// INCLUDE FILES
#include "bootstrapconst.h"
#include "bootstrapdocument.h"
#include "bootstrapapplication.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CBootstrapApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CBootstrapApplication::CreateDocumentL()
	{
	// Create an Bootstrap document, and return a pointer to it
	return (static_cast<CApaDocument*>
					( CBootstrapDocument::NewL( *this ) ) );
	}

// -----------------------------------------------------------------------------
// CBootstrapApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CBootstrapApplication::AppDllUid() const
	{
	// Return the UID for the Bootstrap application
	return KUidBootstrapApp;
	}

// End of File
