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
* Definition of the Swi::Sis::CController
*
*/


/**
 @file siscontroller.cpp
*/

#include "siscontroller.h"
#include "sisinfo.h"
#include "sissupportedlanguages.h"
#include "sissupportedoptions.h"
#include "sisproperties.h"
#include "sislogo.h"
#include "sisinstallblock.h"
#include "sisproperties.h"
#include "sisprerequisites.h"
#include "swi/sistruststatus.h"
#include "certchainconstraints.h"
#include "sisptrprovider.h"
#include "log.h"

using namespace Swi::Sis;


EXPORT_C /*static*/ CController* CController::NewLC(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = new(ELeave) CController();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CController* CController::NewL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C /*static*/ CController* CController::NewLC(MSisDataProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = new(ELeave) CController();
	CleanupStack::PushL(self);
	TInt64 bytesRead=0;
	self->ConstructL(aDataProvider, bytesRead, aTypeReadBehaviour);
	return self;
	}

EXPORT_C /*static*/ CController* CController::NewL(MSisDataProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = NewLC(aDataProvider, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CController::CController() : iTrust(ESisPackageUnsignedOrSelfSigned)
	{
	}
	
EXPORT_C CController* CController::NewLC(TPtrProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = new(ELeave) CController();
	CleanupStack::PushL(self);
	TInt64 bytesRead(0);
	self->ConstructL(aDataProvider, bytesRead, aTypeReadBehaviour);
	return self;
	}
	
EXPORT_C CController* CController::NewL(TPtrProvider& aDataProvider, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = NewLC(aDataProvider, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CController* CController::NewLC(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = new(ELeave) CController();
	CleanupStack::PushL(self);
	self->ConstructL(aDataProvider, aBytesRead, aTypeReadBehaviour);
	return self;
	}
	
EXPORT_C CController* CController::NewL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	CController* self = NewLC(aDataProvider, aBytesRead, aTypeReadBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CController::~CController()
	{
	delete iInfo;
	delete iInstallBlock;
	delete iLanguages;
	delete iSupportedOptions;
	delete iProperties;
	delete iLogo;
	delete iPrerequisites;
	delete iDataIndex;
	delete iTrustStatus;
	delete iCertChainConstraints;	
	iCertChainIndices.Close();
    iSignatureCertificateChains.ResetAndDestroy();
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iApplicationInfo.ResetAndDestroy();
	#endif
	}

void CController::ConstructL(MSisDataProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	iDataOffset=aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeController, aBytesRead, aTypeReadBehaviour);

 	iInfo=CInfo::NewL(aDataProvider, aBytesRead);
 	
	iSupportedOptions=CSupportedOptions::NewL(aDataProvider, aBytesRead);

	iLanguages=CSupportedLanguages::NewL(aDataProvider, aBytesRead);

	iPrerequisites=CPrerequisites::NewL(aDataProvider, aBytesRead);

	iProperties=CProperties::NewL(aDataProvider, aBytesRead);

	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);
	
	if (fieldType == EFieldTypeLogo)
		{
		// Logo present
		iLogo=CLogo::NewL(aDataProvider, aBytesRead, EAssumeType);
		iInstallBlock=CInstallBlock::NewL(aDataProvider, aBytesRead);
		}
	else if (fieldType == EFieldTypeInstallBlock)
		{
		// No logo present
		iInstallBlock=CInstallBlock::NewL(aDataProvider, aBytesRead, EAssumeType);
		}
	else
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	
	ReadMemberCollectionL(aDataProvider, iSignatureCertificateChains, EFieldTypeSignatureCertificateChain, EFieldTypeDataIndex, aBytesRead);
	
	iDataIndex=CDataIndex::NewL(aDataProvider, aBytesRead, EAssumeType);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - iDataOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - iDataOffset, aBytesRead, aTypeReadBehaviour);
	
	iTrustStatus = new (ELeave)Swi::TSisTrustStatus();

	// Create unconstrained cert chain constraints object; this may be replaced.
	iCertChainConstraints = CCertChainConstraints::NewL();
	
	iRemoveWithLastDependent = 0;
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iUserGrantableCapabilities.SetEmpty();
	#endif
	}
	
void CController::ConstructL(TPtrProvider& aDataProvider, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	iDataOffset=aBytesRead;
	
	CField::ConstructL(aDataProvider, EFieldTypeController, aBytesRead, aTypeReadBehaviour);

 	iInfo=CInfo::NewL(aDataProvider, aBytesRead);
 	
	iSupportedOptions=CSupportedOptions::NewL(aDataProvider, aBytesRead);

	iLanguages=CSupportedLanguages::NewL(aDataProvider, aBytesRead);

	iPrerequisites=CPrerequisites::NewL(aDataProvider, aBytesRead);

	iProperties=CProperties::NewL(aDataProvider, aBytesRead);

	TFieldType fieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, fieldType, aBytesRead);
	
	if (fieldType == EFieldTypeLogo)
		{
		// Logo present
		iLogo=CLogo::NewL(aDataProvider, aBytesRead, EAssumeType);
		iInstallBlock=CInstallBlock::NewL(aDataProvider, aBytesRead);
		}
	else if (fieldType == EFieldTypeInstallBlock)
		{
		// No logo present
		iInstallBlock=CInstallBlock::NewL(aDataProvider, aBytesRead, EAssumeType);
		}
	else
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	
	ReadMemberCollectionL(aDataProvider, iSignatureCertificateChains, EFieldTypeSignatureCertificateChain, EFieldTypeDataIndex, aBytesRead);
	
	iDataIndex=CDataIndex::NewL(aDataProvider, aBytesRead, EAssumeType);
	
	CField::SkipUnknownFieldsL(aDataProvider, aBytesRead - iDataOffset, aBytesRead, aTypeReadBehaviour);
	CField::EnsureAlignedL(aDataProvider, aBytesRead - iDataOffset, aBytesRead, aTypeReadBehaviour);
	
	iTrustStatus = new (ELeave)Swi::TSisTrustStatus();

	// Create unconstrained cert chain constraints object; this may be replaced.
	iCertChainConstraints = CCertChainConstraints::NewL();
	
	iRemoveWithLastDependent = 0;
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iUserGrantableCapabilities.SetEmpty();
	#endif
	}	

EXPORT_C void CController::AddChainIndex(TInt aIndex) const
    {
	iCertChainIndices.Append(aIndex);
    }

EXPORT_C void CController::ClearCertChainIndices() const
    {
	iCertChainIndices.Reset();
    }

EXPORT_C void CController::SetCertChainConstraints(CCertChainConstraints* aConstraints)
 	{
 	// Delete existing cert chain constraints object.
 	delete iCertChainConstraints;
 	iCertChainConstraints=aConstraints;
 	}
 	
EXPORT_C TInt CController::RemoveWithLastDependent() const
	{
	return iRemoveWithLastDependent;
	}
	 	
EXPORT_C void CController::SetRemoveWithLastDependent()
	{
	iRemoveWithLastDependent = 1;
	}


EXPORT_C CMessageDigest* CController::GenerateControllerHashLC(const TDesC8& aController)
	{
	TInt64 infoLength = iInfo->Length() + iInfo->HeaderSize() + iInfo->PaddingSize();
	TInt64 installBLength = iInstallBlock->Length() + iInstallBlock->HeaderSize() + iInstallBlock->PaddingSize();	
	TInt64 supportLLength = iLanguages->Length() + iLanguages->HeaderSize() + iLanguages->PaddingSize();
	TInt64 supportOLength = iSupportedOptions->Length() + iSupportedOptions->HeaderSize() + iSupportedOptions->PaddingSize();
	TInt64 propertiesLength = iProperties->Length() + iProperties->HeaderSize() + iProperties->PaddingSize();
	TInt64 prerequisitesLength = iPrerequisites->Length() + iPrerequisites->HeaderSize() + iPrerequisites->PaddingSize();
	TInt64 logoLength = 0;
	
	if (iLogo)
		{
		logoLength = iLogo->Length() + iLogo->HeaderSize() + iLogo->PaddingSize();
		}

   	TInt64 controllerLength = 0;
	controllerLength = infoLength + installBLength + supportLLength + supportOLength + propertiesLength + prerequisitesLength + logoLength;

	// Length octets are either 4 or 8 bytes but since controllers are loaded into
	// bytes should ALWAYS be sufficient.
	TInt offset = 4;
	
	// Should really modify the length and not the offset skipping the length octets. 
	// This makes the controller hash independent of the signature chains.
	//
	// Unfortunately, this is fixed and changing it potentially breaks compatibility
	// E.g. If the firmware is upgraded without reformatting the system drive then SWI 
	// may think that an application on removable media has not been installed.
	controllerLength += (Length() > 0x80000000 ? 8 : 4);
		
	DEBUG_PRINTF4(_L8("GenerateControllerHashLC aController.Length()=%d, offset=%d, controllerLength=%d"), 
		aController.Length(), offset, controllerLength);
	
	if (offset + controllerLength >= aController.Length()) 
		{
		// Mid will panic if the lengths are outside the buffer.
		User::Leave(KErrCorrupt);
		}
	TPtrC8 ptr;
	ptr.Set(aController.Mid(offset, controllerLength));

	CMessageDigest* digest = CMessageDigestFactory::NewDigestLC(CMessageDigest::ESHA1);
	digest->Update(ptr);

	return digest;
	}
