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
* CSisRegistryToken class declaration
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef __SISREGISTRYTOKEN_H__
#define __SISREGISTRYTOKEN_H__

#include <e32base.h>
#include "sisregistrypackage.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "scrhelperutil.h"
#endif

class RReadStream;
class RWriteStream;

namespace Swi
{
class CControllerInfo;

class CSisRegistryToken : public CSisRegistryPackage
	{
public:
	IMPORT_C static CSisRegistryToken* NewL();
	IMPORT_C static CSisRegistryToken* NewLC();

	/**
	 * Constructs a entry from a given existing stream. 
	 */
	IMPORT_C static CSisRegistryToken* NewL(RReadStream& aStream);
	IMPORT_C static CSisRegistryToken* NewLC(RReadStream& aStream);
	
	/**
	 * Constructs a entry from a given existing other CSisRegistryToken entry 
	 * 
	 */
	IMPORT_C static CSisRegistryToken* NewL(const CSisRegistryToken& aToken);
	IMPORT_C static CSisRegistryToken* NewLC(const CSisRegistryToken& aToken);

	virtual ~CSisRegistryToken();
	
	/**
	 * Write the object to a stream 
	 *
	 * @param aStream The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
	/**
	 * Read the object from a stream
	 *
	 * @param aStream The stream to read from
	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	/**
	 * Return an array of executable SIDs belonging to (installed as a part of) this package.
	 *
	 * @param aSids an array of executable SIDs
	 *
	 */
	const RArray<TUid>& Sids() const;

	/**
	 * Information about the controller(s) associated with this application
	 *
	 * @return an array of CControllerInfo object pointers
	 */
	const RPointerArray<CControllerInfo>& ControllerInfo() const;
	
	/**
	 * Returns the version of this package
	 *
	 * @return The version
	 */
	inline TVersion Version() const;

	/** 
	 * Returns a bitmask of all drives used by the package 
	 * at the moment of installation. The drives are encoded with 1 if present/required
	 * and 0 if otherwise. Since potentially there can be uto 26 drives a 32 bit integer 
	 * is sufficient, 1<<i; i=0 encodes drive A as present
	 *
	 * @return drives bitmap 
	 */
	inline TUint Drives() const;
	
	/**
	 *
	 * Sets the current drives status, for initialisation purposes
	 * @param aDrives The drive bitmap of drives currently present in the system
	 */
	 
	inline void SetCurrentDrives(TUint aDrives);
	
	/**
	 * Returns what drive the user selected for files in the Sisx file that
	 * did not specify drive.
	 *
	 * @return TChar The drive selected
	 */
	inline TChar SelectedDrive() const;

	/**
	 * Returns the installed language for this package
	 *
	 * @return The language
	 */
	inline TLanguage Language() const;
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	/**
	 * Indicates whether or not the supplied executable belongs to this token
	 *
	 * @param aSid The executable's SID
	 * @return TBool - ETrue if it is part of the package denoted by 
	 * this token
	 *
	 */
	TBool SidPresent(TUid aSid) const;
	
	/** 
	 * Updates internal state from provided flags which drives are fixed 
	 *
	 * @param aFixedDrives TUint drives map of fixed drives
	 */
    IMPORT_C void SetFixedDrives(TUint aFixedDrives); 
    
    /** 
	 * Returns whether all required drives are present
	 *
	 * @return ETrue if all drives spanned by the package  are present
	 *         EFalse otherwise.
	 */
	inline TBool PresentState() const;
	
	IMPORT_C void AddRemovableDrive(TInt aDrive);
	IMPORT_C void RemoveRemovableDrive(TInt aDrive);
#endif

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
public: // Friend Functions.	
	friend void ScrHelperUtil::ReadFromScrL(Usif::RSoftwareComponentRegistry& aScrSession, Usif::CComponentEntry* aComponentEntry, CSisRegistryToken& aToken, RPointerArray<Usif::CPropertyEntry>* aPropertyArray);
#endif

protected:
	CSisRegistryToken();
	
	void ConstructL();
	void ConstructL(const CSisRegistryToken& aToken);
	void ConstructL(RReadStream& aReadStream);
		
protected:
	// The array of SIDs of executable owned by this package
	RArray<TUid> iSids;

	// a bitmap of drives
	TUint iDrives;

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// [- These variables unused. Included for BC
	TBool iCompletelyPresent;
	TUint iPresentRemovableDrives;
	// End unused section -]
	
	TUint iCurrentDrives;
#endif

	// controller info
	RPointerArray<CControllerInfo> iControllerInfo;
	// version
	TVersion iVersion;
	// language
	TLanguage iLanguage;
	// drive choosen for user selectable files
	TChar iSelectedDrive;

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TInt iUnused1;
	TInt iUnused2; 
#endif
	};
	

inline TUint CSisRegistryToken::Drives() const 
	{
	return iDrives;
	}

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
inline TBool CSisRegistryToken::PresentState() const
	{
	return ((iDrives ^ iCurrentDrives) == 0);
	}

inline TBool CSisRegistryToken::SidPresent(TUid aSid) const
	{
	return (iSids.Find(aSid) != KErrNotFound)? ETrue:EFalse;
	}
#endif
inline  TLanguage CSisRegistryToken::Language() const
	{
	return iLanguage;
	}
	
inline  TChar CSisRegistryToken::SelectedDrive() const
	{
	return iSelectedDrive;
	}

inline const RArray<TUid>& CSisRegistryToken::Sids() const
	{
	return iSids;
	}

inline const RPointerArray<CControllerInfo>& CSisRegistryToken::ControllerInfo() const
	{
	return iControllerInfo;
	}


inline TVersion CSisRegistryToken::Version() const
	{
	return iVersion;
	}

} // namespace
#endif //__SISREGISTRYTOKEN_H__
