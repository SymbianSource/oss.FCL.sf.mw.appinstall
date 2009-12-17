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
* This file defines the new UI mix-in interfaces which UI developers must
* implement to interact with the new software installer.
* The interfaces here defined subsume the old software install mix-in (MInstUIHandler)
* defined in insteng.h
*
*/


/**
 @file
 @internalAll
 @released
*/

#ifndef __MSISUIHANDLERS_H__
#define __MSISUIHANDLERS_H__

#include <e32base.h>
#include <e32std.h>
#include <x509cert.h>
#include <ocsp.h>
#include <pkixvalidationresult.h>

#ifdef SWI_TEXTSHELL_ROM
class CApaMaskedBitmap;
#else
#include <apgicnfl.h>
#endif

class CPKIXValidationResultBase;

namespace Swi
{
/**
 * Software Install error dialog type for SISX files.
 *
 * @see MUiHandler::DisplayErrorL()
 *
 * @publishedPartner
 * @released
 */
enum TErrorDialog
	{
	EUiAlreadyInRom,				 ///< Cannot overwrite ROM component.
	EUiMissingDependency,			 ///< Missing a dependency.
	EUiRequireVer,					 ///< A dependency exists on a specific version of an application which is different from the one installed.
	EUiRequireVerOrGreater,			 ///< A dependency requires a version of an application which is greater than the one installed.  */
	EUiFileCorrupt,					 ///< SISX file corrupt, checksums stored in file do not match actual.
	EUiDiskNotPresent,				 ///< A removable media drive had no media while trying to access it.
	EUiCannotRead,              	 ///< We cannot read a file which is needed for us to proceed.
	EUiCannotDelete,            	 ///< We are forbidden to delete a certain file.
	EUiInvalidFileName,			     ///< A filename given inside the SISX package is not valid.
	EUiFileNotFound,                 ///< A file needed to proceed cannot be found.
	EUiInsufficientSpaceOnDrive,     ///< There was insufficient space left on the drive to perform an operation.
	EUiCapabilitiesCannotBeGranted,  ///< The applications requires system capability it is not signed for.
	EUiUnknownFile,						///< File to install is not a recognized SISX file.
	EUiMissingBasePackage,				///< A base package was missing when installing an augmentation or a partial upgrade, the descriptor parameter contains a TPckg<TUid> with the uid of the missing application, which will be the same as the uid of the package being installed
	EUiConstraintsExceeded, 		 ///< Constraints imposed by a developer mode certificate have been exceeded.
	EUiSIDViolation, 				 ///< The package contains a file with a protected SID which is not signed for.
	EUiVIDViolation, 			 	 ///< The package contains a file with a protected VID which is not signed for.
	EUiNoMemoryInDrive,				/// < No enough space in the selected drive to install the package.
	EUiUIDPackageViolation, 		///< The package uid is protected but not signed for.
	EUiOSExeViolation, 			 	 ///< The package contains an invalid eclipsing EXE that would eclipse an exe in ROM.
	EUiSIDMismatch,                   /// < Application SID doesnt match with existing SID in private Directory.
	EUiBlockingEclipsingFile,			///The package contains an invalid eclipsing file which is already in the device other than ROM.
	};

/**
 * Text file display options. This controls which type of dialog appears when using
 * MUiHandler::DisplayTextL().
 *
 * @see MUiHandler::DisplayTextL()
 *
 * @publishedPartner
 * @released
 */
enum TFileTextOption
	{
	EInstFileTextOptionContinue = 0,	///< It is recommended that the UI displays a dialog containing a Continue button. On exiting the dialog, installation continues regardless of the dialog's return value.
	EInstFileTextOptionSkipOneIfNo,		///< It is recommended that the UI displays a dialog containing Yes/No buttons. The software installer skips installing the next file if the dialog returns false (user pressed No). It will install the next file if the dialog returns true (user pressed Yes).
	EInstFileTextOptionAbortIfNo,		///< It is recommended that the UI displays a dialog containing Yes/No buttons. The software installer continues with the installation if the dialog returns true (user pressed Yes). It will abort the installation if the dialog returns false (user pressed No).
	EInstFileTextOptionExitIfNo,	    ///< Same as EInstFileTextOptionAbortIfNo.
	EInstFileTextOptionForceAbort		///< It is recommended that UI display a TextContinue dialog.On exiting the dialog , installation must abort regardless of the dialog's return value. 
	};

/**
 * Software Install event type.
 *
 * @see MUiHandler::HandleInstallEventL()
 *
 * @publishedPartner
 * @released
 *
 * Note: EEventAbortedInstall and EEventAbortedUnInstall are now
 * considered as superfluous and are no longer generated. The
 * application knows when it requests a cancel, and knows the cancel is
 * complete when either the synchronous Launcher call returns or the
 * asynchronous CAsyncLauncher request completes.
 */
enum TInstallEvent
	{
	EEventSetProgressBarFinalValue,        ///< Gives the final value for the progress bar.
	EEventUpdateProgressBar,               ///< The progress bar must be updated.
	EEventAbortedInstall,                  ///< The installation has been aborted - NOT generated
	EEventCompletedInstall,                ///< The installation has been completed.
	EEventCompletedUnInstall,              ///< The uninstallation has been completed.
	EEventAbortedUnInstall,                ///< The uninstallation has been aborted - NOT generated
	EEventCompletedNoMessage,              ///< The operation has been completed.
	EEventLeave,                           ///< An error has occurred: a function issued a "leave".
	EEventOcspCheckEnd,					   ///< The certificate revocation check has been completed.
	EEventDevCert,						   ///< The installed application is signed by devcert
	};

/**
 * Software Install event types which can be cancelled.
 *
 * @see MUiHandler::HandleInstallCancellableEventL()
 *
 * @publishedPartner
 * @released
 */
enum TInstallCancellableEvent
	{
	EEventRemovingFiles,                   ///< Files are being removed.
	EEventShuttingDownApps,                ///< Some applications are being shut down.
	EEventCopyingFiles,                    ///< Files are being copied.
	EEventOcspCheckStart,				   ///< Certificate status check is being carried out
	};

/**
 * Software Install generic question type.
 *
 * @see MUiHandler::DisplayQuestionL()
 *
 * @released
 * @publishedPartner
 */
enum TQuestionDialog
	{
	EQuestionIncompatible,				///< Ask whether to install an incompatible installation on the device
	EQuestionOverwriteFile,		///< Ask whether to overwrite a file, which is not part of any package installation. The descriptor parameter contains the name of the file which is being overwritten.
	};


/**
 * Software Install signature validation result.
 *
 * @see MUiHandler::DisplaySecurityWarningL()
 *
 * @publishedPartner
 * @released
 */
enum TSignatureValidationResult
	{
	EValidationSucceeded,                   ///< The signature has been validated successfully.
	ESignatureSelfSigned,					///< The signature validates correctly, but chains back to a self-signed root
	ENoCertificate,                         ///< Cannot find the certificate.
	ECertificateValidationError,            ///< An error has occurred during signature validation.
	ESignatureNotPresent,                   ///< There is no signature to verify.
	ESignatureCouldNotBeValidated,          ///< The signature could not be validated.
	ENoCodeSigningExtension,                ///< The certificate is not trusted for code signing usage.
	ENoSupportedPolicyExtension,            ///< Cannot find the certificate policies.
	EMandatorySignatureMissing              ///< A signature resolving to one of the mandatory certificate cannot be found.
	};

/**
 * Message type for the revocation check result dialog.
 *
 * @see MUiHandler::DisplayRevocationL()
 *
 * @publishedPartner
 * @released
 */
enum TRevocationDialogMessage
	{
	EInvalidRevocationServerUrl,            ///< The OCSP server URL provided is invalid.
	EUnableToObtainCertificateStatus,       ///< It is not possible to obtain the certificate status.
	EResponseSignatureValidationFailure,    ///< It was not possible to validate the OCSP server response signature.
	EInvalidRevocationServerResponse,       ///< The OCSP server reply is invalid.
	EInvalidCertificateStatusInformation,   ///< The certificate status information is invalid.
	ECertificateStatusIsUnknown,            ///< The certificate status is unknown.
	ECertificateStatusIsUnknownSelfSigned,	///< The certificate status is unknown, but this is expected for self-signed.
	ECertificateStatusIsRevoked             ///< The certificate has been revoked.
	};


/**
 * This mix-in is used to inform software install that the user has cancelled the operation. Currently only
 * used for the online certificate status check.
 *
 * @see MUiHandler::DisplayOcspCheckProgressL()
 *
 * @publishedPartner
 * @released
 */
class MCancelHandler
 {
 public:
   /**
    * Informs Software Install that the user has cancelled the operation.
    */
   virtual void HandleCancel() = 0;
 };

/**
 * A lightweight class containing certificate information.
 *
 * @publishedPartner
 * @released
 */
class CCertificateInfo : public CBase
	{
public:
	/**
	 * @internalAll
	 * 
	 * Constructs a new certificate info object from an existing X509 Certificate.
	 *
	 * @param aCertificate	The X509 certificate to construct the information from.
	 * @return				A pointer to the new CCertificateInfo object.
	 */
	IMPORT_C static CCertificateInfo* NewL(const CX509Certificate& aCertificate);

