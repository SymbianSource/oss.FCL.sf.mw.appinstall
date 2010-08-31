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
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __INSTALLATIONPROCESSOR_H__
#define __INSTALLATIONPROCESSOR_H__

#include <e32base.h>
#include <f32file.h>
#include <e32ldr_private.h>
#include "msisuihandlers.h"
#include "sisregistryfiledescription.h"
#include "processor.h"

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "registrywrapper.h"
#include "appregextractor.h"
#include <swi/sisregistrypackage.h>
#include <swi/sisregistryentry.h>
#else
#include "sisregistrywritablesession.h"
#endif

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CIntegrityServices;
#endif

namespace Swi
{
class CApplication;
class CSecurityManager;
class RSisHelper;
class RUiHandler;
class CHashContainer;
class CPlan;
class CFileExtractor;
class CSidCache;
class MSisDataProvider;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CAppRegFileData;
#endif
/**
 * This class processes a CApplication created by the installation planner
 * @released
 * @internalTechnology 
 */
class CInstallationProcessor : public CProcessor
	{
	
public:
	/**
	 * This class contains information about the temporary location of a file,
	 * and it's final destination, since the application processor installs files
	 * in a two stage process.
	 */
	class CFileCopyDescription 
		{
	public:
		static CFileCopyDescription* NewL(const TDesC& aTemporaryFileName, const CSisRegistryFileDescription& aFileDescription);
		
		static CFileCopyDescription* NewLC(const TDesC& aTemporaryFileName, const CSisRegistryFileDescription& aFileDescription);
		
		~CFileCopyDescription();
	
		// Access functions
		inline const CSisRegistryFileDescription& FileDescription() const;
		
		inline const TDesC& TemporaryFileName() const;
		
	private:
		CFileCopyDescription(const CSisRegistryFileDescription& aFileDescription);
		
		void ConstructL(const TDesC& aTemporaryFileName);
	
	private:
		/// The temporary filename
		const CSisRegistryFileDescription& iFileDescription;
		
		/// The temporary filename
		HBufC* iTemporaryFileName;
		};

public:
		
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Creates a new CInstallationProcessor
	 * @param aPlan              	Installation plan
	 * @param aSecurityManager   	Used to calculate file hashes
	 * @param aSisHelper         	Extracts new files from the SISX file
	 * @param aUiHandler         	Interface for progress notification
	 * @param aStsSession 			Used to rollback installation process
	 * @param aRegistryWrapper		Used for registy operations
	 * @param aObserver			 	SWI Observer sessision handle which is used to log SWI events.
	 * @return                   	New application processor object
	 */
	static CInstallationProcessor* NewL(const CPlan& aPlan,
		CSecurityManager& aSecurityManager, RSisHelper& aSisHelper,
		RUiHandler& aUiHandler, Usif::RStsSession& aStsSession,
		CRegistryWrapper& aRegistryWrapper,
		const TDesC8& aControllerData, RSwiObserverSession& aObserver);
#else
	/**
	 * Creates a new CInstallationProcessor
	 * @param aPlan              Installation plan
	 * @param aSecurityManager   Used to calculate file hashes
	 * @param aSisHelper         Extracts new files from the SISX file
	 * @param aUiHandler         Interface for progress notification
	 * @param aIntegrityServices Used to rollback installation process
	 * @param aObserver			 SWI Observer sessision handle which is used to log SWI events.
	 * @return                   New application processor object
	 */
	static CInstallationProcessor* NewL(const CPlan& aPlan,
		CSecurityManager& aSecurityManager, RSisHelper& aSisHelper,
		RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices,
		const TDesC8& aControllerData, RSwiObserverSession& aObserver);
#endif

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Creates a new CInstallationProcessor and places it on the cleanup stack
	 * @param aPlan              	Installation plan
	 * @param aSecurityManager   	Used to calculate file hashes
	 * @param aSisHelper         	Extracts new files from the SISX file
	 * @param aUiHandler         	Interface for progress notification
	 * @param aStsSession 			Used to rollback installation process
	 * @param aRegistryWrapper		Used for registy operations
	 * @param aObserver			 	SWI Observer sessision handle which is used to log SWI events.
	 * @return                   	New application processor object on the 
	 *                           	cleanup stack
	 */
	static CInstallationProcessor* NewLC(const CPlan& aPlan,
		CSecurityManager& aSecurityManager, RSisHelper& aSisHelper,
		RUiHandler& aUiHandler, Usif::RStsSession& aStsSession,
		CRegistryWrapper& aRegistryWrapper,
		const TDesC8& aControllerData, RSwiObserverSession& aObserver);
#else
	/**
	 * Creates a new CInstallationProcessor and places it on the cleanup stack
	 * @param aPlan              Installation plan
	 * @param aSecurityManager   Used to calculate file hashes
	 * @param aSisHelper         Extracts new files from the SISX file
	 * @param aUiHandler         Interface for progress notification
	 * @param aIntegrityServices Used to rollback installation process
	 * @param aObserver			 SWI Observer sessision handle which is used to log SWI events.
	 * @return                   New application processor object on the 
	 *                           cleanup stack
	 */
	static CInstallationProcessor* NewLC(const CPlan& aPlan,
		CSecurityManager& aSecurityManager, RSisHelper& aSisHelper,
		RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices,
		const TDesC8& aControllerData, RSwiObserverSession& aObserver);
#endif

