/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SISHelper is a layer between a client (EpocInstall, Installer UI, or Control
* Panel applet) that starts [un]installation.
*
*/


#include <s32mem.h>
#include <caf/caf.h>
#include <ecom/ecom.h>
#include <bautils.h>
#include <e32math.h> // for Math::Random()

#include "swi/launcher.h"
#include "sishelper.h"
#include "filesisdataprovider.h"
#include "log.h"
#include "swi/sisparser.h"
#include "swi/siscontents.h"
#include "securitypolicy.h"
#include "cafsisdataprovider.h"
#include "sisregistrypackage.h"
#include "sisregistrysession.h"
#include "sisregistryentry.h"


namespace Swi
{

// RMessagePtr2::Panic() also completes the message. This is 
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed the second time
void PanicClient(const RMessagePtr2& aMessage, TSisHelperPanic aPanic)
	{
	aMessage.Panic(KSisHelperServerName, aPanic);
	}

//
// Swi::CSisHelperShutdown helper class implementation
//

inline CSisHelperShutdown::CSisHelperShutdown() : CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

CSisHelperShutdown::~CSisHelperShutdown()
	{
	Deque();
	}

inline void CSisHelperShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CSisHelperShutdown::Start()
	{
	After(KShutdownDelay);
	}

// Initiate server exit when the timer expires
void CSisHelperShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

//
// CSisHelperSession
//

inline CSisHelperSession::CSisHelperSession(MSisDataProvider& aDataProvider)
:	iDataProvider(aDataProvider)
	{
	}

inline CSisHelperServer& CSisHelperSession::Server()
	{
	return *static_cast<CSisHelperServer*>(
		const_cast<CServer2*>(CSession2::Server()));
	}

inline TBool CSisHelperSession::ReceivePending() const
	{
	return !iReceiveMsg.IsNull();
	}

// Second phase constructor, called by CServer2 framework
void CSisHelperSession::CreateL()
	{
	Server().AddSession();
	}

CSisHelperSession::~CSisHelperSession()
	{
	delete iCurrentContents;
	// Ignore any leave codes from CleanupAsyncExtractionL -
	// they can't really be handled from the destructor.
	TRAP_IGNORE(CleanupAsyncExtractionL());

	Server().DropSession();
	CSecurityPolicy::ReleaseResource();
	}


// Handle a client request. Leaving is handled by 
// CSisHelperServer::ServiceError() which reports the error code to the client
void CSisHelperSession::ServiceL(const RMessage2& aMessage)
	{
	DEBUG_PRINTF2(_L8("Sis Helper - Servicing Message Type %d."), aMessage.Function());
	
	TInt err = KErrNone;
	switch (aMessage.Function())
		{
		case ESisHelperGetController:
			{
			TRAP(err, GetControllerL(aMessage));
			if (err != KErrNone)
				{
				// Map unexpected KErrOverflow errors to KErrCorrupt; otherwise,
				// sishelperclient will interpret this as a signal to resize the request
				// buffer
				aMessage.Complete((err == KErrOverflow ? KErrCorrupt : err));
				}
			break;
			}
			
		case ESisHelperExtractFile:
			{
			TRAP(err, ExtractFileL(aMessage));
			aMessage.Complete(err);
			break;
			}
			
		case ESisHelperFillDrivesAndSpaces:
			{
			// calculate the likely size of the data transfer buffer
			const TInt KMaxBufSize=
				sizeof(TInt)+                 // number of entries
				KMaxDrives*sizeof(TUint)+  // drive letters stored as TUints
				KMaxDrives*sizeof(TInt64); // drive free spaces
			
			// allocate buffer for the returned arrays
			HBufC8* buf=HBufC8::NewMaxLC(KMaxBufSize);
			
			// create stream on the buffer
			TPtr8 des=buf->Des();
			RDesWriteStream outs(des);
			CleanupClosePushL(outs);
			
			// get drive letter and free space information
			RArray<TChar> driveLetters;
			CleanupClosePushL(driveLetters);
			RArray<TInt64> driveSpaces;
			CleanupClosePushL(driveSpaces);
			FillDrivesAndSpacesL(driveLetters, driveSpaces);
			
			// externalise the arrays
			TInt count=driveLetters.Count();
			outs.WriteInt32L(count);
			TInt i;
			for (i = 0; i < count; ++i)
				{
				outs.WriteInt32L(static_cast<TUint>(driveLetters[i]));
				}
			
			for (i = 0; i < count; ++i)
				{
				outs.WriteInt32L(I64LOW(driveSpaces[i]));
				outs.WriteInt32L(I64HIGH(driveSpaces[i]));
				}
			
			// write the buffer to the passed buffer
			outs.CommitL();
			aMessage.WriteL(0, *buf);
			
			// cleanup
			// driveSpaces, driveLetters, outs, buf
			CleanupStack::PopAndDestroy(4, buf);
			aMessage.Complete(KErrNone);
			break;
			}

		case ESisHelperOpenDrmContent:
			{
			ContentAccess::TIntent intent
				= static_cast<ContentAccess::TIntent>(aMessage.Int0());
			
			TRAPD(err, iDataProvider.OpenDrmContentL(intent));
			aMessage.Complete(err);
			break;
			}
		
		case ESisHelperExecuteDrmIntent:
			{
			ContentAccess::TIntent intent
				= static_cast<ContentAccess::TIntent>(aMessage.Int0());
	
			aMessage.Complete(iDataProvider.ExecuteDrmIntent(intent));
			break;
			}
		
		case ESisHelperSetupAsyncExtraction:
			SetupAsyncExtractionL(aMessage);
			break;
			
		case ESisHelperAsyncExtraction:
			AsyncExtractionL(aMessage);
			break;
			
		case ESisHelperEndAsyncExtraction:
			EndAsyncExtractionL(aMessage);
			break;

		case ESisHelperCreateSisStub:
			{
			// write stub
			TRAPD(err, CreateSisStubL(aMessage));
			
			// cleanup
			aMessage.Complete(err);
			break;
			}
		case ESisHelperIsStub:
			{	
			TRAPD(err,IsStubL(aMessage));
			aMessage.Complete(err);
			break;
			}
		case ESisHelperGetSisFileDrive:
			{
			TRAPD(err,GetSisFileDriveL(aMessage));	
			aMessage.Complete(err);
			break;
			}
		case ESisHelperIsSisFileReadOnly:
			{
			TRAPD(err,IsSisFileReadOnlyL(aMessage));	
			aMessage.Complete(err);
			break;
			}
		case ESisHelperGetSisFileName:
			{
			TRAP(err, GetSisFileNameL(aMessage));
			aMessage.Complete(err);
			break;
			}			
		case ESisHelperGetControllerFromSis:
			{
			TRAP(err, GetControllerFromSisL(aMessage));
			aMessage.Complete(err);
			break;
			}
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		case ESisHelperIsDrmProtected:
			{
			TRAP(err, IsDrmProtectedL(aMessage));
			aMessage.Complete(err);
			break;
			}
		#endif
		case ESisHelperGetEquivalentLanguages:
			{
			TRAP(err, GetEquivalentLanguagesL(aMessage));
			aMessage.Complete(err);
			break;
			}	
		default:
			{
			DEBUG_PRINTF(_L8("Sis Helper - Illegal Message Type."));
			PanicClient(aMessage, EPanicIllegalFunction);
			break;
			}
		}
	}

void CSisHelperSession::GetControllerL(const RMessage2& aMessage)
	{
	// Extract raw controller data into a buffer
	HBufC8* controllerData = CurrentContentsL().ReadControllerL();
	CleanupStack::PushL(controllerData);

	// Get the data to the client using the standard overflow protocol
	// - get the size of the supplied descriptor (parameter 0)
	TInt size=aMessage.GetDesMaxLengthL(0);
	// - see if we have enough space on the client
	if (size < controllerData->Size())
		{
		// not enough space, complete with KErrOverflow and encode the
		// required size into the passed descriptor
		TPckgC<TInt> sizePckg(controllerData->Size());

		// is there enough space for an integer?
		if (size >= sizePckg.Size())
			{
			aMessage.WriteL(0, sizePckg);
			}
		// complete the message with KErrOverflow when resizing requires
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		// enough space, write the controller to the supplied buffer
		aMessage.WriteL(0, *controllerData);
		//Everything went fine so complete the message with KErrNone.
		aMessage.Complete(KErrNone);
		}
	CleanupStack::PopAndDestroy(controllerData); // controllerData				
	}

void CSisHelperSession::ExtractFileL(const RMessage2& aMessage)
	{
	RFile file;
	file.AdoptFromClient(aMessage, 0, 1);			
	CleanupClosePushL(file);
	TInt fileIndex = aMessage.Int2();
	TInt dataUnit = aMessage.Int3();

	CurrentContentsL().ReadDataL(file, fileIndex, dataUnit);
	User::LeaveIfError(file.Flush());
	CleanupStack::PopAndDestroy(&file); // file
	}

void CSisHelperSession::IsStubL(const RMessage2& aMessage)
	{
	TBool stub = CurrentContentsL().IsStub();	
	TPckg<TBool> stubPckg(stub);
	aMessage.WriteL(0, stubPckg);
	}

void CSisHelperSession::GetSisFileDriveL(const RMessage2& aMessage)
	{
	TChar drive;
	User::LeaveIfError(RFs::DriveToChar(Server().GetSisFileDrive(), drive));
	TPckg<TChar> drivePckg(drive);
	aMessage.WriteL(0, drivePckg);
	}

void CSisHelperSession::IsSisFileReadOnlyL(const RMessage2& aMessage)
	{
	TBool readOnly = Server().IsSisFileReadOnly();
	TPckg<TBool> readOnlyPckg(readOnly);
	aMessage.WriteL(0, readOnlyPckg);
	}

void CSisHelperSession::GetSisFileNameL(const RMessage2& aMessage)
	{
	TInt clientDesLength = aMessage.GetDesMaxLengthL(0);
	TPtrC filename = Server().GetSisFileNameL();
	if (clientDesLength < filename.Length())
		{
		User::Leave(KErrOverflow);
		}
	if (filename.Length() == 0)
		{
		User::Leave(KErrNotFound);
		}
	aMessage.WriteL(0, filename);
	}

void CSisHelperSession::GetControllerFromSisL(const RMessage2& aMessage)
	{
	// Get the file handle
	RFile file;
	// Adopt the file using the RFs handle from message slot 1 and the RFile handle from slot 2
	User::LeaveIfError(file.AdoptFromClient(aMessage, 1, 2));          
	CleanupClosePushL(file);

	// Create a file data provider
	MSisDataProvider* dataProvider = NULL;
	CSecurityPolicy* securityPolicy = CSecurityPolicy::GetSecurityPolicyL();
	TBool drmEnabled = securityPolicy->DrmEnabled();
	securityPolicy->ReleaseResource();
	if (drmEnabled)
		{
		dataProvider = CCafSisDataProvider::NewLC(file);
		dataProvider->OpenDrmContentL(ContentAccess::EInstall);
		User::LeaveIfError(dataProvider->ExecuteDrmIntent(ContentAccess::EUnknown));
		}
	else
		{
		dataProvider = CFileSisDataProvider::NewLC(file);
		}

	// Extract raw controller data into a buffer
	TInt64 pos(0);
	User::LeaveIfError(dataProvider->Seek(ESeekStart, pos));
	Sis::CContents *contents = Sis::Parser::ContentsL(*dataProvider);
	CleanupStack::PushL(contents);
	HBufC8* controllerData = NULL;
	TRAPD(err, controllerData = contents->ReadControllerL());
	if(err == KErrOverflow)
   		{
   		User::Leave(KErrCorrupt);			
   		}
	User::LeaveIfError(err);
	CleanupStack::PushL(controllerData);

	// Get the data to the client using the standard overflow protocol
	// - get the size of the supplied descriptor (parameter 0)
	TInt size = aMessage.GetDesMaxLengthL(0);
	// - see if we have enough space on the client
	if (size < controllerData->Size())
		{
		// Not enough space, complete with KErrOverflow and encode the
		// required size into the passed descriptor
		TPckgC<TInt> sizePckg(controllerData->Size());

		// Is there enough space for an integer?
		if (size >= sizePckg.Size())
			{
			aMessage.WriteL(0, sizePckg);
			}
		// complete the message with KErrOverflow when resizing requires
		User::Leave(KErrOverflow);
		}
	else
		{
		// Enough space, write the controller to the supplied buffer
		aMessage.WriteL(0, *controllerData);
		}
	CleanupStack::PopAndDestroy(4, &file); // dataProvider, contents, controllerData
	}


