/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32uid.h>
#include <e32base.h>
#include <e32test.h>
#include <apadef.h>


//const TInt KTexeUidValue=0x147;
//const TUid KUidTExe={KTexeUidValue};

//
// Global functions
//


void DoThings(const TDes &aCommandLine)
	{
	RTest test(_L("TEXE"));
	test.Title();
	test.Start(_L(" @SYMTestcaseID API-SEC-TEXE-0001 The test exe has been loaded."));
	test.Printf(_L("\n"));
	test.Printf(aCommandLine);
	test.Printf(_L("\n"));
	test.End();
	test.Close();
	}

//
// dll/exe bits
//

/*
#if defined(__WINS__)

EXPORT_C TInt EntryPoint(TAny* aParam)
	{
	__UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	//
	TApaCommandLine& docName=*(TApaCommandLine*)aParam;
    DoThings(docName);
	//
    delete(cleanup);
	__UHEAP_MARKEND;
    return(0);
	}


GLDEF_C TInt E32Dll(TDllReason)
	{
	return(KErrNone);
	}

#else
*/
GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CTrapCleanup *cleanup=CTrapCleanup::New();
	//
	TBuf<0x100> commandline;
	User::CommandLine(commandline);
	DoThings(commandline);
	//
	delete(cleanup);
	__UHEAP_MARKEND;
	return(0);
}