	static CInstallationProcessor* NewL(CInstallationProcessor& aProcessor);
	
	virtual ~CInstallationProcessor();
	
	
private:
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	CInstallationProcessor(const CPlan& aPlan,
		CSecurityManager& aSecurityManager, RSisHelper& aSisHelper,
		RUiHandler& aUiHandler, Usif::RStsSession& aStsSession,
		CRegistryWrapper& aRegistryWrapper,
		const TDesC8& aControllerData, RSwiObserverSession& aObserver);
#else
	CInstallationProcessor(const CPlan& aPlan,
		CSecurityManager& aSecurityManager, RSisHelper& aSisHelper,
		RUiHandler& aUiHandler, CIntegrityServices& aIntegrityServices,
		const TDesC8& aControllerData, RSwiObserverSession& aObserver);
#endif

	void ConstructL();

	/**
	 * Uses Integrity Support to first backup then remove old files
	 * @param aFileDescription - the file to display
	 */
	void DisplayFileL(const CSisRegistryFileDescription& aFileDescription, Sis::TSISFileOperationOptions aFileOperationOption);
	
	/**
	 * Uses SISHelper to extract a file into a temporary location, verifies 
	 * its hash using Security Manager, detaches hash to \sys\hash on system drive
	 * if this is an executable file
	 *
	 * @param aFileToExtract - the file to extract
	 * @return ETrue to indicate caller should wait for request completion, EFalse otherwise.
	 */
	 TBool ExtractFileL(CSisRegistryFileDescription& aFileDescription);

	/**
	 * Extracts executable file hash and places it as a file by the same 
	 * name in \sys\hash on system drive ; adds the hash file to the transaction 
	 * in the integrity services.
	 *
	 * @param aFileName Original name of the file
	 * @param aHash     SHA1 hash from SISController
	 */
	void ExtractHashL(const TFileName& aFileName, const CHashContainer& aHash);
	
	/**
	 * Ensures files are only installed in valid locations
	 */
	void VerifyInstallPathL(const CSisRegistryFileDescription& aFileDescription);
	
	/**
	 * Installs files by moving them from the temporary location to
	 * the target location specified in the SISX file.
	 * @param aFileCopyDescription containing the source and target filenames
	 */
	void InstallFileL(const CFileCopyDescription& aFileCopyDescription);
	
	void DoExtractHashL(const CSisRegistryFileDescription& aFileToExtract);
	
	/**
	 * Calculate hash value for the file and compare with the one given.
	 * @leave KErrCorrupt if hashes do not match.
	 */
	void CheckHashL(const CSisRegistryFileDescription& aFileToExtract, const TDesC& aCurrentFileName);
	
	/**
	* Construct the temporary filename
	*/
	void TemporaryFileNameLC(const CSisRegistryFileDescription& aFileToExtract, TDes& aTemporaryFileName);

	/**
	 * Ensures the temporary directory, we use to store files in
	 * before copying them to the final destination, exists.
	 */
	void EnsureTemporaryInstallDirExistsL(const TDesC& aFileTarget);
	void ReportErrorL(TErrorDialog aError);
	void AddEventToLogL(const CSisRegistryFileDescription& aFileDescription);
	
	/**
	 * Add in apparc files (incase of reg files) and Launch the file by checking the file operation.
	 */
	void LaunchFileL(const CSisRegistryFileDescription& aFileDescription);
	
	/*
	 * Checks whether the file is launchable or not.
	 * Returns true if operation flag is set RI. 
	 */
	TBool ShouldLaunchL(const CSisRegistryFileDescription& aFileDescription);
		
	/*
	 * Add apparc registered files in our apparc registered file list.
	 */	
	 void AddApparcFilesInListL(const TDesC& aTargetFileName);
	 
