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
* SisLauncher - session implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <s32mem.h>

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <ct/rcpointerarray.h>
#include "swtypereginfoparser.h"
#ifndef SWI_TEXTSHELL_ROM
#include <apgcli.h>
#endif
#endif

#include "sislauncherserver.h"
#include "sislaunchersession.h"
#include "sislauncherclientserver.h"
#include "threadmonitor.h"
#include "securitypolicy.h"
#include "sisregistryfiledescription.h"
#include "sislauncherdefs.h"
#include "arrayutils.h"  // from source/sisregistry/common/ 
#include "log.h"
#include "queueprocessor.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include "apprscparser.h"
#include "ipcutil.h"
#include <usif/scr/appregentries.h>
#include <usif/scr/screntries_platform.h>
#include "sislauncherclient.h" 
#endif

namespace Swi 
{

template <class T>
void DestroyRPointerArray(TAny* aPtr)
    {
    RPointerArray<T>* self = static_cast<RPointerArray<T>*> (aPtr);
    self->ResetAndDestroy();
    delete self;
    }

template <>
class TTraits<TDesC>
    {
public:
    static TDesC* CopyLC(const TDesC& aOther) { return aOther.AllocLC(); }
    static TDesC* ReadFromStreamLC(RReadStream& aStream) { return HBufC::NewLC(aStream, KMaxTInt); }
    static void WriteToStreamL(const TDesC& aItem, RWriteStream& aStream) { aStream << aItem; }
    };

CSisLauncherSession::CSisLauncherSession()
    {
    }

void CSisLauncherSession::CreateL()
    {
    Server().AddSession();
    }

CSisLauncherSession::~CSisLauncherSession()
    {
    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    delete iCurrentAppRegData;
    #endif
    Server().DropSession();
    }
void CSisLauncherSession::DoRunExecutableL(const RMessage2& aMessage) 
    {
    TFileName filename;
    aMessage.ReadL(0, filename);

    TBool wait;
    TPckg <TBool> waitPckg(wait);

    aMessage.ReadL(1, waitPckg);
    Server().RunExecutableL(filename, wait);

    aMessage.Complete(KErrNone);
    }

void CSisLauncherSession::DoStartDocumentL(const RMessage2& aMessage)
    {
    TFileName filename;
    aMessage.ReadL(0, filename);
    
    TBool wait;
    TPckg <TBool> waitPckg(wait);
    
    aMessage.ReadL(1, waitPckg);
    if (Server().BootUpMode() == KTextShell) 
        {
        // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
        DEBUG_PRINTF2(_L8("Sis Launcher Server - textshell - skipping start document by file handle. wait = %d"), wait);    
        }
    else
        { 
        // emulatore running with GUI
        #ifndef SWI_TEXTSHELL_ROM
            Server().StartDocumentL(filename, wait);
        #endif
        }

    aMessage.Complete(KErrNone);    
    }


void CSisLauncherSession::DoStartDocumentByHandleL(const RMessage2& aMessage)
    {
    RFile file;
    file.AdoptFromClient(aMessage, 0, 1);
    CleanupClosePushL(file);

    TBool wait;
    TPckg <TBool> waitPckg(wait);

    aMessage.ReadL(2, waitPckg);
    if (Server().BootUpMode() == KTextShell)
        {
        // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
        DEBUG_PRINTF3(_L8("Sis Launcher Server - textshell - skipping start document %S, wait = %d"),
                &file, wait);
        }
    else
        {
        // emulatore running with GUI
#ifndef SWI_TEXTSHELL_ROM
        Server().StartDocumentL(file, wait);
#endif
        }
    CleanupStack::PopAndDestroy(&file);
    aMessage.Complete(KErrNone);
}
void CSisLauncherSession::DoStartByMimeL(const RMessage2& aMessage)
    {
    TFileName filename;
    aMessage.ReadL(0, filename);

    TInt srcLen = aMessage.GetDesLengthL(1);

    HBufC8* mimeType = HBufC8::NewLC(srcLen);
    TPtr8 ptr(mimeType->Des());
    aMessage.ReadL(1, ptr);

    TBool wait;
    TPckg <TBool> waitPckg(wait);
    aMessage.ReadL(2, waitPckg);
    if (Server().BootUpMode() == KTextShell) 
        {
        // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
        DEBUG_PRINTF3(_L8("Sis Launcher Server - textshell - skipping start document by file handle mimetype %S, wait = %d"),
                mimeType, wait);    
        }
    else
        {
        // emulatore running with GUI
#ifndef SWI_TEXTSHELL_ROM
        Server().StartByMimeL(filename, *mimeType, wait);
#endif
        }       
    CleanupStack::PopAndDestroy(mimeType);

    aMessage.Complete(KErrNone);
    }
void CSisLauncherSession::DoStartByMimeByHandleL(const RMessage2& aMessage)
    {
    RFile file;
    file.AdoptFromClient(aMessage, 0, 1);           
    CleanupClosePushL(file);

    TInt srcLen = aMessage.GetDesLengthL(2);

    HBufC8* mimeType = HBufC8::NewLC(srcLen);
    TPtr8 ptr(mimeType->Des());
    aMessage.ReadL(2, ptr);

    TBool wait;
    TPckg <TBool> waitPckg(wait);

    aMessage.ReadL(3, waitPckg);
    if (Server().BootUpMode() == KTextShell) 
        {
        // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
        DEBUG_PRINTF4(_L("Sis Launcher Server - textshell - skipping launch of document %S, mimetype %s, wait %d"),
                &file, mimeType, wait);
        }
    else
        {
        // emulatore running with GUI
#ifndef SWI_TEXTSHELL_ROM
        Server().StartByMimeL(file, *mimeType, wait);
#endif
        }       
    CleanupStack::PopAndDestroy(mimeType);
    CleanupStack::PopAndDestroy(&file);

    aMessage.Complete(KErrNone);

    }
void CSisLauncherSession::ServiceL(const RMessage2& aMessage)
    {
    DEBUG_PRINTF2(_L8("Sis Launcher Server - Servicing Message %d"), aMessage.Function());
    
    switch (aMessage.Function())
        {
        case ERunExecutable:
            DoRunExecutableL(aMessage);
            break;
        case EStartDocument:
            DoStartDocumentL(aMessage);         
            break;
        case EStartDocumentByHandle:
            DoStartDocumentByHandleL(aMessage);
            break;  
        case EStartByMime:
            DoStartByMimeL(aMessage);
            break;
        case EStartByMimeByHandle:
            DoStartByMimeByHandleL(aMessage);
            break;
        case EShutdown:
            {
            TInt srcLen=aMessage.GetDesLengthL(0);
            
            HBufC8* uidBuffer = HBufC8::NewLC(srcLen);
            TPtr8 uidPtr = uidBuffer->Des();
            aMessage.ReadL(0, uidPtr, 0);
            
            TUint8* dataPtr=const_cast<TUint8*>(uidPtr.Ptr());
            TUid* tUidPtr=reinterpret_cast<TUid*>(dataPtr);
            TInt8 uidCount=srcLen/sizeof(TUid);
            TInt shutdownTimeout = aMessage.Int1();
            if (Server().BootUpMode() == KTextShell) 
                {
                // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
                DEBUG_PRINTF(_L8("Sis Launcher Server - textshell - skipping graceful shutdown of GUI applications"));
                }
            else
                {
                // emulatore running with GUI
                #ifndef SWI_TEXTSHELL_ROM
                    for (TInt i = 0; i < uidCount; i++)
                        {
                        // graceful shutdown
                        Server().ShutdownL(tUidPtr[i], shutdownTimeout);    
                        }
                #endif
                }
            // Having tried graceful shutdown, we need to kill any remaining processes
            // matching the SID.  Note that killing a process may re-order the list of
            // remaining processes, so the search must start from the top again.
            for (TInt i = 0; i < uidCount; i++)
                {
                Server().ForceShutdownL(tUidPtr[i]);
                }           
            CleanupStack::PopAndDestroy(uidBuffer);
            aMessage.Complete(KErrNone);
            
            break;
            }
        case EShutdownAll:
            {
            if (Server().BootUpMode() == KTextShell) 
                {
                // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
                DEBUG_PRINTF(_L8("Sis Launcher Server - textshell - skipping shutdown of user applications"));  
                }
            else
                {
                // emulatore running with GUI
                #ifndef SWI_TEXTSHELL_ROM
                Server().ShutdownL();
                #endif
                }
            aMessage.Complete(KErrNone);
            break;  
            }
        case ECheckApplicationInUse:
            {
            TInt srcLen=aMessage.GetDesLengthL(0);
                
            HBufC8* appInUseBuffer = HBufC8::NewLC(srcLen);
            TPtr8 appInUseBufferPtr = appInUseBuffer->Des();    
            aMessage.ReadL(0, appInUseBufferPtr, 0);
     
            TAppInUse* appInUsePtr = (TAppInUse*) User::AllocL(srcLen);
            CleanupStack::PushL(appInUsePtr);   
            Mem::Copy(appInUsePtr, appInUseBufferPtr.Ptr(), srcLen);
 
            TUint appInUseCount=srcLen/sizeof(TAppInUse);
            for (TInt i=0;i<appInUseCount;i++)
                {
                TRAPD(err, CheckApplicationInUseL(appInUsePtr[i].iAppUid));
                if (err==KErrInUse)  
                    {
                    appInUsePtr[i].iInUse=ETrue;
                    }            
                }   
     
            TUint8* dataPtr=reinterpret_cast<TUint8*>(appInUsePtr);
            const TPtrC8 data(dataPtr, srcLen); 
        
            //Indicate the client apps in use
            aMessage.WriteL(0, data);                           
            aMessage.Complete(KErrNone);
            CleanupStack::PopAndDestroy(appInUsePtr);
            CleanupStack::PopAndDestroy(appInUseBuffer);    
            break;
            }
        case ENotifyNewApps:
            {
            TInt size = aMessage.GetDesLengthL(0);

            HBufC8* buf = HBufC8::NewLC(size);
            TPtr8 bufPtr = buf->Des();
            aMessage.ReadL(0, bufPtr);

            RDesReadStream stream(*buf);
            RPointerArray<TDesC> files;
            InternalizePointerArrayL(files, stream);
            
            if (Server().BootUpMode() == KTextShell) 
                {
                // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
                DEBUG_PRINTF(_L8("Sis Launcher Server - TextShell - skipping notification of new applications (ENotifyNewApps)."));
                }
            else
                {
                // emulatore running with GUI
                #ifndef SWI_TEXTSHELL_ROM
                Server().NotifyNewAppsL(files);
                #endif // SWI_TEXTSHELL_ROM
                }
            
            files.ResetAndDestroy();
            CleanupStack::PopAndDestroy(buf);
            
            aMessage.Complete(KErrNone);
            break;
            }
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
        case EParseSwTypeRegFile:
            #ifndef SWI_TEXTSHELL_ROM
            ParseSwTypeRegFileL(aMessage);
            #endif
            aMessage.Complete(KErrNone);
            break;
        case ERegisterSifLauncherMimeTypes:
            #ifndef SWI_TEXTSHELL_ROM
            RegisterSifLauncherMimeTypesL(aMessage);
            #endif
            aMessage.Complete(KErrNone);
            break;
        case EUnregisterSifLauncherMimeTypes:
            #ifndef SWI_TEXTSHELL_ROM            
            UnregisterSifLauncherMimeTypesL(aMessage);
            #endif
            aMessage.Complete(KErrNone);
            break;
        case EAsyncParseResourceFileSize:
            {
            TRAPD(err,err = AsyncParseResourceFileSizeL(aMessage));            
            aMessage.Complete(err);            
            break;
            }
        case EAsyncParseResourceFileData:
            {
            TRAPD(err,AsyncParseResourceFileDataL(aMessage));
            aMessage.Complete(err);
            break;
            }
        case ENotifyNewAppsData:
            {
            RIpcReadStream readStream;
            readStream.Open(aMessage, 0);
            CleanupClosePushL(readStream);

            RPointerArray<Usif::CApplicationRegistrationData> appRegInfo;
            CleanupClosePushL(appRegInfo);

            const TInt numElems = readStream.ReadInt32L();
            for (TInt i=0; i<numElems; ++i)
                {
                Usif::CApplicationRegistrationData* info = Usif::CApplicationRegistrationData::NewL(readStream);
                CleanupStack::PushL(info);
                appRegInfo.AppendL(info);
                CleanupStack::Pop(info);
                }

            if (Server().BootUpMode() == KTextShell) 
                {
                // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
                DEBUG_PRINTF(_L8("Sis Launcher Server - TextShell - Skipping notification of force registered applications."));
                }
            else
                {
                // emulatore running with GUI
                #ifndef SWI_TEXTSHELL_ROM
                Server().NotifyNewAppsL(appRegInfo);
                #endif // SWI_TEXTSHELL_ROM
                }
            appRegInfo.ResetAndDestroy();
            CleanupStack::PopAndDestroy(2);
            aMessage.Complete(KErrNone);
            break;
            }
        case ENotifyApparcForApps:
            {
            if (Server().BootUpMode() == KTextShell) 
                {
                DEBUG_PRINTF(_L8("Sis Launcher Server - TextShell - Skipping notification of applications (ENotifyApparcForApps)."));
                }
            else
                {
                #ifndef SWI_TEXTSHELL_ROM
                NotifyApparcForAppsL(aMessage);  
                #endif
                }
            aMessage.Complete(KErrNone);
            break;
            }
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
        case EQueueRunExecutable:
            //fall through
        case EQueueStartDocumentByHandle:
            //fall through
        case EQueueStartByMimeByHandle:
            Server().RunQueue().AddToQueueL(aMessage);
            aMessage.Complete(KErrNone);
            break;
        case EExecuteQueue:
            aMessage.Complete(Server().RunQueue().ExecuteQueue());
            break;
        case EKillQueue:
            Server().RunQueue().ResetQueue();
            aMessage.Complete(KErrNone);
            break;
        default:
            {
            PanicClient(aMessage,EPanicIllegalFunction);
            break;
            }
        }
    }


void CSisLauncherSession::ServiceError(const RMessage2& aMessage,TInt aError)
    {
    DEBUG_PRINTF2(_L8("Sis Launcher Server - ServiceL failed with error code %d."), aError);
    
    if (aError==KErrBadDescriptor)
        {
        PanicClient(aMessage,EPanicBadDescriptor);
        }
    CSession2::ServiceError(aMessage,aError);
    }


void CSisLauncherSession::CheckApplicationInUseL(TUid aUid)
    {
    
    TFindProcess findProcess;
    TFullName fullName;
 
    while(findProcess.Next(fullName) == KErrNone)
        {
        RProcess process;
        User::LeaveIfError(process.Open(findProcess));
        TUid sid(process.SecureId());
        TExitType exitType = process.ExitType();
        process.Close();
        if (sid == aUid && exitType == EExitPending)
                    User::Leave(KErrInUse);
        }
    }


#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK   
TInt CSisLauncherSession::AsyncParseResourceFileSizeL(const RMessage2& aMessage)
    {
    // We only support one call at once currently
    if (iInAsyncParseResourceFile)
        {
        return KErrInUse;
        }
    else
        {
        iInAsyncParseResourceFile=ETrue;
        delete iCurrentAppRegData;
        iCurrentAppRegData=NULL;

        RFs fs;
        RFile file;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        file.AdoptFromClient(aMessage, 0, 1);   
        CleanupClosePushL(file);
        //Read Boolean to see if request is from getComponentInfo      
        TBool isForGetCompInfo(aMessage.Int2());        
        CAppRegInfoReader *appRegInfoReader = CAppRegInfoReader::NewL(fs,file);
        CleanupStack::PushL(appRegInfoReader);        
        TInt err(0);
        
        // If request is for get component info then don't pass any language else pass the device supported languages 
        if(isForGetCompInfo)
            {             
            RArray<TLanguage> appLanguages;
            CleanupClosePushL(appLanguages);
            TRAP(err,iCurrentAppRegData = appRegInfoReader->ReadL(appLanguages));
            CleanupStack::PopAndDestroy(&appLanguages); 
            }
        else
            {
            //check if we alredy have the device supported languages, if yes then we will use that else Get and Set the supported langs in the server
            const RArray<TLanguage>& appLanguages = Server().DeviceSupportedLanguages();
            if(0 == appLanguages.Count())
               {
               Server().ResetInstalledLanguagesL();
               }
            TRAP(err,iCurrentAppRegData = appRegInfoReader->ReadL(appLanguages));            
            }
                    
        if (KErrNone != err)
            {
            iInAsyncParseResourceFile=EFalse;
            User::Leave(err);
            }
        
        iInAsyncParseResourceFile=EFalse;   
        TInt objectSize=0;
        objectSize= GetObjectSizeL(iCurrentAppRegData);
        if (objectSize > 0)
            {
            //Everything went fine so return the buffer size.     
            CleanupStack::PopAndDestroy(3, &fs);    //appRegInfoReader, file
            return objectSize;          
            }
        else
            User::Leave(KErrUnknown);
        }
    return KErrNone;
    }
   
void CSisLauncherSession::AsyncParseResourceFileDataL(const RMessage2& aMessage)
    {
    if (iCurrentAppRegData)
        {
        WriteObjectDataL(aMessage, 0, iCurrentAppRegData); 
        delete iCurrentAppRegData;
        iCurrentAppRegData=NULL;
        }
    else
        User::Leave(KErrArgument);
    }

void CSisLauncherSession::ParseSwTypeRegFileL(const RMessage2& aMessage)
    {
    // Unpack the file handle
    RFile file;
    User::LeaveIfError(file.AdoptFromClient(aMessage, 0, 1));
    CleanupClosePushL(file);
    
    // Read the registration file
    TInt fileSize = 0;
    User::LeaveIfError(file.Size(fileSize));
    HBufC8* buf = HBufC8::NewLC(fileSize);
    TPtr8 bufPtr = buf->Des();
    User::LeaveIfError(file.Read(bufPtr));

    // Parse the registration file
	RCPointerArray<Usif::CSoftwareTypeRegInfo> regInfoArray;
    CleanupClosePushL(regInfoArray);
    
    CSoftwareTypeRegInfoParser* parser = CSoftwareTypeRegInfoParser::NewL();
    CleanupStack::PushL(parser);
    parser->ParseL(*buf, regInfoArray);

    // Pack the registration data
    RBuf8 serializedRegInfo;
    serializedRegInfo.CleanupClosePushL();
    SoftwareTypeRegInfoUtils::SerializeArrayL(regInfoArray, serializedRegInfo);
    aMessage.Write(2, serializedRegInfo);

    CleanupStack::PopAndDestroy(5, &file); // buf, regInfoArray, parser, serializedRegInfo
    }

#ifndef SWI_TEXTSHELL_ROM
void CSisLauncherSession::NotifyApparcForAppsL(const RMessage2& aMessage)
    {
    DEBUG_PRINTF(_L8("Sending the notification to AppArc."));
    RIpcReadStream readStream;
    readStream.Open(aMessage, 0);
    CleanupClosePushL(readStream);    
    RArray<TApaAppUpdateInfo> apparcAppInfoArray;
    CleanupClosePushL(apparcAppInfoArray);
    
    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    if(KErrNone != err)
        {
        DEBUG_PRINTF2(_L8("RApaLsSession::Connect failed with %d. Notification to AppArc failed."), err);
        User::LeaveIfError(err);
        }
    CleanupClosePushL(apaSession);
    TApaAppUpdateInfo::TApaAppAction appaction = TApaAppUpdateInfo::EAppNotPresent;
    const TInt numElems = readStream.ReadInt32L();
    DEBUG_PRINTF2(_L8("Number of applications to be notified is %d."), numElems);
    //Convert the local structure into the structure required by apparc
    for (TInt i=0; i<numElems; ++i)
        {
        TAppUpdateInfo appInfo;
        appInfo.InternalizeL(readStream);   
        if(appInfo.iAction == EAppInstalled)
            {
            appaction = TApaAppUpdateInfo::EAppPresent;
            }
        else if(appInfo.iAction == EAppUninstalled)
            {
            appaction = TApaAppUpdateInfo::EAppNotPresent;
            }
        TApaAppUpdateInfo apparcAppUpdateInfo(appInfo.iAppUid, appaction);
        apparcAppInfoArray.AppendL(apparcAppUpdateInfo);                    
        DEBUG_PRINTF2(_L("AppUid is 0x%x"), appInfo.iAppUid);
        DEBUG_PRINTF2(_L("Action is %d"), appInfo.iAction);         
        }    
    
    err = 0;
    TRAP(err, err = apaSession.UpdateAppListL(apparcAppInfoArray));
    if(KErrNone != err)
        {
        DEBUG_PRINTF2(_L8("RApaLsSession::UpdateAppListL failed with %d. Notification to AppArc failed."), err);
        User::LeaveIfError(err);
        }

    CleanupStack::PopAndDestroy(3, &readStream);       
    }

void CSisLauncherSession::RegisterSifLauncherMimeTypesL(const RMessage2& aMessage)
    {
    RegisterSifLauncherMimeTypesImplL(aMessage, ETrue);
    }

void CSisLauncherSession::UnregisterSifLauncherMimeTypesL(const RMessage2& aMessage)
    {
    RegisterSifLauncherMimeTypesImplL(aMessage, EFalse);
    }

void CSisLauncherSession::RegisterSifLauncherMimeTypesImplL(const RMessage2& aMessage, TBool aRegister)
    {
    // Read serialized MIME types from aMessage
    HBufC8* buf = HBufC8::NewLC(aMessage.GetDesLengthL(0));
    TPtr8 bufPtr(buf->Des());
    aMessage.ReadL(0, bufPtr);
    
    // Unserialize MIME types
    RDesReadStream rs(*buf);
    CleanupClosePushL(rs);
    
    RCPointerArray<HBufC8> mimeTypes;
    CleanupClosePushL(mimeTypes);
    
    InternalizePointerArrayL(mimeTypes, rs);
    
    // Connect to AppArc
    RApaLsSession apa;
    TInt err = apa.Connect();
    if (err != KErrNone)
        {
        DEBUG_PRINTF2(_L8("Failed to connect to the AppArc server, err = %d\n"),err);
        User::Leave(err);
        }
    CleanupClosePushL(apa);

    // Iterate over the MIME types and (un)register them
    for (TInt i=0; i<mimeTypes.Count(); ++i)
        {
        TDataType dataType(*mimeTypes[i]);
        if (aRegister)
            {
            const TUid KSifLauncherUid = {0x10285BD0};
            err = apa.InsertDataMapping(dataType, KDataTypePriorityTrustedHigh, KSifLauncherUid);
            }
        else
            {
            err = apa.DeleteDataMapping(dataType);
            }
        if (err != KErrNone)
            {
            DEBUG_PRINTF2(_L8("Failed to (un)register MIME types to AppArc, err = %d\n"),err);
            }
        }
    
    CleanupStack::PopAndDestroy(4, buf); //rs, mimeTypes, apa
    }

#endif // SWI_TEXTSHELL_ROM
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

} // namespace Swi
