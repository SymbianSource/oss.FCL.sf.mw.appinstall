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
* Definition of the client interface for the UI Support Server
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __SISUIHANDLER_H__
#define __SISUIHANDLER_H__

#include <e32std.h>

#include "msisuihandlers.h"
#include "appinfo.h"
#include "sislogo.h"

namespace Swi
{
class CUissCmd;
class CWriteStream;
class CProgressBarValuePublisher;

/**
 * RClass which is used to make calls to the UI Support server using
 * the client-server framework. The ExecuteL() method is passed a
 * command, in order to display the associated dialog box.
 */
class RUiHandler : public RSessionBase
	{
public:
	/** Open the connection to the UI Support Server*/
	IMPORT_C TInt Connect();

	/** Close the connection to the UI Support Server*/
	IMPORT_C void Close();
	
	/**
	 * Execute a command in order to display a dialog using the
	 * UI Support Server.
	 */
	IMPORT_C void ExecuteL(CUissCmd& aCmd);

	/**
	 * Helper function to update progress bar
	 */
	IMPORT_C void UpdateProgressBarL(const TAppInfo& aAppInfo, TInt aAmount);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/** Sets a reference to the progress bar value publisher object. The ownership is NOT transferred. */
	IMPORT_C void SetProgressBarValuePublisher(CProgressBarValuePublisher* aPublisher);
private:
	CProgressBarValuePublisher* iPublisher; // Owned by the state machine
#endif
	};

/**
 * Base class used for client classes, which marshall the arguments in order to make 
 * a request to the server.  
 */
class CUissCmd : public CBase
	{
protected:
	/** Constructor */
	IMPORT_C CUissCmd(TInt aMessage);

public:
	/** Destructor */
	IMPORT_C ~CUissCmd();

	/**
	 * Override this function in order to unmarshall the arguments,
	 * from a TIpcArgs object after the client-server call has
	 * been completed.
	 *
	 */
	IMPORT_C virtual void UnmarshallArgumentsL();	
	
	/**
	 * Override this function in order to marshall the arguments,
	 * into a TIpcArgs object to pass them to the server. This function will
	 * only ever be called once per object instatiation.
	 *
	 * @param aArguments The IPC arguments for the client-server call. These will be
	 *					 set up by this function.
	 */
	virtual void MarshallArgumentsL(TIpcArgs& aArguments)=0;
	
	/**
	 * Returns the message number which identifies the client-server
	 * message associated with this object.
	 *
	 * @return The message number for the client-server call.
	 */
	TInt Message() const;
	
	/**
	 * This function is used to get the IPC arguments in order to make
	 * the client-server framework call.
	 *
	 * @return The arguments for the client-server call.
	 */
	const TIpcArgs& IpcArgsL();
private:
	TInt iMessage;		///< The message number for the client-server call.
	TIpcArgs iIpcArgs; ///< The IPC argumens for the client-server call
	TBool iArgumentsMarshalled; ///< Whether the arguments have been marshalled

protected:
	CWriteStream* iStream; ///< Stream used to stream Ipc Arguments into
	TPtrC8 iIpcInputPtr;   
	};

/** 
 * Class used to pass a command to shutdown the server to the UI Support
 * Server.
 */
class CShutdownServer : public CUissCmd
	{
public:
	IMPORT_C static CShutdownServer* NewL();
	IMPORT_C static CShutdownServer* NewLC();

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CShutdownServer();
	void ConstructL();
	};

/** 
 * Class used to pass a command to handle an installation event, to 
 * the UI Support Server.
 */
class CHandleInstallEvent : public CUissCmd
	{
public:
	IMPORT_C static CHandleInstallEvent* NewL(const TAppInfo& aAppInfo, TInstallEvent aEvent, 
						 TInt aValue, const TDesC& aDes);
	IMPORT_C static CHandleInstallEvent* NewLC(const TAppInfo& aAppInfo, TInstallEvent aEvent, 
						 TInt aValue, const TDesC& aDes);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CHandleInstallEvent(const TAppInfo& aAppInfo, TInstallEvent aEvent, 
						 TInt aValue=0, const TDesC& aDes=KNullDesC);