	// Functions which get called by CProcessor::RunL to do the work
private:
	virtual TBool DoStateInitializeL();
	virtual TBool DoStateProcessEmbeddedL();
	virtual TBool DoStateExtractFilesL();
	virtual TBool DoStateVerifyPathsL();
	virtual TBool DoStateInstallFilesL();
	virtual TBool DoStateDisplayFilesL();
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	virtual TBool DoParseApplicationRegistrationFilesL();
	TBool ParseRegistrationResourceFileL(const TDesC& aTargetFileName);
	TBool GetComponentIdL(const Usif::RSoftwareComponentRegistry& aScrSession, const TUid& aPackageUid, const TInt aPackageIndex, Usif::TComponentId& aComponentId);	
	void AddAppArcRegResourceFilesL();
	void AddAppArcRegResourceFilesForRegEntryL( RSisRegistryEntry& aEntry);
	TInt UserSelectedLanguageIndexL(const CApplication& aApplication) const;
#endif	
	virtual TBool DoStateUpdateRegistryL();
	virtual TBool DoStateProcessFilesL();
	virtual TBool DoStateProcessSkipFilesL();

	virtual void DoCancel();

	CInstallationProcessor& EmbeddedProcessorL();

	// Verify that all embedded applications were installed on the same drive
	static TBool CheckEmbeddedAppsInstalledOnSameDrive(RPointerArray<CApplication> aArray, TChar aDrive);
	// Check whether the application is permitted in a removable media card SIS stub
	static TBool IsApplicationPermittedInStub(const CApplication& aApplication);
	
	/* Create the stub SIS file 
	@param aFileName Stub sis filename
	*/
	void CreateStubSisFileL(TFileName &aFileName);
	
	/* Register the stub SIS to SWI Registry while uninstallation 
	@param aFileName Stub sis filename
	*/
	void RegisterStubSisFileL(const TFileName &aFileName);
	
	/* Check whether the media is removable or not */
	TBool IsStubSisFileRequiredL();
		
	/** Create a SisRegistry file description for the given
	SIS stub file
	@param aFs A file server session
	@param aFileName The full path and filename of the SIS stub file
	*/
	CSisRegistryFileDescription* CreateSisStubRegistryFileDescriptionLC(RFs& aFs, const TDesC& aFileName);

	/// @return true if aFilename is an Apparc registration file 
	bool FileIsApparcReg(const TDesC& aFilename) const;
	
private:
	/// Security Manager provided by SWIS
	CSecurityManager& iSecurityManager;
		
	/// SisHelper provided by SWIS
	RSisHelper& iSisHelper;
	
	/// Processes embedded applications
	CInstallationProcessor* iEmbeddedProcessor;
	
	/**
	 * A list of files which need to be copied from the temporary location to their
	 * final locations.
	 */
	RPointerArray<CFileCopyDescription> iFilesToCopy;
		
	TInt iCurrent;
	TInt iFilesToCopyCurrent;
	
	CFileExtractor* iFileExtractor;
	
	const TDesC8& iControllerData;

	/**
	 * The list of Apparc registration files to pass to apparc before
	 * running an exe.
	 */
	RPointerArray<TDesC> iApparcRegFiles;
    
	RLoader iLoader;
	
	/**
	 * The list of input received from user for the DisplayText dialog. This array is parallel bit 
	 * array with ApplicationL().FilesToSkipOnInstall()
	 */
	RArray<TBool> iSkipFile;
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/** The list of the software types to be registered read from the XML registration file. */
	RCPointerArray<Usif::CSoftwareTypeRegInfo> iSoftwareTypeRegInfoArray;
	RPointerArray<Usif::CApplicationRegistrationData> iApparcRegFileData;
   /**
     * The list of Apparc registration files to parse to populate SCR
     */ 
    RPointerArray<CAppRegFileData> iApparcRegFilesForParsing;
    
    CAppRegExtractor *iAppRegExtractor;  
        
#endif
	};
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CAppRegFileData:public CBase
    {
public:
    static CAppRegFileData* NewLC(const TDesC& aAppRegFile, const CSisRegistryPackage& aSisRegistryPackage); 
    static CAppRegFileData* NewL(const TDesC& aAppRegFile, const CSisRegistryPackage& aSisRegistryPackage);
    TDesC& GetAppRegFile();
    CSisRegistryPackage& GetSisRegistryPackage();
    ~CAppRegFileData();
private:
    CAppRegFileData();
    void ConstructL(const TDesC& aAppRegFile, const CSisRegistryPackage& aSisRegistryPackage);
    HBufC *iAppRegFile;
    CSisRegistryPackage *iSisRegistryPackage;
    };
#endif
	
// inline functions

inline const CSisRegistryFileDescription& CInstallationProcessor::CFileCopyDescription::FileDescription() const
	{
	return iFileDescription;
	}
			
inline const TDesC& CInstallationProcessor::CFileCopyDescription::TemporaryFileName() const
	{
	return *iTemporaryFileName;
	}


} // namespace Swi

#endif
