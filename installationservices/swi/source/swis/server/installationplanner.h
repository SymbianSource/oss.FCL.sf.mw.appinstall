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
* Definition of the CInstallationPlanner
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef __INSTALLATIONPLANNER_H__
#define __INSTALLATIONPLANNER_H__

#include <e32base.h>
#include "planner.h"

namespace Swi 
{
class RUiHandler;            // UISS handle
class RSisHelper;
class CInstallationResult;	
class CInstallationPlan;
class CApplication;
class CAppInfo;
class TAppInfo;           
class CContentProvider;      // Abstraction layer over SISX controller

class CSwisExpressionEnvironment;
class CSwisCreateExpressionEnvironment;
class CExpressionEvaluator;
class RSisRegistrySession;
class RSisRegistryEntry;
class RSisRegistryWritableEntry;

class CPlan;

namespace Sis 
{
class CController;          // SISX controller
class COptions;             // SISX options
class CFileDescription;
}


	/** T class used to store references to a controller and an application
	 It doesn't own either of them so T class should be ok
 	 * @released
 	 * @internalTechnology
	 */
	class TControllerAndParentApplication
		{
	public:
		TControllerAndParentApplication(const Sis::CController& aController, Swi::CApplication* aParentApplication);
		const Sis::CController& Controller();
		Swi::CApplication* ParentApplication();
	private:
		const Sis::CController& iController;
		Swi::CApplication* iParentApplication;
		};

/**
 * This class plans the installation. It asks the user all the required questions and builds
 * a list of application and files that need to be installed, without actually changing the
 * state of the device.
 * This class is used by Swi::CServerSISInstaller::DoInstallL(...)
 *
 * @released
 * @internalTechnology
 */
class CInstallationPlanner : public CPlanner
	{
public:
	/**
	 * This creates a new CInstallationPlanner object.
	 *
	 * @param aSisHelper          - SISHelper session
	 * @param aInstallerUI        - An implementation of the Installer UI
	 * @param aProvider           - Abstraction layer over the SISX contoller 
	 * @param aInstallationResult - Result of installation
	 */
	static CInstallationPlanner* NewL(RSisHelper& aSisHelper,
	  					RUiHandler& aInstallerUI, 
					    const CContentProvider& aProvider, 
					    CInstallationResult& aInstallationResult);
	/**
	 * This creates a new CInstallationPlanner object.
	 *
	 * @param aSisHelper           - SISHelper session
	 * @param aInstallerUI         - An implementation of the Installer UI
	 * @param aSISSignedController - Signed contoller of the main SIS file
	 * @param aInstallationResult  - Result of installation
	 */
	static CInstallationPlanner* NewLC(RSisHelper& aSisHelper,
	  					RUiHandler& aInstallerUI, 
					    const CContentProvider& aProvider, 
					    CInstallationResult& aInstallationResult);

	/**
	 * The destructor.
	 */
	virtual ~CInstallationPlanner(void);

public:
	/**
	 * Finish the plan once all controllers have been processed
	 */
	void FinalisePlanL(void);
	
	/**
	 * Finds the language index of the display language ID
	 * @param aAvailableLanguages - List of language ID's to find display ID from
	 * @return Index of language
	 */
	TInt FindDisplayLanguageIndex(const RArray<TLanguage>& aAvailableLanguages);
	/**
	 * @return Display language ID
	 */
	TLanguage DisplayLanguage(void) const;

	/** Gets the next controller to be processed in the plan
	@return ETrue If there is another controller or EFalse if finshed
	*/
	TBool GetNextController();

	/** Returns a reference to the current controller being processed
	*/
	const Sis::CController& CurrentController();
	
	/**
	Returns the main controller of the SIS file
	*/
	const Sis::CController& MainController();
	
	/**
	 * Returns an index number to the controller with the given Uid
	 * First found countroller is returned.
	 * @param aUid The Uid of the controller
	 * @return Index of the controller
	 */
	TInt GetControllerReference(TUid aUid);
	
