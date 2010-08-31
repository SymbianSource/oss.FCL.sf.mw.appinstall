/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the CContentProvider
* Life-Cycle methods
*
*/


/**
 @file siscontentprovider.cpp
*/

#include "siscontentprovider.h"
#include "siscontroller.h" 
#include "sissupportedlanguages.h"
#include "sislanguage.h"
#include "sisstring.h"
#include "sisinfo.h"
#include "sissupportedoptions.h"
#include "sissupportedoption.h"
#include "sisversion.h"
#include "sisinstallblock.h"
#include "swi/sisinstallerrors.h"
#include "siselseif.h"

using namespace Swi;
using namespace Swi::Sis;

/*static*/ CContentProvider* CContentProvider::NewLC(const CController& aController)
	{
	CContentProvider* self = new(ELeave) CContentProvider(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/*static*/ CContentProvider* CContentProvider::NewL(const CController& aController)
	{
	CContentProvider* self = CContentProvider::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CContentProvider::CContentProvider(const CController& aController) : iController(aController)
	{
	}

void CContentProvider::ConstructL()
	{
	const CSupportedLanguages& supportedLanguages = iController.SupportedLanguages();
	iAvailableLanguages.Reset();
	
    // builds a list of TLanguage of available languages from SISX file
	for(TInt language = 0; language < supportedLanguages.Count(); language++)
		{
		User::LeaveIfError(iAvailableLanguages.Append(supportedLanguages[language]));
		}
	}

CContentProvider::~CContentProvider()
	{
	iAvailableLanguages.Close();
	}

// Business methods

TAppInfo CContentProvider::DefaultLanguageAppInfoL() const
	{
	return DefaultLanguageAppInfoL(iController.Info());
	}

TAppInfo CContentProvider::LocalizedAppInfoL(TInt aLanguageIndex) const
	{
	return LocalizedAppInfoL(iController.Info(), aLanguageIndex);
	}

TAppInfo CContentProvider::DefaultLanguageAppInfoL(const CInfo& aInfo) const
	{
	TLanguage current = User::Language();    // Language of the current locale
	
	// Check whether the current locale is in the list of available languages
	TInt idx = iAvailableLanguages.Find(current);

	if (idx == KErrNotFound) 
		{
		idx = 0;
		}
	return LocalizedAppInfoL(aInfo, idx);
	}

TAppInfo CContentProvider::LocalizedAppInfoL(const CInfo& aInfo, TInt aLanguageIndex) const
	{
	const RPointerArray<CString>& names = aInfo.Names();

	const RPointerArray<CString>& vendors = aInfo.VendorNames();

	// These two things can leave if the language specified is not present
	if (names.Count() <= aLanguageIndex)
		{
		User::Leave(KErrSISInfoSISNamesMissing);
		}
	CString* localizedName = names[aLanguageIndex];
	
	if (vendors.Count() <= aLanguageIndex)
		{
		User::Leave(KErrSISInfoSISVendorNamesMissing);		
		}
	CString* localizedVendor = vendors[aLanguageIndex];

	const CVersion& version = aInfo.Version();
	TVersion ver(version.Major(), version.Minor(), version.Build());

	TAppInfo appaInfo = TAppInfo(localizedName->Data(), localizedVendor->Data(), ver);

	return appaInfo;
	}

TInt64 CContentProvider::TotalSizeL(const CInstallBlock&  aInstallBlock,
									CExpressionEvaluator* aCreateExpressionEvaluator,
									TBool aEmbedded) const
	{	
	// Calculate the total size of the SIS file descriptions in this SISInstallBlock.
	TInt64 sizeOfInstallationBelowController = SizeOfFileDescriptionsInInstallBlock(aInstallBlock);
	
	// Process the expressions.
	for (TInt i=0 ; i < aInstallBlock.IfStatements().Count(); i++)
		{
		Sis::CIf* ifBlock=aInstallBlock.IfStatements()[i];
		
		if (!ifBlock)
			{
			User::Leave(KErrCorrupt);
			}
		
		// Main expression.
		if (aCreateExpressionEvaluator->EvaluateL(ifBlock->Expression()).BoolValueL())
			{
			sizeOfInstallationBelowController += TotalSizeL(ifBlock->InstallBlock(),
															aCreateExpressionEvaluator,
															aEmbedded);
			}
		else
			{
			// There are Else If blocks.
			const RPointerArray<CElseIf>& elseIfs=ifBlock->ElseIfs();
		
			for (TInt j = 0 ; j < elseIfs.Count(); j++)
				{				
				if (!elseIfs[j])
				 	{
					User::Leave(KErrCorrupt);
					}
				
				if (aCreateExpressionEvaluator->EvaluateL(elseIfs[j]->Expression()).BoolValueL())
					{
					sizeOfInstallationBelowController += TotalSizeL(elseIfs[j]->InstallBlock(),
																	aCreateExpressionEvaluator,
																	aEmbedded);
					break;	// stop processing else if blocks
					}
				}
			}
		}

	if(aEmbedded)
		{
		// Embedded controllers in this SISInstallBlock.
		for (TInt k=0 ; k < aInstallBlock.EmbeddedControllers().Count(); k++)
			{
			if (aInstallBlock.EmbeddedControllers()[k]==NULL)
				{
				User::Leave(KErrCorrupt);
				}
			sizeOfInstallationBelowController += TotalSizeL(aInstallBlock.EmbeddedControllers()[k]->InstallBlock(),
														aCreateExpressionEvaluator,
														aEmbedded);
			}
		}
	return sizeOfInstallationBelowController;
	}
	
RPointerArray<TDesC> CContentProvider::LocalizedOptionsL(TInt aLanguageIndex) const
	{
	const CSupportedOptions& options= iController.SupportedOptions();

	// Now get the options and localize them...
	RPointerArray<TDesC> ret;
	CleanupClosePushL(ret);

	for (TInt option = 0; option < options.Count(); option++)
		{
		const RPointerArray<CString>& names = options[option].Names();
		ret.AppendL(&names[aLanguageIndex]->Data());
		}

	CleanupStack::Pop(&ret);
	return ret;
	}

TInt64 CContentProvider::SizeOfFileDescriptionsInInstallBlock(const CInstallBlock& aInstallBlock) const
	{
	// Get all the SIS file descriptions in this SISInstallBlock.
	const RPointerArray<CFileDescription>& files=aInstallBlock.FileDescriptions();
  	
	TInt64 totalSizeOfFileDescriptions = 0;

	// Calculate the size of all the SIS file descriptions in this SISInstallBlock.
	for (TInt l=0; l < files.Count(); l++)
		{
		CFileDescription& sisFileDescription = (*files[l]);
		totalSizeOfFileDescriptions += sisFileDescription.UncompressedLength();
		}
	return totalSizeOfFileDescriptions;
	}
