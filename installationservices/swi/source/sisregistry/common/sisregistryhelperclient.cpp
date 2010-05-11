/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <s32mem.h>
#include "sisregistryhelperclient.h"

using namespace Swi;

EXPORT_C RSisRegistryHelper::RSisRegistryHelper()
	:	RScsClientBase()
	{
	// empty
	}

EXPORT_C TInt RSisRegistryHelper::Connect()
	{
	TVersion version = SisRegistryHelperServerVersion();
	TUidType fullUid = SisRegistryHelperServerImageFullUid();		
	return RScsClientBase::Connect(KSisRegistryHelperServerName(), version, KSisRegistryHelperServerImg(), fullUid);
	}

EXPORT_C void RSisRegistryHelper::Close()
	{
	RScsClientBase::Close();
	}

EXPORT_C void RSisRegistryHelper::GetEquivalentLanguagesL(TLanguage aLangId,RArray<TLanguage>& aEquivLangs)
	{
	CleanupClosePushL(aEquivLangs);
	// calculate the likely size of the data transfer buffer
	const TInt KMaxBufSize=
		sizeof(TInt)+                 // number of entries
		KMaxEquivalentLanguages*sizeof(TLanguage);  // Languages IDs stored as TLanguage
	
	// allocate buffer for the returned arrays
	HBufC8* buf=HBufC8::NewMaxLC(KMaxBufSize);
	TPtr8 pBuf=buf->Des();
	TInt err = CallSessionFunction(EGetEquivalentLanguages,TIpcArgs(&pBuf,aLangId));
	User::LeaveIfError(err);
	
	// got the buffer, internalise the arrays
	RDesReadStream ins(*buf);
	CleanupClosePushL(ins);

	// first comes the number of entries (TInt)
	TInt count=ins.ReadInt32L();
	
	// then language ID's
	TInt i;
	for (i = 0; i < count; ++i)
		{
		TLanguage langId=(TLanguage)ins.ReadInt32L();
		aEquivLangs.AppendL(langId);
		}
	// cleanup
	CleanupStack::PopAndDestroy(2, buf); // buf
	CleanupStack::Pop(&aEquivLangs);
	}
