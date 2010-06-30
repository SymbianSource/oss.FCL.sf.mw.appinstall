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
*     CGlobalQueryObserver implementation
*
*
*/


//  INCLUDES
#include <eikenv.h>
#include <e32base.h>

#include "ADMglobalqueryobserver.h"
#include "ADMappui.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlobalQueryObserver*
		CGlobalQueryObserver::NewL(CADMAppUi* aCBootstrapAppUiObj)
	{
	CGlobalQueryObserver* active =
		new ( ELeave ) CGlobalQueryObserver(aCBootstrapAppUiObj);
	CleanupStack::PushL( active );
	active->ConstructL();
	CleanupStack::Pop();
	return active;
	}


// ----------------------------------------------------------------------------
// CGlobalQueryObserver::~CGlobalQueryObserver()
// Destructor.
// ----------------------------------------------------------------------------
//
CGlobalQueryObserver::~CGlobalQueryObserver()
	{
	Cancel();
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::Start()
// Start the activator.
// ----------------------------------------------------------------------------
//
void CGlobalQueryObserver::Start()
	{
	SetActive();
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::CGlobalQueryObserver()
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CGlobalQueryObserver::CGlobalQueryObserver(CADMAppUi* aCBootstrapAppUiObj)
: CActive( EActivePriorityDefault ),
  iCBootstrapAppUiObj( aCBootstrapAppUiObj )
	{
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::ConstructL()
// Symbian default constructor can leave.
// ----------------------------------------------------------------------------
//
void CGlobalQueryObserver::ConstructL()
	{
	CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::RunL()
// Handle an event.
// ----------------------------------------------------------------------------
//
void CGlobalQueryObserver::RunL()
	{
	Cancel();
	if ( iSelectItemFlag )
		{
		iCBootstrapAppUiObj->HandleGlobalQueryResponseL( iStatus.Int() );
		}
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::DoCancel()
// Implement the Cancel protocol.
// ----------------------------------------------------------------------------
//
void CGlobalQueryObserver::DoCancel()
	{
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::SetSelectItemFlag()
// Set iSelectItemFlag.
// ----------------------------------------------------------------------------
//
void CGlobalQueryObserver::SetSelectItemFlag()
	{
	iSelectItemFlag = ETrue;
	}

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::SetSelectItemFlag()
// Set iSelectItemFlag.
// ----------------------------------------------------------------------------
//
void CGlobalQueryObserver::UnsetSelectItemFlag()
	{
	iSelectItemFlag = EFalse;
	}