	// Functions for asynchronous extraction
void CSisHelperSession::SetupAsyncExtractionL(const RMessage2& aMessage)
	{
	// We only support extracting one file at once currently
	if (iInAsyncExtraction)
		{
		aMessage.Complete(KErrInUse);
		}
	else
		{
		// Get file and session handles
		User::LeaveIfError(iAsyncFs.Open(aMessage, 0));

		// Adopt the filehandle;
		User::LeaveIfError(iAsyncFile.Adopt(iAsyncFs, aMessage.Int1()));
	
		iAsyncFileIndex=aMessage.Int2();
		iAsyncDataUnit=aMessage.Int3();
	
		iInAsyncExtraction=ETrue;
		aMessage.Complete(KErrNone);
		}
	}

void CSisHelperSession::AsyncExtractionL(const RMessage2& aMessage)
	{
	TInt lengthHigh=aMessage.Int0();
	TInt lengthLow=aMessage.Int1();
	TInt64 length= lengthHigh;
	length=( length << 32) + lengthLow;
				
	CurrentContentsL().ReadDataL(iAsyncFile, iAsyncFileIndex, iAsyncDataUnit, length);
	TInt err = iAsyncFile.Flush();

	aMessage.Complete(err);
	}

void CSisHelperSession::EndAsyncExtractionL(const RMessage2& aMessage)
	{
	if (!iInAsyncExtraction)
		{
		aMessage.Complete(KErrArgument);
		}
	else
		{
		CleanupAsyncExtractionL();
		aMessage.Complete(KErrNone);
		}
	}

void CSisHelperSession::CleanupAsyncExtractionL()
	{
	if (iInAsyncExtraction)
		{
		iAsyncFile.Close();
		iAsyncFs.Close();
		iInAsyncExtraction=EFalse;
		}
	}

// Handle an error from CSisHelperSession::ServiceL(). A bad descriptor 
// error implies a badly programmed client, so panic it; otherwise use the
// default handling (report error to the client)
void CSisHelperSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		PanicClient(aMessage, EPanicBadDescriptor);
		}
	CSession2::ServiceError(aMessage, aError);
	}

