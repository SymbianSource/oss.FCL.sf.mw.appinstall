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
* SisLauncher - client SisLaunchers interface implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include<s32mem.h>
 
#include "sislauncherclient.h"
#include "sislauncherclientserver.h"
#include "sisregistryfiledescription.h"
#include "secutils.h"
#include "arrayutils.h"  // from source/sisregistry/common/ 
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "swtypereginfo.h"
#include "ipcutil.h"
#endif

using namespace Swi;

static TInt StartSisLauncherServer()
    {
    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
    RProcess server;
    TInt err = server.Create(KSisLauncherServerImg, KNullDesC, serverUid);
    if (err != KErrNone)
        {
        return err;
        }
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat != KRequestPending)
        {
        server.Kill(0);     // abort startup
        }
    else
        {
        server.Resume();    // logon OK - start the server
        }
    User::WaitForRequest(stat);     // wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
    server.Close();
    return err;
    }

EXPORT_C TInt RSisLauncherSession::Connect()
//
// Connect to the server, attempting to start it if necessary
//
    {    
    TInt retry=2;
    for (;;)
        {
        TInt err = CreateSession(KSisLauncherServerName, TVersion(0, 0, 0), 1);
        if (err != KErrNotFound && err != KErrServerTerminated)
            {
            return err;
            }
        if (--retry==0)
            {
            return err;
            }
        err = StartSisLauncherServer();
        if (err != KErrNone && err != KErrAlreadyExists)
            {
            return err;
            }
        }    
    }
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisLauncherSession::Close()
    {	
	RSessionBase::Close();
    }
#endif
EXPORT_C void RSisLauncherSession::RunExecutableL(const TDesC& aFileName, TBool aWait)
    {
    TPckgC<TBool> wait(aWait);
    User::LeaveIfError(SendReceive(ERunExecutable, TIpcArgs(&aFileName, &wait)));
    }

EXPORT_C void RSisLauncherSession::StartDocumentL(RFile& aFile, TBool aWait)
    {
    TPckgC<TBool> wait(aWait);
    TIpcArgs ipcArgs;
    aFile.TransferToServer(ipcArgs, 0, 1);
    ipcArgs.Set(2, &wait);
    User::LeaveIfError(SendReceive(EStartDocumentByHandle, ipcArgs));
    }

EXPORT_C void RSisLauncherSession::StartByMimeL(RFile& aFile, const TDesC8& aMimeType, TBool aWait)
    {
    TPckgC<TBool> wait(aWait);
    TIpcArgs ipcArgs;
    aFile.TransferToServer(ipcArgs, 0, 1);
    ipcArgs.Set(2, &aMimeType);
    ipcArgs.Set(3, &wait);
    
    User::LeaveIfError(SendReceive(EStartByMimeByHandle, ipcArgs));
    }

EXPORT_C void RSisLauncherSession::StartDocumentL(const TDesC& aFileName, TBool aWait)
    {
    TPckgC<TBool> wait(aWait);
    User::LeaveIfError(SendReceive(EStartDocument, TIpcArgs(&aFileName, &wait)));
    }

EXPORT_C void RSisLauncherSession::StartByMimeL(const TDesC& aFileName, const TDesC8& aMimeType, TBool aWait)
    {
    TPckgC<TBool> wait(aWait);
    User::LeaveIfError(SendReceive(EStartByMime, TIpcArgs(&aFileName, &aMimeType, &wait)));
    }

EXPORT_C void RSisLauncherSession::ShutdownAllL()
    {
    User::LeaveIfError(SendReceive(EShutdownAll));
    }

EXPORT_C void RSisLauncherSession::CheckApplicationInUseL(RArray<TAppInUse>& aUidList)
    {
    TInt uidCount=aUidList.Count();
    if (uidCount)
        {
        HBufC8* appInUseBuf=HBufC8::NewLC(uidCount * sizeof(TAppInUse));
        TPtr8 appInUsePtr=appInUseBuf->Des();
        TUint8* dataPtr(NULL);
        TInt i;
        for (i = 0 ; i < uidCount ; ++i)
            {
            dataPtr=reinterpret_cast<TUint8*>(&aUidList[i]);
            appInUsePtr.Append(dataPtr, sizeof(TAppInUse));         
            }   
        User::LeaveIfError((SendReceive(ECheckApplicationInUse, TIpcArgs(&appInUsePtr))));

        //Copy the result back to the RArray
        dataPtr=const_cast<TUint8*>(appInUsePtr.Ptr());
        TAppInUse* appInUseDataPtr=reinterpret_cast<TAppInUse*>(dataPtr);
        for (i = 0; i < uidCount; ++i)
            {
            aUidList[i]=appInUseDataPtr[i]; 
            }               
            
        CleanupStack::PopAndDestroy(appInUseBuf);               
        }
    }
    