	const TAppInfo& iAppInfo;
	TInstallEvent iEvent;
	TInt iValue;
	const TDesC& iDes;

	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to handle a cancellable installation 
 * event, to the UI Support Server. An example of such an event is a progress 
 * indication event during installation.
 */
class CHandleCancellableInstallEvent : public CUissCmd
	{
public:
	IMPORT_C static CHandleCancellableInstallEvent* NewL(
		const TAppInfo& aAppInfo, TInstallCancellableEvent aEvent, TInt aValue,
		const TDesC& aDes);
	IMPORT_C static CHandleCancellableInstallEvent* NewLC(
		const TAppInfo& aAppInfo, TInstallCancellableEvent aEvent, TInt aValue,
		const TDesC& aDes);

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CHandleCancellableInstallEvent(const TAppInfo& aAppInfo, 
		TInstallCancellableEvent aEvent, TInt aValue=0, 
		const TDesC& aDes=KNullDesC);

	const TAppInfo& iAppInfo;
	TInstallCancellableEvent iEvent;
	TInt iValue;
	const TDesC& iDes;
	};

/** 
 * Class used to pass a command to display an error dialog, to the UI Support
 * Server.
 */
class CDisplayError : public CUissCmd
	{
public:
	IMPORT_C static CDisplayError* NewL(const TAppInfo& aAppInfo,
				   TErrorDialog aType, const TDesC& aParam);
	IMPORT_C static CDisplayError* NewLC(const TAppInfo& aAppInfo,
				   TErrorDialog aType, const TDesC& aParam);

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayError(const TAppInfo& aAppInfo,
				   TErrorDialog aType, const TDesC& aParam=KNullDesC);

	const TAppInfo& iAppInfo;
	TErrorDialog iType;
	const TDesC& iParam;
	};

/** 
 * Convey that the application is not compatible with the device to the UI support server.
 */
class CDisplayDeviceIncompatibility : public CUissCmd
	{
public:
	IMPORT_C static CDisplayDeviceIncompatibility* NewL(const TAppInfo& aAppInfo);
	IMPORT_C static CDisplayDeviceIncompatibility* NewLC(const TAppInfo& aAppInfo);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayDeviceIncompatibility(const TAppInfo& aAppInfo);

	const TAppInfo& iAppInfo;

	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};
	
/** 
 * Convey that the application has missing dependencies to the UI support server.
 */
class CDisplayDependencyWarning : public CUissCmd
	{
public:
	IMPORT_C static CDisplayDependencyWarning* NewL(
		const TAppInfo& aAppInfo,	const TDesC& aDependencyName,
		TBool aWantedFromValid,		const TVersion& aWantedFrom,
		TBool aWantedToValid,		const TVersion& aWantedTo,
		TBool aAvailableValid,		const TVersion& aAvailable);
	IMPORT_C static CDisplayDependencyWarning* NewLC(
		const TAppInfo& aAppInfo,	const TDesC& aDependencyName,
		TBool aWantedFromValid,		const TVersion& aWantedFrom,
		TBool aWantedToValid,		const TVersion& aWantedTo,
		TBool aAvailableValid,		const TVersion& aAvailable);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayDependencyWarning(
		const TAppInfo& aAppInfo,	const TDesC& aDependencyName,
		TBool aWantedFromValid,		const TVersion& aWantedFrom,
		TBool aWantedToValid,		const TVersion& aWantedTo,
		TBool aAvailableValid,		const TVersion& aAvailable);

