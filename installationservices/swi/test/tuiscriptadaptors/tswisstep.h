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
* SWIS test step declaration
*
*/


/**
 @file
*/

#ifndef __TSWISSTEP_H__
#define __TSWISSTEP_H__

#include <test/testexecutestepbase.h>
#include <swi/asynclauncher.h>
#include "testutilclientswi.h"
#include "tswisserver.h"
#include "tuiscriptadaptors.h"
#include <swi/sistruststatus.h>

#include <e32base.h>

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#ifndef SWI_TEXTSHELL_ROM		
#include <apsidchecker.h>
#else
class CAppSidChecker;
#endif
#endif

namespace Swi
	{
	class CInstallPrefs;
	}

/**
 * Base test step class containing handy functions
 * @test
 * @internalComponent
 */
class CSwisTestStep : public CTestStep
	{
protected:

	enum TReadType
		{
		ESimpleType,		//to read "numexist" and "numnonexist" value from INI.
		ECancelType,		//to read "numexistcancel" and "numnonexistcancel" value from INI.
		ERegisteredType		//to read "numregistered" value from INI.
		};
		
	/**
	 * Get the file names for checking if present or missing.
	 * The test step's INI file must contain entry "numexistcancel=<n>" and one or more entries
	 * "existcancel<i>=<fname>". 
	 * The test step's INI file must contain entry "numnonexistcancel=<n>" and one or more entries 
	 * "nonexistcancel<i>=<fname>".
	 * The test step's INI file must contain entry "numexist=<n>" and one or more entries 
	 * "exist<i>=<fname>".
	 * The test step's INI file must contain entry "numnonexist=<n>" and one or more entries 
	 * "nonexist<i>=<fname>".
	 * 
	 * @param aFileNumExist     this array contains the name of the files which must exist after
	 *						    installation/un-installation/cancellation.
	 * @param aFileNumNonExist	this array contains the name of the files which must not exist after
	 * 							installation/un-installation/cancellation.
	 * @param aDoCancelTest     Used to make decision for reading from the INI file.
	 *                          ETrue  Cancel Testing is enabled, so read entries "numexistcancel" and "numnonexistcancel".
	 *                          EFalse Read entries "numexist" and "numnonexist".
	 */
	void GetFileNamesForCheck(RArray<TPtrC>& aFileNumExist,RArray<TPtrC>& aFileNumNonExist, TReadType aReadType);
	 
	/**
	 * Extract the file names from the INI file. 
	 * @param aEntries     Number of file name entries. 
	 * @param aEntryBase   Base name of the keys specifying names of files to 
	 *                     check; an index (0..n-1) is appended to the base 
	 *                     name to get the actual key name.
	 * @param aFileArray   Contains all the file names to check for missing or present. 
	 */ 
	void ExtractFileName(TInt aEntries, const TDesC& aEntryBase, RArray<TPtrC>& aFileArray);
	 
	/** 
	 * Checks for files that should exist if the installation was successful,
	 * and which shouldn't exist if the installation was cancelled. 
	 * @param aCheckExist  Controls the kind of testing done
	 *                     ETrue  Files must exist
	 *                     EFalse Files must not exist
	 * @param aFileArray   Contains all the file names to check for missing or present depending
	 *					   on the aCheckExist. If ETrue, this contains the names of files that must exist
	 *					   else if EFalse contains the names of files that must not be present.
	 * @param aTestUtil    Contains reference to the testutilserver session.
	 * @param aMsecTimeout Timeout to wait for file to be created, if not already existing
     */
	void CheckIfFilesExist(TBool aCheckExist, const RArray<TPtrC>& aFileArray, RTestUtilSessionSwi& aTestUtil, TInt aMsecTimeout=0);
	 
	/**
	 * Checks whether certain files do or do not exist by checking the registry entry and
	 * sets test step result accordingly.
	 * @param aFileArray   Contains all the file names to check for missing/present.
	 */ 
	void CheckRegistryEntry(RArray<TPtrC>& aFileArray);

	/**
	 * Checks that the correct dialogs have been called during the installation.
	 */
	void CheckDialogsCalled(const Swi::Test::CUIScriptAdaptor& ui);
	
	/**
	 * Checks whether a fatal OCSP response was encountered during the installation.
	 */
	void CheckFatalOcspResponse(const Swi::Test::CUIScriptAdaptor& ui);
	
	/**
	 * Checks whether the values of ocsp outcomes in the ini file matches with that of 
	 * the DisplayOcspResultL function.
	 */
    void  CheckExpectedOcspOutcomes(const Swi::Test::CUIScriptAdaptor& ui);	

   /**
	 * Copy files from one place to another.
	 *
	 * @param aNumEntries Name of the key that specifies number of file name 
	 *                    entries
	 * @param aFrom  Base name of the keys specifying names of files to 
	 *                    copy from; an index (0..n-1) is appended to the base 
	 *                    name to get the actual key name
	 * @param aTo    Base name of the keys specifying names of files to 
	 *                    copy to; an index (0..n-1) is appended to the base 
	 *                    name to get the actual key name
	 */
	void CopyFilesL(const TDesC& aNumEntries, const TDesC& aFrom, const TDesC& aTo);
	
	/**
 	 * Delete files.
 	 *
 	 * @param aNumEntries     Name of the key that specifies number of file name 
 	 *                        entries
 	 * @param aDeleteKeyBase  Base name of the keys specifying names of files
 	 *                        to delete; an index (0..n-1) is appended to the
 	 *                        base name to get the actual key name
 	 */
 	void DeleteFilesL(const TDesC& aNumEntries, const TDesC& aDeleteKeyBase);
	
	/**
	 * Stores all the values of expected install sizes as indicated in the test
	 * step's INI file.
	 * @param aNumExpectedSizeEntries Name of key that specifies the number of
	 *                                expected install sizes.
	 * @param aExpectedInstallSize    A reference to the array of install sizes.
	 */
	void StoreExpectedInstallSizesL(TInt aNumExpectedSizeEntries, RArray<TTableOfInstallSizes>& aExpectedValue);
	
	/** Verify the Pub and Sub value of the current installation/uninstallation */
	void CheckPubSubStatus(TDesC& aPubSubStatus);

	/**
	 * Check that the pub and sub status returns to KErrNone after a test.
	 * This check should typically be last, whereas the previous method should
	 * be done ASAP after an install/uninstall since there's a race condition.
	 */
	void CheckPubSubStatusIdle();
	
   /**
	 * Set or clear read only bit on files.
	 *
	 * @param aNumEntries     Name of the key that specifies number of file name 
	 *                        entries
	 * @param aFileKeyBase    Base name of the keys specifying names of files
	 *                        to set; an index (0..n-1) is appended to the
	 *                        base name to get the actual key name
	 * @param aFileKeyBase    Base name of the keys specifying operation on files
	 *                        ; an index (0..n-1) is appended to the
	 *                        base name to get the actual key name.
	 */
	void SetClearReadOnlyFilesL(const TDesC& aNumEntries,
								const TDesC& aFileKeyBase,
								const TDesC& aSetClearOpBase);
	
   /**
	 * Check current step result against list of expected result codes.
	 */
	void CheckExpectedResultCodesL();

	/**
	 * Compares hashes for pairs of files read from config section, sets the
	 * test step result to fail if any pair does not match or if there is an
	 * error attempting to get a hash.
	 */
	void CompareFilesL();

	/**
	 * Compares file contents with buffer. Sets test step result to fail if
	 * the contents do not match or there is an error in performing the
	 * operation.
	 */
	void CompareFileWithBufferL(const Swi::Test::CUIScriptAdaptor& ui);

	/**
	 * Get the device supported languages from the config section.
	 */
	void GetDevSupportedLanguagesL(RArray<TInt>& aDeviceLanguages);

	};