EXPORT_C void RSisLauncherSession::ShutdownL(const RArray<TUid>& aUidList, TInt aTimeout)
    {
    TInt uidCount=aUidList.Count();
    if (uidCount)   
        {
        HBufC8* tUidBuf=HBufC8::NewLC(uidCount * sizeof(TUid));
        TPtr8 tUidBufPtr=tUidBuf->Des();
        const TUint8* dataPtr(NULL);
        
        for (TInt i=0;i<uidCount;i++)
            {
            dataPtr=reinterpret_cast<const TUint8*>(&aUidList[i]);
            tUidBufPtr.Append(dataPtr, sizeof(TUid));
            }   
        User::LeaveIfError((SendReceive(EShutdown, TIpcArgs(&tUidBufPtr, aTimeout))));      
        CleanupStack::PopAndDestroy(tUidBuf);                           
        }
    }

EXPORT_C void RSisLauncherSession::NotifyNewAppsL(const RPointerArray<TDesC>& aFiles)
    {
    if (aFiles.Count() > 0)
        {
        CBufFlat* tmpBuf = CBufFlat::NewL(255);
        CleanupStack::PushL(tmpBuf);
            
        RBufWriteStream stream(*tmpBuf);
        CleanupClosePushL(stream);

        ExternalizePointerArrayL(aFiles, stream);

        // Create an HBufC8 from the stream buf's length, and copy 
        // the stream buffer into this descriptor
        HBufC8* buffer = HBufC8::NewLC(tmpBuf->Size());
        TPtr8 ptr(buffer->Des());
        tmpBuf->Read(0, ptr, tmpBuf->Size());
    
        User::LeaveIfError(SendReceive(ENotifyNewApps, TIpcArgs(&ptr)));
        CleanupStack::PopAndDestroy(3, tmpBuf);
        }
    }

EXPORT_C void RSisLauncherSession::RunAfterEcomNotificationL(const RPointerArray<CSisRegistryFileDescription>& aFileList)
    {
    TInt count = aFileList.Count();
    if (count <= 0)
        {
        return; 
        }

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.ShareProtected());

    for (TInt i = 0; i < count; ++i)
        {
        CSisRegistryFileDescription* fileDes = aFileList[i];

        // open the file entry
        TEntry entry;
        if (fs.Entry(fileDes->Target(), entry) != KErrNone)
            {
            // just ignore, there's nothing we can do and we should try to process the
            // remaining files.
            continue;
            }
        
        TIpcArgs ipcArgs;
        TBool waitEnd = 0 != (fileDes->OperationOptions() & Sis::EInstFileRunOptionWaitEnd);
        TPckgC<TBool> wait(waitEnd);
        ipcArgs.Set(2, &wait);

        TInt err = KErrNone;

        if (entry.IsTypeValid() && SecUtils::IsExe(entry))
            {
            ipcArgs.Set(0,&fileDes->Target());
            err = SendReceive(EQueueRunExecutable, ipcArgs);
            }
        else 
            {
            RFile file;
            CleanupClosePushL(file);
            err = file.Open(fs, fileDes->Target(), EFileShareExclusive|EFileWrite);

            HBufC8* mimeType = 0;

            if (err == KErrNone)
                {
                if (fileDes->OperationOptions() & Sis::EInstFileRunOptionByMimeType)
                    {
                    mimeType = HBufC8::NewLC(fileDes->MimeType().Length());
                    TPtr8 ptr = mimeType->Des();
                    ptr.Copy(fileDes->MimeType());
                    }

                if (file.TransferToServer(ipcArgs, 0, 1) != KErrNone)
                    {
                    // best effort is to continue with remaining exes
                    }   
                else if (mimeType != 0)
                    {
                    TPtrC8 x = mimeType->Des();
                    ipcArgs.Set(3, &x);
                    err = SendReceive(EQueueStartByMimeByHandle, ipcArgs);
                    }
                else
                    {
                    err = SendReceive(EQueueStartDocumentByHandle, ipcArgs);
                    }
                if (mimeType != 0)
                    {
                    CleanupStack::PopAndDestroy(mimeType);
                    }
                }
            CleanupStack::PopAndDestroy(&file); 
            }
        // best effort is to continue with other exes in the event of an error
        // so do nothing here.
        }
    CleanupStack::PopAndDestroy(&fs);
    User::LeaveIfError(SendReceive(EExecuteQueue));
    
    }

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RSisLauncherSession::ParseSwTypeRegFileL(RFile& aFile, RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray)
    {
    // Pack the file handle
    TIpcArgs ipcArgs;
    aFile.TransferToServer(ipcArgs, 0, 1);

    // Allocate a buffer for the parsed registration info
    TInt bufLen = 0;
    User::LeaveIfError(aFile.Size(bufLen));
    HBufC8* buf = HBufC8::NewLC(bufLen); // using the size of the file here is overhead but provides very simple code
    
    // Packe the buffer
    TPtr8 bufPtr(buf->Des());
    ipcArgs.Set(2, &bufPtr);
    
    // Send request to the server
    User::LeaveIfError(SendReceive(EParseSwTypeRegFile, ipcArgs));

    // Unpack the parsed registration info
    RDesReadStream rs(*buf);
    CleanupClosePushL(rs);
    SoftwareTypeRegInfoUtils::UnserializeArrayL(rs, aSwTypeRegInfoArray);

    CleanupStack::PopAndDestroy(2, buf); // rs
    }