	/**
	 * Constructs a new certificate info object from an existing X509 Certificate.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aCertificate	The X509 certificate to construct the information from.
	 * @return				A pointer to the new CCertificateInfo object.
	 */
	IMPORT_C static CCertificateInfo* NewLC(const CX509Certificate& aCertificate);

	/**
	 * @internalAll
	 * 
	 * Constructs a new certificate info object from a readable stream.
	 *
	 * @param aStream	The stream to read the certificate info from.
	 * @return			A pointer to the new CCertificateInfo object.
	 */
	IMPORT_C static CCertificateInfo* NewL(RReadStream& aStream);

	/**
	 * @internalAll
	 * 
	 * Constructs a new certificate info object from a readable stream.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aStream	The stream to read the certificate info from.
	 * @return			A pointer to the new CCertificateInfo object.
	 */
	IMPORT_C static CCertificateInfo* NewLC(RReadStream& aStream);

	/**
	 * Gets the subject name of the certificate.
	 *
	 * @return	The subject name of the certificate.
	 */
	inline const TDesC& SubjectName() const;

	/**
	 * Gets the issuer name of the certificate.
	 *
	 * @return	The issuer name of the certificate.
	 */
	inline const TDesC& IssuerName() const;

	/**
	 * Gets the date the certificate is valid from.
	 *
	 * @return	The date the certificate is valid from.
	 */
	inline const TDateTime& ValidFrom() const;

