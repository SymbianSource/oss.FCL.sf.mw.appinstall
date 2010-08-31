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
* Base class for installation/uninstallation plans
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __PLAN_H__
#define __PLAN_H__

#include <e32base.h>
#include "sislauncherclient.h"

namespace Swi
{
class CSisRegistryFileDescription;
class CApplication;
class TAppInfo;

namespace Sis
	{
	class CFileDescription;
	}

/**
* Interface for installation or uninstallation plan
*/
class CPlan : public CBase
	{
public:
	class CFileInformation : public CBase
		{
	public:
		static CFileInformation* NewL(const Sis::CFileDescription& aFileDescription, TChar aDrive, TInt aAbsoluteDataIndex);
		
		static CFileInformation* NewLC(const Sis::CFileDescription& aFileDescription, TChar aDrive, TInt aAbsoluteDataIndex);
		
		virtual ~CFileInformation();
	
		TInt AbsoluteDataIndex() const;
		
		const CSisRegistryFileDescription& FileDescription() const;
		
	private:
		CFileInformation(TInt aAbsoluteDataIndex);
		
		void ConstructL(const Sis::CFileDescription& aFileDescription, TChar aDrive);
		
		/// The description of the file
		CSisRegistryFileDescription* iFileDescription;	

		/// The absolute data index this file comes from
		TInt iAbsoluteDataIndex;
		};

// lifecycle methods

	IMPORT_C static CPlan* NewL();
	
	IMPORT_C virtual ~CPlan();

	
	IMPORT_C void SetAppInfoL(const TAppInfo& aAppInfo);
	
	IMPORT_C const TAppInfo& AppInfoL() const;

	IMPORT_C void SetApplicationInformationL(const TDesC& aApplicationName, const TDesC& aVendorName, const TVersion& aVersion);

	/**
	 * Returns the specified application to process
	 * @param aIndex Index of application required
	 * @return       Application object reference
	 */
	IMPORT_C virtual const CApplication& ApplicationL() const;

	/**
	 * Set the main application in the plan. Ownership of the application
	 * is transfered to the plan.
	 *
	 * @param aApplication
	 */
	IMPORT_C virtual void SetApplication(CApplication* aApplication);
	
	IMPORT_C void AddInstallFileForProgress(TInt64 aFileSize);
	
	IMPORT_C void AddUninstallFileForProgress();
	  
	IMPORT_C TInt32 FinalProgressBarValue() const;
	
	/**
	 * Add files which need to be run during the uninstallation,
	 * before any running applications are shut down.
	 * @param aFileDescription Description of the file which needs to be run.
	 */
	IMPORT_C void RunFilesBeforeShutdownL(const CSisRegistryFileDescription& aFileName);

	/** Accessor for the array of files to run during the uninstallation,before any applications are shutdown.
	 * @return An array of files to run during uninstallation, before any applicaions are shut down.
	 */
	IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToRunBeforeShutdown() const;
	
	/** Adds the file to the list of files to run after the install is complete
	 * */ 
	IMPORT_C void RunFileAfterInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive, TBool aIsStub);			

	/** Returns the list of files to run after the install is complete
	 * */ 
	IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToRunAfterInstall() const;

	/** Set to true if the plan contains ECOM plugins
	 * */
	IMPORT_C void SetContainsPlugins(TBool aContainsPlugins);
	
	/**
	 *  @return true if the plan contains ECOM plugins
	 * */
	IMPORT_C TBool ContainsPlugins() const;
	
	/**
	 * Adds a file to the list of app arc registration files that have yet to be processed
	 * */
	IMPORT_C void AddAppArcRegFileL(const TDesC& aFilename);
	
	/**
	 * @return the list of app arc registration files that have yet to be processed
	 * */
	IMPORT_C const RPointerArray<TDesC>& AppArcRegFiles() const;

	/**
	 * Resets the list of app arc registration files that have yet to be processed
	 * */
	IMPORT_C void ResetAppArcRegFiles();
	
    #ifdef  SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	IMPORT_C void  SetAffectedApps(RArray<TAppUpdateInfo>& aAppInfo);	
	IMPORT_C void  GetAffectedApps(RArray<TAppUpdateInfo>& aAppInfo) const;
	IMPORT_C void  ResetAffectedApps();
    #endif	
private:
	CPlan();
	/// Information about the application being installed or uninstalled, used by the UI
	TAppInfo* iAppInfo;

	/// Used to transfer ownership of the vendor name to this class
	HBufC* iVendorName;

	/// Used to transfer ownership of the application name to this class
	HBufC* iApplicationName;

	CApplication* iApplication;
	
	TInt iFinalProgressBarValue;
	
	//Array of all the RBS files to be run before shutdown.
	RPointerArray<CSisRegistryFileDescription> iFilesToRunBeforeShutdown;
	///Array of all the RBS files to be run before shutdown.
	RPointerArray<CSisRegistryFileDescription> iFilesToRunAfterInstall;
	/// is true if the plan contains ECOM plug-ins
	TBool iContainsPlugins;
	/// the list of AppArc registration files that haven't been processed
	RPointerArray<TDesC> iAppArcRegFiles;	
    #ifdef  SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	/// the list of apps affected by insatllation/upgrade, used to notify apparc
	RArray<TAppUpdateInfo> iAffectedApps;
    #endif
	};

// inline functions

inline TInt CPlan::CFileInformation::AbsoluteDataIndex() const
	{
	return iAbsoluteDataIndex;
	}
		
inline const CSisRegistryFileDescription& CPlan::CFileInformation::FileDescription() const
	{
	return *iFileDescription;
	}

} // namespace Swi

#endif // __PLAN_H__