EXPORT_C void RSisLauncherSession::RegisterSifLauncherMimeTypesL(const RPointerArray<HBufC8>& aMimeTypes)
    {
    RegisterSifLauncherMimeTypesImplL(aMimeTypes, ETrue);
    }

EXPORT_C void RSisLauncherSession::UnregisterSifLauncherMimeTypesL(const RPointerArray<HBufC8>& aMimeTypes)
    {
    RegisterSifLauncherMimeTypesImplL(aMimeTypes, EFalse);
    }

void RSisLauncherSession::RegisterSifLauncherMimeTypesImplL(const RPointerArray<HBufC8>& aMimeTypes, TBool aRegister)
    {
    // Calculate the size of aMimeTypes in a buffer
    TInt bufLen = sizeof(TInt);
    for (TInt i=0; i<aMimeTypes.Count(); ++i)
        {
        bufLen += sizeof(TInt) + aMimeTypes[i]->Size();
        }
    
    // Externalize MIME types
    HBufC8* buf = HBufC8::NewLC(bufLen);
    TPtr8 bufPtr(buf->Des());

    RDesWriteStream ws(bufPtr);
    CleanupClosePushL(ws);
    
    ExternalizePointerArrayL(aMimeTypes, ws);

    ws.CommitL();
    CleanupStack::PopAndDestroy(&ws);

    // Send MIME types to the server
    TIpcArgs ipcArgs(&bufPtr);
    User::LeaveIfError(SendReceive(aRegister?ERegisterSifLauncherMimeTypes:EUnregisterSifLauncherMimeTypes, ipcArgs));

    CleanupStack::PopAndDestroy(buf);
    }
    
EXPORT_C void  RSisLauncherSession::AsyncParseResourceFileSizeL(const RFile& aRegistrationFile, TRequestStatus& aStatus, TBool aIsForGetCompInfo)
    {
    TIpcArgs args;
    aRegistrationFile.TransferToServer(args, 0, 1);
    args.Set(2, aIsForGetCompInfo);
    SendReceive(EAsyncParseResourceFileSize, args, aStatus);    
    }
    
