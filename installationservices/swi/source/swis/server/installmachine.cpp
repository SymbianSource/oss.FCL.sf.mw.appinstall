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


#define __INCLUDE_CAPABILITY_NAMES__ 
#include <s32mem.h>
#include "installmachine.h"
#include "plan.h"
#include "dessisdataprovider.h"
#include "siscontroller.h"
#include "siscertificatechain.h"
#include "sisinstallerrors.h"
#include "siscontentprovider.h"
#include "sisinstallblock.h"
#include "prerequisiteschecker.h"
#include "postrequisiteschecker.h"
#include "installationplanner.h"
#include "log.h"
#include "cleanuputils.h"
#include "sisfiledescription.h"
#include "sisinfo.h"
#include "sisuid.h"
#include "siscapabilities.h"
#include "securitypolicy.h"
#include "swispubsubdefs.h"
#include "installationprocessor.h"
#include "swi/launcher.h"
#include "certchainconstraints.h"
#include "swi/sistruststatus.h"
#include <pkixcertchain.h>
#include <swicertstore.h>
#include <sacls.h>
#include <f32file.h>
#include "securitycheckutil.h"
#include "sisptrprovider.h"
#include <e32capability.h>
#include <ocsp.h>
#include "secutils.h"
#include "sislauncherclient.h"
#include "swicenrep.h"
// Security settings.
#include <x509certext.h>
#include <pkixvalidationresult.h>
#include <secsettings/secsettingsclient.h>
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swi/sisversion.h"
#include "swi/nativecomponentinfo.h"
#include <usif/usifcommon.h>
#include <usif/scr/appregentries.h>
#include "scrdbconstants.h"
#endif

using namespace Swi;
using namespace Swi::Sis;

_LIT(KExpressSignedOID, "1.2.826.0.1.1796587.1.1.2.1"); 
_LIT(KCertifiedSignedOID, "1.2.826.0.1.1796587.1.1.2.2"); 
_LIT(KCertifiedSignedWithVerisignOID, "1.2.826.0.1.1796587.1.1.2.3"); 


#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

void DeRegisterForceRegisteredAppsL()
	{
	// Deregister the force registered applications from AppArc
	DEBUG_PRINTF(_L8("Deregistering the force registered applications with AppArc"));
	RSisLauncherSession launcher;
	CleanupClosePushL(launcher);
	User::LeaveIfError(launcher.Connect());
	RArray<TAppUpdateInfo> emptyAppRegDataArray;
	launcher.NotifyNewAppsL(emptyAppRegDataArray);
	CleanupStack::PopAndDestroy(&launcher);
	}

#endif

//
// TInstallState
//

CInstallMachine::TInstallState::TInstallState(CInstallMachine& aInstallMachine)
:	iInstallMachine(aInstallMachine)
	{
	}

//
// TRegistrationState
//

CInstallMachine::TRegistrationState::TRegistrationState(
	CInstallMachine& aInstallMachine)
/**
	Constructor.
 */
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TRegistrationState::EnterL()
/**
	Send registration request to the SWI Observer and then activate the
	installation machine.
 */
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Registration State"));
	//connect to the SWI Observer
	User::LeaveIfError(iInstallMachine.Observer().Connect());
	
	//Register to the SWI Observer; which completes this request 
	//when the SWI Observer Processor is idle.
	iInstallMachine.Observer().Register(iInstallMachine.iStatus);
	iInstallMachine.SetActive();
	}

CInstallMachine::TState* CInstallMachine::TRegistrationState::CompleteL()
/**
	Obtains the log file handle and its name from the SWI Observer.
	Adds the log file to the transaction.
 */
	{
	DEBUG_PRINTF(_L8("Install Machine - Registration State complete"));
	RBuf logFileName;
	logFileName.CreateL(KMaxFileName);
	logFileName.CleanupClosePushL();
	
	//Get created a log file and obtains its full name.
	iInstallMachine.Observer().GetFileHandleL(logFileName);

	//Add the log file the transaction
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	iInstallMachine.TransactionSession().RegisterNewL(logFileName);
#else
	iInstallMachine.IntegrityServicesL().AddL(logFileName);
#endif

	CleanupStack::PopAndDestroy(&logFileName);
	// Get Controllers.
	return static_cast<TState*>(&iInstallMachine.iGetControllerState);		
	}
	
CInstallMachine::TState* CInstallMachine::TRegistrationState::ErrorL(TInt aCode)
/**
	If there is any error, closes the SWI Observer connection.
 */
	{
	DEBUG_PRINTF2(_L8("Install Machine - Registration State failed with code '%d'"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TRegistrationState::Cancel()
/**
	Cancels the registration request.
 */
	{
	DEBUG_PRINTF(_L8("Install Machine - Registration State cancelled!"));
	iInstallMachine.Observer().CancelRegistration();	
	}


//
// TGetControllerState
//

CInstallMachine::TGetControllerState::TGetControllerState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TGetControllerState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Get Controller state"));

	if (iInstallMachine.iSecurityManager->SecurityPolicy().DrmEnabled())
		{
		iInstallMachine.iSisHelper.OpenDrmContentL(static_cast<ContentAccess::TIntent>(iInstallMachine.iSecurityManager->SecurityPolicy().DrmIntent()));
		}

	iInstallMachine.iControllerData=
		iInstallMachine.iSisHelper.SisControllerLC();
	CleanupStack::Pop();
	DEBUG_PRINTF2(_L8("Retrieved controller size %d"), iInstallMachine.iControllerData->Size());
	
	TPtrProvider provider(iInstallMachine.iControllerData->Des());

	// Create controller
	iInstallMachine.iController=Sis::CController::NewL(provider);

	if(!iInstallMachine.iSecurityManager->SecurityPolicy().AllowPackagePropagate() 
		 && iInstallMachine.iSisHelper.IsStubL())
		{
		// If it's a stub and our SWIPolicy AllowPackagePropagate
		// flag is false we can only install if it's a 
		// preinstalled stub not a removable media stub
		if(iInstallMachine.iController->Info().InstallType() != EInstPreInstalledApp
		 && iInstallMachine.iController->Info().InstallType() != EInstPreInstalledPatch)
			{
			// It is a removable media stub but our SWI policy forbids
			// installation
			User::Leave(KErrSecurityError);
			}
		
		}
	
	// Create content provider which will be used everywhere for things like 
	// TAppInfo etc.
	iInstallMachine.iContentProvider=
		CContentProvider::NewL(*iInstallMachine.iController);
	
	iInstallMachine.iCurrentContentProvider = iInstallMachine.iContentProvider;

	// Create stuff for the installation planner to use later
	iInstallMachine.iResult=CInstallationResult::NewL();
	iInstallMachine.iPlanner=CInstallationPlanner::NewL(iInstallMachine.iSisHelper,iInstallMachine.UiHandler(), 
		*iInstallMachine.iCurrentContentProvider, *iInstallMachine.iResult);
	
	iInstallMachine.iPlanner->SetDeviceSupportedLanguages(iInstallMachine.iDeviceSupportedLanguages);

	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Check if the machine runs in info collection mode. If so, set the planner too in the same mode.
	if (iInstallMachine.IsInInfoMode())
		{
		iInstallMachine.iPlanner->SetInInfoCollectionMode(ETrue);
		}
	#endif
	
	iInstallMachine.CompleteSelf();
	iInstallMachine.SetActive();
	}

CInstallMachine::TState* CInstallMachine::TGetControllerState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Get Controller State complete"));
	// Confirm installation with the user.
	return static_cast<TState*>(&iInstallMachine.iConfirmationState);
	}
	
CInstallMachine::TState* CInstallMachine::TGetControllerState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Get Controller State failed with code '%d'"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TGetControllerState::Cancel()
	{
	}

//
// TConfirmationState
//

CInstallMachine::TConfirmationState::TConfirmationState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TConfirmationState::EnterL()
	{	
	DEBUG_PRINTF(_L8("Install Machine - Entering Confirmation State"));
	
	TBool retval;
	
	TAppInfo appInfo(
		iInstallMachine.iCurrentContentProvider->DefaultLanguageAppInfoL());
	
	RPointerArray<CX509Certificate> x509certs;
	CleanupResetAndDestroyPushL(x509certs);
	iInstallMachine.iSecurityManager->GetCertificatesFromControllerL(
		*iInstallMachine.iController,x509certs);
	
	RPointerArray<CCertificateInfo> certs;
	CleanupResetAndDestroyPushL(certs);
	CSecurityManager::FillCertInfoArrayL(x509certs,certs);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	if (iInstallMachine.IsInInfoMode())
		{
		// If we operate in info mode, we just need the certs, no UI confirmation or logo will be displayed
		CleanupStack::PopAndDestroy(2, &x509certs); // certs, x509certs
		
		iInstallMachine.CompleteSelf();
		iInstallMachine.SetActive();

		iInstallMachine.iOperationConfirmed = ETrue;				
		return;
		}
	#endif
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
		
	// Retrieve logo data which may not be present.
	const CLogo* logo=iInstallMachine.iController->Logo();
	if (logo)
		{
		const CFileDescription& fdesc(logo->FileDescription());
		
		// Create a temporary file for the logo.
		
		// Construct temporary file name for the logo.
		TUint driveCh(RFs::GetSystemDriveChar());

		TFileName logoFileName;
		_LIT(KLogoFileNameFmt, "%c:\\sys\\install\\temp\\%08X-logo");
		logoFileName.Format(KLogoFileNameFmt, driveCh,
			iInstallMachine.iController->Info().Uid().Uid().iUid);
		
		TInt err = fs.MkDirAll(logoFileName);
		if (err!= KErrNone && err != KErrAlreadyExists)
			User::LeaveIfError(err);
		
		RFile logoFile;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iInstallMachine.TransactionSession().RegisterTemporaryL(logoFileName);
#else
		iInstallMachine.IntegrityServicesL().TemporaryL(logoFileName);
#endif

		User::LeaveIfError(logoFile.Replace(fs, logoFileName, 
			EFileStream|EFileWrite|EFileRead|EFileShareExclusive));
		CleanupClosePushL(logoFile);
		
		// Extract logo to a temporary file.
		err=iInstallMachine.iSisHelper.ExtractFileL(fs, logoFile, 
			fdesc.Index(), iInstallMachine.iController->DataIndex(), 
			iInstallMachine.UiHandler());
		User::LeaveIfError(err);
		CleanupStack::PopAndDestroy(&logoFile);
		
		// Reopen file for reading (the handle will have been closed 
		// by SISHelper).
		User::LeaveIfError(logoFile.Open(fs, logoFileName, 
			EFileStream|EFileRead|EFileShareExclusive));
		CleanupClosePushL(logoFile);
		CDisplayInstall* cmd=CDisplayInstall::NewLC(appInfo, fs, &logoFile, 
			certs);
		iInstallMachine.UiHandler().ExecuteL(*cmd);
		retval=cmd->ReturnResult();
		CleanupStack::PopAndDestroy(2, &logoFile); // logoFile, cmd

		RLoader loader;
		User::LeaveIfError(loader.Connect());
		CleanupClosePushL(loader);
		
		// Delete temporary logo file.
		User::LeaveIfError(loader.Delete(logoFileName));
		
		CleanupStack::PopAndDestroy(&loader);
		}
	else
		{
		CDisplayInstall* cmd=CDisplayInstall::NewLC(appInfo, fs, NULL, certs);
		iInstallMachine.UiHandler().ExecuteL(*cmd);
		retval=cmd->ReturnResult();
		CleanupStack::PopAndDestroy(cmd);
		}
	
	CleanupStack::PopAndDestroy(3, &x509certs); // fs, certs, x509certs
	
	// Check if the user cancelled installation.
	if (!retval)
		{
		DEBUG_PRINTF(_L8("User canceled install at install dialog"));
		User::Leave(KErrCancel);
		}
	iInstallMachine.CompleteSelf();
	iInstallMachine.SetActive();
	
	// user hasn't cancelled so mark the installation as confirmed. This
	// will allow the registry cache to be regenerated further on during the
	// installation process.
	iInstallMachine.iOperationConfirmed = ETrue;	
	}

CInstallMachine::TState* CInstallMachine::TConfirmationState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Confirmation State complete"));
	// Verify signature(s) if any.
	return static_cast<TState*>(&iInstallMachine.iVerifyControllerState);
	}
	
