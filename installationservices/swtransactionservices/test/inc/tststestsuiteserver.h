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


/**
 @file
 @test
 @internalComponent
*/

#ifndef __TSTSTESTSUITESERVER_H__
#define __TSTSTESTSUITESERVER_H__
#include <test/testexecuteserverbase.h>
#include <f32file.h> 
#include <usif/sts/sts.h>
#include "testutilclientswi.h"

using namespace Usif;

class CStsTestServer : public CTestServer
	{
	public:
	    //general test framework related stuff
		static CStsTestServer* NewL();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		~CStsTestServer();

		//STS specific functions
        TStsTransactionId CreateTransactionL(TInt aPos);
        void OpenTransactionL(TInt aPos, TStsTransactionId aTransactionID);
        void CloseTransactionL(TInt aPos);
		void RegisterNewL(TInt aPos, const TDesC& aFileName);
		void CreateNewL(TInt aPos, const TDesC& aFileName, RFile &aFile, TUint aFileMode);
		void RemoveL(TInt aPos, const TDesC& aFileName);
		void RegisterTemporaryL(TInt aPos, const TDesC& aFileName);
		void CreateTemporaryL(TInt aPos, const TDesC& aFileName, RFile &aFile, TUint aFileMode);
		void OverwriteL(TInt aPos, const TDesC& aFileName, RFile &aFile, TUint aFileMode);
		void CommitL(TInt aPos);
		void RollBackL(TInt aPos);
	    void RollbackAllPendingL(TInt aPos);

	    TStsTransactionId GetTransactionIDL(TInt aPos);

		RTestUtilSessionSwi& GetTestUtil() {return iTestutil;}
		
	private:
		RStsSession& GetSessionL(TInt aPos);
		void ConstructL();

	private:
		static const TInt 							maxNumberOfSessions=10;
		RStsSession 					            iSessions[maxNumberOfSessions];
		RTestUtilSessionSwi                            iTestutil; //for file manipulation in protected directories as well
	};
#endif
