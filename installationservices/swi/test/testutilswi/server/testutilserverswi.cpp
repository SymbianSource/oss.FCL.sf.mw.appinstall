/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestUtilSwi - server implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/
 
#include "testutilserverswi.h"
#include "testutilsessionswi.h"
#include "testutilclientserverswi.h"

CServer2* CTestUtilServerSwi::NewLC()
	{
	CTestUtilServerSwi* self=new(ELeave) CTestUtilServerSwi;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CTestUtilServerSwi::~CTestUtilServerSwi()
	{
	iFs.Close();
	delete iFileMan;
	delete iTestUtilSessionCommon;
	}

void CTestUtilServerSwi::ConstructL()
	{
	StartL(KTestUtilServerName);
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();
	iFileMan = CFileMan::NewL(iFs);
	iTestUtilSessionCommon = new (ELeave) CTestUtilSessionCommon();
	}

CSession2* CTestUtilServerSwi::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new(ELeave) CTestUtilSessionSwi();
	}

void CTestUtilServerSwi::AddSession()
	{
	++iSessionCount;
	}

void CTestUtilServerSwi::DropSession()
	{
	--iSessionCount;
	}

void CTestUtilServerSwi::Send(const TDesC& aMessage)
	{
	iSessionIter.SetToFirst();
	while ((iSessionIter++)!=0)
		{
		iTestUtilSessionCommon->Send(aMessage);
		}
	}
