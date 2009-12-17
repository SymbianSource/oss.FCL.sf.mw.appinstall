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
*
*/


#ifndef	SISREGISTRYTOKEN_H
#define	SISREGISTRYTOKEN_H

#include <iostream>
#include <algorithm>
#include <vector>
#include "controllerinfo.h"
#include "sisregistrypackage.h"
#include "version.h"

typedef 	std::vector<TUint32> Sids;
typedef 	std::vector<ControllerInfo*> Controllers;

/**
* @file SISREGISTRYTOKEN.H
*
* @internalComponent
* @released
*/
class SisRegistryToken : public SisRegistryPackage
{
public:
	SisRegistryToken ();

	SisRegistryToken(
		TUint32 aIndex,
		const std::wstring& aVendorName,
		const std::wstring& aPackageName,
		TUint32 aPackageUid,
		TUint32 aDrive,
	  	TUint32 aLanguage,
		const Version& aVersion,
		const Controllers& aControllerInfos,
		TUint32 aDrives);

	virtual ~SisRegistryToken ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	inline TInt GetUnused3() const;
	inline void SetUnused3(TInt& aUnused3);
	inline TInt GetUnused2() const;
	inline void SetUnused2(TInt& aUnused2);
	inline TInt GetSelectedDrive() const;
	inline void SetSelectedDrive(TInt& aSelectedDrive);
	inline TInt GetLanguage() const;
	inline void SetLanguage(TInt& aLanguage);
	inline Version GetVersion() const;
	inline void SetVersion(Version& aVersion);
	inline const Controllers& GetControllerInfo() const;
	void SetControllerInfo(const Controllers& aControllerInfo);
	inline const Sids& GetSids() const;
	inline void SetSids(const Sids& aSids);
	inline TUint32  GetUnused1() const;
	inline void SetUnused1(TUint32 aUnused1);
	inline TUint32 GetDrives() const;
	inline void SetDrives(TUint32 aDrives);

	void StoreControllerInfo(const Controllers& aControllerInfo);

protected:

	TUint32			iDrives;
	TUint32			iUnused1; ///< was iCompletelyPresent
	Sids			iSids;
	Controllers		iControllerInfo;
	Version			iVersion;
	TInt			iLanguage;
	TInt			iSelectedDrive;
	TInt			iUnused2;
	TInt			iUnused3;
};


// Inline Member Functions

inline TInt SisRegistryToken::GetUnused3() const
	{
	return iUnused3; 
	}

inline void SisRegistryToken::SetUnused3(TInt& aUnused3)
	{
	iUnused3 = aUnused3; 
	}

inline TInt SisRegistryToken::GetUnused2() const
	{
	return iUnused2; 
	}

inline void SisRegistryToken::SetUnused2(TInt& aUnused2)
	{
	iUnused2 = aUnused2; 
	}

inline TInt SisRegistryToken::GetSelectedDrive() const
	{
	return iSelectedDrive; 
	}

inline void SisRegistryToken::SetSelectedDrive(TInt& aSelectedDrive)
	{
	iSelectedDrive = aSelectedDrive; 
	}

inline TInt SisRegistryToken::GetLanguage() const
	{
	return iLanguage; 
	}

inline void SisRegistryToken::SetLanguage(TInt& aLanguage)
	{
	iLanguage = aLanguage; 
	}

inline Version SisRegistryToken::GetVersion() const
	{
	return iVersion; 
	}

inline void SisRegistryToken::SetVersion(Version& aVersion)
	{
	iVersion = aVersion; 
	}

inline const Controllers& SisRegistryToken::GetControllerInfo() const
	{
	return iControllerInfo; 
	}

inline const Sids& SisRegistryToken::GetSids() const
	{
	return iSids; 
	}

inline void SisRegistryToken::SetSids(const Sids& aSids)
	{
	iSids = aSids; 
	}

inline TUint32  SisRegistryToken::GetUnused1() const
	{
	return iUnused1; 
	}

inline void SisRegistryToken::SetUnused1(TUint32 aUnused1)
	{
	iUnused1 = aUnused1; 
	}

inline TUint32 SisRegistryToken::GetDrives() const
	{
	return iDrives; 
	}

inline void SisRegistryToken::SetDrives(TUint32 aDrives)
	{
	iDrives = aDrives; 
	}

#endif	/* SISREGISTRYTOKEN_H */