CInstallMachine::TState* CInstallMachine::TConfirmationState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Confirmation State failed with code '%d'"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TConfirmationState::Cancel()
	{
	}

//
// TVerifyControllerState
//

CInstallMachine::TVerifyControllerState::TVerifyControllerState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

// Verify signatures and cert chains using security manager
void CInstallMachine::TVerifyControllerState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Verify Controller State"));

	// Clear any results from previous controllers
 	iInstallMachine.iSigValidationResult=EValidationSucceeded;
 	iInstallMachine.iPkixResults.ResetAndDestroy();
 	iInstallMachine.iCertificates.ResetAndDestroy();
 	iInstallMachine.iGrantableCapabilitySet.SetEmpty();
	iInstallMachine.iSecurityManager->ResetValidCertChains();
	iInstallMachine.iOcspOutcomes.ResetAndDestroy();
	iInstallMachine.iCertInfos.ResetAndDestroy();
	
	TInt64 dataOffset = iInstallMachine.CurrentController().DataOffset()-iInstallMachine.iController->DataOffset();
	
	// Get current controller data.
	TPtrC8 data(iInstallMachine.iControllerData->Mid(dataOffset));
						
	// Verify certificate chains and prepare for OCSP.
	iInstallMachine.iSecurityManager->VerifyControllerL(
		data,
		iInstallMachine.CurrentController(), 
		&iInstallMachine.iSigValidationResult, 
		iInstallMachine.iPkixResults,
		iInstallMachine.iCertificates,
		&iInstallMachine.iGrantableCapabilitySet,
		iInstallMachine.iAllowUnsigned,
		iInstallMachine.iEmbedded,
		iInstallMachine.iStatus);

	iInstallMachine.SetActive();
	}

// Handle signature and certificate validation errors
CInstallMachine::TState* CInstallMachine::TVerifyControllerState::CompleteL()
	{	
	DEBUG_PRINTF(_L8("Install Machine - Verify Controller State Complete"));
		
	// Populate the cert info array in preparation for any
	// dialogs we have to launch
	
	CSecurityManager::FillCertInfoArrayL(iInstallMachine.iCertificates,
			iInstallMachine.iCertInfos);
	
	DEBUG_PRINTF2(_L8("Signature Validation Result is set to code %d"), iInstallMachine.iSigValidationResult);
	
	// check what was the result of validation
	switch (iInstallMachine.iSigValidationResult)
		{
		default:
			// BC break, unknown validation code, abort
			User::Leave(KErrNotSupported);
			break;
		
		case EValidationSucceeded:
			// Chain was validated
			// Increase the trust status of this install process
			iInstallMachine.SetTrust(ESisPackageCertificateChainValidatedToTrustAnchor);
			iInstallMachine.SetValidationStatus(EValidatedToAnchor);
			
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Alert only when machine not runs in info collection mode
			if(iInstallMachine.IsInInfoMode())
				break;
			#endif
			// Continue normally, display security warning dialog so that the 
			// UI gets certificate information.
			// If the package is embedded, then skip notification to hide the embedding details from the user
			if (!iInstallMachine.iEmbedded && !SecurityAlertL(ETrue))
				User::Leave(KErrCancel); // User or UI cancelled installation.
			break;
				
		case ESignatureNotPresent:
			// This is a special case because we need to look at the policy 
			// setting to determine if unsigned SIS files are allowed at all.
			// Display security warning dialog.
			{
			
			iInstallMachine.SetTrust(ESisPackageUnsignedOrSelfSigned);
			iInstallMachine.SetValidationStatus(EUnsigned);   
			
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Alert only when machine not runs in info collection mode
			if(iInstallMachine.IsInInfoMode())
				break;
			#endif
			
			TBool cont = SecurityAlertL(iInstallMachine.iAllowUnsigned);
			if (!cont || !iInstallMachine.iAllowUnsigned)
				User::Leave(KErrSecurityError);

			break;	
			}
		
		case ESignatureSelfSigned:	
		    {
			iInstallMachine.SetTrust(ESisPackageCertificateChainNoTrustAnchor);
		    iInstallMachine.SetValidationStatus(EValidated);
		    
		    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		    // Alert only when machine not runs in info collection mode
			if(iInstallMachine.IsInInfoMode())
				break;
			#endif
			
            TBool allowSelfSigned(ETrue);
	          
	        // Session to access Install Central Repository Server.
	        SecuritySettingsServer::RSecSettingsSession secSettingsSession;

	        // Connect to the Central Repository server.
	        User::LeaveIfError(secSettingsSession.Connect());

            CleanupClosePushL(secSettingsSession);
	             
            // Read-in the values of the settings - KAllowSelfSignedInstallKey. 
            // These will retain the default values if any error occurs.
	        TRAPD(err, (allowSelfSigned = secSettingsSession.SettingValueL(KUidInstallationRepository , KAllowSelfSignedInstallKey)));
            if (err == KErrNone || err == KErrSettingNotFound || err == KErrNotFound || err == KErrCorrupt)
                {
                if (err == KErrCorrupt)
                    {
                    DEBUG_PRINTF(_L8("Install Machine - CenRep file 2002cff6.txt is corrupt. Using Default Value to Install."));
                    }
                
                if (!allowSelfSigned || !SecurityAlertL(ETrue))
                    {
                    User::Leave(KErrCancel);
                    }
                }
            else
                {
                User::Leave(err);
                }
            CleanupStack::PopAndDestroy(&secSettingsSession);    	         			
			break;
		    }		   
        case ECertificateValidationError:
		case ENoCertificate:
		case ENoCodeSigningExtension:
		case ENoSupportedPolicyExtension:
			{
			// Unable to validate the chain
			// We apply the same policy as per unsigned SIS files
			iInstallMachine.SetTrust(ESisPackageValidationFailed);	
			iInstallMachine.SetValidationStatus(EInvalid);
			
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Alert only when machine not runs in info collection mode
			if(iInstallMachine.IsInInfoMode())
				break;
			#endif

			// uh-oh, not good, ask the user because these are not fatal
			TBool cont = SecurityAlertL(iInstallMachine.iAllowUnsigned);
			if (!cont || !iInstallMachine.iAllowUnsigned)
				User::Leave(KErrSecurityError);

			break;
			}
			
		case ESignatureCouldNotBeValidated:
		case EMandatorySignatureMissing:
            iInstallMachine.SetValidationStatus(EInvalid);
            // we're in trouble, because these are fatal errors
			
			#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
			// Alert only when machine not runs in info collection mode
			if(iInstallMachine.IsInInfoMode())
				break;
			#endif	

			// coverity[unchecked_value]
			SecurityAlertL(EFalse); // user cannot override the error
			User::Leave(KErrSecurityError);

			break;
		}
		
	// All subsequent controllers will be embedded controllers	
	iInstallMachine.iEmbedded = ETrue;		
	
				
	// Devcert warning	
	if (iInstallMachine.iSecurityManager->GetDevCertWarningState()==EFoundDevCerts
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		&& !iInstallMachine.IsInInfoMode()
	#endif
		)
		{
		// Warn only when machine runs in normal mode.
		TAppInfo appInfo(
			iInstallMachine.iContentProvider->DefaultLanguageAppInfoL());
			
		CHandleInstallEvent* cmd = CHandleInstallEvent::NewLC(appInfo, EEventDevCert, 0, KNullDesC);
		iInstallMachine.UiHandler().ExecuteL(*cmd);
		if (!cmd->ReturnResult())
			{
			User::Leave(KErrSecurityError);	
			}
		CleanupStack::PopAndDestroy(cmd);	

		//Only warn once
		iInstallMachine.iSecurityManager->SetDevCertWarningState(EDevCertsWarned);	
		}		
		
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Forcibly skip the OCSP & prerequisites checking and directly go to the plan installation state when the 
	// machine runs in component information collection mode. OCSP would introduce latency which is not expected 
	// when retrieving component info.
	if(iInstallMachine.IsInInfoMode())
		{	
		return static_cast<TState*>(&iInstallMachine.iPlanInstallationState);
		}
	#endif

	return static_cast<TState*>(&iInstallMachine.iOcspState);
	}
												
