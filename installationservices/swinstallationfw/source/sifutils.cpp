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
* This file implements the SifUtils library
*
*/


#include <usif/scr/scr.h>
#include <usif/sts/sts.h>

namespace Usif
	{
	EXPORT_C void UninstallL(TComponentId aComponentId)
		{
		// Connect to the SCR and start a transaction
		RSoftwareComponentRegistry scr;
		User::LeaveIfError(scr.Connect());
		CleanupClosePushL(scr);
		scr.CreateTransactionL();

		// Connect to the STS and start a transaction
		RStsSession sts;
		sts.CreateTransactionL();
		CleanupClosePushL(sts);

		// Get a list of files to be deleted
		RSoftwareComponentRegistryFilesList fileList;
		fileList.OpenListL(scr, aComponentId);
		CleanupClosePushL(fileList);

		// Unregister and delete the files from iFileList
		HBufC* file = NULL;
		while ((file = fileList.NextFileL()) != NULL)
			{
			CleanupStack::PushL(file);
			sts.RemoveL(*file);
			CleanupStack::PopAndDestroy(file);
			}
		CleanupStack::PopAndDestroy(&fileList);

		// Delete the component from the SCR
		scr.DeleteComponentL(aComponentId);

		// Commit the STS & SCR transactions
		sts.CommitL();
		scr.CommitTransactionL();

		CleanupStack::PopAndDestroy(2, &scr);
		}
	} // namespace Usif