	const TAppInfo& iAppInfo;
	const TDesC&	iDependencyName;
	const TBool		iWantedFromValid;
	const TVersion& iWantedFrom;
	const TBool		iWantedToValid;
	const TVersion& iWantedTo;
	const TBool		iAvailableValid;
	const TVersion& iAvailable;

	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display an error dialog, to the UI Support
 * Server.
 */
class CDisplayCannotOverwriteFile : public CUissCmd
	{
public:
	IMPORT_C static CDisplayCannotOverwriteFile* NewL(const TAppInfo& aAppInfo,
						 const TAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName);
	IMPORT_C static CDisplayCannotOverwriteFile* NewLC(const TAppInfo& aAppInfo,
						 const TAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayCannotOverwriteFile(const TAppInfo& aAppInfo,
						 const TAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName);

	const TAppInfo& iAppInfo;
	const TAppInfo& iInstalledAppInfo;
	const TDesC& iFileName;

	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display a security warning dialog,
 * to the UI Support Server.
 */
class CDisplaySecurityWarning : public CUissCmd
	{
public:
	/**
	 * Constructs a new command object to display security warning dialog
	 *
	 * @param aAppInfo             Application info from the SIS file
	 * @param aSigValidationResult Validation result to pass to dialog
	 * @param aPkixResults         Certificate chain validation results 
	 * @param aCertificates        End certificates corresponding to the results
	 * @param aInstallAnyway       Can the user override the warning?
	 *
	 * @return New command object
	 */
	IMPORT_C static CDisplaySecurityWarning* NewL(const TAppInfo& aAppInfo,
					     TSignatureValidationResult aSigValidationResult,
					     RPointerArray<CPKIXValidationResultBase>& aPkixResults,
					     RPointerArray<CCertificateInfo>& aCertificates,
					     TBool aInstallAnyway);
						  
	/**
	 * Constructs a new command object to display security warning dialog and 
	 * puts it on the cleanup stack 
	 *
	 * @param aAppInfo             [in] Application info from the SIS file
	 * @param aSigValidationResult [in] Validation result to pass to dialog
	 * @param aPkixResult          [in] Certificate chain validation result 
	 *                             (optional)
	 * @param aCertificate         [in] Offending end certificate or NULL
	 * @param aInstallAnyway       [in] Can the user override the warning?
	 *
	 * @return New command object on the cleanup stack
	 */
	IMPORT_C static CDisplaySecurityWarning* NewLC(const TAppInfo& aAppInfo,
					     TSignatureValidationResult aSigValidationResult,
					     RPointerArray<CPKIXValidationResultBase>& aPkixResults,
					     RPointerArray<CCertificateInfo>& aCertificates,
					     TBool aInstallAnyway);

	/**
	 * Provides result of the dialog
	 *
	 * @return The result code of the dialog
	 */
	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplaySecurityWarning(const TAppInfo& aAppInfo,
					     TSignatureValidationResult aSigValidationResult,
					     RPointerArray<CPKIXValidationResultBase>& aPkixResults,
					     RPointerArray<CCertificateInfo>& aCertificates,
					     TBool aInstallAnyway);

	const TAppInfo&                           iAppInfo;
	TSignatureValidationResult                iSigValidationResult;
	RPointerArray<CPKIXValidationResultBase>& iPkixResults;
	RPointerArray<CCertificateInfo>&          iCertificates;
	TBool                                     iInstallAnyway;

	// return result
	TBool        iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the initial install dialog,
 * to the UI Support Server.
 */
class CDisplayInstall : public CUissCmd
	{
public:
	/**
	 * Constructs a new command to display the Install dialog.
	 *
	 * @param aAppInfo      Application info object
	 * @param aFs           File sesson that has the logo file open.
	 * @param aLogoFile     Logo file handle (can be NULL).
	 * @param aCertificates Certificates in the SIS file (the array can be empty
	 *                      if the SIS file was not signed)
	 *
	 * @return New command object to display the Install dialog
	 */
	IMPORT_C static CDisplayInstall* NewL(const TAppInfo& aAppInfo,
		RFs& aFs, RFile* aLogoFile,
		const RPointerArray<CCertificateInfo>& aCertificates);
				
	/**
	 * Constructs a new command to display the Install dialog and puts it on the 
	 * cleanup stack. 
	 *
	 * @param aAppInfo      Application info object
	 * @param aFs           File sesson that has the logo file open.
	 * @param aLogoFile     Optional logo file handle.
	 * @param aCertificates Certificates in the SIS file (the array can be empty
	 *                      if the SIS file was not signed)
	 *
	 * @return New command object to display the Install dialog on the 
	 *         cleanup stack.
	 */
	IMPORT_C static CDisplayInstall* NewLC(const TAppInfo& aAppInfo,
		RFs& aFs, RFile* aLogoFile,
		const RPointerArray<CCertificateInfo>& aCertificates);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
	
private:
	CDisplayInstall(const TAppInfo& aAppInfo, RFs& aFs, RFile* aLogoFile,
		const RPointerArray<CCertificateInfo>& aCertificates);

	const TAppInfo& iAppInfo; ///< Application information.
	RFs& iFs; ///< File session that has the logo file open.
	RFile* iLogoFile; ///< Logo data file handle, can be NULL if no logo.
	/// End-entity certificates.
	const RPointerArray<CCertificateInfo>& iCertificates;
	/// Return result.
	TBool iResult;
	/// Packaged return result.
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the grant capabilities
 * dialog, to the UI Support Server.
 */
class CDisplayGrantCapabilities : public CUissCmd
	{
public:
	IMPORT_C static CDisplayGrantCapabilities* NewL(const TAppInfo& aAppInfo, 
							const TCapabilitySet& aCapabilitySet);
	IMPORT_C static CDisplayGrantCapabilities* NewLC(const TAppInfo& aAppInfo, 
							const TCapabilitySet& aCapabilitySet);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayGrantCapabilities(const TAppInfo& aAppInfo, 
							const TCapabilitySet& aCapabilitySet);

	const TAppInfo& iAppInfo;
	const TCapabilitySet& iCapabilitySet;

	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};
	
/** 
 * Class used to pass a command to display the language selection
 * dialog, to the UI Support Server.
 */
class CDisplayLanguage : public CUissCmd
	{
public:
	IMPORT_C static CDisplayLanguage* NewL(const TAppInfo& aAppInfo, 
					      const RArray<TLanguage>& aLanguages);
	IMPORT_C static CDisplayLanguage* NewLC(const TAppInfo& aAppInfo, 
					      const RArray<TLanguage>& aLanguages);

	IMPORT_C TInt ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayLanguage(const TAppInfo& aAppInfo, 
					      const RArray<TLanguage>& aLanguages);

	const TAppInfo& iAppInfo;
	const RArray<TLanguage>& iLanguages;
					      
	// return result
	TInt iResult;
	TPckg<TInt> iResultPackage;
	};	

/** 
 * Class used to pass a command to display the drive selection
 * dialog, to the UI Support Server.
 */
class CDisplayDrive : public CUissCmd
	{
public:
	IMPORT_C static CDisplayDrive* NewL(const TAppInfo& aAppInfo, 
					    TInt64 aSize,
					    const RArray<TChar>& aDriveLetters,
					    const RArray<TInt64>& aDriveSpaces);
	IMPORT_C static CDisplayDrive* NewLC(const TAppInfo& aAppInfo, 
					    TInt64 aSize,
					    const RArray<TChar>& aDriveLetters,
					    const RArray<TInt64>& aDriveSpaces);

	IMPORT_C TInt ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayDrive(const TAppInfo& aAppInfo, 
					    TInt64 aSize,
					    const RArray<TChar>& aDriveLetters,
					    const RArray<TInt64>& aDriveSpaces);

	const TAppInfo& iAppInfo;
	TInt64 iSize;
	const RArray<TChar>& iDriveLetters;
	const RArray<TInt64>& iDriveSpaces;					      

	// return result
	TInt iResult;
	TPckg<TInt> iResultPackage;
	};

/** 
 * Class used to pass a command to display the upgrade dialog,
 * to the UI Support Server.
 */
class CDisplayUpgrade : public CUissCmd
	{
public:
	IMPORT_C static CDisplayUpgrade* NewL(const TAppInfo& aAppInfo, 
					      const TAppInfo& aExistingAppInfo);
	IMPORT_C static CDisplayUpgrade* NewLC(const TAppInfo& aAppInfo, 
					      const TAppInfo& aExistingAppInfo);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayUpgrade(const TAppInfo& aAppInfo, 
					      const TAppInfo& aExistingAppInfo);

	const TAppInfo& iAppInfo;
	const TAppInfo& iExistingAppInfo;
	
	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display a generic question
 * dialog, to the UI Support Server.
 */
class CDisplayQuestion : public CUissCmd
	{
public:
	IMPORT_C static CDisplayQuestion* NewL(const TAppInfo& aAppInfo, TQuestionDialog aQuestion,
										   const TDesC& aDes = KNullDesC);
	IMPORT_C static CDisplayQuestion* NewLC(const TAppInfo& aAppInfo, TQuestionDialog aQuestion,
										   const TDesC& aDes = KNullDesC);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayQuestion(const TAppInfo& aAppInfo, TQuestionDialog aQuestion,
					 const TDesC& aDes);

	const TAppInfo& iAppInfo;
	TQuestionDialog iQuestion;
	const TDesC& iDes;
	
	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the option selection
 * dialog, to the UI Support Server.
 */
class CDisplayOptions : public CUissCmd
	{
public:
	IMPORT_C static CDisplayOptions* NewL(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aOptions,
					      RArray<TBool>& aSelections);
	IMPORT_C static CDisplayOptions* NewLC(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aOptions,
					      RArray<TBool>& aSelections);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
	void UnmarshallArgumentsL();	

	~CDisplayOptions();
private:
	CDisplayOptions(const TAppInfo& aAppInfo,
					const RPointerArray<TDesC>& aOptions,
					RArray<TBool>& aSelections);

	const TAppInfo& iAppInfo;
	const RPointerArray<TDesC>& iOptions;
	RArray<TBool>& iSelections;
	
	// vars needed to marshall user selections 
	HBufC8* iSelectionsBuffer;
	TPtr8*  iSelectionsBufferPtr;
					      	
	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the OCSP result dialog to the UI 
 * Support Server.
 */
class CDisplayOcspResult : public CUissCmd
	{
public:
	/**
	 * Constructs a new command object to display OCSP result dialog
	 *
	 * @param aAppInfo      [in] Application info from SIS file
	 * @param aMessage      [in] Dialog message code (see
	 *                      TRevocationDialogMessage enum)
	 * @param aOutcomes     [in] OCSP outcomes
	 * @param aCertificates [in] End certificates
	 * @param aWarningOnly  [in] Can the user override the warning and continue?
	 *
	 * @return A new command object
	 */
	IMPORT_C static CDisplayOcspResult* NewL(const TAppInfo& aAppInfo,
						 TRevocationDialogMessage aMessage, 
						 RPointerArray<TOCSPOutcome>& aOutcomes,
						 RPointerArray<CCertificateInfo>& aCertificates,
						 TBool aWarningOnly);
	
	/**
	 * Constructs a new command object to display OCSP result dialog and puts 
	 * it on the cleanup stack
	 *
	 * @param aAppInfo      [in] Application info from SIS file
	 * @param aMessage      [in] Dialog message code (see
	 *                      TRevocationDialogMessage enum)
	 * @param aOutcomes     [in] OCSP outcomes
	 * @param aCertificates [in] End certificates
	 * @param aWarningOnly  [in] Can the user override the warning and continue?
	 *
	 * @return A new command object on the cleanup stack
	 */
	IMPORT_C static CDisplayOcspResult* NewLC(const TAppInfo& aAppInfo,
						 TRevocationDialogMessage aMessage, 
						 RPointerArray<TOCSPOutcome>& aOutcomes,
						 RPointerArray<CCertificateInfo>& aCertificates,
						 TBool aWarningOnly);

	/**
	 * Provides result of the dialog
	 *
	 * @return The result code of the dialog
	 */
	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayOcspResult(const TAppInfo& aAppInfo,
						 TRevocationDialogMessage aMessage, 
						 RPointerArray<TOCSPOutcome>& aOutcomes,
						 RPointerArray<CCertificateInfo>& aCertificates,
						 TBool aWarningOnly);

	const TAppInfo&                  iAppInfo;
	TRevocationDialogMessage         iMessage;
	RPointerArray<TOCSPOutcome>&     iOutcomes;
	RPointerArray<CCertificateInfo>& iCertificates;
	TBool                            iWarningOnly;					      	
	
	// return result
	TBool        iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the uninstall
 * dialog, to the UI Support Server.
 */
class CDisplayUninstall : public CUissCmd
	{
public:
	IMPORT_C static CDisplayUninstall* NewL(const TAppInfo& aAppInfo);
	IMPORT_C static CDisplayUninstall* NewLC(const TAppInfo& aAppInfo);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayUninstall(const TAppInfo& aAppInfo);

	const TAppInfo& iAppInfo;
	
	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the text
 * dialog, to the UI Support Server.
 */
class CDisplayText : public CUissCmd
	{
public:
	IMPORT_C static CDisplayText* NewL(const TAppInfo& aAppInfo, 
				    TFileTextOption aOption,
				    const TDesC8& aText);
	IMPORT_C static CDisplayText* NewLC(const TAppInfo& aAppInfo, 
				    TFileTextOption aOption,
				    const TDesC8& aText);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayText(const TAppInfo& aAppInfo, 
				    TFileTextOption aOption,
				    const TDesC8& aText);

	const TAppInfo& iAppInfo;
	TFileTextOption iOption;
	const TDesC8& iText;
	
	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the text
 * dialog, to the UI Support Server.
 */
class CDisplayDependencyBreak : public CUissCmd
	{
public:
	IMPORT_C static CDisplayDependencyBreak* NewL(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents);
	IMPORT_C static CDisplayDependencyBreak* NewLC(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayDependencyBreak(const TAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents);

	const TAppInfo& iAppInfo;
	const RPointerArray<TDesC>& iComponents;

	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};

/** 
 * Class used to pass a command to display the text
 * dialog, to the UI Support Server.
 */
class CDisplayApplicationsInUse : public CUissCmd
	{
public:
	IMPORT_C static CDisplayApplicationsInUse* NewL(const TAppInfo& aAppInfo,
							  const RPointerArray<TDesC>& aAppNames);
	IMPORT_C static CDisplayApplicationsInUse* NewLC(const TAppInfo& aAppInfo,
							  const RPointerArray<TDesC>& aAppNames);

	IMPORT_C TBool ReturnResult() const;

	void MarshallArgumentsL(TIpcArgs& aArguments);
private:
	CDisplayApplicationsInUse(const TAppInfo& aAppInfo,
							  const RPointerArray<TDesC>& aAppNames);

	const TAppInfo& iAppInfo;
	const RPointerArray<TDesC>& iAppNames;

	// return result
	TBool iResult;
	TPckg<TBool> iResultPackage;
	};


// inline functions from TAppInfo

inline const TDesC& TAppInfo::AppName() const
	{
	return iAppName ? *iAppName : KNullDesC;
	}

inline const TDesC& TAppInfo::AppVendor() const
	{
	return iAppVendor ? *iAppVendor : KNullDesC;
	}

inline const TVersion& TAppInfo::AppVersion() const
	{
	return iAppVersion;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	NONSHARABLE_CLASS(CProgressBarValuePublisher) : public CBase
	/**
	 * This class is used to publish the percentage value of the installation progress bar.
	 * The progress bar value is published by using a publish&subscribe property. @see Swi::KUidSwiProgressBarValueKey.
	 */
		{
	public:
		IMPORT_C static CProgressBarValuePublisher* NewL();
		~CProgressBarValuePublisher();
		
		/** Sets the final value of the installation progress bar. */
		IMPORT_C void SetFinalProgressBarValue(TInt aValue);
		
		/** Updates the value of the progress bar value property by adding the given value to the current value. */
		IMPORT_C TInt UpdateProgressBarValueL(TInt aValue);
	
	private:
		CProgressBarValuePublisher();
		void ConstructL();
		
	private:
		TInt iCurrentProgressValue;
		TInt iFinalProgressValue;
		TInt iLastPercentCompletion;
		TInt iLastProgressValue;
		};
#endif

} // namespace Swi

#endif // #ifndef __SISUIHANDLER_H__