	/** Plans the installation of the current controller.
	Asks for user options and finds embedded controllers and 
	conditional install blocks within the current controller for
	later use
	*/
	void PlanCurrentControllerL(RPointerArray<Sis::CFileDescription>& aFilesToCapabilityCheck);
	
	/** set the pointer to array of device supported languages
	to the reference of  aDeviceSupportedLanguages from install machine
	*/
	void SetDeviceSupportedLanguages(RArray<TInt>& aDeviceSupportedLanguages);

	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Set the planner to do it's planning activity only for component information collection. Not for installation.
	void SetInInfoCollectionMode(TBool aMode);
	
	// Says whether the planner plans only for component information collection.
	TBool IsInInfoCollectionMode();	
	#endif	 	

private:
	/**
	 * The constructor.
	 *
	 * @param aSisHelper          SISHelper session
	 * @param aInstallerUI        An implementation of the Installer UI
	 * @param aProvider           SISX file content provider
	 * @param aInstallationResult Result of installation
	 */
	CInstallationPlanner(RSisHelper& aSisHelper,
				RUiHandler& aInstallerUI, 
			    const CContentProvider& aProvider, 
			    CInstallationResult& aInstallationResult);
	/** 
	 * Second phase constructor.
	 * It builds the CAppInfo structure.
	 */
	void ConstructL();
	
private:

    /**
	 * Class used to store uid-application pairs.
     * @internalTechnology
     * @released
     */
	class CPackageApplicationPair:public CBase
		{
	public:
		static CPackageApplicationPair* NewLC(CSisRegistryPackage& aPackage, CApplication& aApplication);
		~CPackageApplicationPair();
		
		CSisRegistryPackage& Package() const;
		CApplication& Application();
	private:
		CPackageApplicationPair(CApplication& aApplication);
		
		void ConstructL(CSisRegistryPackage& aPackage);

		CSisRegistryPackage* iPackage;
		CApplication& iApplication;	
		};
	
	/**
	 * Class used to store some required information about a planned controller
	 * @internalTechnology
	 */
	class CFilesFromPlannedController:public CBase
		{
		public:
		
		static CFilesFromPlannedController* NewLC(TUid aUid, TUid aParentUid, Sis::TInstallType aInstallType, Sis::CString* aName);
		~CFilesFromPlannedController();
		/**
		 * Append a new file name into the file names list. 
		 * Ownership is transferred.
		 * @param aFileName A new file name to be added into the list
		 */
		void AppendL(HBufC* aFileName);
		/**
		 * Gets package UID 
		 * @return UID
		 */
		TUid Uid();
		/**
		 * Gets embedding package (parent) UID. Base packages have the same UID as parent UID.
		 * @return Parent UID
		 */
		TUid ParentUid();
		/**
		 * Gets the number of files in the list
		 */ 
		TInt Count();
		/**
		 * Gets the drive on which the application is being installed
		 */
		TChar Drive();
		/**
		 * Sets the drive on which the application is being installed
		 */
		void SetDrive(TChar& aDrive);
		/**
		 * Gets the name of the package
		 */
		Sis::CString* Name();
		/**
		 * Gets the install type of the package
		 */
		Sis::TInstallType InstallType();
		/**
		 * Returns whether the file name given matches with any files in the list
		 */
		TBool IsMatched(const TDesC& aFileName);
		/**
		 * Returns whether the file name given eclipses any files in the list 
		 */
		TBool IsEclipsed(const TDesC& aFileName);
		
		private:
		CFilesFromPlannedController(TUid aUid, TUid aParentUid, Sis::TInstallType aInstallType, Sis::CString* aName);
		//Package UID
		TUid iUid;
		//Embedding package UID
		TUid iParentUid;
		//Number of files in the list
		TInt iCount;
		//Installation drive
		TChar iDrive;
		//The list of file names to be installed
		RPointerArray<HBufC> iFileNames;
		//Install type of the package
		Sis::TInstallType iInstallType;
		//Name of the package
		Sis::CString* iName;
		};

