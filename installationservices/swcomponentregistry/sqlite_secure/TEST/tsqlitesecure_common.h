// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef TSQLITESECURE_COMMON
#define TSQLITESECURE_COMMON

#include <e32std.h>

void Check1(TInt aValue, const char* aFile, TInt aLine);
void Check2(TInt aValue, TInt aExpected, const char* aFile, TInt aLine);

#define TEST(arg) ::Check1((arg), __FILE__, __LINE__)
#define TEST2(aValue, aExpected) ::Check2(aValue, aExpected, __FILE__, __LINE__)

void DoTest(RFile& aMainDb, RFile& aJournal, const TDesC& aProcessParamter = KNullDesC);

#endif// TSQLITESECURE_COMMON