void CSisHelperSession::FillDrivesAndSpacesL(RArray<TChar>& aDriveLetters, 
					                        RArray<TInt64>& aDriveSpaces)
	{
	// This is the LFSS free space threshold
	TInt freeSpaceAdjustment = 1024 * 128;    // Bytes	

    // get information about drives
    TDriveList driveList;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    // List all drives in the system
    User::LeaveIfError(fs.DriveList(driveList));

	// Check all drives
    for (TInt driveNumber=EDriveA; driveNumber<=EDriveZ; driveNumber++)
        {
        if (!driveList[driveNumber])
 			{
 			// Not a recognised drive
            continue;
			}
			
    	TVolumeInfo volInfo;
        if (fs.Volume(volInfo, driveNumber) != KErrNone)
			{
			// The volume is not usable (e.g. no media card inserted)
            continue;
			}

        if (volInfo.iDrive.iType==EMediaNotPresent || 
            volInfo.iDrive.iType==EMediaRom || 
            volInfo.iDrive.iType==EMediaRemote)
			{
			// Exclude drives not suitable for installation
            continue;
			}
			
        // Do not list read only and substituted drives as an option to install to
        if (volInfo.iDrive.iDriveAtt & KDriveAttRom || 
         	volInfo.iDrive.iDriveAtt & KDriveAttSubsted) 
			{
            continue;
			}

        TInt64 volSpace = volInfo.iFree - freeSpaceAdjustment;  // bytes
        if (volSpace < 0)
			{
            volSpace = 0;
			}
			
		TChar aDrive;
		User::LeaveIfError(fs.DriveToChar(driveNumber, aDrive));
		User::LeaveIfError(aDriveLetters.Append(TChar(aDrive)));
        User::LeaveIfError(aDriveSpaces.Append(volSpace));
        }
    CleanupStack::PopAndDestroy(&fs);
	}