	/**
	 * Gets the date the certificate is valid until.
	 *
	 * @return	The date the certificate is valid until.
	 */
	inline const TDateTime& ValidTo() const;

	/**
	 * Gets the subject name of the certificate.
	 *
	 * @return	The subject name of the certificate.
	 */
	inline const TDesC8& Fingerprint() const;

	/**
	 * Gets the subject name of the certificate.
	 *
	 * @return	The subject name of the certificate.
	 */
	inline const TDesC8& SerialNumber() const;

	/**
	 * @internalAll
	 * 
	 * Initializes this object from the specified read stream.
	 *
	 * @param aStream	Stream from which the contents should be internalized.
 	 */
	void InternalizeL(RReadStream& aStream);

	/**
	 * @internalAll
	 * 
	 * Externalizes this object to the specified write stream.
	 *
	 * @param aStream	Stream to which the contents should be externalized.
 	 */
	void ExternalizeL(RWriteStream& aStream) const;

	virtual ~CCertificateInfo();
private:
	CCertificateInfo();

	void ConstructL(const CX509Certificate& aCertificate);

	/**
	 * Helper function used to internalize a descriptor from a stream, into a member variable.
	 * This function frees the memory pointed to first.
	 *
	 * @param aBuffer	Reference to the pointer which will point at newly allocated buffer.
	 * @param aStream	Stream from which the descriptor is read.
	 */
	void ReadBufFromStreamL(HBufC*& aBuffer, RReadStream& aStream);

