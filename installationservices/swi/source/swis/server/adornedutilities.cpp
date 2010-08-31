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
* Implementation of the adorned filename handling utility functions
*
*/


/**
 @file
*/

#include "adornedutilities.h"
#include "log.h"

_LIT(KAdornedWildCharString, "{????????}");
const TInt Swi::FileNameUnadornedPartLength = 10;

void Swi::GetUnadornedFileName(const TDesC& aAdornedFilename, TDes& aUnadornedFilename)
	{
	aUnadornedFilename = aAdornedFilename;
	// Check this name is not adorned, if it is remove version no
 	TInt startVersion = aAdornedFilename.Locate('{');
 	if (startVersion != KErrNotFound)
 	{ 		
		TInt endVersion = aAdornedFilename.Locate('}');
		if (( endVersion != KErrNotFound ) && (endVersion - startVersion == (Swi::FileNameUnadornedPartLength-1)))
			{
			// get the name without the version
			aUnadornedFilename = aAdornedFilename.Left(startVersion);
			//for names ending with } i.e: DummyFile{12345678} 
			//nothing is to be appended to aUnadornedFilename
			//in such a case aAdornedFilename.Mid(endVersion+1) would panic
			if(endVersion < (aAdornedFilename.Length()-1))
				{
				aUnadornedFilename.Append(aAdornedFilename.Mid(endVersion+1));
				}
			}
		}
	}

TBool Swi::IsAdornedVariationOfL(const TDesC& aFileName1, const TDesC& aFileName2)
	{
  	RBuf unadornedFileName1;
  	RBuf unadornedFileName2;
  
  	unadornedFileName1.CreateL(KMaxFileName);
  	CleanupClosePushL(unadornedFileName1);
  	unadornedFileName2.CreateL(KMaxFileName);
  	CleanupClosePushL(unadornedFileName2);
  
  	Swi::GetUnadornedFileName(aFileName1, unadornedFileName1);
  	Swi::GetUnadornedFileName(aFileName2, unadornedFileName2);
  	
  	//Checks whether filename2 is a variant of filename1
  	//e.g: d:\sys\bin\DummyDll{000A0001}.dll is considered a variant of c:\sys\bin\DummyDll.dll 
  	//because they both break down to \sys\bin\DummyDll.dll
  	TParsePtrC parsePtr1(unadornedFileName1);
  	TParsePtrC parsePtr2(unadornedFileName2);
	TBool ret = (parsePtr1.Path() == parsePtr2.Path()) && (parsePtr1.NameAndExt() == parsePtr2.NameAndExt());
  	CleanupStack::PopAndDestroy(2, &unadornedFileName1);
	return ret;
	}

void Swi::FindAllAdornedVariantsL(RFs& aFs, const TDesC& aSearchNameWild, const TDesC& aSearchPath, RPointerArray<HBufC>& aAdornedFileNamesFound)
	{
	TFindFile finder(aFs);
	CDir* dirList=0;
	TBool matchFound = (finder.FindWildByDir( aSearchNameWild, aSearchPath, *&dirList) == KErrNone);
	CleanupStack::PushL(dirList);
	
	while(matchFound)
		{
		TParsePtrC fileFound(finder.File());

		TInt count = dirList->Count();
		for(TInt d=0; d<count; ++d)
			{
			const TEntry& entry = (*dirList)[d];
			if(!entry.IsDir()) //dirlist theoretically might contain not only file names but also nested directory entries that we are not interested in
				{
				HBufC* adornedFileName=HBufC::NewLC(fileFound.DriveAndPath().Length()+entry.iName.Length());
				TPtr ptr = adornedFileName->Des();
				ptr.Append(fileFound.DriveAndPath());
				ptr.Append(entry.iName);
				aAdornedFileNamesFound.AppendL(adornedFileName);
				CleanupStack::Pop(adornedFileName);
				}
			}
		CleanupStack::PopAndDestroy(dirList);
		dirList = 0;
		matchFound = (finder.FindWild(dirList) == KErrNone);
		CleanupStack::PushL(dirList);
		}
	CleanupStack::PopAndDestroy(dirList);
	}

void Swi::GenerateSearchNameWildL(const TDesC& aFileName, TDes& aSearchNameWild)
	{
	RBuf unadornedFileName;
	unadornedFileName.CreateL(aFileName.Length()); //unadorned filename is never longer than aFileName
	unadornedFileName.CleanupClosePushL();
	Swi::GetUnadornedFileName(aFileName, unadornedFileName);
	TParsePtrC unadornedFileNameParse(unadornedFileName);
	
	aSearchNameWild = unadornedFileNameParse.Name();
	aSearchNameWild.Append(KAdornedWildCharString);
	aSearchNameWild.Append( unadornedFileNameParse.Ext() );
	CleanupStack::PopAndDestroy(&unadornedFileName);
	}
