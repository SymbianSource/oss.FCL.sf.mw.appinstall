/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* installswtypehelper.h
*
*/


#include <f32file.h>
#include <ct/rcpointerarray.h>
#include <swi/sisinstallerrors.h>
#include <usif/usifcommon.h>
#include "installswtypehelper.h"
#include "swtypereginfo.h"
#include "sislauncherclient.h"
#include "log.h"

namespace Swi
	{
	namespace InstallSoftwareTypeHelper
		{
		_LIT(KSwTypeRegDir, "\\resource\\usif\\leestore\\");
		_LIT(KSwTypeRegFileExt, ".xml");
		const TInt KSwTypeRegFileMaxSize = 0x3FFF; // 16kB

		TBool IsValidSwRegFileL(const TDesC& aFileName, TInt32 aAppUid)
			{
			// Check file path and extension
			TParsePtrC filename(aFileName);
			if (filename.Path().CompareF(KSwTypeRegDir) || filename.Ext().CompareF(KSwTypeRegFileExt))
				{
				return EFalse;
				}
			
			// Check file name
			TLex lex(filename.Name());
			TUint32 uid(0);
			TInt err = lex.Val(uid, EHex);
			if (err != KErrNone)
				{
				DEBUG_PRINTF3(_L8("Failed to convert file name %S to UID, err = %d\n"), &aFileName, err);
				User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
				}
			if (uid != static_cast<TUint32>(aAppUid))
				{
				DEBUG_PRINTF3(_L8("Registration file name %S doesn't match package UID %d\n"), &aFileName, uid);
				User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
				}
			
			return ETrue;
			}

		void ParseRegFileL(RFs& aFs, const TDesC& aFileName, RPointerArray<Usif::CSoftwareTypeRegInfo>& aInfoArray)
			{
			// Open the file for reading
			RFile file;
			TInt err = file.Open(aFs, aFileName, EFileShareReadersOnly);
			if (err != KErrNone)
				{
				DEBUG_PRINTF3(_L("Failed to open the file %S for reading, err = %d\n"), &aFileName, err);
				User::Leave(err);
				}
			CleanupClosePushL(file);

			// Check the size of the registration file
			TInt size = 0;
			err = file.Size(size);
			if (err != KErrNone)
				{
				DEBUG_PRINTF3(_L("Failed to get the size of the file %S for reading, err = %d\n"), &aFileName, err);
				User::Leave(err);
				}
			if (size > KSwTypeRegFileMaxSize)
				{
				DEBUG_PRINTF3(_L8("Software type registration file is too big. Current size: %d, max size: %d\n"), size, KSwTypeRegFileMaxSize);
				User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
				}

			// Use SISLauncher to parse the xml registration file
			RSisLauncherSession launcher;
			CleanupClosePushL(launcher);
			err = launcher.Connect();
			if (err != KErrNone)
				{
				DEBUG_PRINTF2(_L8("Failed to connect to the SISLauncher, err = %d\n"), err);
				User::Leave(err);
				}
			launcher.ParseSwTypeRegFileL(file, aInfoArray);
			
			// Verify that the native type is not claimed - this would be a security violation
			for (TInt i = 0; i < aInfoArray.Count(); ++i)
				{
				const TDesC& uniqueTypeName = aInfoArray[i]->UniqueSoftwareTypeName();
				if (uniqueTypeName == Usif::KSoftwareTypeNative)
					{
					DEBUG_PRINTF(_L8("Software type registration file attempts changing the registation for the native installer\n"));
					User::Leave(KErrSecurityError);
					}
				}			

			CleanupStack::PopAndDestroy(2, &file); // launcher
			}

		void RegisterMimeTypesL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aInfoArray)
			{
			const TInt numElems = aInfoArray.Count();
			if (numElems == 0)
				{
				return;
				}

			// Create a list of MIME types
			RCPointerArray<HBufC8> mimeTypes;
			CleanupClosePushL(mimeTypes);
			SoftwareTypeRegInfoUtils::ExtractMimeTypesL(aInfoArray, mimeTypes);

			// Use SISLauncher to register MIME types for SIFLauncher
			RSisLauncherSession launcher;
			CleanupClosePushL(launcher);
			TInt err = launcher.Connect();
			if (err != KErrNone)
				{
				DEBUG_PRINTF2(_L8("Failed to connect to the SISLauncher, err = %d\n"), err);
				User::Leave(err);
				}
			launcher.RegisterSifLauncherMimeTypesL(mimeTypes);

			CleanupStack::PopAndDestroy(2, &mimeTypes); // launcher
			}
		} // namespace InstallSoftwareTypeHelper
	} // namespace Swi
