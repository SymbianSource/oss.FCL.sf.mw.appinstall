/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <e32def.h>
#include <e32err.h>
#include <f32file.h>
#include <e32debug.h>

namespace 
{
	_LIT(KSwiCertstoreDirWildCard, "\\Resource\\SwiCertstore\\*");
}

/*Ensure that the temporary file is cleaned up if a leave
 *	occurs.
 */ 
void CleanUpL(void)
	{
	RFs rfssession;
	User::LeaveIfError(rfssession.Connect());
	
	CFileMan *cfm(CFileMan::NewL(rfssession));
	CleanupStack::PushL(cfm);
	
	TDriveUnit sysDrive (rfssession.GetSystemDrive());
	TBuf<128> swiCertstoreDirWildCard (sysDrive.Name());
	swiCertstoreDirWildCard.Append(KSwiCertstoreDirWildCard);

	TInt status(cfm->Delete(swiCertstoreDirWildCard, 
							CFileMan::ERecurse));

	if (KErrNotFound!= status and KErrNone != status and KErrPathNotFound != status)
		{
		User::Leave(status);
		}

	CleanupStack::PopAndDestroy(cfm);
	rfssession.Close();
	}

/*Main.
 *
 * @return A status code.
 */
TInt E32Main(void)
	// No arguments are permitted
	{
	__UHEAP_MARK;
	
	const CTrapCleanup *const stack(CTrapCleanup::New());
	TInt result(KErrNoMemory);
	
	if (stack)
		{
		TRAP(result, CleanUpL());
		
		delete stack;
		}
	
	__UHEAP_MARKEND;

	return result;
	}
