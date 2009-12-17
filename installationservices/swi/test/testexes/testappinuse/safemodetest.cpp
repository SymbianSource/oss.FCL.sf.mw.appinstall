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
* runwait.cpp
* This file is used to build 3 console exes. The idea of which is that if the 
* three are run in the right order one after the other then they will succeed.
* This is used to test that files can be launched in the correct order using 
* the RUNWAIT/RW option in a SIS file.
* The correct behaviour for each exe is controlled by the MMP files defining 
* one of RUNWAIT1, RUNWAIT2 or RUNWAIT3,
*
*/


/**
 @file
*/

#include <e32base.h>
#include <f32file.h>
#include <sacls.h>
#include <e32property.h>
#include <ecom/ecom.h>

#ifdef RUNWAIT1
TInt INDEX = 1;
#endif
#ifdef RUNWAIT2
TInt INDEX = 2;
#endif
#ifdef RUNWAIT3
TInt INDEX = 3;
#endif

_LIT(KTxtEPOC32EX,"runwait: mainL failed");

// private
LOCAL_C void mainL();

TInt E32Main() // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt error = 0;

	TRAP(error,mainL());

	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0;
	}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
LOCAL_C void mainL() 	
	{
	return;
	}
	
#else

_LIT(KFile1,"c:\\runwait1.txt");
_LIT(KFile2,"c:\\runwait2.txt");

LOCAL_C void mainL() 

	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	TEntry f1;
	TInt err1 = fs.Entry(KFile1,f1);
				
	TEntry f2;
	TInt err2 = fs.Entry(KFile2,f2);
				
	TInt val;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, val));
	RDebug::Printf("runwait: SWI P&S flag = %d", val);
		
	REComSession& ecom = REComSession::OpenL();
	RImplInfoPtrArray implInfoArray;
	
	ecom.ListImplementationsL(TUid::Uid(0x1028372), implInfoArray);
	TInt numPlugins = implInfoArray.Count();
	RDebug::Printf("runwait: found %d ECOM implementations for = %d", numPlugins, 0x1028372);
	implInfoArray.ResetAndDestroy();	
	implInfoArray.Close();
	ecom.Close();
	ecom.FinalClose();

	switch (INDEX)
		{
		case 1:
			RDebug::Printf("runwait: Running exe 1");
			if (err1 == KErrNone || err2 == KErrNone)
				{
				RDebug::Printf("runwait: Error - an unexpected file already exists");
				}
			else 
				{
				RFile file;
				CleanupClosePushL(file);
				err1 = file.Create(fs,KFile1,EFileStreamText|EFileWrite|EFileShareExclusive);
				RDebug::Printf("runwait: Created file1 with error:%d",err1);
				CleanupStack::PopAndDestroy();
				}
			break;
		case 2:
			RDebug::Printf("runwait: Running exe 2");
			if (err1 != KErrNone)
				{
				RDebug::Printf("runwait: file1 does not seem to exist");
				}
			else if (err2 == KErrNone)
				{
				RDebug::Printf("runwait: Error - an unexpected file already exists");
				}
			else 
				{
				RFile file;
				CleanupClosePushL(file);
				err2 = file.Create(fs,KFile2,EFileStreamText|EFileWrite|EFileShareExclusive);
				RDebug::Printf("runwait: Created file2 with error:%d",err2);
				CleanupStack::PopAndDestroy();
				}
			break;
		case 3:
			{
			TBuf<40> privatePath;
			fs.PrivatePath(privatePath);
			TBuf<180> fileName;
			_LIT(KNameFormat, "%c:%S%S");	
			_LIT(KShortFile, "runwait3.txt");
			fileName.Format(KNameFormat, fs.GetSystemDriveChar().GetLowerCase(),
					&privatePath, &KShortFile); 

			TEntry f3;
			TInt err3 = fs.Entry(fileName,f3);
			RDebug::Printf("runwait: Running exe 3");

			if (err1 != KErrNone || err2 != KErrNone )
				{
				RDebug::Printf("runwait: file1 or file2 does not seem to exist");
				}
			else if (err3 == KErrNone )
				{
				RDebug::Printf("runwait: Error - file3 already exists");
				}
			else 
				{
				RFile file;
				CleanupClosePushL(file);

				err3 = fs.CreatePrivatePath(fs.GetSystemDrive());

				if (err3 == KErrNone || err3 == KErrAlreadyExists)
					{
					err3 = file.Create(fs,fileName,EFileStreamText|EFileWrite|EFileShareExclusive);
					RDebug::Printf("runwait: Created file3 with error:%d",err2);
					fs.Delete(KFile1);
					fs.Delete(KFile2);
					}
				CleanupStack::PopAndDestroy();

				}
			}
			break;
		default:
			RDebug::Printf("runwait: Unknown INDEX, suspect mmp problem.");
		}


	CleanupStack::PopAndDestroy(); // fs
	}
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
