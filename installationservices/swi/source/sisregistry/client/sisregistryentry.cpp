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
* sisregistry - client registry entry interface implementation
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#include<s32mem.h>
#include <hash.h>
#include <swi/sistruststatus.h>
#include <x509cert.h>
#include "sisregistrydependency.h"
#include "sisregistryentry.h"
#include "sisregistrysession.h"
#include "sisregistryclientserver.h"
#include "sisregistrypackage.h"
#include "sisregistryobject.h"
#include "hashcontainer.h"
#include "arrayutils.h"
#include "cleanuputils.h"

using namespace Swi;

const TInt KMinBufferSize = 20;

EXPORT_C TInt RSisRegistryEntry::Open(RSisRegistrySession& aSession, TUid aUid)
	{
	TPckgC<TUid> uid(aUid);
	
	return CreateSubSession(aSession, EOpenRegistryUid, TIpcArgs(&uid));
	}

EXPORT_C TInt RSisRegistryEntry::OpenL(RSisRegistrySession& aSession,
										const CSisRegistryPackage& aPackage)
	{	
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	
	aPackage.ExternalizeL(stream);
	stream.CommitL();
	
	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewLC(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	
	TInt err = CreateSubSession(aSession, EOpenRegistryPackage, TIpcArgs(&ptr));
	User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer

	return err;
	}
	
EXPORT_C TInt RSisRegistryEntry::Open(RSisRegistrySession &aSession, const TDesC& aPackageName, const TDesC& aVendorName)
	{
	return CreateSubSession(aSession, EOpenRegistryNames, TIpcArgs(&aPackageName, &aVendorName));
	}	

EXPORT_C void RSisRegistryEntry::Close()
	{
	CloseSubSession(ECloseRegistryEntry);
	}

EXPORT_C TBool RSisRegistryEntry::IsPresentL()
	{
	TBool isPresent = EFalse ;
	TPckg<TBool> isPresentPkg(isPresent);
	
	User::LeaveIfError(SendReceive(EUidPresent, TIpcArgs(&isPresentPkg)));
	
	return isPresent;
	}

EXPORT_C TBool RSisRegistryEntry::IsSignedL()
	{
	TBool isSigned = EFalse;
	TPckg<TBool> isSignedPkg(isSigned);
	
	User::LeaveIfError(SendReceive(ESigned, TIpcArgs(&isSignedPkg)));
	return isSigned;
	}

EXPORT_C TSisPackageTrust RSisRegistryEntry::TrustL() const
	{
	TSisPackageTrust trust;
	TPckg<TSisPackageTrust> trustPkg(trust);
	
	User::LeaveIfError(SendReceive(EGetTrust, TIpcArgs(&trustPkg)));
	return trust;
	}

EXPORT_C TTime RSisRegistryEntry::TrustTimeStampL() const
	{
	TTime trustTimeStamp;
	TPckg<TTime> timePkg(trustTimeStamp);
	
	User::LeaveIfError(SendReceive(ETrustTimeStamp, TIpcArgs(&timePkg)));
	return trustTimeStamp;
	}

EXPORT_C TSisTrustStatus RSisRegistryEntry::TrustStatusL() 
    {
    HBufC8* buffer = SendReceiveBufferLC((TInt)ETrustStatus);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble from the stream
	TSisTrustStatus trustStatus;
	
	trustStatus.InternalizeL(stream);
	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
    
    return trustStatus;
    
    } 

EXPORT_C TBool RSisRegistryEntry::IsInRomL()
	{
	TBool isInRom = EFalse;
	TPckg<TBool> isInRomPkg(isInRom);
	
	User::LeaveIfError(SendReceive(EInRom, TIpcArgs(&isInRomPkg)));
	return isInRom;
	}
	


EXPORT_C TBool RSisRegistryEntry::IsAugmentationL()
	{
	TBool isAugmentation = EFalse;
	TPckg<TBool> isAugmentationPkg(isAugmentation);
	
	User::LeaveIfError(SendReceive(EAugmentation, TIpcArgs(&isAugmentationPkg)));
	return isAugmentation;
	}

EXPORT_C TVersion RSisRegistryEntry::VersionL()
	{
	TVersion version;
	TPckg<TVersion> packageVersion(version);
	
	User::LeaveIfError(SendReceive(EVersion, TIpcArgs(&packageVersion)));
	return version;
	}

EXPORT_C TLanguage RSisRegistryEntry::LanguageL()
	{
	TLanguage language;
	TPckg<TLanguage> packageLanguage(language);
	
	User::LeaveIfError(SendReceive(ELanguage, TIpcArgs(&packageLanguage)));
	return language;
	}

EXPORT_C TUid RSisRegistryEntry::UidL()
	{
	TUid uid;
	TPckg<TUid> packageUid(uid);
	
	User::LeaveIfError(SendReceive(EUid, TIpcArgs(&packageUid)));
	return uid;
	}

EXPORT_C HBufC* RSisRegistryEntry::PackageNameL()
	{
	HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr ptr = buffer->Des();
	User::LeaveIfError(SendReceive(EPackageName, TIpcArgs(&ptr)));
	CleanupStack::Pop(buffer);
	return buffer;
	}

EXPORT_C HBufC* RSisRegistryEntry::UniqueVendorNameL()
	{
	HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr ptr = buffer->Des();
	User::LeaveIfError(SendReceive(EUniqueVendorName, TIpcArgs(&ptr)));
	CleanupStack::Pop(buffer);
	return buffer;
	}

EXPORT_C HBufC* RSisRegistryEntry::LocalizedVendorNameL()
	{
	HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr ptr=buffer->Des();
	User::LeaveIfError(SendReceive(ELocalizedVendorName, TIpcArgs(&ptr)));
	CleanupStack::Pop(buffer);
	return buffer;
	}
	
EXPORT_C void RSisRegistryEntry::SidsL(RArray<TUid>& aSids)
	{
	HBufC8* buffer = SendReceiveBufferLC(ESids);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizeArrayL(aSids, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
	}


EXPORT_C void RSisRegistryEntry::FilesL(RPointerArray<HBufC>& aFiles)
	{
	TInt startingFile = 0;
	TPckgBuf<TInt> filesCount;
	TStubExtractionMode operationMode = EGetCount;	

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TPckgC<TInt> pckgStartingFile(startingFile);
#endif

	// Get the total file count.	
	TPckgC<TStubExtractionMode> packageMode(operationMode);
	
	TInt result = SendReceive(EFiles, TIpcArgs(&packageMode, &filesCount));
	User::LeaveIfError(result);	
	
	TInt totalFilesCount = filesCount();
	operationMode = EGetFiles;	
	
	HBufC8* buffer = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(buffer->Des());
	while (startingFile < totalFilesCount)
		{	
		// Fetch the files.	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		User::LeaveIfError(SendReceive(EFiles, TIpcArgs(&packageMode, &pOutput)));
#else
		User::LeaveIfError(SendReceive(EFiles, TIpcArgs(&packageMode, &pckgStartingFile, &pOutput)));
#endif
		// create a stream based on the buffer
		RDesReadStream stream(*buffer);
		CleanupClosePushL(stream);
		
		// reassemble the array from the stream
		InternalizePointerArrayL(aFiles, stream);
		
		CleanupStack::PopAndDestroy(&stream);  //stream
		
		startingFile = aFiles.Count();	
		}
	
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C void RSisRegistryEntry::CertificateChainsL(RPointerArray<HBufC8>& aCertificateChains)
	{
	HBufC8* buffer = SendReceiveBufferLC(ECertificateChains);

	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
    InternalizePointerArrayL(aCertificateChains, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
	}
	
EXPORT_C CHashContainer* RSisRegistryEntry::HashL(const TDesC& aFileName)
	{
	HBufC8* buffer = SendReceiveBufferLC(EHashEntry, aFileName);

	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the hash from the stream
    CHashContainer* hash = CHashContainer::NewLC(stream);
	CleanupStack::Pop(hash);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
	
	return hash;
	}

EXPORT_C TInt RSisRegistryEntry::PropertyL(TInt aKey)
	{
	TInt value;
	TPckg<TInt> propertyValuePckg(value);
	User::LeaveIfError(SendReceive(EProperty, TIpcArgs(aKey, &propertyValuePckg)));	
	return value;
	}

EXPORT_C void RSisRegistryEntry::AugmentationsL(RPointerArray<HBufC>& aPackageNames, RPointerArray<HBufC>& aVendorNames)
	{
	CleanupResetAndDestroyPushL(aPackageNames);
    CleanupResetAndDestroyPushL(aVendorNames);
	HBufC8* buffer = RSisRegistryEntry::SendReceiveBufferLC(EPackageAugmentations);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	RPointerArray<CSisRegistryPackage> packages;
	CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(packages);
	InternalizePointerArrayL(packages, stream);

	aPackageNames.ResetAndDestroy();
	aVendorNames.ResetAndDestroy();

	for (TInt i = 0; i < packages.Count(); i++)
		{
		HBufC* package = packages[i]->Name().AllocLC();
		HBufC* vendor = packages[i]->Vendor().AllocLC();
		
		aPackageNames.AppendL(package);	
		aVendorNames.AppendL(vendor);
		
		CleanupStack::Pop(2, vendor);	//vendor package, 
		}
	CleanupStack::PopAndDestroy(3, buffer);// buffer, stream, packages
	CleanupStack::Pop(2, &aPackageNames);
	}

EXPORT_C void RSisRegistryEntry::AugmentationsL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	HBufC8* buffer = RSisRegistryEntry::SendReceiveBufferLC(EPackageAugmentations);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPackages, stream);

	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
	}

EXPORT_C TInt RSisRegistryEntry::AugmentationsNumberL()
	{
	TInt numAugmentation = 0;
	TPckg<TInt> numAugmentationPkg(numAugmentation);	
	
	User::LeaveIfError(SendReceive(EPackageAugmentationsNumber, TIpcArgs(&numAugmentationPkg)));
	
	return numAugmentation;
	}
		
EXPORT_C CSisRegistryPackage* RSisRegistryEntry::PackageL()
	{
	HBufC8* buffer = SendReceiveBufferLC(EPackage);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// Create the package from the stream
    CSisRegistryPackage* package = CSisRegistryPackage::NewLC(stream);
	CleanupStack::Pop(package);

	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
	return package;
	}

EXPORT_C TInt64 RSisRegistryEntry::SizeL()
	{
	TInt64 size = 0;
	TPckg<TInt64> packageSize(size);
	User::LeaveIfError(SendReceive(ESize, TIpcArgs(&packageSize)));
	return size;
	}

EXPORT_C void RSisRegistryEntry::ControllersL(RPointerArray<HBufC8>& aControllers)
	{
	CleanupResetAndDestroyPushL(aControllers);
	HBufC8* buffer = SendReceiveBufferLC(EControllers);
	
	// this call returns pure data, so we don't incur the overheads of the streamstore
	TInt len(0);
	TPckg<TInt> lenPckg(len);
	
	TInt pos(0);
	TInt lenLen = lenPckg.Length();
	TInt bufLen = buffer->Length();
	while (pos != bufLen)
		{
		lenPckg.Copy(buffer->Mid(pos, lenLen));
		pos += lenLen;
		
		HBufC8* controller = buffer->Mid(pos, len).AllocLC();
		aControllers.AppendL(controller);
		CleanupStack::Pop(controller);
		pos += len;
		}
	
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::Pop(&aControllers);
	}

EXPORT_C TChar RSisRegistryEntry::SelectedDriveL()
	{
	TInt drive;
	TPckg<TInt> selectedDrive(drive);
	
	User::LeaveIfError(SendReceive(ESelectedDrive, TIpcArgs(&selectedDrive)));
	return drive;
	}

EXPORT_C TUint RSisRegistryEntry::InstalledDrivesL()
	{
	TUint drive;
	TPckg<TUint> installeddrives(drive);
	
	User::LeaveIfError(SendReceive(EInstalledDrives, TIpcArgs(&installeddrives)));
	return drive;
	}	

EXPORT_C TBool RSisRegistryEntry::ShutdownAllAppsL()
	{
	TBool shutdownAllApps = EFalse;
	TPckg<TBool> packageShutdownAllApps(shutdownAllApps);
	User::LeaveIfError(SendReceive(EShutdownAllApps, TIpcArgs(&packageShutdownAllApps)));
	return shutdownAllApps;
	}

EXPORT_C TBool RSisRegistryEntry::VerifyControllerSignatureL(RPointerArray<CX509Certificate>& aX509CertArray)
	{
	return VerifyControllerSignatureL(aX509CertArray, ETrue);
	}

EXPORT_C TBool RSisRegistryEntry::VerifyControllerSignatureL(RPointerArray<CX509Certificate>& aX509CertArray, TBool aCheckDateAndTime)
	{
	TInt count = aX509CertArray.Count();
	TInt bufferSize =  Max(count * KDefaultBufferSize / 2, KMinBufferSize);
	CBufFlat* tempBuffer = CBufFlat::NewL(bufferSize);
	CleanupStack::PushL(tempBuffer);

	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	
	// Externalise the pointer array  
	ExternalizePointerArrayL(aX509CertArray, stream);
	stream.CommitL();

	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	bufferSize = tempBuffer->Size();
	HBufC8* buffer = HBufC8::NewLC(bufferSize);
	TPtr8 pbuffer(buffer->Des());
	tempBuffer->Read(0, pbuffer, bufferSize);
	
	TBool isVerified = EFalse;
	TPckg<TBool> packagedResult(isVerified);
	
	TPckgC<TBool> packagedDateCheck(aCheckDateAndTime);
	
	User::LeaveIfError(SendReceive(EVerifyController, TIpcArgs(&packagedResult, &pbuffer, &packagedDateCheck)));
	
	CleanupStack::PopAndDestroy(3, tempBuffer); // tempBuffer, stream, buffer  
	
	return isVerified;	
	}

EXPORT_C TInt RSisRegistryEntry::RemoveWithLastDependentL()
	{
	TInt isRemoveWithLastDependent = 0;
	TPckg<TInt> isRemoveWithLastDependentPkg(isRemoveWithLastDependent);
	
	User::LeaveIfError(SendReceive(ERemoveWithLastDependent, TIpcArgs(&isRemoveWithLastDependentPkg)));
	return isRemoveWithLastDependent;
	}
	
EXPORT_C void RSisRegistryEntry::SetRemoveWithLastDependentL(TUid uid)
	{
	TPckg<TUid> uidPkg(uid);
	User::LeaveIfError(SendReceive(ESetRemoveWithLastDependent, TIpcArgs(&uidPkg)));
	}
	
EXPORT_C TBool RSisRegistryEntry::RemovableL()
	{	
	TBool isRemovable = ETrue;
	TPckg<TBool> packageIsRemovable(isRemovable);
	User::LeaveIfError(SendReceive(ENonRemovable, TIpcArgs(&packageIsRemovable)));
	return isRemovable;
	}
	
HBufC8* RSisRegistryEntry::SendReceiveBufferLC(TInt aMessage) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());
	
	TInt result=SendReceive(aMessage, TIpcArgs(&pOutput));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
	
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		
		result=SendReceive(aMessage, TIpcArgs(&pResizedOutput));
		}
	User::LeaveIfError(result);
	return output;
	}