	/**
	 * Check the filename to see if it is in apparc's import directory and if it 
	 * is add it to the plan
	 * */
	void AddApparcFilesToPlanL(const TDesC& aFilename);
	
	/**
	 * This function remembers that a package might need uninstalling, if we reach the end
	 * of the planning process and no upgrades for this package have been found.
	 *
	 * @param aUid				The UID of the applicattion which may need removing later.
	 * @param aApplication	The application which is upgrading the package this package was
	 *								originally embedded in. This is used to add it to the plan as embedded
	 *								in this original application.
	 */
	void NotePackageForProcessingL(CSisRegistryPackage& aPackage, CApplication& aApplication);

	/**
	 * This function is called whenever a package is upgraded. If this package UID was present
	 * on the list of packages which still need processing then it is removed.
	 *
	 * @param aUid The UID of the application which has been processed.
	 */
	void NotePackageProcessedL(TUid aUid);

	/**
	 * This function processes the remaining packages by adding them to the plan under the
	 * correct applications.
	 */
	void ProcessRemainingPackagesL();

	
	TBool IsPackageProcessed(TUid aUid) const;
	
	void PrepareEclipsableFilesListL(const Sis::CController& aController);

	void SetupExpressionEnvironmentL(const CApplication& aApplication);

	TBool IsValidSaUpgradeL(RSisRegistryWritableEntry& aRegistryEntry, const TBool aIsSisNonRemovable, const TBool aControllerFlag);

	/**
	 * Non-SU packages can Eclipse iff:
	 * a) The package uids match and 
	 * b) The package to be installed is signed for (not self signed) and 
	 * c) The package to be installed is immediate Upgrade to ROM ( SP or PU or PP over ROM 
	 *    (PA will have errored before reached here. If Not this function will panic with KErrNotSupported.)
	 *     (SA is allowed based on next condition)
	 * (or) The package to be installed is (SA+RU type or PU type ) and it has a matching ROM stub SIS file
	 * are allowed to eclipse files.	
	 * The package to be installed is not of preinstall Apps
	 */
	TBool IsValidEclipsingUpgradeL(const Sis::CController& aController, RSisRegistryWritableEntry& aRegistryEntry, const TBool aIsUpgradedRomStub);
	
	
	TBool IsEclipsableL(const TDesC& aRomFile);
	TBool ValidEclipseL(RFs& aFs, TFileName& aFileName, CApplication& aApplication);
	void WarnEclipseOverWriteL(CApplication& aApplication);
	void AdornedProcessingOfFileL(RFs& aFs, TDesC& aFileName, TDes& aUnadornedName, RPointerArray<HBufC>& aAdornedFileNamesFound);
	void HandleFileOverwritingL(TDesC& aFilename, CApplication& aApplication);

	/**
	 * Process a controller to create a plan. This processes controllers 
	 * recursively, adds files where necessary, and also adds property
	 * key,value pairs to the plan, to be inserted into the registry at a 
	 * later date.
	 */
	CApplication* ProcessControllerL(const Sis::CController& aController, TInt aCumulativeDataIndex, RPointerArray<Sis::CFileDescription>& aFilesToCapabilityCheck, TUid aParentUid);

	/**
	 * Generates an installation plan for a controller. It should be called after ProcessControllerL,
	 * which populates the lists of processed files and embedded controllers.
	 * This function relies on those lists, translates them into a plan understood by installation processor,
	 * and also processes conditional statements.
	 *
	 * @param aInstallBlock The block to process.
 	 * @param aApplication Installation plan to create.	 
	 */
	void ProcessInstallBlockL(const Sis::CInstallBlock& aInstallBlock, CApplication& aApplication, RPointerArray<Sis::CFileDescription>& aFilesToCapabilityCheck, CFilesFromPlannedController& aPlannedFiles);