EXPORT_C Usif::CApplicationRegistrationData*  RSisLauncherSession::AsyncParseResourceFileDataL(TInt aDataSize)
    {
    HBufC8* appRegData=HBufC8::NewMaxLC(aDataSize);
    TPtr8 appRegDataDes = appRegData->Des();
    TInt err=SendReceive(EAsyncParseResourceFileData, TIpcArgs(&appRegDataDes ));        
    User::LeaveIfError(err);
    
    Usif::CApplicationRegistrationData* applicationData= Usif::CApplicationRegistrationData::NewLC();
    // Got the buffer, internalise the arrays
    RDesReadStream readStream(*appRegData);
    CleanupClosePushL(readStream);    
    applicationData->InternalizeL(readStream);
    CleanupStack::PopAndDestroy(&readStream); 
    CleanupStack::Pop(applicationData);
    CleanupStack::PopAndDestroy(appRegData);
	return applicationData;
    }

EXPORT_C Usif::CApplicationRegistrationData* RSisLauncherSession::SyncParseResourceFileL(const RFile& aRegistrationFile, TBool aIsForGetCompInfo)
    {
	// Get the size of data
    TRequestStatus status;
    AsyncParseResourceFileSizeL (aRegistrationFile, status, aIsForGetCompInfo);
    User::WaitForRequest(status);
    TInt size = status.Int(); 
    if (size < KErrNone)
        {
        User::LeaveIfError(status.Int());
        return NULL;
        }

	// Return the app reg data
    Usif::CApplicationRegistrationData* appRegData = AsyncParseResourceFileDataL(size);
    return appRegData;
    }


EXPORT_C void RSisLauncherSession::NotifyNewAppsL(const RPointerArray<Usif::CApplicationRegistrationData>& aAppRegData)
    {
    if (aAppRegData.Count() > 0)
        {
        TInt bufLen = sizeof(TInt);
        const TInt appCount = aAppRegData.Count();
        for (TInt i=0; i<appCount; ++i)
            {
			bufLen += GetObjectSizeL(aAppRegData[i]);
            }

        HBufC8* buffer = HBufC8::NewLC(bufLen);
        TPtr8 bufPtr(buffer->Des());

        RDesWriteStream ws(bufPtr);
        CleanupClosePushL(ws);

        ws.WriteInt32L(appCount);
        for (TInt i=0; i<appCount; ++i)
            {
            const Usif::CApplicationRegistrationData& info = *aAppRegData[i];
            ws << info;
            }
        ws.CommitL();
        CleanupStack::PopAndDestroy(&ws);
        
        TIpcArgs ipcArgs(&bufPtr);
        User::LeaveIfError(SendReceive(ENotifyNewAppsData, ipcArgs));

        CleanupStack::PopAndDestroy(buffer);
        }
    }


EXPORT_C void RSisLauncherSession::NotifyNewAppsL(const RArray<TAppUpdateInfo>& aAppUpdateInfo)
    {
    TInt bufLen = sizeof(TInt);
    const TInt appCount = aAppUpdateInfo.Count();
    for (TInt i=0; i<appCount; ++i)
        {
        bufLen += GetObjectSizeL(&aAppUpdateInfo[i]);
        }

    HBufC8* buffer = HBufC8::NewLC(bufLen);
    TPtr8 bufPtr(buffer->Des());

    RDesWriteStream ws(bufPtr);
    CleanupClosePushL(ws);

    ws.WriteInt32L(appCount);
    for (TInt i=0; i<appCount; ++i)
        {
        const TAppUpdateInfo& info = aAppUpdateInfo[i];
        ws << info;
        }
    ws.CommitL();
    CleanupStack::PopAndDestroy(&ws);
    
    TIpcArgs ipcArgs(&bufPtr);
    User::LeaveIfError(SendReceive(ENotifyApparcForApps, ipcArgs));

    CleanupStack::PopAndDestroy(buffer);
    }

EXPORT_C TAppUpdateInfo::TAppUpdateInfo()
    {    
    }

EXPORT_C TAppUpdateInfo::TAppUpdateInfo(TUid aAppUid, InstallActions aAction)
    {
    iAppUid = aAppUid;
    iAction = aAction;
    }

EXPORT_C void TAppUpdateInfo::InternalizeL(RReadStream& aReadStream)
    {
    iAppUid = TUid::Uid(aReadStream.ReadUint32L());
    iAction = (Swi::InstallActions)aReadStream.ReadUint32L();
    }

EXPORT_C void TAppUpdateInfo::ExternalizeL(RWriteStream& aWriteStream) const
    {
    aWriteStream.WriteUint32L(iAppUid.iUid);
    aWriteStream.WriteInt32L(iAction);
    }
#endif