CInstallMachine::TState* CInstallMachine::TVerifyControllerState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Verify Controller State failed with code '%d'"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TVerifyControllerState::Cancel()
	{
	}

TBool CInstallMachine::TVerifyControllerState::SecurityAlertL(
	TBool aCanOverride)
	{
	TAppInfo appInfo(
		iInstallMachine.iCurrentContentProvider->DefaultLanguageAppInfoL());
		
	CDisplaySecurityWarning* cmd=CDisplaySecurityWarning::NewLC(appInfo,
		iInstallMachine.iSigValidationResult, iInstallMachine.iPkixResults, 
		iInstallMachine.iCertInfos, aCanOverride);
		
	iInstallMachine.UiHandler().ExecuteL(*cmd);
	TBool retval=cmd->ReturnResult();
	CleanupStack::PopAndDestroy(cmd);
	return retval;
	}

//
// TOcspState
//

CInstallMachine::TOcspState::TOcspState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
,	iNeedOcsp(ETrue)
	{
	}

// Verify signatures and cert chains using security manager
void CInstallMachine::TOcspState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering OCSP State"));
	
	// Determine is the check is necessary.
	iNeedOcsp = iInstallMachine.iInstallPrefs->PerformRevocationCheck() && iInstallMachine.iSecurityManager->SecurityPolicy().OcspEnabled();
	
	if ((iInstallMachine.iCertificates.Count()) && iNeedOcsp)
		{
		TInt checkOCSPForExpressSignedPkg = 1;
		TInt checkOCSPForSelfSignedPkg = 1;
		TInt checkOCSPForCertifiedSignedPkg = 1;
		TInt checkOCSPForCertifiedWithVeriSignPkg = 1;

		// Session to access Security Central Repository Server.
		SecuritySettingsServer::RSecSettingsSession secSettingsSession;

		// Connect to the Security Central Repository server.
		User::LeaveIfError(secSettingsSession.Connect());

		CleanupClosePushL(secSettingsSession);
    
		// Read-in the values of the settings from the Install Central Repository. 
		// These will retain the default values if any error occurs.
		TRAPD(err, checkOCSPForExpressSignedPkg = secSettingsSession.SettingValueL(KUidInstallationRepository, KCheckOCSPForExpressedSignedPkgKey));
		TRAP(err, checkOCSPForSelfSignedPkg = secSettingsSession.SettingValueL(KUidInstallationRepository, KCheckOCSPForSelfSignedPkgKey));
		TRAP(err, checkOCSPForCertifiedSignedPkg = secSettingsSession.SettingValueL(KUidInstallationRepository, KCheckOCSPForCertifiedSignedPkgKey));
		TRAP(err, checkOCSPForCertifiedWithVeriSignPkg = secSettingsSession.SettingValueL(KUidInstallationRepository, KCheckOCSPForCertifiedWithVeriSignPkgKey));
		
		CleanupStack::PopAndDestroy(&secSettingsSession);

		TBool makeOcspCheck = (checkOCSPForExpressSignedPkg == 1) && (checkOCSPForSelfSignedPkg == 1) && (checkOCSPForCertifiedSignedPkg == 1) && (checkOCSPForCertifiedWithVeriSignPkg == 1);
	
		//Find the OID of the certificate and make ocsp check based on settings retreived from cenrep
		for (TInt i=0; i<iInstallMachine.iCertificates.Count(); ++i)
		    {
		    if(makeOcspCheck)//Could be true if set in the previous iCertificates.
		        {
		        break;
		        }
		    CX509Certificate* cert = iInstallMachine.iCertificates[i];
		    const CX509CertExtension* certExt =  cert->Extension(KCertPolicies);
			if (certExt == NULL)
				{
				makeOcspCheck = ETrue;
				continue;
				}
		    CX509CertPoliciesExt* policyExt = CX509CertPoliciesExt::NewLC(certExt->Data());
		    const CArrayPtrFlat<CX509CertPolicyInfo>& policies = policyExt->Policies();
		    if(policies.Count() == 0)
				{
		        makeOcspCheck = ETrue;
				}
			for(TInt j=0; j<policies.Count(); ++j)
				{
				HBufC* oid = (policies[j])->Id().AllocLC();
				if (oid->Compare(KExpressSignedOID) == 0)
					{
					makeOcspCheck = (checkOCSPForExpressSignedPkg==1);   
					}
				else if(oid->Compare(KCertifiedSignedOID) == 0)
					{
					makeOcspCheck = (checkOCSPForCertifiedSignedPkg==1); 
					}
				else if(oid->Compare(KCertifiedSignedWithVerisignOID) == 0)
					{
					makeOcspCheck = (checkOCSPForCertifiedWithVeriSignPkg==1);   
					}
				else if(iInstallMachine.iSigValidationResult == ESignatureSelfSigned)
					{
					makeOcspCheck = (checkOCSPForSelfSignedPkg==1);
					}
				CleanupStack::PopAndDestroy(oid);
				if(makeOcspCheck)
					{
					break;
					}                
				}
		    CleanupStack::PopAndDestroy(policyExt);       
			}
		
		if (makeOcspCheck)
		    {
            // We haven't done the planning phase so we need to use the default
            TAppInfo appInfo(iInstallMachine.iCurrentContentProvider->DefaultLanguageAppInfoL());
    
            // Signal OCSP check starting
            CHandleCancellableInstallEvent* cmd = CHandleCancellableInstallEvent::NewLC(appInfo, EEventOcspCheckStart, 0, KNullDesC);
            iInstallMachine.UiHandler().ExecuteL(*cmd);
            CleanupStack::PopAndDestroy(cmd);
            
            // Start OCSP check.
            TBuf8<256> ocspUri(iInstallMachine.iInstallPrefs->RevocationServerUri());
            iInstallMachine.iSecurityManager->PerformOcspL(ocspUri, iInstallMachine.iIap,
                &iInstallMachine.iOcspMsg,iInstallMachine.iOcspOutcomes,
                iInstallMachine.iCertificates,iInstallMachine.iStatus);
    
            TTime time;
            time.UniversalTime();
            TSisTrustStatus& trustStatus =  iInstallMachine.iController->TrustStatus();
            trustStatus.SetLastCheckDate(time);
		    }
        else
            {
            iNeedOcsp = EFalse;
            iInstallMachine.CompleteSelf();
            }
		}
	else
		{
		iNeedOcsp = EFalse;
		iInstallMachine.CompleteSelf();
		}
	
	iInstallMachine.SetActive();
	}

