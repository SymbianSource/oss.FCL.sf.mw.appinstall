/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <f32file.h>
#include <e32property.h>
#include <swi/pkgremovererrors.h>
#include <swi/swispubsubdefs.h>
#include "sishelper.h"
#include "log.h"
#include "installclientserver.h"
#include <swi/pkgremover.h>
#include <connect/sbdefs.h>
#include "cleanuputils.h"

// Maximum buffer size
const TInt KMaxBufferSize = 1024;

namespace Swi
{

// Forward declarations
TInt StartSisHelper(RThread& aServer);
TInt SisHelperThreadFunction(TAny* );
void StopSisHelper(TAny* aServer);
TBool IsSWISBusy();
TInt StartSwi();
TInt FilterErrors(TInt aError);

/*static*/
EXPORT_C void UninstalledSisPackages::ListL(TDriveNumber aDrive, RPointerArray<CUninstalledPackageEntry>& aPackageList)
	{
	CleanupResetAndDestroyPushL(aPackageList);
	aPackageList.ResetAndDestroy();

	// Check if SWIS is busy
	if (IsSWISBusy())
		{
		User::Leave(KErrServerBusy);
		}

	RThread server;

	// Ensure SisHelper is stopped in the event of a leave
	CleanupStack::PushL(TCleanupItem(StopSisHelper, &server));
	User::LeaveIfError(StartSisHelper(server));

	// Connect to SWIS and issue request
	RUninstalledPkgsSession uninstalledPkgSession;
	User::LeaveIfError(uninstalledPkgSession.Connect());
	CleanupClosePushL(uninstalledPkgSession);

	HBufC8* output = HBufC8::NewLC(KMaxBufferSize);
	
	TPtr8 pOutput(output->Des());
	TInt result = uninstalledPkgSession.RequestUnInstalledPkgsList(TIpcArgs(aDrive, &pOutput));

	if (result == KErrOverflow)
		{
		TInt sizeNeeded = 0;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);

		// Shutdown 
		CleanupStack::PopAndDestroy(3, &server); //InstallServer , SisHelper and output

		// Check if SWIS is busy
		if (IsSWISBusy())
			{
			User::Leave(KErrServerBusy);
			}

		// Restart sishelper and ReConnect to SWIS to issue request
		// Ensure SisHelper is stopped in the event of a leave
		CleanupStack::PushL(TCleanupItem(StopSisHelper, &server));
		User::LeaveIfError(StartSisHelper(server));
		User::LeaveIfError(uninstalledPkgSession.Connect());
		CleanupClosePushL(uninstalledPkgSession);

		// Re-allocate buffer
		output = HBufC8::NewLC(sizeNeeded);
		pOutput.Set(output->Des());
	
		result = uninstalledPkgSession.RequestUnInstalledPkgsList(TIpcArgs(aDrive, &pOutput));
		}
	User::LeaveIfError(FilterErrors(result));

	// Internalise the pointer array  
	RDesReadStream readStream(pOutput);
	CleanupClosePushL(readStream);

	// read in the attribute values from the buffer
	TInt count = readStream.ReadInt32L();
	for (TInt i = 0; i < count; i++)
		{
		TInt uidValue = readStream.ReadInt32L();
		TUid uid = TUid::Uid(uidValue);
		
		// Read the package name
		TInt size = readStream.ReadInt32L();
		HBufC* pName = HBufC::NewLC(size);
		TPtr pPtr(pName->Des());
		readStream.ReadL(pPtr, size);
					
		// Read the vendor name
		size = readStream.ReadInt32L();
		HBufC* vName = HBufC::NewLC(size);
		TPtr vPtr(vName->Des());
		readStream.ReadL(vPtr, size);
		
		TInt major = readStream.ReadInt32L();
		TInt minor = readStream.ReadInt32L();
		TInt build = readStream.ReadInt32L();
		TVersion version(major,minor,build);
		
		// Read the package type
		CUninstalledPackageEntry::TPackageType pkgType = CUninstalledPackageEntry::TPackageType(readStream.ReadInt32L());

		// Read the file name
		size = readStream.ReadInt32L();
		HBufC* fName = HBufC::NewLC(size);
		TPtr fPtr(fName->Des());
		readStream.ReadL(fPtr, size);

		// Read the associated stub file 
		size = readStream.ReadInt32L();
		HBufC* aName = HBufC::NewLC(size);
		TPtr aPtr(aName->Des());
		readStream.ReadL(aPtr, size);
		
		CUninstalledPackageEntry* PkgEntry = CUninstalledPackageEntry::NewLC(uid,
		*pName, *vName, version, pkgType, *fName, *aName);
		aPackageList.AppendL(PkgEntry);
		CleanupStack::Pop(PkgEntry); 			// PkgEntry
		CleanupStack::PopAndDestroy(4, pName); 	// aName, fName, vName, pName
		}
	CleanupStack::PopAndDestroy(2, output); 	// readStream, output

