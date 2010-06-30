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
*     Wrapper DLL
*
*
*/

#include <cmmanager.h>      // @since 3.2
#include <cmdestination.h>  // @since 3.2

#include "smartinstallerdll.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHelper* CHelper::NewL()
	{
	CHelper* self = CHelper::NewLC();
	CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHelper* CHelper::NewLC()
	{
	CHelper* self = new (ELeave) CHelper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// Default destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CHelper::~CHelper()
	{
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CHelper::GetDefaultIapForSNAPL(const TUint aSNAPId)
	{
	RCmManager cmManager;
	TUint iap( 0 );

	cmManager.OpenLC();
	RCmDestination dest( cmManager.DestinationL( aSNAPId ) );
	CleanupClosePushL( dest );
	if ( dest.ConnectionMethodCount() <= 0 )
		{
		User::Leave( KErrNotFound );
		}

	RCmConnectionMethod meth( dest.ConnectionMethodL(0) );
	CleanupClosePushL(meth);
	iap = meth.GetIntAttributeL(CMManager::ECmIapId);
	CleanupStack::PopAndDestroy( 3, &cmManager ); // cMeth, dest, cmManager

	return iap;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHelper::GetSnapIapsL(const TUint aSNAPId, RArray<TUint32>& aIapArray)
	{
	RCmManager cmManager;
	cmManager.OpenLC();

	RCmDestination dest = cmManager.DestinationL( aSNAPId );
	CleanupClosePushL(dest);
	const TInt count = dest.ConnectionMethodCount();
	if ( count <= 0 )
		{
		User::Leave(KErrNotFound);
		}
	// Fill the array with IAP IDs. We're assuming they are in priority order.
	for (TInt i = 0; i < count; i++)
		{
		RCmConnectionMethod meth( dest.ConnectionMethodL(i) );
		CleanupClosePushL(meth);
		aIapArray.AppendL( meth.GetIntAttributeL(CMManager::ECmIapId) );
		CleanupStack::PopAndDestroy(&meth);
		}
	CleanupStack::PopAndDestroy( 2, &cmManager ); // destination, cmManager
	}

// ---------------------------------------------------------------------------
// Default 2nd level constructor
// ---------------------------------------------------------------------------
//
void CHelper::ConstructL()
	{
	}

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CHelper::CHelper()
	{
	}