	/**
	 * Helper function used to internalize a descriptor from a stream, into a member variable.
	 * This function frees the memory pointed to first.
	 *
	 * @param aBuffer	Reference to the pointer which will point at newly allocated buffer.
	 * @param aStream	Stream from which the descriptor is read.
	 */
	void ReadBufFromStreamL(HBufC8*& aBuffer, RReadStream& aStream);

private:
	HBufC* iSubjectName;
	HBufC* iIssuerName;
	HBufC8* iFingerprint;
	HBufC8* iSerialNumber;

	TDateTime iValidFrom;
	TDateTime iValidTo;
};


/**
 * A lightweight class containing information about the application being installed.
 *
 * @publishedPartner
 * @released
 */
class CAppInfo : public CBase
	{
public:
	/**
	 * @internalAll
	 * 
	 * Creates a new CAppInfo object from the specified read stream.
	 *
	 * @param aStream	Stream from which the contents should be internalised.
	 * @return			The new CAppInfo object
	 */

	IMPORT_C static CAppInfo* NewL(RReadStream& aStream);

	/**
	 * @internalAll
	 * 
	 * Creates a new CAppInfo object from the specified read stream.
	 * The object is inserted into the CleanupStack.
	 *
	 * @param aStream	Stream from which the contents should be internalised.
	 * @return			The new CAppInfo object
	 */
	IMPORT_C static CAppInfo* NewLC(RReadStream& aStream);

	/**
	 * Creates a new CAppInfo object  from the given parameters.
	 *
	 * @param aAppName		Stream from which the contents should be internalised.
	 * @param aAppVendor	The application vendor.
	 * @param aAppVersion	The application version.
	 * @return				The new CAppInfo object
	 */
 	IMPORT_C static CAppInfo* NewL(const TDesC& aAppName, const TDesC& aAppVendor, const TVersion& aAppVersion);

	/**
	 * @internalAll
	 * 
	 * Creates a new CAppInfo object from the given parameters.
	 * The object is inserted into the CleanupStack.
	 *
	 * @param aAppName		The application name.
	 * @param aAppVendor	The application vendor.
 	 * @param aAppVersion	The application version.
	 * @return				The new CAppInfo object
	 */
	IMPORT_C static CAppInfo* NewLC(const TDesC& aAppName, const TDesC& aAppVendor, const TVersion& aAppVersion);

	/**
	 * @internalAll
	 * 
	 * Initializes this object from the specified read stream.
	 *
	 * @param aStream	Stream from which the contents should be internalized.
 	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);

	/**
	 * @internalAll
	 * 
	 * Externalizes this object to the specified write stream.
	 *
	 * @param aStream	Stream to which the contents should be externalized.
 	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/**
	 * Gets the application name.
	 *
	 * @return	The application name.
	 */
	inline const TDesC& AppName() const;

	/**
	 * Gets the application vendor.
	 *
	 * @return	The application vendor.
	 */
	inline const TDesC& AppVendor() const;

	/**
	 * Gets the application version.
	 *
	 * @return The application version.
	 */
	inline const TVersion& AppVersion() const;

	virtual ~CAppInfo();
private:
	CAppInfo(const TVersion& aVersion=TVersion(0,0,0));
	void ConstructL(const TDesC& aAppName, const TDesC& aAppVendor);

	/**
	 * Helper function used to internalize a descriptor from a stream, into a member variable.
	 * This function frees the memory pointed to first.
	 *
	 * @param aBuffer	Reference to the pointer which will point at newly allocated buffer.
	 * @param aStream	Stream from which the descriptor is read.
	 */
	void ReadBufFromStreamL(HBufC*& aBuffer, RReadStream& aStream);

	HBufC* iAppName;
	HBufC* iAppVendor;
	TVersion iAppVersion;
 	};

/**
 * This represents the User Interface handler for the installation of SISX files by Software Install Server.
 *
 * @publishedPartner
 * @released
 */
class MInstallerUiHandler
	{
public:
	/**
	 * Displays a dialog at the beginning of an installation with details about the SISX package
	 * being installed. This dialog must be shown in TrustedUI mode.
	 *
     * An installer UI should call REComSession::FinalClose() as part of clean up.
	 *
	 * @note Trusted UI
	 *
	 * @param aAppInfo		Information about the application.
	 * @param aLogo			The logo data in the SISX file. If no logo is present it will be NULL.
	 * @param aCertificates	An array of end entity certificates, one for each chain in the SISX file.
	 *						If the SISX file is unsigned this array will have zero length.
	 * @return				ETrue if the user pressed the Yes button to continue the install;
	 *						EFalse if the user pressed the No button, aborting the installation.
	 */
	virtual TBool DisplayInstallL(const CAppInfo& aAppInfo,
				const CApaMaskedBitmap* aLogo,
				const RPointerArray<CCertificateInfo>& aCertificates)=0;