HBufC8* RSisRegistryEntry::SendReceiveBufferLC(TInt aMessage, const TDesC& aInputDescriptor) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());
	
	TInt result=SendReceive(aMessage, TIpcArgs(&aInputDescriptor, &pOutput));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		
		result=SendReceive(aMessage, TIpcArgs(&aInputDescriptor, &pResizedOutput));
		}
	User::LeaveIfError(result);
	return output;
	}

EXPORT_C TBool RSisRegistryEntry::PreInstalledL()
	{
	TBool preInstalled = EFalse;
	TPckg<TBool> isPreInstalled(preInstalled);
	
	User::LeaveIfError(SendReceive(EPreInstalled, TIpcArgs(&isPreInstalled)));
	return preInstalled;
	}

EXPORT_C TBool RSisRegistryEntry::IsDeletablePreInstalledL()
	{
	TBool deletablePreInstalled = EFalse;
	TPckg<TBool> deletablePreInstalledPckg(deletablePreInstalled);
	
	User::LeaveIfError(SendReceive(EDeletablePreInstalled, TIpcArgs(&deletablePreInstalledPckg)));
	return deletablePreInstalled;
	}


EXPORT_C void RSisRegistryEntry::DependentPackagesL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	HBufC8* buffer = SendReceiveBufferLC(EDependentPackages);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPackages, stream);
	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
	}