	/**
	 * Processes properties, by adding them to the plan of the current CApplication.
	 *
	 * @param aProperties	The properties to add to the application plan.
	 * @param aApplication	The application plan to add the properties to.
	 */
	void ProcessPropertiesL(const Sis::CProperties& aProperties, CPlan& aPlan, CApplication& aApplication);
	
	/**
	 * Processes any IF THEN ELSE blocks of embbedded SIS files
	 * @param sisFileOptionsList - List of embedded options to process
	 * @param aApplication - The created application object to add files to
	 */
	void ProcessEmbeddedIfBlockL(const RPointerArray<Sis::COptions>& aSISFileOptionsList, 
				      CPlan& aPlan, CApplication& aApplication);


	void ProcessFileDescriptionL(const Sis::CFileDescription& aFileDescription, CApplication& aApplication, RPointerArray<Sis::CFileDescription>& aFilesToCapabilityCheck, CFilesFromPlannedController& aPlannedFiles);

	/**
	 * Process files to be removed as part of this install
	 *
	 */
	void ProcessFilesToRemoveL(CApplication& aApplication, RSisRegistryWritableEntry& aRegistryEntry); 

	/**
	 * Displays the upgrade dialog
	 *
	 * @param aController The controller to take the application
	 * @param aUpgradedApp The registry entry of the application being upgraded.
	 */
	void DisplayUpgradeDialogL(const TAppInfo& aCurrentAppInfo, RSisRegistryEntry& aUpgradedApp);	

	/**
	 * Gets the display language ID from the UI
	 */
	void ChooseLanguageDialogL(CContentProvider& aContent, CApplication& aApplication);

	/**
	 * Gets the drive to install onto from UI
	 */
	void ChooseDriveDialogL(CContentProvider& aContent, CApplication& aApplication, const TInt64 &aSize);

	/**
	 * Gets list of options user wishes to install from UI
	 */
	void ChooseOptionsDialogL(CContentProvider& aContent, CApplication& aApplication);

	/** Display an error dialog for the current application
	@param aType The error that occured
	@param aParam Additional information to display with the error
	*/
	void DisplayErrorL(TErrorDialog aType, const TDesC& aParam);

	/** Determine whether it is necessary to display a drive selection 
	dialog 
	@param aInstallBlock The install block to search for files that require user drive selection
	@return ETrue if any files require user drive selection, EFalse if they are all hard coded
	*/
	TBool IsUserDriveSelectionRequiredL(const Sis::CInstallBlock& aInstallBlock) const;

	/** Reclaim the space used by an older version of this application
	that will be removed before copying files for the new version
	@param aApplication The application with the list of files to add / remove
	*/
	void ReclaimUninstallSpaceL(const CApplication& aApplication);

	/** Check that a file being added does not match or eclipse another file
	being added from another controller in the plan.
	@param aFileName The file to check for clashes with previous embedded
	                 controller.
	@leave KErrAlreadyExists If the file exists with the same name, drive and
	                         path in a previously planned controller.
	@leave KErrInvalidEclipse If the file exists with the same name and path
	                          but a different drive in a previously planned
	                          controller.
	*/
	void CheckFilesFromPlannedControllersL(const TDesC& aFileName, CFilesFromPlannedController& aPlannedFiles) const;

	/**
	 *
	 * Check if the augmentation being installed is an upgrade to an existing
	 * augmentation, or an entirely new augmenation being installed.
	 *
	 * @param aController The controller of the SIS file being installecd
	 * @param aRegistryEntry The registry entry for the base package being upgraded
	 * @param aRegistrySession The registry session associated with the entry
	 * @return ETrue if this is an SP upgrade, EFalse otherwise
	 *
	 */
	TBool IsAugmentationUpgradeL(const Sis::CController& aController,
								 RSisRegistryEntry& aRegistryEntry,
								 RSisRegistrySession& aRegistrySession);
	/**
	 *
	 * Check if the augmentation being installed is an upgrade to a planned
	 * augmentation, or an entirely new augmenation being installed.
	 *
	 * @param aController The controller of the SIS file being installecd
	 * @return ETrue if this is an SP upgrade, EFalse otherwise
	 *
	 */
	TBool IsAugmentationUpgrade(const Sis::CController& aController);
	