// Handle signature and certificate validation errors
CInstallMachine::TState* CInstallMachine::TOcspState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Completed OCSP State"));
	DEBUG_PRINTF2(_L8("OCSP Overall Message: %d"), iInstallMachine.iOcspMsg); 

	TBool ocspError = EFalse;
	for (TInt i = iInstallMachine.iOcspOutcomes.Count() - 1; i >= 0; i--)
		{
		DEBUG_PRINTF3(_L8("OCSP Result for chain %d - %d"), i, iInstallMachine.iOcspOutcomes[i]->iResult);
		
		if (iInstallMachine.iOcspOutcomes[i]->iResult != OCSP::EGood)
			{
			ocspError = ETrue;
			break;
			}
		}
    
    TSisTrustStatus& trustStatus =  iInstallMachine.iController->TrustStatus();
	
	if (iNeedOcsp && !ocspError)
	    {
	    if (iInstallMachine.Trust() == ESisPackageCertificateChainValidatedToTrustAnchor)
	    	{
	    	iInstallMachine.SetTrust(ESisPackageChainValidatedToTrustAnchorAndOCSPValid);
			}
		iInstallMachine.SetRevocationStatus(EOcspGood);
		trustStatus.SetResultDate(trustStatus.LastCheckDate());        
		}
	else if (iNeedOcsp && ocspError)
		{

 		// Regenerate cert info array since cert list may have been pruned before
 		// OCSP check.
 		iInstallMachine.iCertInfos.ResetAndDestroy();
 		CSecurityManager::FillCertInfoArrayL(iInstallMachine.iCertificates,
 											iInstallMachine.iCertInfos);
 

 		// If OcspMandatory() is ETrue, then all OCSP errors are fatal.
 		TBool fatalError=iInstallMachine.iSecurityManager->SecurityPolicy().OcspMandatory() ? ETrue : EFalse;
		switch (iInstallMachine.iOcspMsg)
			{
			default:
				// Unknown value, must be a BC break!
				User::Leave(KErrNotSupported);
				break;
				
			case EResponseSignatureValidationFailure:
			case EInvalidRevocationServerResponse:
			case EInvalidCertificateStatusInformation:
                iInstallMachine.SetRevocationStatus(EOcspTransient);
				break;
				
			case EInvalidRevocationServerUrl:
			case EUnableToObtainCertificateStatus:
				// Possibly transient error, may retry.
				if (iInstallMachine.Trust() == ESisPackageCertificateChainValidatedToTrustAnchor)
	    			{
					iInstallMachine.SetTrust(ESisPackageChainValidatedToTrustAnchorOCSPTransientError);
					}
				iInstallMachine.SetRevocationStatus(EOcspTransient);
				break;
			
			case ECertificateStatusIsUnknown:
				// Non-fatal permanent error, ask the user.
				if (iInstallMachine.iSigValidationResult == ESignatureSelfSigned)
					{
					iInstallMachine.iOcspMsg = ECertificateStatusIsUnknownSelfSigned;
					}
				iInstallMachine.SetRevocationStatus(EOcspUnknown);
				trustStatus.SetResultDate(trustStatus.LastCheckDate());        
				break;
				
			case ECertificateStatusIsRevoked:
				// Fatal security error, don't allow installation to proceed.
				fatalError=ETrue;

				// Install attempted with revoked certificate, reset trust
				iInstallMachine.SetTrust(ESisPackageValidationFailed);
				iInstallMachine.SetRevocationStatus(EOcspRevoked);
				trustStatus.SetResultDate(trustStatus.LastCheckDate());        
				break;
			}

		// We haven't done the planning phase so we need to use the default
		TAppInfo appInfo(iInstallMachine.iCurrentContentProvider->DefaultLanguageAppInfoL());
		
	
		// Signal OCSP check complete
		CHandleInstallEvent* cmd = CHandleInstallEvent::NewLC(appInfo, EEventOcspCheckEnd, 0, KNullDesC);
		iInstallMachine.UiHandler().ExecuteL(*cmd);
		

		if (!cmd->ReturnResult())
			{
			User::Leave(KErrCancel);
			}
		CleanupStack::PopAndDestroy(cmd);
		
		// Display OCSP result dialog.
		CDisplayOcspResult* dialogCmd=CDisplayOcspResult::NewLC(appInfo,
			iInstallMachine.iOcspMsg,iInstallMachine.iOcspOutcomes,
			iInstallMachine.iCertInfos,fatalError ? EFalse : ETrue);
		iInstallMachine.UiHandler().ExecuteL(*dialogCmd);
		
		if (fatalError || dialogCmd->ReturnResult()==EFalse)
			{
			User::Leave(KErrCancel);
			}
		
		CleanupStack::PopAndDestroy(dialogCmd);
		} // else if (iNeedOcsp && ocspError)
	else 
	    {
	    iInstallMachine.SetRevocationStatus(EOcspNotPerformed);
	    }	
	// finished verifying the controller
	return static_cast<TState*>(&iInstallMachine.iCheckPrerequisitesState);
	}
												
CInstallMachine::TState* CInstallMachine::TOcspState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - OCSP State failed with code %d"), aCode);
	
	
	//If the error code is -7603 (KErrNoCertificates), handle it by popping up a warning and asking the user whether 
	//he or she wants to continue. For any other error code leave the function.	
	if ((OCSP::KErrNoCertificates == aCode) && !iInstallMachine.iSecurityManager->SecurityPolicy().OcspMandatory())
		{			
		TAppInfo appInfo(iInstallMachine.iCurrentContentProvider->DefaultLanguageAppInfoL());
		CDisplayOcspResult* cmd=CDisplayOcspResult::NewLC(appInfo,
				iInstallMachine.iOcspMsg, iInstallMachine.iOcspOutcomes, 
				iInstallMachine.iCertInfos, ETrue);
			
		iInstallMachine.UiHandler().ExecuteL(*cmd);
		TBool retval=cmd->ReturnResult();
		CleanupStack::PopAndDestroy(cmd);
		if (retval)
			{
			iInstallMachine.SetRevocationStatus(EOcspNotPerformed);	
			return static_cast<TState*>(&iInstallMachine.iCheckPrerequisitesState);
			}
		else
			{
			User::Leave(aCode);
			return NULL;		
			}
		}
	else
		{
		User::Leave(aCode);
		return NULL;
		}
	}

void CInstallMachine::TOcspState::Cancel()
	{
	DEBUG_PRINTF(_L8("Cancelling OCSP check"));
	iInstallMachine.iSecurityManager->Cancel();
	}

//
// TCheckPrerequisitesState
//

CInstallMachine::TCheckPrerequisitesState::TCheckPrerequisitesState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TCheckPrerequisitesState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Prerequisites Check State"));

	CPrerequisitesChecker* checker = CPrerequisitesChecker::NewLC(
		iInstallMachine.UiHandler(), iInstallMachine.CurrentController(),
		*iInstallMachine.iResult, *iInstallMachine.iCurrentContentProvider,
		iInstallMachine.MainController());

	checker->CheckPrerequisitesL();

	CleanupStack::PopAndDestroy(checker);
	iInstallMachine.CompleteSelf();
	iInstallMachine.SetActive();
	}

CInstallMachine::TState* CInstallMachine::TCheckPrerequisitesState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Completed Prerequisites Check State"));
	return static_cast<TState*>(&iInstallMachine.iPlanInstallationState);
	}
	
CInstallMachine::TState* CInstallMachine::TCheckPrerequisitesState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Prerequisites Check State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TCheckPrerequisitesState::Cancel()
	{
	}
	
//
// TPlanInstallationState
//

CInstallMachine::TPlanInstallationState::TPlanInstallationState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TPlanInstallationState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Installation Planning State"));
	
	// The following function may leave either because of an error or because 
	// the user cancelled installation (a dialog), in which case it notifies 
	// the UI so that we don't need to display anything (cancellation is not an
	// error). See RunError() for more details.
	
	// used to store an array of file descriptions of the files that
	// require capability checking
	// This array does not OWN any of the pointers
	RPointerArray<CFileDescription> filesToCapabilityCheck;
	CleanupClosePushL(filesToCapabilityCheck);
	
	// Do the planning for the controller we just validated
	iInstallMachine.iPlanner->PlanCurrentControllerL(filesToCapabilityCheck);	
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Get the Capabilities required by the executables in the given component and set it in current controller.
	//Setting the HasExecutable flag in the current contoller if it 
	//contains any executable(.exe or .dll).
	if (iInstallMachine.IsInInfoMode())
		{
		Sis::CController& controller = const_cast <Sis::CController&>(iInstallMachine.iPlanner->CurrentController());
		controller.SetHasExecutable(EFalse);
		TInt noOfFiles = filesToCapabilityCheck.Count();		
		for(TInt i = 0 ; i < noOfFiles ; i++)
		   {
		   Swi::Sis::CFileDescription* fileDesc = filesToCapabilityCheck[i];
		   const Swi::Sis::CString& filePath = fileDesc->Target();	
		   const TDesC& path = filePath.Data();
		   TInt pathLength = path.Length();
		   if(pathLength < 4)
			  continue;
		   const TPtrC& extension = path.Mid(pathLength-4,4);					
		   TBuf16<5> extnInLowerCase = extension;
		   extnInLowerCase.LowerCase();
		   if(extnInLowerCase == _L(".exe") || extnInLowerCase == _L(".dll"))
			   {
			   controller.SetHasExecutable(ETrue);
			   break;
			   }
		    }
		}
	
	#endif

	// Check capabilities and ask the user to grant them if some of the user 
	// capabilities are not signed for.
	iInstallMachine.CheckAndGrantCapabilitiesL(filesToCapabilityCheck);

 	// Check if the Device ID is in the constraints.
 	iInstallMachine.CheckDeviceIdConstraintsL();
	
	iInstallMachine.CompleteSelf();
	iInstallMachine.SetActive();
	CleanupStack::PopAndDestroy(&filesToCapabilityCheck);
	}

// The next state will be interfacing with STS
CInstallMachine::TState* CInstallMachine::TPlanInstallationState::CompleteL()
	{		
	DEBUG_PRINTF(_L8("Install Machine - Completed Installation Planning State"));
	// find the next controller to validate, otherwise go on to next stage
	if(!iInstallMachine.iPlanner->GetNextController())
		{
		DEBUG_PRINTF(_L8("Finished planning final controller"));
		
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
		// If the machine runs in component information extraction mode,
		// All the needed info would have been available by this point.
		// So, directly go to the final state and write the collected information back to the client.
		if(iInstallMachine.IsInInfoMode())
			{		
			return static_cast<TState*>(&iInstallMachine.iFinalState);
			}
		#endif
		
		iInstallMachine.iPlanner->FinalisePlanL();
		
		iInstallMachine.iPlan=iInstallMachine.iPlanner->TransferPlanOwnership(); // pass on ownership!
		delete iInstallMachine.iPlanner;
		iInstallMachine.iPlanner = NULL;
		iInstallMachine.iSecurityManager->SetDevCertWarningState(ENoDevCerts);
		
		if (iInstallMachine.iContentProvider != iInstallMachine.iCurrentContentProvider)
			{
			delete iInstallMachine.iCurrentContentProvider;
			iInstallMachine.iCurrentContentProvider = iInstallMachine.iContentProvider;
			}		
        return static_cast<TState*>(&iInstallMachine.iCheckPostrequisitesState);        
        }
	else
		{
		if (iInstallMachine.iContentProvider != iInstallMachine.iCurrentContentProvider)
			{
			delete iInstallMachine.iCurrentContentProvider;

			// In case NewL() in the following line leaves.
			iInstallMachine.iCurrentContentProvider = NULL;
			}
			
		iInstallMachine.iCurrentContentProvider = CContentProvider::NewL(iInstallMachine.CurrentController());
		
		return static_cast<TState*>(&iInstallMachine.iVerifyControllerState);
		}
	}