	// Shutdown InstallServer and SisHelper
	CleanupStack::PopAndDestroy(2, &server);
	CleanupStack::Pop(&aPackageList);
	}

/*static*/
EXPORT_C void UninstalledSisPackages::RemoveL(const CUninstalledPackageEntry& aPackage)
	{
	// Check if SWIS is busy
	if (IsSWISBusy())
		{
		User::Leave(KErrServerBusy);
		}

	// Get the filename
	RThread server;
	// Ensure SisHelper is stopped in the event of a leave
	CleanupStack::PushL(TCleanupItem(StopSisHelper, &server));
	User::LeaveIfError(StartSisHelper(server));

	// Connect to SWIS and issue request
	RUninstalledPkgsSession uninstalledPkgSession;
	User::LeaveIfError(uninstalledPkgSession.Connect());
	CleanupClosePushL(uninstalledPkgSession);
	TIpcArgs args;
	args.Set(0, &aPackage.PackageFile());
	args.Set(1, &aPackage.AssociatedStubSisFile());
	User::LeaveIfError(FilterErrors(uninstalledPkgSession.RemoveUnInstalledPkg(args)));

	// Shutdown InstallServer and SisHelper
	CleanupStack::PopAndDestroy(2, &server);
	}

CUninstalledPackageEntry* CUninstalledPackageEntry::NewLC(const TUid& aUid, const TDesC& aPackageName,
		const TDesC& aVendorName, const TVersion& aVersion, const TPackageType aPackageType,
		const TDesC& aPackageFile, const TDesC& aAssocStubSisFile)
 	{
 	CUninstalledPackageEntry* self = new(ELeave) CUninstalledPackageEntry();
 	CleanupStack::PushL(self);
 	self->ConstructL(aUid, aPackageName, aVendorName, aVersion, aPackageType, aPackageFile, aAssocStubSisFile);
 	return self;
 	}

CUninstalledPackageEntry::CUninstalledPackageEntry()
	{
	}

CUninstalledPackageEntry::~CUninstalledPackageEntry()
	{
	delete iVendorName;
	delete iPackageName;
	delete iPackageFile;
	delete iAssocStubSisFile;
	}

// Constructs the objects using the property values
void CUninstalledPackageEntry::ConstructL(const TUid& aUid, const TDesC& aPackageName,
		const TDesC& aVendorName, const TVersion& aVersion, const TPackageType aPackageType, 
		const TDesC& aPackageFile, const TDesC& aAssocStubSisFile)
	{
	iUid = aUid;
	iPackageName = aPackageName.AllocL();
	iVendorName = aVendorName.AllocL();
	iVersion = aVersion;
	iType = aPackageType;
	iPackageFile = aPackageFile.AllocL();
	iAssocStubSisFile = aAssocStubSisFile.AllocL();	
	}

// Get methods
EXPORT_C const TUid& CUninstalledPackageEntry::Uid() const
	{
	return iUid;
	}

EXPORT_C const TDesC& CUninstalledPackageEntry::Name() const
	{
	return *iPackageName;
	}

EXPORT_C const TDesC& CUninstalledPackageEntry::Vendor() const
	{
	return *iVendorName;
	}

EXPORT_C const TVersion& CUninstalledPackageEntry::Version() const
	{
	return iVersion;
	}

EXPORT_C const CUninstalledPackageEntry::TPackageType& CUninstalledPackageEntry::PackageType() const
	{
	return iType;
	}

const TDesC& CUninstalledPackageEntry::PackageFile() const
	{
	return *iPackageFile;
	}

const TDesC& CUninstalledPackageEntry::AssociatedStubSisFile() const
	{
	return *iAssocStubSisFile;
	}

// Client-side session implementation
TInt RUninstalledPkgsSession::Connect()
	{
	TInt retry = 2;
	for(;;)
		{
		TInt err = CreateSession(Swi::KInstallServerName, 
		    TVersion(KInstallServerVersionMajor, KInstallServerVersionMinor, KInstallServerVersionBuild));
		if (err != KErrNotFound && err != KErrServerTerminated)
			{
			return err;
			}
		if (--retry == 0)
			{
			return err;
			}
		err = StartSwi();
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			return err;
			}
		}
	}

void RUninstalledPkgsSession::Close()
	{
	RSessionBase::Close();
	}

TInt RUninstalledPkgsSession::RequestUnInstalledPkgsList(const TIpcArgs& aArgs)
	{
	return SendReceive(Swi::EListUnInstalledPkgs, aArgs);
	}

