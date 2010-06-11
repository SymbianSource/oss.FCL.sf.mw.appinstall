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
*
*/


/**
 @file 
 @internalComponent 
*/

#ifndef RESTORECONTROLLER_H
#define RESTORECONTROLLER_H

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <hash.h>

#include "dessisdataprovider.h"
#include "siscontroller.h"
#include "sisfiledescription.h"
#include "securitymanager.h"
#include "msisuihandlers.h"
#include "sisfiledescription.h"
#include "sisfieldtypes.h"
#include "plan.h"
#include "application.h"

class RDesReadStream;

namespace Swi 
	{
	
	using namespace Sis;
	
	/**
	 * A complete description of a file to be restored,
	 * including its target file name and source controller
	 * 
	 *
	 * @internalComponent
	 * @released
	 */
	
	class TRestoreFileInformation
		{
		
	public:
		TRestoreFileInformation(CFileDescription& aFileDescription, CApplication& aParentApplication);
		
		CFileDescription& FileDescription();
		CApplication& ParentApplication();
		
	private:
		CFileDescription& iFileDescription;
		CApplication&  iParentApplication;
		
		};

	/**
	 * Requests the security manager verify the certificates and signatures in
	 * the given SIS controller object, then awaits the result.
	 *
	 * @internalComponent
	 * @released
	 */

	class CRestoreController : public CBase 
		{
		
	public:
	
		class CSisCertificateVerifier : public CActive
			{
			
			public:
				static CSisCertificateVerifier* NewLC(CController* aController, TDesC8& aControllerBinary, CSecurityManager& aSecurityManager, CApplication* aApplication);
				static CSisCertificateVerifier* NewL(CController* aController, TDesC8& aControllerBinary, CSecurityManager& aSecurityManager, CApplication* aApplication);
				
				void StartL(TRequestStatus& aStatus);
				~CSisCertificateVerifier();
				
				const TDesC8& ControllerBinary();
				const CController* Controller();
				
				void WaitOnCompletion(TRequestStatus& aWatcherStatus);
				
			protected:
				CSisCertificateVerifier(CController* aController, CSecurityManager& aSecurityManager, CApplication* aApplication, TDesC8& aControllerBinary);
			
				void RunL();
				void DoCancel();
			
			private:
				void CheckDeviceIdConstraintsL();
				void CheckCapabilitiesL();
				CSecurityManager& iSecurityManager;
				TSignatureValidationResult iResult;
				CController* iController;
				TBool iAllowUnsigned;
				RPointerArray<CPKIXValidationResultBase> iValidationResults;
				RPointerArray<CX509Certificate> iCerts;
				TCapabilitySet iGrantableCapabilitySet;
				
				TRequestStatus* iWatcherStatus;
				CApplication* iApplication;
				TDesC8& iControllerBinary;
				
			};
	
	public:
		static CRestoreController* NewLC(TDesC8& aController, CSecurityManager& aSecurityManager, RFs& aFs);
		static CRestoreController* NewL(TDesC8& aController, CSecurityManager& aSecurityManager, RFs& aFs);
	
		const RPointerArray<CPlan>& Plans();
		const RPointerArray<CSisCertificateVerifier>& Verifiers();
		const TChar& InstallDrive();
		inline const RPointerArray<TPtrC8>& ControllerBinaries() const;
		
		void AddFileDescriptionL(RFile& aFile, TDesC& aTarget, TDes& aTempFilename, TBool aHashCheckRequired);
		void AddEmbeddedAppsAndFilesL(); 
	
		~CRestoreController();
	
	private:
		void ConstructL(TDesC8& aController);
		CRestoreController(CSecurityManager& aSecurityManager, RFs& aFs);
	
		CMessageDigest* CalculateHashL(RFile& aFile, TSISHashAlgorithm aAlgorithm); 
		TInt SetApplicationLanguageL(CApplication& aApplication);
		void InternalizeControllersL(TDesC8& aController);	
		CFileDescription* FindFileDescriptionL(const CInstallBlock& aInstallBlock, RFile& aFile, TDesC& aTarget, TBool aReadOnly, const TChar& aDrive);
		TRestoreFileInformation LookupFileInformationL(RFile& aFile, TDesC& aTarget, TBool aHashCheckRequired); 
		void DoAddEmbeddedAppsAndFilesL(
			const CInstallBlock& aInstallBlock,
			CApplication& aApplication);
		TPtrC8* CreateRawControllerPtrLC(
			RDesReadStream& aStream, 
			const TUint8* aBasePtr, 
			TInt& aOffset );
			
	private:
	 	RPointerArray<CController> iControllers;
	 	RPointerArray<TPtrC8> iControllerBinaries;
	 	
	 	RPointerArray<CSisCertificateVerifier> iVerifiers;
	 	
	 	RPointerArray<CPlan> iInstallPlans;
	 	RPointerArray<CApplication> iTopLevelApplications;
	 	RPointerArray<HBufC> iInstalledFiles;
   	 	
  	 	RFs& iFs;
	 	CSecurityManager& iSecurityManager;
	 	TChar iInstallDrive;
		RArray<TChar> iAugmentationDrives;
		RArray<TLanguage> iMatchingSupportedLanguagesArray;
		
		/// The drive on which to store device integrity data (hashes, registry etc) 
		TChar iSystemDriveChar;					
		};
		
	
	inline CFileDescription& TRestoreFileInformation::FileDescription()
		{
		return iFileDescription;
		}
		
	inline CApplication& TRestoreFileInformation::ParentApplication()
		{
		return iParentApplication;
		}
		
	inline const RPointerArray<CRestoreController::CSisCertificateVerifier>& CRestoreController::Verifiers()
		{
		return iVerifiers;
		}
		
	inline const TDesC8& CRestoreController::CSisCertificateVerifier::ControllerBinary()
		{
		return iControllerBinary;
		}
		
	inline const CController* CRestoreController::CSisCertificateVerifier::Controller()
		{
		return iController;
		}
		
	inline const RPointerArray<CPlan>& CRestoreController::Plans()
		{			
		return iInstallPlans;			
		}
	
	inline const RPointerArray<TPtrC8>& CRestoreController::ControllerBinaries() const
		{
		return iControllerBinaries;
		}
	
	inline const TChar& CRestoreController::InstallDrive()
		{		
		return iInstallDrive;		
		}
	}

#endif // RESTORECONTROLLER_H

