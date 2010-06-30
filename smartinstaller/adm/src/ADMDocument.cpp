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
*     CADMDocument implementation
*
*
*/



// INCLUDE FILES
#include "ADMAppUi.h"
#include "ADMDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CADMDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CADMDocument* CADMDocument::NewL(CEikApplication& aApp)
	{
	CADMDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CADMDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CADMDocument* CADMDocument::NewLC(CEikApplication& aApp)
	{
	CADMDocument* self = new (ELeave) CADMDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CADMDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CADMDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CADMDocument::CADMDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CADMDocument::CADMDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CADMDocument::~CADMDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CADMDocument::~CADMDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CADMDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CADMDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CADMAppUi;
	}

// End of File
