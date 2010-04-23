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
* This file provides the class declaration of CPreferencesHandler.
* @internalComponent
*
*/


#ifndef _CPREFERENCES_HANDLER_H_
#define _CPREFERENCES_HANDLER_H_

// System includes
#include <e32base.h>
#include <swi/msisuihandlers.h>

// Forward declarations
class CPreferences;

// Constants and literals
const TInt KMaxFileNameLength 	= 100;
const TInt KMaxNameLength		= 80;
const TInt KMaxUriLength		= 200;

// enums
enum TOperationMode 
	{
	ESwiNormalMode,
	ESwiUnAttendedMode,
	ESwiOptimalMode
	};

enum TOperation
	{
	ESwiUsage,
	ESwiHelp,
	ESwiInstall,
	ESwiUnInstall,
	ESwiList,
	ESwiVersion
	};

enum TProgressInfoType
	{
	EShowProgress,
	EHideProgress,
	EPromptProgress
	};


class CPreferenceHandler: public CBase
	{
public:	// Friend Declaration
	friend class CCommandLineParser;

public: // Public constructors
	static CPreferenceHandler* NewL();
	static CPreferenceHandler* NewLC();
	
	~CPreferenceHandler();	

public: // Public Member functions
	// Creates and adds an instance of CPreferences into preference lsit
	CPreferences* CreatePreferencesL();
	// This function retrieves the CPreferences class whose information
	// matches with the instance of the CAppInfo class
	CPreferences* GetPreferences(const Swi::CAppInfo&) const;
	// This function retrieves the default instance of CPreferences class
	CPreferences* GetDefaultPreference() const;
	// Retrieves the operation Mode
	inline TOperationMode GetOperationMode() const;
	// Retrieves the operation Mode
	inline TOperation GetOperation() const;
	// Retrieves the log file name
	inline const TDesC& GetLogFileName() const;
	// Retrieves the SIS filename
	inline const TDes& GetSisFileName() const;
	// Retrieves the Vendor name
	inline const TDes& GetPackageName() const;
	// Retrieves the Vendor name
	inline const TDes& GetVendorName() const;
	// Retrieves the package UID
	inline const TUid&	GetPackageUid() const;
	// Retrieves the behaviour(type) of progress bar
	inline TProgressInfoType GetProgressInfoType() const;
	// Checks whether logging is enabled or not
	inline TBool IsLoggingEnabled() const;
	// Checks whether verbose mode is enabled or not
	inline TBool IsVerboseEnabled() const;
	// Checks whether OCSP check is enabled or not
	inline TBool GetOcspCheckStatus() const;
	// Retrieves the OCSP Server URI
	inline const TDes8& GetOcspServerUri() const;

private: // Private Member Functions
	// Private Copy Constructor
	CPreferenceHandler(const CPreferenceHandler&) {}
	// Priavet assignment operator
	CPreferenceHandler& operator=(const CPreferenceHandler&) {return *this;}

	// Default constructor
	CPreferenceHandler();	
	// IInd phase constructor
	void ConstructL();

private: // Private Member variables
	RPointerArray<CPreferences>	iPreferenceList;
	TOperationMode				iMode;
	TOperation					iOperation;
	TBuf<KMaxFileNameLength>	iLogFileName;
	TBuf<KMaxNameLength>		iPackageName;
	TBuf<KMaxNameLength>		iVendorName;
	TFileName 					iSisFileName;
	TUid						iPackageUID;
	TProgressInfoType			iProgessInfoType;
	TBool						iIsLoggingEnabled;
	TBool						iIsVerboseEnabled;
	TBool						iEnableOcspCheck;
	TBuf8<KMaxUriLength>		iOcspServerUri;
	};

// Inline Function definition
inline TOperationMode CPreferenceHandler::GetOperationMode() const
	{
	return iMode;
	}

// Inline Function definition
inline TOperation CPreferenceHandler::GetOperation() const
	{
	return iOperation;
	}
	
// Retrieves the SIS filename
inline const TDes& CPreferenceHandler::GetSisFileName() const
	{
	return iSisFileName;
	}

// Retrieves the log file name
inline const TDesC& CPreferenceHandler::GetLogFileName() const
	{
	return	iLogFileName;
	}

// Retrieves the Package Name
inline const TDes& CPreferenceHandler::GetPackageName() const
	{
	return iPackageName;
	}

// Retrieves the Vendor Name
inline const TDes& CPreferenceHandler::GetVendorName() const
	{
	return iVendorName;
	}

// Retrieves the package UID
inline const TUid&	CPreferenceHandler::GetPackageUid() const
	{
	return iPackageUID;
	}
	
// Retrieves the behaviour(type) of progress bar
inline TProgressInfoType CPreferenceHandler::GetProgressInfoType() const
	{
	return iProgessInfoType;
	}

// Checks whether logging is enabled or not
inline TBool CPreferenceHandler::IsLoggingEnabled() const
	{
	return iIsLoggingEnabled;
	}

// Checks whether verbose mode is enabled or not
inline TBool CPreferenceHandler::IsVerboseEnabled() const
	{
	return iIsVerboseEnabled;
	}

// Checks whether OCSP check is enabled or not
inline TBool CPreferenceHandler::GetOcspCheckStatus() const
	{
	return	iEnableOcspCheck;
	}

// Retrieves the OCSP Server URI
inline const TDes8& CPreferenceHandler::GetOcspServerUri() const
	{
	return	iOcspServerUri;
	}


#endif //_CPREFERENCES_HANDLER_H_