/**
 * Installation test step class
 * @test
 * @internalComponent
 */
class CSwisInstallStep : public CSwisTestStep
	{
public:
	enum TInstallType { EUseFileHandle, EUseMemory, EUseFileName, EUseCAF, EUseOpenFileName, ECheckExitValue, ECheckInstallPerformance};

	CSwisInstallStep(TInstallType aInstallType, TBool aDoCancelTest = EFalse);
	~CSwisInstallStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	TInt DoInstallL(Swi::CInstallPrefs& aInstallPrefs);
	void GetFilesToHoldOpenL();
	void PrintPerformanceLog(TTime aTime);

private:
	TFileName iSisFileName; // name of the file to install
	TFileName iXmlFileName; // name of the file containing dialog data
	Swi::Test::CUIScriptAdaptor* iUi; // XML script-based UI implementation
	TInstallType iInstallType; // use file handle to install
	RArray<TTableOfInstallSizes> iExpectedValue; // array of expected install sizes
	TBool iDoCancelTest;
	TPtrC iExpectedHash;
	TInt iExpectedDialog;
	TInt  iKErrCode;
	TPtrC Outcomes;// outcomes as received from ini file
	TUid iUid;
	TInt iExpectedUserDrive; // -1 if not checked
	TPtrC iPubSubStatus;
	HBufC* iFileToWatch;
	TBool iExpectFileChange;
	RPointerArray<RFile> iFilesHeldOpen;
	TBool iUseDeviceLanguages;
	RArray<TInt> iDeviceLanguages;
	};