	/**
	 * Displays a dialog requesting that the user grants these capabilities to the SISX package
	 * being installed. This dialog must be shown in TrustedUI mode.
	 *
	 * @note Trusted UI
	 *
	 * @param aAppInfo			Information about the application.
	 * @param aCapabilitySet	The capabilities requested by the SISX package.
	 * @return					ETrue to continue the installation; EFalse to abort the installation.
	 */
	virtual TBool DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo,
						const TCapabilitySet& aCapabilitySet)=0;

	/**
	 * Requests preferred language and dialect information from the UI.
	 *
	 * @param aAppInfo		Information about the application.
	 * @param aLanguages	Array of languages supported by the SIS file.
	 * @return				Must return index in the passed array that corresponds to the user's selected
	 *						preferred language. If out-of-bounds value is returned, the engine will abort
	 *						installation.
	 */
	virtual TInt DisplayLanguageL(const CAppInfo& aAppInfo,
				      const RArray<TLanguage>& aLanguages)=0;

	/**
	 * Requests the drive to install the package on, from the UI.
	 *
	 * @param aAppInfo		Information about the application.
	 * @param aSize			Total size of the files that will be installed on the configurable drive.
	 * @param aDriveLetters	Array of drive letters on which the package can be installed.
	 * @param aDriveSpaces	Array of space free (in bytes) on each of the drives in the aDriveLetters array.
	 * @return				An index in the drive array. If the index is out of range the engine will abort
	 *						the installation.
	 */
	virtual TInt DisplayDriveL(const CAppInfo& aAppInfo,
				   TInt64 aSize,
				   const RArray<TChar>& aDriveLetters,
				   const RArray<TInt64>& aDriveSpaces)=0;

	/**
	 * Displays a dialog asking whether we want to upgrade an application present on the device.
	 *
	 * @param aAppInfo			Information about the application being installed.
	 * @param aExistingAppInfo	Information about the existing application.
	 * @return					ETrue if the user pressed the Yes button;
	 *							EFalse if the user pressed the No button.
	 */
	virtual TBool DisplayUpgradeL(const CAppInfo& aAppInfo,
				      const CAppInfo& aExistingAppInfo)=0;

	/**
	 * Displays a dialog box asking which options the user wishes to install.
	 *
	 * @param aAppInfo		Information about the application.
	 * @param aOptions		An array of option names.
	 * @param aSelections	An array of booleans, corresponding to the aOptions array.
	 * @return				ETrue on success; EFalse on error.
	 */
	virtual TBool DisplayOptionsL(const CAppInfo& aAppInfo,
				      const RPointerArray<TDesC>& aOptions,
				      RArray<TBool>& aSelections)=0;

	/**
	 * Informs the UI of events occurring during installation.
	 *
	 * @param aAppInfo	Information about the application.
	 * @param aEvent	The event which has occurred.
	 * @param aValue	The value associated with the event if applicable.
	 * @param aDes		A descriptor associated with the event if applicable.
	 * @return ETrue if installation is not cancelled; otherwise EFalse.


	 */
	virtual TBool HandleInstallEventL(const CAppInfo& aAppInfo,
					 TInstallEvent aEvent,
					 TInt aValue=0,
					 const TDesC& aDes=KNullDesC)=0;
	/**
	 * Informs the UI of cancellable events occurring during installation.
	 *
	 * @param aAppInfo			Information about the application.
	 * @param aEvent			The event which has occurred.
	 * @param aCancelHandler	This mix-in is used to inform Software Install that the user has
	 *							pressed the cancel button on the dialog.
	 * @param aValue			The value associated with the event if applicable.
	 * @param aDes				A descriptor associated with the event if applicable.
	 */
	virtual void HandleCancellableInstallEventL(const CAppInfo& aAppInfo,
							TInstallCancellableEvent aEvent,
						    MCancelHandler& aCancelHandler,
						    TInt aValue=0,
						    const TDesC& aDes=KNullDesC)=0;