CInstallMachine::TState* CInstallMachine::TPlanInstallationState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Planning Installation State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TPlanInstallationState::Cancel()
	{
	}

//
// TCheckPostrequisitesState
//

CInstallMachine::TCheckPostrequisitesState::TCheckPostrequisitesState(CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TCheckPostrequisitesState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Postrequisites Check State"));
	
	CPostrequisitesChecker* checker =
		CPostrequisitesChecker::NewLC(iInstallMachine.UiHandler(),
									  *iInstallMachine.iPlan,
									  *iInstallMachine.iResult,
									  *iInstallMachine.iCurrentContentProvider);

	checker->CheckPostrequisitesL();

	CleanupStack::PopAndDestroy(checker);
	iInstallMachine.CompleteSelf();
	iInstallMachine.SetActive();
	}

CInstallMachine::TState* CInstallMachine::TCheckPostrequisitesState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Completed Postrequisites Check State"));
	// Inform UI of final progress bar value.
	iInstallMachine.SetProgressBarFinalValueL();
	return static_cast<TState*>(&iInstallMachine.iIntegritySupportState);
	}
	
CInstallMachine::TState* CInstallMachine::TCheckPostrequisitesState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Postrequisites Check State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TCheckPostrequisitesState::Cancel()
	{
	}
	
//
// TIntegritySupportState
//

CInstallMachine::TIntegritySupportState::TIntegritySupportState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::TIntegritySupportState::EnterL()
	{
 	DEBUG_PRINTF(_L8("Install Machine - Entering Integrity Support State"));
	
	// delete some data that we don't need while doing this memory
	// intensive operation.
	delete iInstallMachine.iPlanner;
	iInstallMachine.iPlanner = NULL;
	
	// create an application processor
	iInstallMachine.iProcessor = CInstallationProcessor::NewL(
		*iInstallMachine.iPlan, *iInstallMachine.iSecurityManager, 
		iInstallMachine.iSisHelper, iInstallMachine.UiHandler(),
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		iInstallMachine.TransactionSession(), iInstallMachine.RegistryWrapper(),
#else
		iInstallMachine.IntegrityServicesL(), 
#endif
		*iInstallMachine.iControllerData, iInstallMachine.Observer());
	
	// execute the plan, install files, update registry, all in one step
	iInstallMachine.iProcessor->ProcessPlanL(iInstallMachine.iStatus);
	iInstallMachine.SetActive();
	}

CInstallMachine::TState* CInstallMachine::TIntegritySupportState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Completed Integrity Support State"));
	return static_cast<TState*>(&iInstallMachine.iFinalState);
	}
	
CInstallMachine::TState* CInstallMachine::TIntegritySupportState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Integrity Support State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TIntegritySupportState::Cancel()
	{
	DEBUG_PRINTF(_L8("Install Machine - Cancelling Integrity Support State"));

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	DeRegisterForceRegisteredAppsL();
#endif

	if (iInstallMachine.iProcessor)
		{
		iInstallMachine.iProcessor->Cancel();
		}
	}

//
// TFinalState
//

CInstallMachine::TFinalState::TFinalState(
	CInstallMachine& aInstallMachine)
:   CInstallMachine::TInstallState(aInstallMachine)
	{
	}

void CInstallMachine::RunFileL(RSisLauncherSession& aLauncher, const TDesC& aFileName, const TDesC& aMimeType, Sis::TSISFileOperationOptions aFileOperationOption)
	{
	TBool wait = EFalse;
	if (aFileOperationOption & Sis::EInstFileRunOptionWaitEnd)
		{
		// always wait for completion or timeout when uninstalling since 
		// cannot remove the file when it is in use!
		wait = ETrue;
		}
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
	
	// Is the file an executable ?
	TEntry entry;
	User::LeaveIfError(fs.Entry(aFileName, entry));
	if (entry.IsTypeValid() && SecUtils::IsExe(entry))
		{
		DEBUG_PRINTF2(_L("Install Machine - Running %S"),&aFileName);
		aLauncher.RunExecutableL(aFileName, wait);
		}
	else 
		{
		RFile file;
		CleanupClosePushL(file);				
		if (aFileOperationOption & Sis::EInstFileRunOptionByMimeType)
			{
			DEBUG_PRINTF2(_L("Install Machine - Running via Mime %S"),&aFileName);
			HBufC8* mimeType = HBufC8::NewLC(aMimeType.Length());
			TPtr8 ptr = mimeType->Des();
			ptr.Copy(aMimeType);
			TRAPD(err, aLauncher.StartByMimeL(aFileName, *mimeType, wait));
			if (err!=KErrNone) 
				{
				User::LeaveIfError(file.Open(fs, aFileName, EFileShareExclusive|EFileWrite));
				TRAP_IGNORE(aLauncher.StartByMimeL(file, *mimeType, wait));
				}
			CleanupStack::PopAndDestroy(mimeType);		
			}
		else
			{
			DEBUG_PRINTF2(_L("Install Machine - Launching doc %S"),&aFileName);
			TRAPD(err, aLauncher.StartDocumentL(aFileName, wait));		
			if (err!=KErrNone)
				{
				User::LeaveIfError(file.Open(fs, aFileName, EFileShareExclusive|EFileWrite));
				TRAP_IGNORE(aLauncher.StartDocumentL(file, wait));	
				}
			}
		CleanupStack::PopAndDestroy(&file);		
		}
	CleanupStack::PopAndDestroy(&fs);
	}

void CInstallMachine::PublishPropertiesL()
	{
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisStatusSuccess | ESwisInstall));
		
	// Store the top level controller's UID as the most recent successful installation
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidSwiLatestInstallation, iController->Info().Uid().Uid().iUid));
	}
    
void CInstallMachine::PostJournalFinalizationL(TInt aError)
	{
	DEBUG_PRINTF(_L8("Install Machine - PostJournalFinalization"));
   
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Do nothing in info mode
	if(IsInInfoMode())
		{
		return;
		}	    
#endif
	
	if (!iPlan)
		{
		return;
		}
	const RPointerArray<CSisRegistryFileDescription>& filesToRun = iPlan->FilesToRunAfterInstall();
    RSisLauncherSession launcher;	        
    if (launcher.Connect() != KErrNone)
        {
        DEBUG_PRINTF(_L8("Install Machine - Failed to connect to SisLauncher"));
        return;
        }
    CleanupClosePushL(launcher);
	 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	//Notify apparc for the the change in the Applications
    RArray<TAppUpdateInfo> affectedApps;
    iPlan->GetAffectedApps(affectedApps);
    if (affectedApps.Count() > 0)
        {
        launcher.NotifyNewAppsL(affectedApps);
        }
    affectedApps.Close();
#endif
	    
	TInt numFiles = filesToRun.Count();	
	if (aError != KErrNone || numFiles <= 0)
		{
		CleanupStack::PopAndDestroy(&launcher);
		return;
		}
	
	// Check the top level application, if it's not trusted and the policy says no
	// then don't run anything.
	CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
	if (!secPolicy->AllowRunOnInstallUninstall() && 
			iPlan->ApplicationL().ControllerL().TrustStatus().ValidationStatus() < EValidatedToAnchor)
		{
		DEBUG_PRINTF(_L8("Install Machine - Toplevel controller is untrusted, not running files after install"));
		return;
		}
	
	DEBUG_PRINTF(_L8("Install Machine - Processing files to run after install"));	

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK		
	
	launcher.NotifyNewAppsL(iPlan->AppArcRegFiles());
#endif
	if (iPlan->ContainsPlugins())

		{
		// set the "I'm done" propererty AFTER running ECOM reliant files.
		launcher.RunAfterEcomNotificationL(filesToRun);
		RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisNone);
		}
	else
		{
		// set the "I'm done" propererty before running non-ECOM reliant files.
		RProperty::Set(KUidSystemCategory, KUidSoftwareInstallKey, ESwisNone);
		for (int i = 0; i < numFiles; ++i)
			{
			CSisRegistryFileDescription* file = filesToRun[i];
			RunFileL(launcher,file->Target(),file->MimeType(),file->OperationOptions());
			} 
		}
	CleanupStack::PopAndDestroy(&launcher);
	}

 void CInstallMachine::TFinalState::EnterL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Entering Final State"));
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// If the machine runs in component information extraction mode,
	// All the needed info would have been available by this point.
	// So, call the method to populate the details back to the given structure
	// and go to the final state of the machine.
	if(iInstallMachine.IsInInfoMode())
		{
		DEBUG_PRINTF(_L8("Install Machine - Returning component info"));
		// Call in to populate the native component info in to the given structure.
		PopulateNativeComponentInfoL(iInstallMachine.MainController(), iInstallMachine.iNativeComponentInfo);
		
		// Write the component information collected back in to the buffer passed in.
		SendBackComponentInfoL();
		}
	else
	#endif
		{
        iInstallMachine.PublishPropertiesL();
		
		if (iInstallMachine.iSecurityManager->SecurityPolicy().DrmEnabled())
			{
			DEBUG_PRINTF(_L8("Attempting to access (possibly) DRM'd SIS file via CAF"));
			iInstallMachine.iSisHelper.ExecuteDrmIntentL(static_cast<ContentAccess::TIntent>(iInstallMachine.iSecurityManager->SecurityPolicy().DrmIntent()));
			}
		}
	
    iInstallMachine.CompleteSelf();
	iInstallMachine.SetActive();
	}
    