/**
 * Uninstallation test step class
 * @test
 * @internalComponent
 */
class CSwisUninstallStep : public CSwisTestStep
	{
public:
	enum TUninstallType { EByUid, EByPackage };
	
	CSwisUninstallStep(TUninstallType aType, TBool aDoCancelTest = EFalse);
	~CSwisUninstallStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	TInt DoUninstallL();

private:
	TUid iUid;
	TFileName iXmlFileName; // name of the file containing dialog data
	TPtrC iVendorName;
	TInt iExpectedDialog;
	TInt iKErrCode;
	Swi::Test::CUIScriptAdaptor* iUi; // XML script-based UI implementation
	TPtrC iPackageName;
	TUninstallType iType;
	TBool iDoCancelTest;
	TPtrC iPubSubStatus;
	TBool iIsSafeMode;
	};


/**
 * CTrustStatusStep 
 * @test
 * @internalComponent
 */
class CTrustStatusStep : public CSwisTestStep
	{
public:
	CTrustStatusStep();
	virtual TVerdict doTestStepL();
	TPtrC* GetRevocationName(Swi::TRevocationStatus t);
	TPtrC* GetValidationName(Swi::TValidationStatus t);
	};

/**
 * CPostInstallRevocationCheckStep 
 * @test
 * @internalComponent
 */
class CPostInstallRevocationCheckStep : public CSwisTestStep
	{
public:
	CPostInstallRevocationCheckStep();
	virtual TVerdict doTestStepL();
	};
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
/**
 * CheckSCRField test step class
 * @test
 * @internalComponent
 */
class CCheckScrFieldStep : public CSwisTestStep
	{
public:	
	virtual TVerdict doTestStepL();
	};
	
class CCheckScrCompPropertyStep : public CSwisTestStep
	{
public:	
	virtual TVerdict doTestStepL();
	};
#endif

_LIT(KSwisInstallStep, "InstallStep");
_LIT(KSwisInstallPerformanceStep, "InstallPerformanceStep");
_LIT(KSwisInstallFHStep, "InstallFHStep"); // install using file handles
_LIT(KSwisInstallMemStep, "InstallMemStep"); // install from memory
_LIT(KSwisInstallCAFStep, "InstallCAFStep"); // install from CAF
_LIT(KSwisInstallOpenFileStep, "InstallOpenFileStep"); // install with already opened file
_LIT(KSwisCheckedInstallStep, "CheckedInstallStep");

// Cancel testing 
_LIT(KSwisInstallCancelStep, "InstallStepCancel");
_LIT(KSwisInstallFHCancelStep, "InstallFHStepCancel"); // install using file handles
_LIT(KSwisInstallMemCancelStep, "InstallMemStepCancel"); // install from memory
_LIT(KSwisInstallCAFCancelStep, "InstallCAFStepCancel"); // install from CAF

_LIT(KSwisUninstallStep, "UninstallStep");
_LIT(KSwisUninstallAugStep, "UninstallAugStep"); // uninstall of augmentation
_LIT(KTrustStatus, "TrustStatus");
_LIT(KPirCheck, "PostInstallRevocationCheck");

_LIT(KSwisUninstallCancelStep, "UninstallStepCancel");
_LIT(KSwisUninstallAugCancelStep, "UninstallAugStepCancel"); // uninstall of augmentation
_LIT(KNumExpectedSizes, "numExpectedSizes"); // This specifies that the test section
											 // has values for installation size(s).

_LIT(KSwisMmcFormatStep, "MmcFormatStep");
_LIT(KSwisMmcMountStep, "MmcMountStep");
_LIT(KSwisMmcUnMountStep, "MmcUnMountStep");
_LIT(KSwisMmcCopyFiles, "MmcCopyFilesStep");
_LIT(KSwisMmcCheckFiles, "MmcCheckFilesStep");
_LIT(KSwisMmcResetTags, "MmcResetTagsStep");
_LIT(KSwisMmcDeleteFiles, "MmcDeleteFilesStep");
_LIT(KSwisMmcGetNumFilesFiles, "MmcGetNumFilesStep");
_LIT(KAskRegistry, "MmcAskRegistry");
_LIT(KSetClearReadOnly, "SetClearReadOnlyStep");
_LIT(KQuerySidViaApparcPlugin, "QuerySidViaApparcPlugin");

