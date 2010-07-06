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
* RSisRegistryWritableSession - restricted client registry session interface
*
*/


/**
 @file 
 @internalTechnology
 @released
*/

#ifndef __SISREGISTRYWRITABLESESSION_H__
#define __SISREGISTRYWRITABLESESSION_H__

#include <e32std.h>
#include "sisregistrysession.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/usifcommon.h>
#include <usif/scr/appregentries.h>
#include <usif/scr/screntries_platform.h>
#endif
namespace Swi
{

class CApplication;
class CSisRegistryPackage;

namespace Sis
	{
	class CController;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
using Usif::TComponentId;
using Usif::TScomoState;

class CSoftwareTypeRegInfo;
#endif

class RSisRegistryWritableSession : public RSisRegistrySession
	{
public:

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Starts a transaction with SCR for the mutable operations of this session.
	 * The APIs that this transaction covers are AddEntryL, UpdateEntryL and DeleteEntryL.
	 * Note that this transaction has no relation with the transaction ID (aTransactionID)
	 * parameter being supplied to the mutable APIs.
	 */
	IMPORT_C void CreateTransactionL();

	/**
	 * Commits the changes performed in the SCR after the call to BeginTransactionL.
	 */
	IMPORT_C void CommitTransactionL();

	/**
	 * Discards the changes performed in the SCR after the call to BeginTransactionL.
	 */
	IMPORT_C void RollbackTransactionL();
#endif

	/**
	 * Adds a registry entry representing this package
	 *
	 * @param aApplication The application description provided by Swi
	 * @param aController The controller in a buffer 
	 * @param aTransactionID The TransactionID for IntegrityServices provided
	 *		  				 by Swis of TInt64 type
	 *
	 */
	IMPORT_C void AddEntryL(const CApplication& aApplication, const TDesC8& aController, TInt64 aTransactionID);

	/**
	 * Updates the registry entry representing this package
	 *
	 * @param aApplication The application description provided by Swi
	 * @param aController The controller in a buffer 
	 * @param aTransactionID The TransactionID for IntegrityServices provided
	 *		  				 by Swis of TInt64 type
	 *
	 */
	IMPORT_C void UpdateEntryL(const CApplication& aApplication, const TDesC8& aController, TInt64 aTransactionID);
	
	/**
	 * Deletes the entry from the registry represented by the given package
	 *
	 * @param aPackage The package to search for
	 * @param aTransactionID The TransactionID for IntegrityServices provided
	 *		  				 by Swis of TInt64 type
	 *
	 */
	IMPORT_C void DeleteEntryL(const CSisRegistryPackage& aPackage, TInt64 aTransactionID);

	/**
	 * Notification to registry that a drive has been mounted
	 *
	 * @param aDrive Drive number; 	 
	 * 
	 * @note valid value are between 0 and KMaxDrives - 1 inclusive
	 * 0 stands for A drive and  KMaxDrives - 1 for Z
	 *
	 */
	IMPORT_C void AddDriveL(const TInt aDrive);
	
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Notification to registry that a drive has been dismounted
	 *
	 * @param aDrive Drive as a number; 
	 *
	 * @note  valid value are between 0 and KMaxDrives - 1 inclusive
	 * 0 stands for A drive and  KMaxDrives - 1 for Z
	 *
	 */
	IMPORT_C void RemoveDriveL(const TInt aDrive);

	/**
	 * Notification to the registry that Software Installation has been rolled
	 * back and the cache needs to be regenerated from the contents on disk.
	 */
	IMPORT_C void RegenerateCacheL();
#endif
	
	/**
 	* Returns ETrue if any ROM stub in the filesystem has the package uid specified
 	* in aPackageId 
 	* With the addition of SA upgrades to ROM, a package may be both in ROM, upgraded 
 	* on another drive and this method can be used to check for the presence 
 	* of an upgraded ROM package.
 	*
 	*
 	* @param aPackageId		Package Id to be searched in the ROM stub files.
 	*
 	* @return ETrue if it can find the aPackageId in any of the ROM stub SIS.
 	*         EFalse otherwise
 	*/
	IMPORT_C TBool PackageExistsInRomL(const TUid& aPackageId);
	

	
	/**
	* Gets all the eclipsable file's entries from the ROM stub file of a ROM based package.
	*
	* @param aUid		Package UId to identify the right stub file.
	*
	* @param aFiles		A pointer array of file names to be populated.
	*
	*/
	IMPORT_C void GetFilesForRomApplicationL(const TUid& aPackageId, RPointerArray<HBufC>& aFiles);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	* Activates the component identified by the supplied parameter.
	*
	* @param aComponentId		Identifies the installed component, which is to be activated.
	*
	*/
	IMPORT_C void ActivateComponentL(TComponentId aComponentId);

	/**
	* Deactivates the component identified by the supplied parameter.
	*
	* @param aComponentId		Identifies the installed component, which is to be deactivated.
	*
	*/
	IMPORT_C void DeactivateComponentL(TComponentId aComponentId);
	
	/**
    * Sets the component presence property for a given component id (If a package contains files 
    * installed to a removable media, the package is considered as not fully present if the same
    * media is not present. This property will be used to filter out applications, that are not
    * fully present, from being diaplayed in the AppLib Menu.).
    *
    * @param aComponentId       Identifies a installed component
    * @param aState             True, if the component is fully present, else False.
    *
    */
	IMPORT_C void SetComponentPresenceL(TComponentId aComponentId, TBool aState);

	/**
	 * Adds a registry entry representing a package containing a Layered Execution Environment
	 *
	 * @param aApplication The application description provided by Swi
	 * @param aController The controller in a buffer
	 * @param aSwTypeRegInfoArray The array of the software types to be registered
	 * @param aTransactionID The TransactionID for IntegrityServices provided by Swis of TInt64 type
	 *
	 */
	IMPORT_C void AddEntryL(const CApplication& aApplication, const TDesC8& aController, const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, TInt64 aTransactionID);

	/**
	 * Updates the registry entry representing a package containing a Layered Execution Environment.
	 * The SISRegistryServer checks if the registration info passed in aSwTypeRegInfoArray matches
	 * the data passed during the installation of the base package. If they differ the installation terinates.
	 * Hence, this upgrade package may upgarde the LEE but cannot change its registration data.
	 *
	 * @param aApplication The application description provided by Swi
	 * @param aController The controller in a buffer
	 * @param aSwTypeRegInfoArray The array of the software types to be verified against the already registered base package
	 * @param aTransactionID The TransactionID for IntegrityServices provided by Swis of TInt64 type
	 *
	 */
	IMPORT_C void UpdateEntryL(const CApplication& aApplication, const TDesC8& aController, const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, TInt64 aTransactionID);

	IMPORT_C void AddEntryL(const Usif::CApplicationRegistrationData& aApparcRegFileData, const CSisRegistryPackage& aSisRegistryPackage);
	IMPORT_C void UpdateEntryL(const CApplication& aApplication, const Usif::CApplicationRegistrationData& aApparcRegFileData, const CSisRegistryPackage& aSisRegistryPackage);
private:
	void SetComponentStateL(TComponentId aComponentId, TScomoState aState);
	TInt UserSelectedLanguageIndexL(const CApplication& aApplication) const;
#endif
	void AddEntryImplL(TInt aMessage, const CApplication& aApplication, const TDesC8& aController, TInt64 aTransactionID, TIpcArgs& aIpcArgs);
	void UpdateEntryImplL(TInt aMessage, const CApplication& aApplication, const TDesC8& aController, TInt64 aTransactionID, TIpcArgs& aIpcArgs);
	};

} // namespace
#endif