	/**
	 * Displays a dialog indicating that the package is unsigned, or that
	 * signature validation has failed.
	 *
	 * @param aAppInfo             Information about the application.
	 * @param aSigValidationResult The result of signature validation.
	 * @param aPkixResults         Results of a validation of the PKIX
	 *                             certificate chains
	 * @param aCertificates        Certificate information for the failed end
	 *                             (signing) certificates
	 * @param aInstallAnyway	   ETrue if installation is allowed to continue.
	 *
	 * @return ETrue to continue the installation, EFalse to abort.
	 */
	virtual TBool DisplaySecurityWarningL(const CAppInfo& aAppInfo,
		TSignatureValidationResult aSigValidationResult,
		RPointerArray<CPKIXValidationResultBase>& aPkixResults,
		RPointerArray<CCertificateInfo>& aCertificates,
		TBool aInstallAnyway) = 0;

	/**
	 * Displays a dialog indicating that there was an error carrying out online
	 * certificate status checking.
	 *
	 * @param aAppInfo      Application info from the package file
	 * @param aMessage      The error which occurred during online certificate
	 *                      status checking
	 * @param aOutcomes     The outcomes of the OCSP checks
	 * @param aCertificates Certificate information of the end (signing)
	 *                      certificates that correspond to the outcomes
	 * @param aWarningOnly  ETrue if the installation is allowed to continue
	 *                      after a warning, EFalse otherwise.
	 *
	 * @return ETrue on success, EFalse on error.
	 */
	virtual TBool DisplayOcspResultL(const CAppInfo& aAppInfo,
		TRevocationDialogMessage aMessage,
		RPointerArray<TOCSPOutcome>& aOutcomes,
		RPointerArray<CCertificateInfo>& aCertificates,
		TBool aWarningOnly) = 0;

	/**
	 * Used to inform the user the installation they are attempting is trying to overwrite
	 * a file that does not belong to it. Following this message installation is aborted.
	 *
	 * @param aAppInfo			Information about the application.
	 * @param aInstalledAppInfo	Information about the application that owns the file in question.
	 * @param aFileName			The filename of the file in question
	 */
	virtual void DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo,
						 const CAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName) = 0;

	/**
	 * Used to inform the user of a missing dependency needed to install the
	 * current package. It is up to the UI implementors and/or the user whether
	 * or not this warning can be overridden.
	 *
	 * @param aAppInfo           Info about the current application.
	 * @param aDependencyName    Name of the missing dependency.
	 * @param aWantedVersionFrom Starting suitable version number of the mising
	 *                           dependency. Can be all zeros, meaning it is
	 *                           unknown.
	 * @param aWantedVersionFrom Ending suitable version number of the mising
	 *                           dependency. Can be all zeros, meaning it is
	 *                           unknown.
	 * @param aInstalledVersion  The installed (unsuitable) version of the
	 *                           missing dependency. Can be all zeros, meaning
	 *                           the missing dependency is not installed.
	 *
	 * @return \e ETrue to continue installation, \e EFalse to abort.
	 */
	virtual TBool DisplayMissingDependencyL(
		const CAppInfo& aAppInfo,
		const TDesC& aDependencyName,
		TVersion aWantedVersionFrom,
		TVersion aWantedVersionTo,
		TVersion aInstalledVersion)=0;
	}; // class MInstallerUiHandler

/**
 * This represents the User Interface handler for the uninstallation of SISX files by Software Install Server.
 *
 * @publishedPartner
 * @released
 */