CInstallMachine::TState* CInstallMachine::TFinalState::CompleteL()
	{
	DEBUG_PRINTF(_L8("Install Machine - Completed Final State"));
	return NULL;
	}
	
CInstallMachine::TState* CInstallMachine::TFinalState::ErrorL(
	TInt aCode)
	{
	DEBUG_PRINTF2(_L8("Install Machine - Final State failed with code %d"), aCode);
	User::Leave(aCode);
	return NULL;
	}

void CInstallMachine::TFinalState::Cancel()
	{
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	DeRegisterForceRegisteredAppsL();
#endif
	}

//
// TControllerInfo
//

CInstallMachine::TControllerInfo::TControllerInfo(
	const CController& aController, TInt64 aDataOffset)
	: iController(aController), iDataOffset(aDataOffset)
	{
	}
			
const CController& CInstallMachine::TControllerInfo::Controller() const
	{
	return iController;
	}
			
TInt64 CInstallMachine::TControllerInfo::DataOffset() const
	{
	return iDataOffset;
	}

//
// CInstallMachine
//

CInstallMachine* CInstallMachine::NewLC(const RMessage2& aMessage)
	{
	CInstallMachine* self=new(ELeave) CInstallMachine(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CInstallMachine* CInstallMachine::NewLC(const RMessage2& aMessage, const TBool aInInfoMode)
	{
	CInstallMachine* self=new(ELeave) CInstallMachine(aMessage, aInInfoMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
#endif

CInstallMachine* CInstallMachine::NewL(const RMessage2& aMessage)
	{
	CInstallMachine* self=NewLC(aMessage);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CInstallMachine* CInstallMachine::NewL(const RMessage2& aMessage,const TBool aInInfoMode)
	{
	CInstallMachine* self=NewLC(aMessage, aInInfoMode);
	CleanupStack::Pop(self);
	return self;
	}
#endif

void CInstallMachine::ConstructL()
	{
	DEBUG_PRINTF(_L8("Constructing new Install Machine"));

	// connect to SIS helper before we start a transaction with transaction server
	// so that long running rollbacks don't cause install to fail
	User::LeaveIfError(iSisHelper.Connect());
	
	// Construct base class ( transaction)
	CSwisStateMachine::ConstructL();
		
	// Read install preferences from the stream (they are passed in the 
	// message's parameter 0)
	TInt len=Message().GetDesLengthL(0);
	HBufC8* buf=HBufC8::NewMaxLC(len);
	TPtr8 p=buf->Des();
	Message().ReadL(0,p);
	RMemReadStream is;
	is.Open(p.Ptr(),len);
	CleanupClosePushL(is);
	iInstallPrefs=CInstallPrefs::NewL(is);
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK   
	if(!IsInInfoMode()) 
         { 
	#endif 
         TInt deviceLanguageCount = is.ReadInt32L(); 
         for(TInt i=0;i<deviceLanguageCount;i++) 
        	 { 
        	 iDeviceSupportedLanguages.AppendL(is.ReadInt32L()); 
        	 DEBUG_PRINTF2(_L8("Language id that device supports =  %d"), iDeviceSupportedLanguages[i]); 
        	 } 
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK   
         } 
	iNativeComponentInfo = CNativeComponentInfo::NewL();   
	#endif 
	
	CleanupStack::PopAndDestroy(2,buf); // is, buf

	// Create security manager here because we'll need its functionality very 
	// early to display certificates to the user in the Install dialog (see 
	// TConfirmationState::EnterL()).
	iSecurityManager=CSecurityManager::NewL();
	}
	
// Constructor, sets this pointer into the state objects
CInstallMachine::CInstallMachine(const RMessage2& aMessage)
:   CSwisStateMachine(aMessage),
	iRegistrationState(*this),
	iGetControllerState(*this),
	iConfirmationState(*this),
	iVerifyControllerState(*this),
	iOcspState(*this),
	iCheckPrerequisitesState(*this),
    iPlanInstallationState(*this),
    iCheckPostrequisitesState(*this),
    iIntegritySupportState(*this),
    iFinalState(*this),
	iSigValidationResult(EValidationSucceeded),
	iEmbedded(EFalse)
	{
	}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CInstallMachine::CInstallMachine(const RMessage2& aMessage,const TBool aInInfoMode)
:   CSwisStateMachine(aMessage, aInInfoMode),
	iRegistrationState(*this),
	iGetControllerState(*this),
	iConfirmationState(*this),
	iVerifyControllerState(*this),
	iOcspState(*this),
	iCheckPrerequisitesState(*this),
    iPlanInstallationState(*this),
    iCheckPostrequisitesState(*this),
    iIntegritySupportState(*this),
    iFinalState(*this),
	iSigValidationResult(EValidationSucceeded),
	iEmbedded(EFalse)
	{
	
	}
#endif
// Connects to UISS and SISHelper server
TInt CInstallMachine::Start()
	{
	DEBUG_PRINTF(_L8("Starting Install"));
	return CSwisStateMachine::Start();
	}

// Delete objects, close connection to the servers
CInstallMachine::~CInstallMachine()
	{
	DEBUG_PRINTF(_L8("Destroying Install Machine"));
	
	if (iCurrentContentProvider != iContentProvider)
		{
		delete iCurrentContentProvider;
		}
		
	delete iInstallPrefs;
	delete iControllerData;
	delete iController;
	delete iContentProvider;
	delete iPlan;
	delete iResult;
	delete iProcessor;
	delete iSecurityManager;
	iPkixResults.ResetAndDestroy();
	iCertificates.ResetAndDestroy();
	iCertInfos.ResetAndDestroy();
	iOcspOutcomes.ResetAndDestroy();
	iSisHelper.Close();
	iDeviceSupportedLanguages.Close();
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	delete iNativeComponentInfo;
	#endif
	delete iPlanner;
	}

CInstallMachine::TState* CInstallMachine::FirstState()
	{
	return &iRegistrationState;
	}

// This is called from RunL() whenever a state leaves
CInstallMachine::TState* CInstallMachine::ErrorOnStateEntryL(TInt aError)
	{
	return CurrentState()->ErrorL(aError);
	}

void CInstallMachine::GetRequestedCapabilities(TCapabilitySet& aRequestedCaps, RPointerArray<CFileDescription>& aFilesToCapabilityCheck)
	{
	// UserPromptService private directory
	_LIT(KUpsPrivateDir, ":\\private\\10283558\\");	
	
	TInt numFiles = aFilesToCapabilityCheck.Count();
	for (TInt i=0; i<numFiles; i++)
		{
		const CFileDescription* fd = aFilesToCapabilityCheck[i];
		const CCapabilities* caps = fd->Capabilities();
		
		// Prevent un-authorised modification to User Prompt Service Policy Files
		if (fd->Target().Data().FindF(KUpsPrivateDir) != KErrNotFound)
			{
			DEBUG_PRINTF2(_L("Install Machine - Found %S: Appending AllFiles capability"), &fd->Target().Data());
			aRequestedCaps.AddCapability(ECapabilityAllFiles);
			}		
		
		if (!caps)
			continue;

		const TInt KCapSetSize=sizeof(TUint32); // size of a capability bit set
		const TInt KCapSetSizeBits=8*KCapSetSize;
		const TDesC8& rawCaps=caps->Data();
		const TInt KNumCapSets=rawCaps.Size()/KCapSetSize;
		for (TInt set=0; set<KNumCapSets; set++)
			{
			TUint32 capsValue=*(reinterpret_cast<const TUint32*>(rawCaps.Ptr())+set);
			for (TInt capIndex=0; capIndex<KCapSetSizeBits; capIndex++)
				{
				if (capsValue & (0x1<<capIndex))
					{
					TCapability cap=static_cast<TCapability>(capIndex+set*KCapSetSizeBits);
					aRequestedCaps.AddCapability(cap);
					}
				}
			}
		}	
	}

// This is called from TVerifyControllerState::CompleteL()
void CInstallMachine::CheckAndGrantCapabilitiesL(RPointerArray<CFileDescription>& aFilesToCapabilityCheck)
	{
	// The controller contains capabilities requested by all 
	// executables in it. Let's make a union of them all.
	TCapabilitySet requestedCaps;
	requestedCaps.SetEmpty();
	
	// Get the set of capabilities required by the package.
	GetRequestedCapabilities(requestedCaps, aFilesToCapabilityCheck);
		
	//Get the CertChainConstraint instance built by the SecurityManager.
	const CCertChainConstraints* certChainConstraints = CurrentController().CertChainConstraints();
				
	//Get the capbibilies contrained from the CertChainConstraints
	TCapabilitySet initCapConstraints=certChainConstraints->ValidCapabilities();
	TCapabilitySet supportedCapabilitiesByBoth=initCapConstraints;

	//build the capability constraints from the constrained capabilities and root capbilities
	supportedCapabilitiesByBoth.Intersection(iGrantableCapabilitySet);

  	// We have a set of granted capabilities, which is the intersection of iCapabilitySet  and 
  	// Capbilities constraints(filled in the security manager).
  	// Let's see if that's enough for the files to be installed.
  	requestedCaps.Remove(supportedCapabilitiesByBoth);
  	
  	// Remove capabilities granted by the loader i.e disabled capabilities
  	TCapabilitySet loaderGrantedCaps;
  	loaderGrantedCaps.SetDisabled();
  	requestedCaps.Remove(loaderGrantedCaps);
	
  	// Any capabilities left in requestedCaps after this are not signed for and not in the certs constraints. 
	// Check if any of them are system capabilities. If so, bail out.
  	TCapabilitySet requiredExtraSysCaps(requestedCaps);
  	SecurityCheckUtil::RemoveUserCaps(requiredExtraSysCaps, *iSecurityManager);
		
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Report the error to the user only when machine not runs in info collection mode
	if(IsInInfoMode())
		{
		TCapabilitySet requiredUserCaps(requestedCaps);
		requiredUserCaps.Remove(requiredExtraSysCaps);
		SetUserGrantableCapabilities(requiredUserCaps);
		return;
		}
	#endif

	TAppInfo appInfo(iCurrentContentProvider->DefaultLanguageAppInfoL());
	if (SecurityCheckUtil::NotEmpty(requiredExtraSysCaps)||(SecurityCheckUtil::NotEmpty(requestedCaps) && EFalse==iSecurityManager->SecurityPolicy().AllowGrantUserCaps()))
		{
		// Report error to the user. Include the list of capabilities that are left in requestedCaps.
		// Among these will be at least one cap that is not allowed. This is because 
		// - it is a system cap which hasn't been granted or 
		// - a user cap where granting of user caps during install is not allowed. 

		// Build requested capabilities string.
		const TInt KMaxCapNameLength=32; // should be enough for one capability name and
										 // a space before the next capability name
							  
		HBufC* capsDisplayBuf=HBufC::NewLC(ECapability_Limit*KMaxCapNameLength);
		TPtr p=capsDisplayBuf->Des();
		_LIT(KSpace," "); 
  
		// Loop through the set of all possible capabilities. Check if each
		// is in the requestedCaps set. If it is add the name of the 
		// capability to the string in the capsDisplayBuf
		for (TInt cap=ECapabilityTCB; cap<ECapability_Limit; cap++)
			{ 
			if (requestedCaps.HasCapability(static_cast<TCapability>(cap)))
				{
				// Get the string name for the capability
				const char* capName = CapabilityNames[cap];
				while (*capName)
					{
					p.Append(*capName++);
					}			
		
				// Add a space following each capability name in string
				p.Append(KSpace);
				}
			}
			
		// Report error to the user.
		// Include the list of capability names 
		CDisplayError* cmd=
			CDisplayError::NewLC(appInfo,EUiCapabilitiesCannotBeGranted,
			*capsDisplayBuf);		
				
		UiHandler().ExecuteL(*cmd);
		CleanupStack::PopAndDestroy(cmd);
		CleanupStack::PopAndDestroy(capsDisplayBuf);
		User::Leave(KErrSecurityError);			
		}
	else if (SecurityCheckUtil::NotEmpty(requestedCaps)) //Required more user capabilities then
		{
		// User capabilities are supported by the CertChainConstraints, but not by root certificates.
		if (initCapConstraints.HasCapabilities(requestedCaps))
		{
		// User capabilities only, ask the user to grant them
		CDisplayGrantCapabilities* cmd=
			CDisplayGrantCapabilities::NewLC(appInfo,requestedCaps);
		UiHandler().ExecuteL(*cmd);
		TBool result=cmd->ReturnResult();
		CleanupStack::PopAndDestroy(cmd);
		if (!result)
			User::Leave(KErrSecurityError);
		}
		else
			{
			// User capabilities are not all supported by the CertChainConstraints.
			CDisplayError* cmd=
				CDisplayError::NewLC(appInfo,EUiConstraintsExceeded,
				KNullDesC);
			UiHandler().ExecuteL(*cmd);
			CleanupStack::PopAndDestroy(cmd);			
			User::Leave(KErrSecurityError);		
			}
		}
    }

void CInstallMachine::SignalCompletedL()
	{
	HandleInstallationEventL(iPlan, EEventCompletedInstall);
	}
	
void CInstallMachine::SetTrust(TSisPackageTrust aTrust)
	{
	DEBUG_PRINTF2(_L8("Package Trust Status set to %d"), aTrust);
	
	// create a modifyable reference to the current controller
	CController& controller = const_cast <CController&>
						      (CurrentController());
	
	// set the trust
	controller.SetTrust(aTrust);
	}

TSisPackageTrust CInstallMachine::Trust()
	{
	
	CController& controller = const_cast <CController&>
						      (CurrentController());
	return controller.Trust();						      

	}

void CInstallMachine::SetValidationStatus(TValidationStatus status)
    {
	// set the validation status
	CurrentController().TrustStatus().SetValidationStatus(status);
    }
    
void CInstallMachine::SetRevocationStatus(TRevocationStatus status)
    {
	// set the revocation status
	CurrentController().TrustStatus().SetRevocationStatus(status);
    }
    
void CInstallMachine::CheckDeviceIdConstraintsL()
	{
 	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
 	// Report the error to the user only when machine not runs in info collection mode
	if(IsInInfoMode())
		return;
	#endif			
	
	const CCertChainConstraints* certChainConstraints = CurrentController().CertChainConstraints();
			
	const RPointerArray<HBufC>& deviceIDs=iSecurityManager->DeviceIDsInfo();	
 	TRAPD(err, SecurityCheckUtil::CheckDeviceIdConstraintsL(certChainConstraints, deviceIDs));
 	
	if (err)
		{
		TAppInfo appInfo(iCurrentContentProvider->DefaultLanguageAppInfoL());
		CDisplayError* cmd=
			CDisplayError::NewLC(appInfo,EUiConstraintsExceeded,
			KNullDesC);
		UiHandler().ExecuteL(*cmd);
		CleanupStack::PopAndDestroy(cmd);
		User::Leave(KErrSecurityError);					
		}	
	}

const Sis::CController& CInstallMachine::CurrentController()
	{
	return iPlanner->CurrentController();
	}

const Sis::CController& CInstallMachine::MainController()
	{
	return iPlanner->MainController();
	}

void CInstallMachine::SetProgressBarFinalValueL()
	{
	HandleInstallationEventL(iPlan, EEventSetProgressBarFinalValue, iPlan->FinalProgressBarValue());
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	SetFinalProgressBarValue(iPlan->FinalProgressBarValue());
	#endif
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
// Populate the native component information.
void CInstallMachine::TFinalState::PopulateNativeComponentInfoL(const CController& aController, CNativeComponentInfo* aNativeComponentInfo)
	{	
	const Sis::CController& controller(aController);
	const Sis::CInfo& controllerInfo(controller.Info());
	const TDesC& componentName((controllerInfo.Names()[0])->Data());
	// Set the Component Name attribute.	
	aNativeComponentInfo->iComponentName = HBufC::NewL(componentName.Length());
	TPtr bufPtr(aNativeComponentInfo->iComponentName->Des());	
	bufPtr.Copy(componentName);
		
	// Set the Component Version details.
	TVersion version(controllerInfo.Version().Major(),
					 controllerInfo.Version().Minor(),
					 controllerInfo.Version().Build());	
	TBuf<KSmlBufferSize> versionBuf;	
	versionBuf.AppendFormat(KVersionFormat, version.iMajor, version.iMinor, version.iBuild);
	aNativeComponentInfo->iVersion = HBufC::NewL(versionBuf.Length());
	bufPtr.Set(aNativeComponentInfo->iVersion->Des());
	bufPtr.Copy(versionBuf);
	
	// Set the Vendor Name attribute	
	const TDesC& vendorName(controllerInfo.VendorNames()[0]->Data());
	aNativeComponentInfo->iVendor = HBufC::NewL(vendorName.Length());
	bufPtr.Set(aNativeComponentInfo->iVendor->Des());
	bufPtr.Copy(vendorName);
	
	// Set the gloabl component Id
	aNativeComponentInfo->iGlobalComponentId = FormatGlobalIdLC(controllerInfo.Uid().Uid(), componentName, controllerInfo.InstallType());
	// It's already a member of the class. So, don't keep it in the cleanup stack, pop it out.
	CleanupStack::Pop(aNativeComponentInfo->iGlobalComponentId);
		
	// Initially assume component is not installed (so, id is 0) and the scomo state to be deactivated
	aNativeComponentInfo->iComponentId = 0;
	aNativeComponentInfo->iScomoState = Usif::EDeactivated;
	
	// Connect to the SCR.
	Usif::RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	// Find and set the install status.	
	PopulateInstallStatusL(aController, scrSession, aNativeComponentInfo);
		
	// Get the actual component ID and the SCOMO state if the component entry is found in the SCR
	if ((aNativeComponentInfo->iInstallStatus == Usif::EUpgrade) || 
		(aNativeComponentInfo->iInstallStatus == Usif::EAlreadyInstalled) ||
		(aNativeComponentInfo->iInstallStatus == Usif::ENewerVersionAlreadyInstalled))
		{
		Usif::CComponentEntry* componentEntry = NULL;
		// Get the component entry using the global Id and the software type.
		TRAPD(err, componentEntry = scrSession.GetComponentL(*(aNativeComponentInfo->iGlobalComponentId), Usif::KSoftwareTypeNative));
		if (err == KErrNone)
			{			
			aNativeComponentInfo->iComponentId = componentEntry->ComponentId();
			aNativeComponentInfo->iScomoState = static_cast<Usif::TScomoState>(componentEntry->ScomoState());
			delete componentEntry;
			}
		}

	CleanupStack::PopAndDestroy(&scrSession);
	
	// Set the authenticity info based on the validation status.
	if (controller.TrustStatus().ValidationStatus() >= EValidatedToAnchor)
		{
		aNativeComponentInfo->iAuthenticity = Usif::EAuthenticated;
		}
	else
		{
		aNativeComponentInfo->iAuthenticity = Usif::ENotAuthenticated;
		}
		
	// Set the user grantable capabilities
	aNativeComponentInfo->iUserGrantableCaps = const_cast<CController&>(aController).UserGrantableCapabilities();
	
	// Calculate the component size from the sontroller's file descriptions.
	const RPointerArray<CFileDescription>& fileDescriptions = controller.InstallBlock().FileDescriptions();
	for(TInt i = 0; i < fileDescriptions.Count(); i++)
		{
		aNativeComponentInfo->iMaxInstalledSize += fileDescriptions[i]->UncompressedLength();
		}
	
	//Setting the HasExecutable flag
	aNativeComponentInfo->iHasExe = controller.HasExecutable();
	
	//Setting the drive selection requird flag
	aNativeComponentInfo->iIsDriveSelectionRequired = controller.DriveSelectionRequired();
	
	//Populate ApplicationInfo, copy the app info from controller's iApplicationInfo  to install machines's iApplicationInfo 
	RCPointerArray<CNativeComponentInfo::CNativeApplicationInfo> applications;
	applications = const_cast<CController&>(aController).GetApplicationInfo();
	for(TInt i = 0 ; i < applications.Count() ; i++)
	    {
	    CNativeComponentInfo::CNativeApplicationInfo*  appInfo = NULL;
	    appInfo = CNativeComponentInfo::CNativeApplicationInfo::NewLC(applications[i]->AppUid(), applications[i]->Name(), applications[i]->GroupName(), applications[i]->IconFileName());
	    aNativeComponentInfo->iApplications.AppendL(appInfo);
	    CleanupStack::Pop(appInfo);
	    }	
	
	const RPointerArray<CController>& embeddedControllers = aController.InstallBlock().EmbeddedControllers();
	TInt totalEmbeddedControllers = embeddedControllers.Count();
	for (TInt controller = 0; controller < totalEmbeddedControllers; controller++)
		{
		CNativeComponentInfo* infoNode = CNativeComponentInfo::NewLC();
		PopulateNativeComponentInfoL(*embeddedControllers[controller], infoNode);
		aNativeComponentInfo->iChildren.AppendL(infoNode);
		CleanupStack::Pop(infoNode);
		}	
	}
	
// Populate the install state based on its earlier installation on the device.
void CInstallMachine::TFinalState::PopulateInstallStatusL(const CController& aController, Usif::RSoftwareComponentRegistry& aScrSession, CNativeComponentInfo* aNativeComponentInfo)
	{
	const Sis::CInfo& controllerInfo(aController.Info());		
	const TDesC& componentName((controllerInfo.Names()[0])->Data());
	const TDesC& uniqueVendorName(controllerInfo.UniqueVendorName().Data());
	TInstallType installType = controllerInfo.InstallType();

	// As PA and PP packages are not support for getting component info, we check for them here
	__ASSERT_ALWAYS(installType != EInstPreInstalledApp && installType != EInstPreInstalledPatch, User::Leave(KErrNotSupported));
	
	// Check to see if there is a package installed with this UID.
	RSisRegistryWritableSession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);	
	
	RSisRegistryWritableEntry registryEntry;
	// Open the registry entry using the package name and vendor name.
	TInt error = registryEntry.Open(registrySession, componentName, uniqueVendorName);		
	if (error != KErrNone && error != KErrNotFound)
		User::Leave(error);
	CleanupClosePushL(registryEntry);
	
	TInt versionComparision = 0;
	
	// Registry entry found
	if (error == KErrNone)
		{
		// Take the versions of the installed package and the new package
		TVersion regVer = registryEntry.VersionL();		
		TVersion newVer(controllerInfo.Version().Major(),
				 controllerInfo.Version().Minor(),
				 controllerInfo.Version().Build());
				 
		TBuf<KSmlBufferSize> regVersion;
		TBuf<KSmlBufferSize> newVersion;		
		regVersion.AppendFormat(KVersionFormat, regVer.iMajor, regVer.iMinor, regVer.iBuild);
		newVersion.AppendFormat(KVersionFormat, newVer.iMajor, newVer.iMinor, newVer.iBuild);
		
		// Compare versions of the already installed package and the current package.
		versionComparision = aScrSession.CompareVersionsL(regVersion, newVersion);
		
		// If both the versions are equal, say that, the same package is installed already
		if (versionComparision == 0)
			{
			aNativeComponentInfo->iInstallStatus = Usif::EAlreadyInstalled;
			}
		// If the version installed is higher than the current version, say that, higher version is already installed.
		else if (versionComparision > 0)
			{
			aNativeComponentInfo->iInstallStatus = Usif::ENewerVersionAlreadyInstalled;
			}
		// If the version installed is lower than the current version, say that, this is an upgrade.
		else  // versionComparision < 0
			{
			aNativeComponentInfo->iInstallStatus = Usif::EUpgrade;
			}
		}
	// If the registry entry is not found and the current package is a type 
	// of base (SA or PA) package, say that, it's a new installation.
	else if (installType == EInstInstallation)
		{
		aNativeComponentInfo->iInstallStatus = Usif::ENewComponent;
		}
	// If the registry entry is not found and the current package is a type 
	// of partial upgrade (PU) package, say that, it's invalid because of the missing base.
	else if (installType == EInstPartialUpgrade)
		{
		aNativeComponentInfo->iInstallStatus = Usif::EInvalid;
		}
	else if (installType == EInstAugmentation)
		{		
		// Get the component entry from SCR using the base package's globalId (UID). If the entry exists, the base package is installed.
		// So, say the current component will become a new component. If the entry is not found in SCR, the base package is not installed.
		// So, say the current component will become invalid (as the base is missing).
		Usif::CComponentEntry* componentEntry = NULL;
		HBufC* globalId = FormatGlobalIdLC(controllerInfo.Uid().Uid(), componentName, EInstInstallation);
		componentEntry = aScrSession.GetComponentL(*globalId, Usif::KSoftwareTypeNative);
		if (componentEntry != NULL)		
			{
			aNativeComponentInfo->iInstallStatus = Usif::ENewComponent;
			}
		else
			{	
			aNativeComponentInfo->iInstallStatus = Usif::EInvalid;
			}
		CleanupStack::PopAndDestroy(globalId);
		delete componentEntry;
		}	
	CleanupStack::PopAndDestroy(2, &registrySession);
	}

void CInstallMachine::TFinalState::SendBackComponentInfoL()
	{		
	const static TInt KDefaultBufferSize = 2048;
	
	TInt err = KErrNone;
	TInt streamBufSize = KDefaultBufferSize;
	CBufFlat* tempBuffer = NULL;
	RBufWriteStream stream;
	do
		{
		tempBuffer = CBufFlat::NewL(streamBufSize);
		CleanupStack::PushL(tempBuffer);
		
		stream.Open(*tempBuffer);
		CleanupClosePushL(stream);		
	
		// Externalise the component info in to the stream		
		TRAP(err, iInstallMachine.iNativeComponentInfo->ExternalizeL(stream));
		if (err == KErrOverflow)
			{
			// Release the last allocation and re allocate with one more fold
			CleanupStack::PopAndDestroy(2, tempBuffer); // tempBuffer, stream
			streamBufSize += KDefaultBufferSize;
			}			
		} while(err == KErrOverflow);
		
	stream.CommitL();

	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
		    	    
    // Comp info buffer in the IPC args is in 2nd position. So, the index will be 1 (starts at 0).
    TInt compInfoBufIndex = 1;
    TInt argBufMaxLength = iInstallMachine.Message().GetDesMaxLengthL(compInfoBufIndex);
    TInt localBufSize = buffer->Size();
    
    // If the allocation is insufficient for sending back the data, send the Over-Flow message to the client.
    // client will re-allocate the required amount of memory and will issue the request again.
	if ( argBufMaxLength < localBufSize)
		{			
		DEBUG_PRINTF3(_L8("Discovered overflow in component info buffer - needed %d but only %d was available"),
			localBufSize, argBufMaxLength);
		TPckgC<TInt> bufferSizePackage(localBufSize);
		iInstallMachine.Message().WriteL(compInfoBufIndex, bufferSizePackage);
		User::Leave(KErrOverflow);
		}
	// If the memory is sufficient, write the details back in to the client allocated memory.
	else
		{
		iInstallMachine.Message().WriteL(compInfoBufIndex, *buffer);
		}
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer		
	}
	
void CInstallMachine::SetUserGrantableCapabilities(TCapabilitySet aCapabilitySet)
	{
	DEBUG_PRINTF(_L8("Setting Package's User Granted Capabilities"));
	
	// create a modifyable reference to the current controller
	CController& controller = const_cast<CController&>(CurrentController());
	
	// set the capabilities
	controller.SetUserGrantableCapabilities(aCapabilitySet);
	}
#endif
