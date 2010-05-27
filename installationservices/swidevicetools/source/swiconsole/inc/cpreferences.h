/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This file provides the class declaration of CPreferences.
* @internalComponent
*
*/


#ifndef _CPREFERENCES_H_
#define _CPREFERENCES_H_

#include <e32std.h>
#include <swi/msisuihandlers.h>

enum TPromptResponse
	{
	EAnswerNo,
	EAnswerYes,
	ENotAnswered = 0x03 // Answer to prompt is not provided
	};


enum TDialogType
	{
	EDisplayInstall,
	EGrantCapabilities,
	EDisplayUpgrade,
	EDisplaySecurityWarning,
	EDisplayOCSPError,
	EDisplayMissingDependency,
	EDisplayText,
	EDisplayQuestionIncompatible,
	EDisplayQuestionOverwrite,
	EDisplayUnInstall,
	EDisplayDependencyBreak,
	EDisplayAppInUse,
	EDevCertWarning,
	//EDisplayLanguage,
	//EDisplayDrive,
	EDisplayOption,
	EDialogTypeCount
	};
	
enum TEventType
{
	//TInstallCancellableEvent
	EEventRemovingFiles = Swi::EEventRemovingFiles,
	EEventShuttingDownApps,
	EEventCopyingFiles,
	EEventOcspCheckStart,
	//TInstallEvent
	EEventSetProgressBarFinalValue,
	EEventUpdateProgressBar,  
	EEventAbortedInstall,     
	EEventCompletedInstall,   
	EEventCompletedUnInstall, 
	EEventAbortedUnInstall,   
	EEventCompletedNoMessage, 
	EEventLeave,              
	EEventOcspCheckEnd,		
	EEventDevCert,
	EEventTypeCount
};


//Forward declaration
class CCommandLineParser;

class CPreferences: public CBase
	{
public: // Friend Declaration
	friend class CCommandLineParser;

public: // Public constructors
	static CPreferences* NewL();
	static CPreferences* NewLC();
	
	~CPreferences();	

public: // Public Accessor inline Functions
	inline const TDes& GetPackageName() const;
	//Add the package Name in the package name list
	void AddPackageNameL(const TDesC& aPackageName);
	//Add the vendor Name in the vendor name list
	void AddVendorNameL(const TDesC& aVendorName);
	// Checks whether the given package is present in the package list or not
	TBool IsPackageNamePresent(const TDesC& aPackageName) const;
	// Checks whether the given vendor is present in the vendor list or not
	TBool IsVendorNamePresent(const TDesC& avendorName) const;
	// Retrieves the selected language
	inline TLanguage GetLanguage() const;
	// Retrieves the selected drive
	inline TInt8 GetSelectedDrive() const;
	// Retrieves the option list selected by the user
	inline const RArray<TInt>& GetOptionList() const;

public: // Public Member functions
	// Checks whether an event is cancellable or not
	TInt8 GetCancellableEventOption(TEventType) const;
	// Retrieves the options selected for a dialog
	TInt8 GetDialogOption(TDialogType) const;
	
private: // Private Member Functions
	// Sets the response of dialogs
	void SetDialogOption(TDialogType, TInt8);
	// Sets the response of Cancellable event.
	void SetCancellableEventResponse(TEventType, TInt8);

private: // Private Member Functions
	// Private Copy Constructor
	CPreferences(const CPreferences&) {}
	// Priavet assignment operator
	CPreferences& operator=(const CPreferences&) {return *this;}

	// Default constructor
	CPreferences();	
	// IInd phase constructor
	void ConstructL() {}

private: // Private Member Variables
	TLanguage					iLanguage;
	// Dialog responses
	TInt8					iDialogResponses[EDialogTypeCount];
	// Event responses
	TInt8					iEventResponses[EEventTypeCount];
	// General responses
	TChar						iDrive;
	RPointerArray<HBufC>		iPackageNameList;
	RPointerArray<HBufC>		iVendorNameList;
	RArray<TInt>				iOptionList;
	};
	
// Retrieves the selected language
inline TLanguage CPreferences::GetLanguage() const
	{
	return iLanguage;
	}

// Retrieves the selected drive
inline TInt8 CPreferences::GetSelectedDrive() const
	{
	return iDrive;
	}
	

// Retrieves the option list selected by the user
inline const RArray<TInt>& CPreferences::GetOptionList() const
	{
	return iOptionList;
	}


#endif //_CPREFERENCES_H_