void CSisHelperSession::CreateSisStubL(const RMessage2& aMessage)
	{
	RFile file;
	file.AdoptFromClient(aMessage, 0, 1);			
	CleanupClosePushL(file);
		
	// rewind the data provider
	TInt64 pos(0);
	User::LeaveIfError(iDataProvider.Seek(ESeekStart, pos));
	
	Sis::Parser::CreateSisStubL(file, iDataProvider);	
	CleanupStack::PopAndDestroy(&file);  //close
	}

//
// CSisHelperServer implementation
//

// All functions can only be accessed by SWIS
const TInt CSisHelperServer::iRanges[iRangeCount] = 
                {
                0 // All connect attempts
                };

const TUint8 CSisHelperServer::iElementsIndex[iRangeCount] = 
                {
                0 //Only SWIS can use sishelper. This is policed by the SID of SWIS
                };

const CPolicyServer::TPolicyElement CSisHelperServer::iPolicyElements[2] = 
                {
                {_INIT_SECURITY_POLICY_S0(0x101F7295), //SWIS' SID = 0x101F7295
                	CPolicyServer::EFailClient},		
                
                              
   
                };

const CPolicyServer::TPolicy CSisHelperServer::iPolicy =
                {
                0,					//specifies all connect attempts need SWIS' SID
                iRangeCount,
                iRanges,
                iElementsIndex,
                iPolicyElements,
                };