_LIT(KSwisListUninstallPkgsStep, "ListUninstallPkgsStep");
_LIT(KSwisRemoveUninstallPkgsStep, "RemoveUninstallPkgsStep");
_LIT(KSwisGetPackageDetails, "GetPackageDetails");
_LIT(KSwisGetPublishedUidArrayStep, "GetPublishedUidArrayStep");
_LIT(KSwisRemoveWithLastDependent, "RemoveWithLastDependent");
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
_LIT(KCheckSCRFieldStep, "CheckSCRFieldStep");
_LIT(KCheckSCRCompPropertyStep, "CheckSCRCompPropertyStep");
#endif

/**
 * Listing and Removing Uninslled Pkgs test step class
 * @test
 * @internalComponent
 */
class CSwisUninstallPkgsStep : public CSwisTestStep
	{
public:
	enum TUnInstallAppType { EListPkgs, ERemovePkgs, EPkgDetails};

	CSwisUninstallPkgsStep(TUnInstallAppType aInstallType);
	~CSwisUninstallPkgsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TUnInstallAppType iUnInstallAppType; // use file handle to install
	TUid iUid;
	TInt iDrive;
	TChar iDriveChar;
	};

/**
 * MMC emulation test step class
 * @test
 * @internalComponent
 */
class CSwisMmcStep : public CSwisTestStep
	{
public:
	enum TMmcOperation { 
		/**
		 * EFormat - Format the specified drive
		 *
		 * If the drive is NOT mounted the format will fail with error -18 KErrNotReady
		 *
		 * nb. Formating a drive looks like an MMC insertion to the SWI
		 * daemon (it does NOT look like a remove followed by an insert).
		 */
		EFormat,
		/**
		 * EMount - Mount the specified drive
		 *
		 * Mounting an already mounted drive fails with error -21 KErrAccessDenied
		 * 
		 * Mounting an unmounted drive looks exactly like a MMC card
		 * insertion to the SWI daemon.
		 *
		 * [The SWI daemon detects MMC insertion by registering with the
		 * FS server using NotifyChange(ENotifyEntry,,) for a non-existent
		 * file on the drive being watched. It then checks if it can read
		 * the Volume info to decide if media has been inserted or
		 * removed.]
		 */
		EMount,
		/**
		 * EUnMount - UnMount the specified drive
		 *
		 * Un-mounting an already un-mounted drive fails with error -18 KErrNotReady
		 *
		 * Unmounting an mounted drive looks exactly like a MMC card
		 * removal to the SWI daemon.
		 *
		 * Will fail if there are any open file descriptors on the drive.
		 */
		EUnMount,
		
		/**
		 * ECopyFiles - Copy some files to the MMC
		 */
		ECopyFiles,
		/**
		 * ECheckFiles - Check set of files specified by
		 * numexist/existX and numnonexist/nonexistX are present/absent as specified.
		 */
		ECheckFiles,
		/**
		 * EResetTags - Delete the tags directory for a new test
		 */
		EResetTags,
		/**
 		 * EDeleteFiles - Delete some files which may be in private directories
 		 */
		EDeleteFiles,
		/**
 		 * EGetNumFiles - Get the number of files in a given directory
 		 */
		EGetNumFiles,
		EAskReg,
		ESetClearReadOnly,
		/**
		 * Query if the specified SID is installed
		 */
		EQuerySidViaApparcPlugin
	};
	CSwisMmcStep(TMmcOperation aOperation);
	~CSwisMmcStep();




	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	TMmcOperation iOperation;
	TInt iDrive;
	TInt iBootMode;
	TChar iDriveChar;

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#ifndef SWI_TEXTSHELL_ROM
	CAppSidChecker *iSwiSidChecker;
#endif
#endif

	};
	
/**
 * Check Published Uid test step class
 * @test
 * @internalComponent
 */
const TInt KTestMaxUidCount=20; //One more than the maximum number of Uids that the array, publishing the Uids, holds
class CSwisCheckPublishUidStep : public CSwisTestStep
    {
public:
    CSwisCheckPublishUidStep();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();

private:
	void GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex);
	
private:
TUid iExpectedUidList[KTestMaxUidCount];    //An array whose first element holds the number of uids in the subsequent indices of the array, followed by the the list of uids that are being published.
TBool iJustDefineProperty;
	};

class CSwisSetRemoveWithLastDependent : public CSwisTestStep
    {
public:    
    CSwisSetRemoveWithLastDependent();
    ~CSwisSetRemoveWithLastDependent();
    virtual TVerdict doTestStepL();
    };
#endif // __TSWISSTEP_H__