	/**
	 * Check the planned controllers' array to find out if there is any controller that
	 * has got the same UID with the controller being processed. If the current
	 * controller is an partial upgrade or standard patch, its base package must 
	 * be already planned.
	 *
	 * @param aUid The UID of the SIS file being processed
	 * @param aIndex Index of the base package
	 * @param aParentUid The UID of the base package
	 * @return ETrue if there is any controller with the same UID in the list,
	 *		   EFalse otherwise
	 */
	 TBool IsPlannedControllerL(const Sis::CController& aController, TInt& aIndex, TUid aParentUid);

	 /**
	 * Sets the flag to indicate if an installation package contains a supported_language
	 * token and if there are matching languages
	 * @param aInstallBlock 
	 * @param aApplication 
	 * @param aContent 

	 */
	 void SetDeviceLanguagesL(const Sis::CInstallBlock& aInstallBlock, CApplication& aApplication, const CContentProvider& aContent);

	 
	 /**
	 Checks the SIS is allowed to install/upgrade to eclipse the ROM storage
	 though the ROM stub exist or not
	 @param aApplication The application of the SIS file being installed
	 @return ETrue if the file allowed to install based on SU Cert Rules, EFalse otherwise
	 */
	TBool IsValidSystemUpgradeL(const CApplication& aApplication);
	
private:
	/** Handle to SISHelper */
	RSisHelper& iSisHelper;

	/**
	 * Abstraction layer over the SISX controller
	 */
	const CContentProvider& iContentProvider;

	/**
	 * Index of language to display messages to user in
	 */
	TInt iDisplayLanguageIndex;

	/**
	 * Information about the application being installed.
	 * This is passed to the UI callbacks.
	 */
	CAppInfo* iAppInfo;

	/// Packages to be processed still
	RPointerArray<CPackageApplicationPair> iPackagesToBeProcessed;

	// Packages already processed.
	RArray<TUid> iPackagesProcessed;
	
	CSwisExpressionEnvironment* iExpressionEnvironment;
	CExpressionEvaluator* iExpressionEvaluator;
	
	// ROM files which are "eclipsable"
	RPointerArray<HBufC> iEclipsableRomFiles;
	RPointerArray<CSisRegistryFileDescription> iOverwriteableFiles;

	// An array of controllers that must be processed in the plan
	// Initially this just contains the main controller but as the plan
	// is processed subsequent embedded controllers are added to the array.
	RArray <TControllerAndParentApplication> iControllersToProcess;

	// current controller in the iControllersToProcess array
	TInt iCurrentController;
	
	// Is the application being processed sourced from a sis stub
	TBool iIsStub;
	
	// Is the application being processed a propagation? iIsStub will be ETrue
	TBool iIsPropagated;
	
	// Holds the drive letter of the first file in a stub SIS file
	TChar iStubFirstDriveLetter;
	
	// Holds the main application object until it can be passed to the plan
	CApplication* iMainApplication;
	
	// Array of system drives
	RArray<TChar> iDrives;
	// Space available on each drive
	RArray<TInt64> iDriveSpaces;
	
 	RArray<TFileName> iEclipsableOverwriteFiles;

	// Array of file names and other information from planned controllers, used to check eclipsing
	// and overwriting between controllers within the same plan.
	RPointerArray<CFilesFromPlannedController> iFilesFromPlannedControllers;
	
	// Pointer to the array of languages that Device supports
    RArray<TInt>* iDeviceSupportedLanguages;
	
	// This flag indicates whether the package has been signed by an SU cert., 
	// i.e. an authority allowed to update all ROM software
	TBool iIsValidSystemUpgrade;
	
	// This flag indicates whether the the next file should be skipped or not.
	TBool iTextSkip;
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Is the planning hapens just for component information collection?
	TBool iIsInInfoCollectionMode;
	#endif
	};
} // namespace Swi 


#endif
