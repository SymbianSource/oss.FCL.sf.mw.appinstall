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
*     CBootstrapDocument class implementation.
*
*
*/


// INCLUDE FILES
#include "BootstrapAppUi.h"
#include "BootstrapDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBootstrapDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBootstrapDocument* CBootstrapDocument::NewL( CEikApplication& aApp )
	{
	CBootstrapDocument* self = NewLC( aApp );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CBootstrapDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBootstrapDocument* CBootstrapDocument::NewLC( CEikApplication&
															 aApp )
	{
	CBootstrapDocument* self =
		new ( ELeave ) CBootstrapDocument( aApp );

	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CBootstrapDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBootstrapDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CBootstrapDocument::CBootstrapDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CBootstrapDocument::CBootstrapDocument( CEikApplication& aApp )
	: CAknDocument( aApp )
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CBootstrapDocument::~CBootstrapDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CBootstrapDocument::~CBootstrapDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CBootstrapDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CBootstrapDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return ( static_cast <CEikAppUi*> ( new ( ELeave )
										CBootstrapAppUi ) );
	}

// End of File