EXPORT_C void RSisRegistryEntry::DependenciesL(RPointerArray<CSisRegistryDependency>& aDependencies)
	{
	HBufC8* buffer = SendReceiveBufferLC(EDependencies);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aDependencies, stream);
	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
	}

EXPORT_C void RSisRegistryEntry::EmbeddedPackagesL(RPointerArray<CSisRegistryPackage>& aPackages)
	{
	HBufC8* buffer = SendReceiveBufferLC(EEmbeddedPackages);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPackages, stream);
	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
	}

EXPORT_C void RSisRegistryEntry::EmbeddingPackagesL(RPointerArray<CSisRegistryPackage>& aEmbeddingPackages)
	{
	HBufC8* buffer = SendReceiveBufferLC(EEmbeddingPackages);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aEmbeddingPackages, stream);
	
	CleanupStack::PopAndDestroy(2, buffer); // buffer, stream
	}

EXPORT_C TBool RSisRegistryEntry::IsSignedBySuCertL()
	{
	TBool signedSuCert = EFalse;
	TPckg<TBool> signedSuCertPckg(signedSuCert);
	
	User::LeaveIfError(SendReceive(ESignedBySuCert, TIpcArgs(&signedSuCertPckg)));
	return signedSuCert;	
	}

EXPORT_C void RSisRegistryEntry::RegistryFilesL(RPointerArray<HBufC>& aRegistryFiles)
    {
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    HBufC8* outputBuffer = HBufC8::NewLC(KDefaultBufferSize);
	TPtr8 outputPtr(outputBuffer->Des());
    
    TInt result = SendReceive(ERegistryFiles, TIpcArgs(&outputPtr));
    
    if (KErrOverflow == result)
        {
        TInt lenNeeded;
        TPckg<TInt> lenNeededPckg(lenNeeded);
        lenNeededPckg.Copy(*outputBuffer);
        
        // Re-allocate output buffer
        CleanupStack::PopAndDestroy(outputBuffer);
        outputBuffer = HBufC8::NewLC(lenNeeded);
        TPtr8 newOutputPtr(outputBuffer->Des());
        
        result = SendReceive(ERegistryFiles, TIpcArgs(&newOutputPtr));
        }
    User::LeaveIfError(result);

    // Reading the list of files from the buffer
    RDesReadStream outputStream(*outputBuffer);
    CleanupClosePushL(outputStream);    
    InternalizePointerArrayL(aRegistryFiles, outputStream);
        
    CleanupStack::PopAndDestroy(2, outputBuffer); // outputStream  
#else
    // There is API is not currently supported with USIF
    User::Leave(KErrNotSupported);
	aRegistryFiles.Close(); // To remove the compiler warning: variable/argument 'aRegistryFiles' is not used in function
#endif
    }
