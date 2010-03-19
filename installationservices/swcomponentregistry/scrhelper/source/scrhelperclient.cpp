/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32cmn.h>
#include "scrhelperclient.h"

using namespace Usif;

EXPORT_C RScrHelper::RScrHelper()
	:	RScsClientBase()
	{
	// empty
	}

EXPORT_C TInt RScrHelper::Connect()
	{
	DEBUG_PRINTF(_L8("Connecting to SCR Helper Server!"));
	TVersion version = ScrHelperServerVersion();
	TUidType fullUid = ScrHelperServerImageFullUid();		
	return RScsClientBase::Connect(KScrHelperServerName(), version, KScrHelperServerImg(), fullUid);
	}

EXPORT_C void RScrHelper::Close()
	{
	DEBUG_PRINTF(_L8("Closing SCR Helper connection!"));
	RScsClientBase::Close();
	}

EXPORT_C void RScrHelper::RetrieveFileHandlesL(RFile& aScrDatabase, RFile& aScrJournal)
	{
	DEBUG_PRINTF(_L8("Retrieving  database and journal file handles from SCR Helper Server!"));
	// First, retrieve database file handle
	GetFileHandleL(EGetDatabaseFileHandle, aScrDatabase);
	// Then, retrieve journal file handle
	GetFileHandleL(EGetJournalFileHandle, aScrJournal);
	}

void RScrHelper::GetFileHandleL(Usif::TScrHelperServerMessages aFunction, RFile& aFile)
	{
	// Retrieve the RFs and RFile handles from the server
	TInt fsh;          	 // File server handle (RFs - session)
	TPckgBuf<TInt> fh;   // File handle (RFile - subsession)
		
	fsh = CallSessionFunction(aFunction, TIpcArgs(&fh));   // pointer to fh in slot 0
	User::LeaveIfError(fsh);
	
	// Adopt the file using the returned handles
	User::LeaveIfError(aFile.AdoptFromServer(fsh, fh()));
	}