class MUninstallerUiHandler
	{
public:
	/**
	 * Displays a dialog at the start of the uninstallation with information about the application
	 * to be uninstalled. The user may cancel the uninstallation at this stage. This dialog must
	 * be shown in TrustedUI mode.
	 *
	 * @note Trusted UI
	 *
	 * @param aAppInfo	Information about the application.
	 * @return			ETrue to continue the uninstallation; EFalse to abort the uninstallation.
	 */
	virtual TBool DisplayUninstallL(const CAppInfo& aAppInfo)=0;
	}; // class MUninstallerUiHandler

/**
 * Common dialogs
 *
 * @publishedPartner
 * @released
 */
class MCommonDialogs
	{
public:
	/**
	 * Displays a dialog containing text. It is recommended that this dialog has only a Continue
	 * button if aOption is EInstFileTextOptionContinue, or Yes and No buttons otherwise.
	 *
	 * @param aAppInfo	Information about the application.
	 * @param aOption	The type of dialog box to display.
	 * @param aText		The text to display
	 * @return			ETrue is the user wants to go forward; EFalse otherwise.
	 */
	 virtual TBool DisplayTextL(const CAppInfo& aAppInfo,
				    TFileTextOption aOption,
				    const TDesC& aText)=0;

	/**
	 * Displays a dialog showing an error message.
	 *
	 * @param aAppInfo	Information about the application.
	 * @param aType		The error message to display.
	 * @param aParam	An error specific parameter.
	 */
	virtual void DisplayErrorL(const CAppInfo& aAppInfo,
				   TErrorDialog aType,
				   const TDesC& aParam)=0;

	/**
	 * Displays a dialog indicating that a component which is being uninstalled is a dependency
	 * of another component installed.
	 *
	 * @param aAppInfo		Information about the application.
	 * @param aComponents	An array of components which depend on components of the application
	 * @return				ETrue to continue the uninstallation; EFalse to abort.
	 */
	virtual TBool DisplayDependencyBreakL(const CAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents) = 0;

	/**
	 * Informs user that the applications are in use during an uninstallation.
	 * Return ETrue if the user wishes to continue the removal process and
	 * close the applications anyway
	 *
	 * @param aAppInfo	Information about the application.
	 * @param aAppNames	An array containing the name of the application in use.
	 * @return			ETrue if the user wishes to continue with removal.
	 */
	virtual TBool DisplayApplicationsInUseL(const CAppInfo& aAppInfo, const RPointerArray<TDesC>& aAppNames) = 0;

	/**
	 * Displays a generic question dialog with a Yes/No answer.
	 *
	 * @param aAppInfo	Information about the application.
	 * @param aQuestion	The question to ask.
	 * @param aDes		A descriptor associated with the question if applicable.
	 * @return			ETrue if the user pressed the Yes button;
	 *					EFalse if the user pressed the No button.
	 */
	virtual TBool DisplayQuestionL(const CAppInfo& aAppInfo, TQuestionDialog aQuestion, const TDesC& aDes=KNullDesC)=0;

	}; // class MCommonDialogs


/**
 * The global mix-in that UI must implement to interact with Software Install.
 *
 * @publishedPartner
 * @released
 */
class MUiHandler : public MCommonDialogs, public MInstallerUiHandler, public MUninstallerUiHandler
	{
	};


// inline functions from CAppInfo

inline const TDesC& CAppInfo::AppName() const
	{
	return *iAppName;
	}

inline const TDesC& CAppInfo::AppVendor() const
	{
	return *iAppVendor;
	}

inline const TVersion& CAppInfo::AppVersion() const
	{
	return iAppVersion;
	}


// inline functions from CCertificateInfo

inline const TDesC& CCertificateInfo::SubjectName() const
	{
	return *iSubjectName;
	}

inline const TDesC& CCertificateInfo::IssuerName() const
	{
	return *iIssuerName;
	}

inline const TDateTime& CCertificateInfo::ValidFrom() const
	{
	return iValidFrom;
	}

inline const TDateTime& CCertificateInfo::ValidTo() const
	{
	return iValidTo;
	}

inline const TDesC8& CCertificateInfo::Fingerprint() const
	{
	return *iFingerprint;
	}

inline const TDesC8& CCertificateInfo::SerialNumber() const
	{
	return *iSerialNumber;
	}

} // namespace Swi

#endif // #ifndef __MSISUIHANDLERS_H__


