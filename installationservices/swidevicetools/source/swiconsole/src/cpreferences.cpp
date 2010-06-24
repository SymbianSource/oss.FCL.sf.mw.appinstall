/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* CPreferences: This class is acts as an container for storing all the 
* options provided in the command line.
* @internalComponent
*
*/

 
 #include "cpreferences.h"
 #include "swiconsoleerrors.h"

CPreferences::CPreferences():
				iLanguage(ELangNone),
				iDrive(EKeyNull)
	{
	TInt i;
	for(i = 0; i < EDialogTypeCount; ++i)
		{
		iDialogResponses[i] = ENotAnswered;
		}
	for(i = 0; i < EEventTypeCount; ++i)
		{
		iEventResponses[i] = ENotAnswered;
		}
	return;
	}

CPreferences* CPreferences::NewL()
	{
	CPreferences* self = CPreferences::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CPreferences* CPreferences::NewLC()
	{
	CPreferences* self = new (ELeave) CPreferences;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CPreferences::~CPreferences()
	{
	iOptionList.Reset();
	iPackageNameList.ResetAndDestroy();
	iVendorNameList.ResetAndDestroy();
	}


// Checks whether an event is cancellable or not
TInt8 CPreferences::GetCancellableEventOption(TEventType aEventType) const
	{
	__ASSERT_DEBUG(static_cast<TUint>(aEventType) < EEventTypeCount, User::Panic(KGeneralPanicString, KInvalidArgument));
	
	return iEventResponses[aEventType];
	}

// Retrieves the options selected for a dialog
TInt8 CPreferences::GetDialogOption(TDialogType aDialogType) const
	{
	__ASSERT_DEBUG(static_cast<TUint>(aDialogType) < EDialogTypeCount, User::Panic(KGeneralPanicString, KInvalidArgument));
	
	return iDialogResponses[aDialogType];
	}

// Sets the response of dialogs
void CPreferences::SetDialogOption(
							TDialogType aDialogType, 
							TInt8 aResponse)
	{
	__ASSERT_DEBUG(static_cast<TUint>(aDialogType) < EDialogTypeCount, User::Panic(KGeneralPanicString, KInvalidArgument));
	
	iDialogResponses[aDialogType] = aResponse;
	return;
	}


// Sets the response of Cancellable event.
void CPreferences::SetCancellableEventResponse(
										TEventType aEventType, 
										TInt8 aResponse)
	{
	__ASSERT_DEBUG(static_cast<TUint>(aEventType) < EEventTypeCount, User::Panic(KGeneralPanicString, KInvalidArgument));
	
	iEventResponses[aEventType] = aResponse;
	return;
	}

void CPreferences::AddPackageNameL(const TDesC& aPackageName)
	{
	HBufC* ptr = aPackageName.AllocL();
	// Ownership transfered to the list
	iPackageNameList.AppendL(ptr);
	}

void CPreferences::AddVendorNameL(const TDesC& aVendorName)
	{
	HBufC* ptr = aVendorName.AllocL();
	// Ownership transfered to the list
	iVendorNameList.AppendL(ptr);
	}

TBool CPreferences::IsPackageNamePresent(const TDesC& aPackageName) const
	{
	for(TInt i = iPackageNameList.Count() - 1; i >= 0; --i)
		{
		if(0 == aPackageName.CompareF(iPackageNameList[i]->Des()))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TBool CPreferences::IsVendorNamePresent(const TDesC& aVendorName) const
	{
	for(TInt i = iVendorNameList.Count() - 1; i >= 0; --i)
		{
		if(0 == aVendorName.CompareF(iVendorNameList[i]->Des()))
			{
			return ETrue;
			}
		}
	return EFalse;
	}