// Construction, the result is on the cleanup stack
EXPORT_C CSisHelperServer* CSisHelperServer::NewLC(TSisHelperStartParams& aParams)
	{
	CSisHelperServer* self=new(ELeave) CSisHelperServer;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}

// Construction
EXPORT_C CSisHelperServer* CSisHelperServer::NewL(TSisHelperStartParams& aParams)
	{
	CSisHelperServer* self=NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// 2nd phase constructor; ensure the timer and server objects are running
void CSisHelperServer::ConstructL(TSisHelperStartParams& aParams)
	{
	// See what kind of startup information is passed to the server
	switch (aParams.iType)
		{
		case TSisHelperStartParams::ETypeFileName:
			{
			// Create a file data provider
			User::LeaveIfError(iFs.Connect());
			iSessionConnected=ETrue;

			CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();
			
			_LIT(KDriveSeparator, ":\\");
			TPtrC sisFileName(*aParams.iFileName);	
			if (sisFileName.Find(KDriveSeparator) != 1)
				{
				// The full path (inc. drive letter) has not been specified
				TEntry* fileExits = new(ELeave) TEntry;
				CleanupStack::PushL(fileExits);
				TInt err = iFs.Entry(sisFileName, *fileExits);
				if (err == KErrNone)
					{
					// Found it, must be /xxxxx.sis. Fill in the missing
					// drive path
					TParse* sisFileWithDrv = new(ELeave) TParse;
					CleanupStack::PushL(sisFileWithDrv);
					iFs.Parse(sisFileName, *sisFileWithDrv);
					// Get the SIS file name
					iSisFileName = sisFileWithDrv->FullName().AllocL();
					User::LeaveIfError(RFs::CharToDrive(sisFileWithDrv->Drive()[0], iSisFileDrive));
					CleanupStack::PopAndDestroy(sisFileWithDrv);				
					}
				CleanupStack::PopAndDestroy(fileExits);		
				}
			else
				{
				// figure out which drive this SIS file came from
				TParsePtrC parser(sisFileName);
				User::LeaveIfError(RFs::CharToDrive(parser.Drive()[0], iSisFileDrive));
				// Get the SIS file name
				iSisFileName = sisFileName.AllocL();
				}
			
			if (securityPolicy->DrmEnabled())
				{
				iDataProvider=CCafSisDataProvider::NewL(sisFileName);
				}
			else
				{
				iDataProvider=CFileSisDataProvider::NewL(iFs, sisFileName);
				}	

			iDeleteDataProvider=ETrue;

			// Figure out whether the sis file is read only or the drive is
			// write-protected or ROM.
			TDriveInfo driveInfo;
			User::LeaveIfError(iFs.Drive(driveInfo,iSisFileDrive));
			if (!((driveInfo.iMediaAtt & KMediaAttWriteProtected) ||
				(driveInfo.iDriveAtt & KDriveAttRom)))
				{
				TEntry entry;
				User::LeaveIfError(iFs.Entry(sisFileName, entry));
				iSisFileReadOnly = entry.IsReadOnly();
				}
			}
			break;

		case TSisHelperStartParams::ETypeFileHandle:
			{
			CSecurityPolicy* securityPolicy=CSecurityPolicy::GetSecurityPolicyL();

			if (securityPolicy->DrmEnabled())
				{
				iDataProvider=CCafSisDataProvider::NewL(*aParams.iFileHandle);
				}
			else
				{
				iDataProvider=CFileSisDataProvider::NewL(*aParams.iFileHandle);
				}	

			iDeleteDataProvider=ETrue;
			
			// figure out which drive this SIS file came from
			TDriveInfo driveInfo;
			User::LeaveIfError(aParams.iFileHandle->Drive(iSisFileDrive, driveInfo));

			// Figure out whether the sis file is read only or the drive is
			// write-protected or ROM.
			if (!((driveInfo.iMediaAtt & KMediaAttWriteProtected) ||
				(driveInfo.iDriveAtt & KDriveAttRom)))
				{
				TUint attributes;
				User::LeaveIfError(aParams.iFileHandle->Att(attributes));
				iSisFileReadOnly = (attributes & KEntryAttReadOnly) ? ETrue : EFalse;
				}

			// Get SIS file name
			TFileName tempFileName;
			User::LeaveIfError(aParams.iFileHandle->FullName(tempFileName));
			iSisFileName = tempFileName.AllocL();
			DEBUG_PRINTF2(_L("Sis Helper - processing %S by file handle"), iSisFileName);
			}			
			break;			
		case TSisHelperStartParams::ETypeDataProvider:
			// store the pointer, assuming ownership, check for NULL
			iDataProvider=aParams.iDataProvider;
			iDeleteDataProvider=EFalse;
			if (iDataProvider==NULL)
				{
				PanicClient(Message(), EPanicBadDataProvider);
				}
			break;
		case TSisHelperStartParams::ETypeNull:
			// For use where file is passed in later along with request
			break;

		default:
			// panic the client
			PanicClient(Message(), EPanicBadStartupDataType);
			break;
		}
		
	StartL(KSisHelperServerName);
	iShutdown=new(ELeave) CSisHelperShutdown;
	iShutdown->ConstructL();
	
	// ensure that the server still exists even if the 1st client fails 
	// to connect; the server will be terminated if no one connects to it
	iShutdown->Start();
	}

CSisHelperServer::CSisHelperServer()
:	CPolicyServer(CActive::EPriorityStandard, iPolicy, ESharableSessions),
	iSisFileDrive(-1),
	iSisFileReadOnly(ETrue)
	{
	}

// Destruction; delete a data provider since we own it whether it was 
// transferred from outside or created in the server.
CSisHelperServer::~CSisHelperServer()
	{
	delete iShutdown;
	
	if (iDeleteDataProvider)
		{
		delete iDataProvider;
		}

	if (iSessionConnected)
		{
		iFs.Close();
		}

	if (iSisFileName)
		{
		delete iSisFileName;
		}
	
	// Free ECOM memory reserved in this thread
	// for example, plugins loaded by the CAF framework	
	REComSession::FinalClose();


		
	}

// Create a new client session
CSession2* CSisHelperServer::NewSessionL(const TVersion&,
	const RMessage2& ) const
	{
	
	return new(ELeave) CSisHelperSession(*iDataProvider);
	}
	
// A new session is being created; cancel the shutdown timer if it was running
void CSisHelperServer::AddSession()
	{
	++iSessionCount;
	DEBUG_PRINTF2(_L8("Sis Helper - Adding Session (%d active.)"), iSessionCount);
	// stop shutdown object once the first (and only) session is connected
	delete iShutdown;
	iShutdown=NULL;
	}

// A session is being destroyed; terminate server immediately
void CSisHelperServer::DropSession()
	{
	if (--iSessionCount==0)
		{
		CActiveScheduler::Stop();
		}
	DEBUG_PRINTF2(_L8("Sis Helper - Dropping Session (%d active.)"), iSessionCount);
	}

CSisRegistryPackage* CSisHelperServer::MainPackageEntryL(TUid aUid)
	{
	RSisRegistrySession registrySession;
	User::LeaveIfError(registrySession.Connect());
	CleanupClosePushL(registrySession);

	RSisRegistryEntry registryEntry;
	User::LeaveIfError(registryEntry.Open(registrySession, aUid));
	CleanupClosePushL(registryEntry);

    // Get the package object associated with the entry
    CSisRegistryPackage* package = registryEntry.PackageL();

    CleanupStack::PopAndDestroy(2, &registrySession);
	return package;
	}

//
// Starts SISHelper in a new thread
//
TInt CSisHelperServer::StartSisHelper(TSisHelperStartParams& aParams, RThread& aServer)
	{
	// We might be trying to restart a server, in which case we could
	// get a thread name clash. We could make the name unique/random,
	// but the server name would still clash...

	const TInt KSisHelperServerStackSize=0x2000;
	TInt err = KErrNone;
		
	for (TInt retry=0; retry < 2; ++retry)
		{
		err = aServer.Create(KSisHelperServerName, SisHelperThreadFunction, 
					   		KSisHelperServerStackSize, NULL, static_cast<TAny*>(&aParams), 
					   		EOwnerThread);
					   		
		if (err == KErrAlreadyExists || err == KErrInUse)
			{
			  User::After(80000);
			}
		else
			{
			break;
			}
		}

	if (err==KErrAlreadyExists)
		{
		return KErrServerBusy;
		}

	if (err!=KErrNone)
		{
		return err;
		}
	
	// The following code is the same whether the server runs in a new thread 
	// or process
	TRequestStatus stat;
	aServer.Rendezvous(stat);
	if (stat!=KRequestPending)
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
	err=(aServer.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	//We don't close aServer here. If asynchlauncher called, it will be closed
	//in the RunL() method of uissclienthandler to make sure that SISHelper has been 
	//disconnected by SWISInstaller. Otherwise, just after this function call.
	return err;
	}

void CSisHelperServer::Abort()
	{
	TFullName fullName = RProcess().FullName();
	fullName.Append(':');
	fullName.Append(':');
	fullName.Append(KSisHelperServerName);

	RThread server;
	TInt err = server.Open(fullName);
	if (err == KErrNone)
		{
		server.Terminate(KErrAbort);
		server.Close();
		}
	}

// Entry point for the thread the SISHelper runs in
TInt CSisHelperServer::SisHelperThreadFunction(TAny *aPtr)
	{
	if (aPtr==NULL)
		{
		return KErrArgument;
		}
		
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	TSisHelperStartParams* params=
	static_cast<TSisHelperStartParams*>(aPtr);

	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	if (!scheduler)
		{
		return KErrNoMemory;
		}
		
	DEBUG_PRINTF(_L8("Sis Helper - Starting Server"));

	CActiveScheduler::Install(scheduler);
	CSisHelperServer* server=NULL;
	
	TRAPD(err, server=CSisHelperServer::NewL(*params));

	if (err==KErrNone)
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

	CSecurityPolicy::ReleaseResource();
	
	return err;
	}

TInt CSisHelperServer::GetSisFileDrive()
	{
	return iSisFileDrive;
	}

TBool CSisHelperServer::IsSisFileReadOnly()
	{
	return iSisFileReadOnly;
	}

const TDesC& CSisHelperServer::GetSisFileNameL()
	{
	if (!iSisFileName)
		{
		User::Leave(KErrNotFound);
		}
	return *iSisFileName;
	}

Sis::CContents& CSisHelperSession::CurrentContentsL()
	{
	if(iCurrentContents == NULL)
		{
		TInt64 pos(0);
		User::LeaveIfError(iDataProvider.Seek(ESeekStart, pos));
		iCurrentContents=Sis::Parser::ContentsL(iDataProvider);
		}
	return *iCurrentContents;
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CSisHelperSession::IsDrmProtectedL(const RMessage2& aMessage)
	{
	// Data provider will be of CCafSisDataProvider only when DRM is enabled in SWI policy.
	// So, check the policy before querying content protection status from the data provider.
	CSecurityPolicy* securityPolicy = CSecurityPolicy::GetSecurityPolicyL();
	if (securityPolicy->DrmEnabled())
		{
		CCafSisDataProvider* cafDataProvider = static_cast<CCafSisDataProvider*>(&iDataProvider);
 		TBool isProtected = cafDataProvider->IsContentProtected();	
   		TPckgBuf<TBool> isContentProtected(isProtected);
   		aMessage.WriteL(0, isContentProtected);
   		securityPolicy->ReleaseResource();
		}
	// If DRM is NOT enabled in SWI policy, no way to query the protection status. So, set it to Fasle.
	else
		{
		aMessage.WriteL(0, TPckgBuf<TBool>(EFalse));
		}
	}
#endif

void CSisHelperSession::GetEquivalentLanguagesL(const RMessage2& aMessage)
	{
	TLanguage srcLangID = (TLanguage)aMessage.Int1();
	
	TLanguagePath equivalentLangs;
	BaflUtils::GetEquivalentLanguageList(srcLangID,equivalentLangs);
	
	// calculate the likely size of the data transfer buffer
	const TInt KMaxBufSize=
		sizeof(TInt)+                 // number of entries
		(KMaxDowngradeLanguages+1)*sizeof(TLanguage);  // Languages IDs stored as TLanguage
	
	// allocate buffer for the array
	HBufC8* buf=HBufC8::NewMaxLC(KMaxBufSize);
	
	TInt size = 1;
	while (equivalentLangs[size++] != ELangNone){}
		
	TPtr8 pBuf=buf->Des();
	RDesWriteStream outs(pBuf);
	CleanupClosePushL(outs);
	
	// Since the size contains the source language ID and ELangNone, the actual size is equal to size - 2
	outs.WriteInt32L(size-2);
	TInt i;
	for (i = 1; i < size-1; ++i)
		{
		outs.WriteInt32L(static_cast<TInt>(equivalentLangs[i]));
		}
	
	outs.CommitL();
	aMessage.WriteL(0, *buf);
	
	CleanupStack::PopAndDestroy(2,buf); //outs,buf
	}


} // namespace Swi