TInt RUninstalledPkgsSession::RemoveUnInstalledPkg(const TIpcArgs& aArgs)
	{
	return SendReceive(Swi::ERemoveUnInstalledPkg, aArgs);
	}

// Starts SISHelper in a new thread
TInt StartSisHelper(RThread& aServer)
	{
	// We might be trying to restart a server, in which case we could
	// get a thread name clash. We could make the name unique/random,
	// but the server name would still clash...

	const TInt KSisHelperServerStackSize = 0x2000;
	TInt err = KErrNone;
	for (TInt retry = 0; retry < 2; ++retry)
		{
		err = aServer.Create(KSisHelperServerName, SisHelperThreadFunction, 
					   		KSisHelperServerStackSize, NULL, NULL, EOwnerThread);

		if (err == KErrAlreadyExists || err == KErrInUse)
			{
			User::After(80000);
			}
		else
			{
			break;
			}
		}

	if ((err == KErrAlreadyExists) || (err == KErrInUse))
		{
		return KErrServerBusy;
		}

	if (err != KErrNone)
		{
		return err;
		}

	// The following code is the same whether the server runs in a new thread 
	// or process
	TRequestStatus stat;
	aServer.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		aServer.Kill(0); // abort startup
		}
	else
		{
		aServer.Resume(); // logon OK, start the server
		}

	User::WaitForRequest(stat); // wait for start or death

	// we can't use the 'exit reason' if the server panicked as this is the 
	// panic 'reason' and may be 0 which cannot be distinguished from KErrNone
	err = (aServer.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	//We don't close aServer here. If asynchlauncher called, it will be closed
	//in the RunL() method of uissclienthandler to make sure that SISHelper has been 
	//disconnected by SWISInstaller. Otherwise, just after this function call.
	return err;
	}

// Stops SISHelper
void StopSisHelper(TAny* aServer)
	{
	RThread* rServer = static_cast<RThread*> (aServer);
	if(rServer->Handle() > 0)
		{
		TRequestStatus reqStatus;
		rServer->Logon(reqStatus);
		User::WaitForRequest(reqStatus);
		rServer->Close();
		}
	}

// Entry point for the thread the SISHelper runs in
TInt SisHelperThreadFunction(TAny* )
	{
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	if (!cleanup)
		{
		return KErrNoMemory;
		}
		
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (!scheduler)
		{
		delete cleanup;
		return KErrNoMemory;
		}

	DEBUG_PRINTF(_L8("Sis Helper - Starting Server"));

	CActiveScheduler::Install(scheduler);

	TSisHelperStartParams params;
	CSisHelperServer* server = static_cast<CSisHelperServer*>(NULL);
	TRAPD(err, server = CSisHelperServer::NewL(params));

	if (err == KErrNone)
		{
		// only continue launching the server if no error
		RThread::Rendezvous(KErrNone);
		scheduler->Start();
		}

	DEBUG_PRINTF(_L8("Sis Helper - Stopping Server"));

	CActiveScheduler::Install(NULL);
	delete server;
	delete scheduler;
	delete cleanup; // destroy clean-up stack

	return err;
	}

// Starts SWIS in a new process
TInt StartSwi()
	{
	const TUidType serverUid(KNullUid, KNullUid, Swi::KInstallServerUid3);
	RProcess server;

	TInt err = server.Create(Swi::KInstallServerImage, KNullDesC, serverUid);
	if (err != KErrNone)
		{
		return err;
		}
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		{
		server.Kill(0); // abort startup
		}
	else
		{
		server.Resume(); // logon OK, start the server
		}

	User::WaitForRequest(stat); // wait for start or death

	// We can't use the 'exit reason' if the server panicked as this is the
	// panic 'reason' and may be 0 which cannot be distinguished from KErrNone.
	err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return err;
	}

TBool IsSWISBusy()
	{
	// Check for Install/Uninstall operation
	TInt property = 0;
	TInt err = RProperty::Get(KUidSystemCategory, KUidSoftwareInstallKey, property);
	if ((err != KErrNotFound) && ((property & KSwisOperationMask) != ESwisNone))
		{
		return ETrue;
		}
	return EFalse;
	}

// Filter the error codes returned by the List and Remove API
TInt FilterErrors(TInt aError)
	{
	TInt outError = aError;
	// If the error is in SWI range (publishedPartner) then return a publishedAll equivalent
	// for some of them
	if (aError > KErrNotRemovable && aError <= KErrSISFieldIdMissing)	// Range is -10280 to -10100
		{
		outError = KErrPackageFileCorrupt;
		}

	return outError;
	}

} // namespace Swi
