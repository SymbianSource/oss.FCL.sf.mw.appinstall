/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*   Static utility methods
*/

#include <e32std.h>
#include <e32debug.h>
#include <centralrepository.h>
#include "utils.h"

const TInt KCenRepBufferSize = 255;
const TText KStrComma = ',';
const TUint32 KMenuHideApplication = 0x00000008;

// Ovi Store launcher UID
const TInt KUidOviStoreLauncher = 0x0;
// Ovi Store widget UID
const TInt KUidOviStoreWidget = 0x0;
// Ovi Store downloader UID
const TInt KUidOviStoreDownloader = 0x0;
// Ovi Store ASServer UID
const TInt KUidOviStoreASServer = 0x0;

// -----------------------------------------------------------------------------
// Check if Ovi Store client is running
// @return ETrue, if the process is running
// -----------------------------------------------------------------------------
//
TBool CUtils::OviStoreRunning()
	{
	TBool running = EFalse;
	RArray<TInt> uids( 4 );
	uids.Append(KUidOviStoreLauncher);
	uids.Append(KUidOviStoreWidget);
	uids.Append(KUidOviStoreDownloader);
	uids.Append(KUidOviStoreASServer);
	if ( CUtils::AreProcessesRunning(uids) != KErrNotFound )
		{
		running = ETrue;
		}
	uids.Close();
	return running;
	}

// -----------------------------------------------------------------------------
// Checks if the process with given UID is running.
// @param aUid UID of the process
// @return ETrue, if the process is running
// -----------------------------------------------------------------------------
//
TBool CUtils::IsProcessRunning(const TInt aUid)
	{
	RArray<TInt> uids;
	uids.Append(aUid);
	const TBool ret = (AreProcessesRunning(uids) != KErrNotFound);
	uids.Close();
	return ret;
	}

// -----------------------------------------------------------------------------
// @param aUid UID of the process
// @return The index of the first matching UIDwithin the array.
//         KErrNotFound, if no matching entry can be found.
// -----------------------------------------------------------------------------
//
TInt CUtils::AreProcessesRunning(const RArray<TInt>& aUids)
	{
	TInt ret = KErrNotFound;
	TFileName res;
	TFindProcess find;
	//RDebug::Print(_L("Searching. 0x%08x"), &aUids)

	while (find.Next(res) == KErrNone)
		{
		RProcess ph;
		ph.Open(res);
		const TInt sid = ph.SecureId();
		//RDebug::Print(_L("Scanning process: '%S' (0x%08x)"), &res, sid);

		if ( (ret = aUids.Find(sid)) != KErrNotFound )
			{
			//RDebug::Print(_L("   Found process: '%S' %d (0x%08x)"), &res, ret, aUids[ret]);
			ph.Close();
			break;
			}
		ret = KErrNotFound;
/*
		if (ph.SecureId() == aUid)
			{
			RDebug::Print(_L("   Found process: '%S'"), &res);
			ph.Close();
			ret = ETrue;
			break;
			}
*/
		ph.Close();
		}

	return ret;
	}

// -----------------------------------------------------------------------------
// HideApplicationFromMenu() - Non-leaving version
//
// @param aUid UID of the application to hide/reveal from the menu
// @param aHidden Flag to indicate the application visibility. ETrue = Hide
// -----------------------------------------------------------------------------
//
TInt CUtils::HideApplicationFromMenu(TInt aUid, TBool aHidden)
	{
	TRAPD( ret, HideApplicationFromMenuL(aUid, aHidden) );
	return ret;
	}

// -----------------------------------------------------------------------------
// HideApplicationFromMenuL()
//
// @param aUid UID of the application to hide/reveal from the menu
// @param aHidden Flag to indicate the application visibility. ETrue = Hide
// -----------------------------------------------------------------------------
//
void CUtils::HideApplicationFromMenuL(TInt aUid, TBool aHidden)
	{
	const TUid KCRUidMenu = { 0x0 };

	TBuf<KCenRepBufferSize> keyContent;
	TBool updated( EFalse );

	CRepository* aAppShellRepository = CRepository::NewLC( KCRUidMenu );
	User::LeaveIfError(aAppShellRepository->Get( KMenuHideApplication, keyContent ));

	TBuf<8> MenuUid;
	UIDText( MenuUid, aUid);
	TInt offset = keyContent.FindC( MenuUid );
	if ( offset == KErrNotFound && aHidden )
		{
		__ASSERT_ALWAYS( keyContent.Length() + 9 < KCenRepBufferSize,
						 User::Leave( KErrOverflow ) );
		if ( keyContent.Length() > 0 )
			keyContent.Append( KStrComma );
		keyContent.Append( MenuUid );
		updated = ETrue;
		}
	else if ( offset != KErrNotFound && !aHidden )
		{
		if ( offset == 0 )
			{
			keyContent.Delete( 0, Min( 9, keyContent.Length() ) );
			updated = ETrue;
			}
		else if ( keyContent[offset - 1] == KStrComma )
			{
			keyContent.Delete( offset - 1, 9 );
			updated = ETrue;
			}
		else
			User::Leave( KErrCorrupt );
		}
	if ( updated )
		User::LeaveIfError(aAppShellRepository->Set( KMenuHideApplication, keyContent ));

	CleanupStack::PopAndDestroy(); // aAppShellRepository
	}

// -----------------------------------------------------------------------------
// UIDText - Converts UID specified by aInt to string representation
//
// @param aUid Descriptor containing the string
// @param aInt Int representing the UID
// -----------------------------------------------------------------------------
//
void CUtils::UIDText(TDes& aUid, TInt aInt)
	{
	TUidName uidName( TUid::Uid( aInt ).Name() );
	aUid.Copy( uidName.Mid( 1, KMaxUidName - 2 ) );
	aUid.UpperCase();
	}
