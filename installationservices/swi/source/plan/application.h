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
* Definition of the CApplication
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <e32base.h>
#include "sispackagetrust.h"

namespace Swi
{
  class CSisRegistryFileDescription;
  class CSisRegistryPackage;
  class CUserSelections;
  class RSisHelper;
    
  namespace Sis
  {
    class CFileDescription;
    class CController;
  }

  /**
   * Container class for an application that is going to be installed. 
   *
   * @released
   * @internalTechnology
   */
	class CApplication : public CBase
		{
	public:
		/// Flags for this application
		enum TFlags
			{
			EUpgrade=1,
			EUninstall=2,
			EInstall=4,
			EAugmentation=8,
			EPreInstalled=16,
			EPartialUpgrade=32,
			EPreInstalledApp=64,
			EPreInstalledPatch=128,
			EInRom=256,
			EInstallSuCertBased=512
			};

		// Constant used to reset install flags, covers all the flags applying to install types
		static const TInt KInstallFlags = EUpgrade | EUninstall | EInstall
												  | EAugmentation | EPreInstalled | EPartialUpgrade
												  | EPreInstalledApp | EPreInstalledPatch;
	
		/**
		 * Class which represents a property to be added to the registry
		 */
		class TProperty
			{
		public:
			TProperty(TInt aKey, TInt aValue);

		public:
			TInt iKey;
			TInt iValue;
			};

		
				/**
		 * This creates a new CApplication object.
		 */
		IMPORT_C static CApplication* NewL();

		/**
		 * This creates a new CApplication object and leaves it in the cleanup stack
		 */
		IMPORT_C static CApplication* NewLC();
		
		/**
		 * The destructor.
		 */
		virtual ~CApplication();

	private:
		CApplication();
		
		void ConstructL();

	public:
	
		/**
		 * @return The contoller of the app
		 * @leave KErrGeneral The application is an uninstall and doesn't have a controller
		 */
		IMPORT_C const Sis::CController& ControllerL() const;

		/**
		 * Adds an embedded Application object to class
		 * @param aApplication - Embedded Application to add to class
		 */
		IMPORT_C void AddEmbeddedApplicationL(CApplication* aApplication);

		/**
		 * @return apps that were embedded in this app
		 */
		IMPORT_C const RPointerArray<CApplication>& EmbeddedApplications() const;

		/**
		 * Sets the uid
		 * @param aUid  The Uid of the app
		 */
		 IMPORT_C void SetController(TUid aUid);

		/**
		 Add a file to the list of files that need to be added during the installation, removed during uninstallation.
	 	 
	 	 @param aFileDescription Description of the file which needs to be added
	 	 @param aDrive The drive where the file will be installed
	 	 */
		IMPORT_C void AddFileL(const Sis::CFileDescription& aFileDescription, TChar aDrive);
	
		/**
		 * Add file to the list of files that need to be removed during the installation.
	 	 *
	 	 * @param aFileDescription Description of the file which needs to be removed
	 	 */
		IMPORT_C void RemoveFileL(const CSisRegistryFileDescription& aFileName);
		
		/**
		 * Add file to the list of files that need to be removed during the
		 * installation.  This is used for a file which is not actually part
	 	 * of an existing registry entry.
	 	 *
	 	 * @param aFileName Name of file which needs to be removed
	 	 */
		IMPORT_C void RemoveFileL(const TDesC& aFileName);

		/** Accessor for the array of files to add during the installation
		@return An array of files to add during installation
		*/
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToAdd() const;

		/** Accessor for the array of files to remove during the installation
		@return An array of files to remove during installation
		*/
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToRemove() const;
	
	
		/**
		 Add the file description for a removable media SIS stub file generated during this installation. 
		This allows the SIS stub to be removed during an uninstallation
		 
	 	 @param aFileDescription Description of the SIS stub file that needs to be added
	 	*/
		IMPORT_C void AddSisStubFileL(const CSisRegistryFileDescription& aFileDescription);
	
		/**
		 * Add files which need to be run during the installation.
	 	 *
	 	 * @param aFileDescription Description of the file which needs to be run
	 	 */
		IMPORT_C void RunFileOnInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive);
	
