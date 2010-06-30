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

#include "globalqueryobserver.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CGlobalQueryObserver::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlobalQueryObserver*
		CGlobalQueryObserver::NewL(MGlobalQueryClient* aClient, const TBool aIgnoreResponse)
	{
	CGlobalQueryObserver* self =
		new ( ELeave ) CGlobalQueryObserver(aClient, aIgnoreResponse);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
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
CGlobalQueryObserver::CGlobalQueryObserver(MGlobalQueryClient* aClient, const TBool aIgnoreResponse) :
		CActive( EActivePriorityDefault ),
		iClient( aClient ),
		iIgnoreResponse( aIgnoreResponse )
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
	if ( !iIgnoreResponse )
		{
		iClient->HandleGlobalQueryResponseL( iStatus.Int() );
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
