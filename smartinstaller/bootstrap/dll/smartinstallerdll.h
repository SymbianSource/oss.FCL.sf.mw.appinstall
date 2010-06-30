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

#include <e32base.h>

// Forward declarations
class RCmManager;

class MHelperWrapper
	{
public:
	virtual TUint GetDefaultIapForSNAPL(const TUint aSNAPId) = 0;
	virtual void GetSnapIapsL(const TUint aSNAPId, RArray<TUint32>& aIapArray) = 0;
	};

class CHelper : public CBase,
	public MHelperWrapper
	{
public:
	IMPORT_C static CHelper* NewL();
	IMPORT_C static CHelper* NewLC();
	IMPORT_C ~CHelper();

	TUint GetDefaultIapForSNAPL(const TUint aSNAPId);
	void GetSnapIapsL(const TUint aSNAPId, RArray<TUint32>& aIapArray);

private:
	CHelper();
	void ConstructL();
	};