		/**
		 * Add files which need to be run during the uninstallation.
	 	 *
	 	 * @param aFileDescription Description of the file which needs to be run
	 	 */
		IMPORT_C void RunFileOnUninstallL(const CSisRegistryFileDescription& aFileName);
		
		/** Accessor for the array of files to run during the installation
		@return An array of files to run during installation
		*/
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToRunOnInstall() const;

		/** Accessor for the array of files to run during the uninstallation
		@return An array of files to run during uninstallation
		*/
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToRunOnUninstall() const;
	
		/**
	 	 * Add files which need to be displayed during the installation.
	 	 *
	 	 * @param aFileDescription Description of the file which needs to be displayed
	 	 */
		IMPORT_C void DisplayFileOnInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive);
	
		/**
	 	 * Add files which need to be displayed during the uninstallation.
	 	 *
	 	 * @param aFileDescription Description of the file which needs to be displayed
	 	 */
		IMPORT_C void DisplayFileOnUninstallL(const CSisRegistryFileDescription& aFileName);

		/** Accessor for the array of files to display during the installation
		@return An array of files to display during installation
		*/
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToDisplayOnInstall() const;
	
		/** Accessor for the array of files to display during the uninstallation
		@return An array of files to display during uninstallation
		*/
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToDisplayOnUninstall() const;

		/**
		 * @return - ETrue if Application is an install
		 */
		IMPORT_C TBool IsInstall(void) const;

		/**
		 * Informs the class, the Application is an install
		 */
		IMPORT_C void SetInstall(const Sis::CController& aController);

		/**
		 * @return - ETrue if Application is a removal
		 */
		IMPORT_C TBool IsUninstall() const;

		/**
		 * Informs the class, the Application is a removal
		 */
		IMPORT_C void SetUninstallL(const CSisRegistryPackage& aPackage);

		/**
		 * @return - ETrue if Application is an upgrade
		 */
		IMPORT_C TBool IsUpgrade() const;

		/**
		 * Informs the class, the Application is an upgrade
		 */
		IMPORT_C void SetUpgrade(const Sis::CController& aController);

		/**
		 * @return - ETrue if Application is an augmentation
		 */
		IMPORT_C TBool IsAugmentation() const;
		
		/**
		 * Informs the class, the Application is an augmentation
		 */
		IMPORT_C void SetAugmentation(const Sis::CController& aController);

		/**
		 * Informs the class, the Application is a partial upgrade
		 */
		IMPORT_C void SetPartialUpgrade(const Sis::CController& aController);

		/**
		 * @return - ETrue if Application is a partial upgrade
		 */
		IMPORT_C TBool IsPartialUpgrade() const;

		/**
		 * @return - ETrue if Application is a preinstalled app
		 */
		IMPORT_C TBool IsPreInstalledApp() const;
		
		/**
		  * Informs the class that the application is a preinstalled app and
  	      * sets the controller.
		  * @param aController - Identifies the application's controller.
		  */
		IMPORT_C void SetPreInstalledApp(const Sis::CController& aController);

		/**
          * Informs the class that the application is a preinstalled app.
 		  */
 		IMPORT_C void SetPreInstalledApp();
 
 		/**
		 * @return - ETrue if Application is a preinstalled patch
		 */
		IMPORT_C TBool IsPreInstalledPatch() const;
	
		/**
		 * Informs the class that the application is a preinstalled patch and
 		 * sets the controller.
		 * @param aController - Identifies the application's controller.
		 */
		IMPORT_C void SetPreInstalledPatch(const Sis::CController& aController);

		/**
		 * Informs the class that the application is a preinstalled patch
 		 */
 		IMPORT_C void SetPreInstalledPatch();
 
 		/**
		 * @return - ETrue if application is in ROM
		 */
		IMPORT_C TBool IsInROM(void) const;

		/**
		 * Informs the class, the Application is in ROM
		 */
		IMPORT_C void SetInROM(void);

		/**
		 * Adds a property which will need to be added
		 *
		 * @param aProperty The property to be added.
		 */
		IMPORT_C void AddPropertyL(const TProperty& aProperty);

		/**
		 * Returns the list of properties associated with the application.
		 *
		 * @return The list of properties associated with the application.
		 */
		IMPORT_C const RArray<TProperty>& Properties() const;
		
		/**
		 *
		 * Returns if the package is currently allowed to be propagated
		 * as a SIS stub (this means it has at least one file description
		 * and all the files are on the same drive)
		 *
		 * @return ETrue if the package may be propagated, EFalse otherwise
		 *
		 */
		
		IMPORT_C TBool CanPropagate() const;
		
		/**
		 *
		 * Returns the drive that the package should be propagated to.
		 *
		 * @return A drive letter to which the package should be propagated
		 *
		 */

		IMPORT_C TChar StubDrive() const;
		
		const CUserSelections& UserSelections() const;

		CUserSelections& UserSelections();

		TInt AbsoluteDataIndex() const;

		void SetAbsoluteDataIndex(TInt aDataIndex);

		/**
		 * Sets the controller
		 * @param aController - Signed contoller of app to install
		 */
		IMPORT_C void SetController(const Sis::CController& aController);

		IMPORT_C const CSisRegistryPackage& PackageL() const;

		IMPORT_C void SetPackageL(const CSisRegistryPackage& aPackage);
		
		TBool ShutdownAllApps() const;
		void SetShutdownAllApps(TBool aShutdown);

		/**
		 * @return - ETrue if the preinstalled application's files can be
		 * deleted on uninstall
		 */
		TBool IsDeletablePreinstalled() const;
		/**
		 * @param - ETrue if the preinstalled application's files can be
		 * deleted on uninstall.
		 */
		void SetDeletablePreinstalled(TBool aDeletablePreinstalled);

        /**
         * @deprecated This is replaced by the TSisTrustStatus object in the
         * SisController.
         **/
		TSisPackageTrust Trust() const;
	
		inline void SetStub(TBool aIsStub);
		inline TBool IsStub() const;
		
		/**
		* Sets the device supported language flag
		* @return void
		*/
		inline void SetDeviceSupportedLanguages(const TBool aIsDeviceSupportedLanguages);
		
		/**
		* Return the device supported language flag
		* @return TBool Flag
		*/
		inline TBool IsDeviceSupportedLanguages() const;
		
		/**
		* Sets the language match flag
		* @return void
		*/
		inline void SetLanguageMatch(const TBool aIsLanguageMatch);
		
		/**
		* Return the language match flag
		* @return TBool flag
		*/
		inline TBool IsLanguageMatch() const;
		
		/**
		* Copies the device supported languages
		* @param aDeviceSupportedLanguages
		*/
		IMPORT_C void CopyDeviceSupportedLanguagesL(const RArray<TInt> &aDeviceSupportedLanguages);
		
		/**
		* Adds the matching language ID's on to languages Array
		* @param aLanguageId
		*/
		IMPORT_C void PopulateMatchingDeviceLanguagesL(TInt aLanguageId);

		/**
		* Return flag indicating whether there is a language match(Exact match) 
		* or not.
		* @param alangId - language id
		* @return TBool flag
		*/
		IMPORT_C TBool IsDeviceMatchingLanguage(TInt alangId) const;

		/**
		* Return RArray containing list of language ID's that are present in both device supported languages and
		* with SUPPORTED_LANGUAGE token used in PKG file 
		* @return RArray<TInt>& 
		*/		
		IMPORT_C const RArray<TInt>& GetMatchingDeviceLanguages(void);
		
		/**
		* Set the DRM protection status of the package.
		* @param aIsDrmProtected ETrue if the package is DRM protected. EFalse otherwise.
		*/ 
		IMPORT_C void SetDrmProtected(TBool aIsDrmProtected);
		
		/**
		* Get the DRM protection status of the package.
		* @return - ETrue if the package is DRM protected. EFalse otherwise.
		*/ 
		IMPORT_C TBool IsDrmProtected() const;

		/**
		 * Informs the class, the Application is allowed to install based on SU Cert rules
		 */
		IMPORT_C void SetInstallSuCertBased();
	
		/**
		 * @return - ETrue if the Application is allowed to install based on SU Cert rules
		 */
		IMPORT_C TBool IsInstallSuCertBased() const;

		/**
	 	 * Add files which may be skipped during the installation.
	 	 *
	 	 * @param aFileDescription Description of the file which needs to be skipped
	 	 */
		IMPORT_C void SkipFileOnInstallL(const Sis::CFileDescription& aFileDescription, TChar aDrive);
		
		/** Accessor for the array of files to skip during the installation
	 	 * @return An array of files to skip during installation
		 */
		IMPORT_C const RPointerArray<CSisRegistryFileDescription>& FilesToSkipOnInstall() const;	 

		/**
		 Add a file to the list of files that need to be added during the installation, removed during uninstallation.
	 	 
	 	 @param aFileDescription Description of the file which needs to be added
	 	 */
		IMPORT_C void AddFileL(const CSisRegistryFileDescription& aFileDescription);
	
		/**
		 * Sets the sishelper
		 * @param aSisHelper - handle to sishelper
		 */
		IMPORT_C void SetSisHelper(const RSisHelper& aSisHelper);
		IMPORT_C RSisHelper& GetSisHelper();
		
		const RArray<TInt>& GetDeviceSupportedLanguages(void);
	private:

		/**
		 * The signed contoller of the application
		 */
		const Sis::CController* iController;
 		TUid* iUid;
 	
		/**
		 * List of files to install
		 */
		RPointerArray<CSisRegistryFileDescription> iFilesToAdd;

		/**
		 * List of existing file (for partial upgrade), that need backing up and placing back into
		 * install directory once upgrade is completed. i.e. user data files
		 */
		RPointerArray<CSisRegistryFileDescription> iFilesToRemove;

		/// List of files to run during installation or uninstallation
		RPointerArray<CSisRegistryFileDescription> iFilesToRunOnInstall;
	
		RPointerArray<CSisRegistryFileDescription> iFilesToRunOnUninstall;
	
		/// List of files to display during installation or uninstallation
		RPointerArray<CSisRegistryFileDescription> iFilesToDisplayOnInstall;
	
		/// List of files to skip during installation
		RPointerArray<CSisRegistryFileDescription> iFilesToSkipOnInstall;

		RPointerArray<CSisRegistryFileDescription> iFilesToDisplayOnUninstall;

		/**
		 * List of applications that are embedded in this one
		 */
		RPointerArray<CApplication> iEmbeddedApplications;
	
		/**
		 * Properties associated with this application. The registry
		 * needs to add these to it's records.
		 */
		RArray<TProperty> iProperties;

		/// User selected options
		CUserSelections* iUserSelections;

		/// AbsoluteDataIndex of the corresponding SISDataUnit in the SISFile
		TInt iAbsoluteDataIndex;
    
    	/// Flags for this application
    	TInt iFlags;

		CSisRegistryPackage* iPackage;
		
		/// ETrue if all running applications should be shutdown on install
		/// and uninstall. EFalse otherwise.
		TBool iShutdown;
		
		/// Member variables needed to determine if it is permitted to
		/// propagate this package as a stub
		TBool iCanPropagate;
		TChar iPropagateDrive;

		// Flag to determine if the files from a pre-installed package should
		// be deleted on uninstall.
		TBool iDeletablePreinstalled;
		
		TBool iIsStub;
		
		// Array which contains the set of languages that device supports
		RArray<TInt> iDeviceSupportedLanguages;
		
		// Array which contains the set of languages that device supports
		// and are matched with SUPPORTED_LANGUAGE token in PKG file
		RArray<TInt> iMatchingDeviceLanguages;
		
		// Flag used to indicate whether a SIS file contains device language option
		TBool iHasDeviceSupportedLanguages;
		
		// Flag used to indicate whether there is an exact match between  
		// the languages which device supports and languages which SIS file supports.
		TBool iHasMatchingLanguages;
		
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		// Flag determines the SIS file is DRM protected or not.
		// It will be used only when DRM is enabled in the swi policy.
		TBool iIsDrmProtected;
		#endif
		/** Handle to SISHelper */
		const RSisHelper* iSisHelper;
   };

// inline functions from CApplication::TProperty
inline CApplication::TProperty::TProperty(TInt aKey, TInt aValue)
	: iKey(aKey), iValue(aValue)
	{
	}

} // end namespace Swi

#include "application.inl"

#endif
