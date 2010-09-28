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
* SisLauncher - server implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
 
#include "sislauncherserver.h"
#include "sislaunchersession.h"
#include "sislauncherclientserver.h"
#include "log.h"
#include "securitypolicy.h"
#include <w32std.h>

#include "threadmonitor.h"
#include "queueprocessor.h"

#ifndef SWI_TEXTSHELL_ROM
    #include <apmstd.h>
    #include <apgcli.h>
    #include <apgwgnam.h>
    #include <apgtask.h>
#endif

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
const TUint KInstallServerUid = 0x101F7295;
const TUint KSisRegistryServerUid = 0x10202DCA;
#endif

using namespace Swi;

CServer2* CSisLauncherServer::NewLC()
    {
    CSisLauncherServer* self=new(ELeave) CSisLauncherServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CSisLauncherServer::~CSisLauncherServer()
    {
#ifndef SWI_TEXTSHELL_ROM
    if (iBootMode != KTextShell)
        {
        iWsSession.Close(); 
        }
#endif
    delete iShutdown;
    delete iQueueProcessor;    
    iDeviceSupportedLanguages.Close();
    CSecurityPolicy::ReleaseResource();
    }

void CSisLauncherServer::ConstructL()
    {
    StartL(KSisLauncherServerName);
    iShutdown = new (ELeave) CSisLauncherServerShutdown;
    iShutdown->ConstructL();
    TInt connectErr = KErrNotFound;
#ifndef SWI_TEXTSHELL_ROM
    // Connect to Window Session
    connectErr = iWsSession.Connect();
    if (connectErr != KErrNotFound)
        {
        User::LeaveIfError(connectErr);
        }
#endif
    if (connectErr == KErrNotFound)
        {
        // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
        iBootMode = KTextShell;
        }
    iQueueProcessor = CQueueProcessor::NewL(*this);
    }

CSession2* CSisLauncherServer::NewSessionL(const TVersion&,const RMessage2&) const
    {
    return new(ELeave) CSisLauncherSession();
    }

void CSisLauncherServer::AddSession()
    {
    ++iSessionCount;
    
    DEBUG_PRINTF2(_L8("Sis Launcher Server - Adding Session (%d sessions total.)"), iSessionCount);
    
    CancelShutdown();   
    }

void CSisLauncherServer::DropSession()
    {
    --iSessionCount;
    
    DEBUG_PRINTF2(_L8("Sis Launcher Server - Dropping Session (%d sessions total.)"), iSessionCount);
    
    if (0==iSessionCount && iShutdown)
        {
        DEBUG_PRINTF(_L8("Sis Launcher Server - Starting shutdown timer."));
        // ignored if a long timer is active
        iShutdown->StartShort(EFalse);
        } 
    }
/**
 * Cancels an existing shutdown timer before renabling with the long timeout. This is used to catch
 * the case where we need to allow ECOM to work before killing the server. ECOM should finish scanning 
 * files within a couple of seconds of SWI finishing but just in case we allow ECOM a little longer.
 * The other reason we have the timer is to handle the case where SWI mistakenly identified a plugin 
 * resource file, we don't want to hang around forever waiting for an event that never comes.
 * */
void CSisLauncherServer::LongServerShutdown()
    {
    DEBUG_PRINTF(_L8("Sis Launcher Server - Switching to long shutdown timer"));
    if (iShutdown)
        {
        iShutdown->StartLong();
        }
    }

void CSisLauncherServer::CancelShutdown()
    {
    DEBUG_PRINTF(_L8("Sis Launcher Server - Cancelling shutdown timer"));
        
    if (iShutdown)
        {
        iShutdown->Cancel();
        }   
    }

void CSisLauncherServer::ShortServerShutdown()
    {
    DEBUG_PRINTF(_L8("Sis Launcher Server - Re-enabling short shutdown timer"));
    if (0==iSessionCount && iShutdown)
        {
        DEBUG_PRINTF(_L8("Sis Launcher Server - Starting shutdown timer"));
        iShutdown->StartShort(ETrue);
        } 
    }

const TInt CSisLauncherServer::iRanges[iRangeCount] = 
    {
    0, // All connect attempts
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    EParseSwTypeRegFile,                                // accessible by Installserver 
    EUnregisterSifLauncherMimeTypes,                    // accessible by SisRegistryServer
    ENotifyApparcForApps,                               // accessible by Installserver and SisRegistry
    EAsyncParseResourceFileSize,                        // accessible by all clients
#endif
    ESeparatorEndAll,
    };

const TUint8 CSisLauncherServer::iElementsIndex[iRangeCount] = 
    {
    0, // Used by Client which is only swis and TCB is needed.
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    1, // Utility services used by InstallServer
    2, // Utility services used by SisRegistryServer
    CPolicyServer::ECustomCheck,
    CPolicyServer::EAlwaysPass,
#endif  
    CPolicyServer::ENotSupported,
    };

const CPolicyServer::TPolicyElement CSisLauncherServer::iPolicyElements[] = 
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityTCB), CPolicyServer::EFailClient},
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    {_INIT_SECURITY_POLICY_S0(KInstallServerUid), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_S0(KSisRegistryServerUid), CPolicyServer::EFailClient},  
#endif
    };

const CPolicyServer::TPolicy CSisLauncherServer::iPolicy =
    {
    0,                  //specifies all connect attempts need TCB
    iRangeCount,
    iRanges,
    iElementsIndex,
    iPolicyElements,
    };
    
    
// shutdown timer

CSisLauncherServerShutdown::~CSisLauncherServerShutdown()
    {
    Cancel();
    }

void CSisLauncherServerShutdown::RunL()
    {
    CActiveScheduler::Stop();
    }
    
void CSisLauncherServerShutdown::StartShort(TBool aCancelLongTimer)
    {
    if (iLongTimerActive & !aCancelLongTimer)
        {
        return;
        }
    else
        {
        Cancel();
        iLongTimerActive=EFalse;
        After(KSisLauncherShutdownDelay);
        }
    }

inline void CSisLauncherServerShutdown::StartLong()
    {
    Cancel();
    iLongTimerActive=ETrue;
    After(KSisLauncherLongShutdownDelay);
    }


void CSisLauncherServer::RunExecutableL(const TDesC& aFileName, TBool aWait)
    {
    DEBUG_CODE_SECTION(
        if (aWait)
            {
            DEBUG_PRINTF2(_L("Sis Launcher Server - Attempting to run executable by filename ('%S'), waiting for termination."),
                &aFileName);
            }
        else
            {
            DEBUG_PRINTF2(_L("Sis Launcher Server - Attempting to run executable by filename ('%S'), not waiting for termination."),
                &aFileName);
            }
    ); // DEBUG_CODE_SECTION
    
    RProcess process;
    _LIT(KNullArgs, "");
    User::LeaveIfError(process.Create(aFileName, KNullArgs));
    CleanupClosePushL(process);
    
    // Get the thread id of the main thread in the process
    // From CApaExeRecognizer::RunL 
    TFullName fullName(process.Name());
    _LIT(KCCMain,"::Main");
    fullName.Append(KCCMain);
    TFindThread fT(fullName);
    User::LeaveIfError(fT.Next(fullName));
    RThread thread;
    User::LeaveIfError(thread.Open(fT));
    TThreadId threadId = thread.Id();
    thread.Close();
    process.Resume();
    
    CleanupStack::PopAndDestroy(&process);
    if (aWait)
        HandleShutdownL(threadId, ETrue);
    }

void CSisLauncherServer::ForceShutdownL(TUid aUid)
    {
    TBool needToScanFullList;
    TFullName fullName;
    do
        {
        needToScanFullList = EFalse;
        TFindProcess findProcess;

        while(findProcess.Next(fullName) == KErrNone)
            {
            RProcess process;
            User::LeaveIfError(process.Open(findProcess));
            TUid sid(process.SecureId());
            if (sid == aUid && process.ExitType() == EExitPending)
                {
                process.Kill(KErrNone);
                needToScanFullList = ETrue;
                }
            process.Close();
            }
        } while (needToScanFullList);
    }


void CSisLauncherServer::HandleShutdownL(TThreadId aThread, TBool aKillOnTimeout)
    {
    RWsSession* wsSession = NULL;
    if (iBootMode == KTextShell) 
        {
        // emulator tests running in textshell or in textshell ROM (#def SWI_TEXTSHELL_ROM)
        DEBUG_PRINTF(_L8("Sis Launcher Server - textshell - skipping shutdown of user applications"));  
        }
    else
        {
        #ifndef SWI_TEXTSHELL_ROM
            DEBUG_CODE_SECTION(
                if (aKillOnTimeout)
                    {
                    DEBUG_PRINTF(_L8("Sis Launcher Server - Awaiting thread shutdown, will forcibly kill process on timeout."));
                    }
                else
                    {
                    DEBUG_PRINTF(_L8("Sis Launcher Server - Awaiting thread shutdown, will not forcibly kill process on timeout."));
                    }
                );
            wsSession = &iWsSession;    
        #endif
        }
    // Read timeout value from software install security
    // policy.
    CSecurityPolicy* secPolicy = CSecurityPolicy::GetSecurityPolicyL();
    TInt runWaitTimeout = secPolicy->RunWaitTimeout();
    TInt shutdownTimeout = secPolicy->ApplicationShutdownTimeout();
    // Wait until the thread finishes or a timeout occurs
    CThreadMonitor* threadMonitor = CThreadMonitor::NewLC(aThread, wsSession);
    threadMonitor->SyncShutdownL(runWaitTimeout, aKillOnTimeout, shutdownTimeout);
    CleanupStack::PopAndDestroy(threadMonitor);
    }

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
CPolicyServer::TCustomResult CSisLauncherServer::CustomSecurityCheckL(const RMessage2& aMsg, 
    TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
    {
    TUint32 secureId = aMsg.SecureId().iId;
    if (secureId == KInstallServerUid || secureId == KSisRegistryServerUid)
        {
        return CPolicyServer::EPass;
        }
    else
        {
        //client accessing the function is neither Install Server nor Sis Registry
        return CPolicyServer::EFail;
        }
    }

const RArray<TLanguage>& CSisLauncherServer::DeviceSupportedLanguages() const
    {
    return iDeviceSupportedLanguages;
    }

void CSisLauncherServer::ResetInstalledLanguagesL()
    {
    _LIT(KLanguagesIni, "z:\\resource\\bootdata\\languages.txt");
    const TInt KReadBufSize = 10;
    
    iDeviceSupportedLanguages.Reset();
    
    RFile file;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    TInt err = file.Open(fs, KLanguagesIni, EFileRead|EFileShareReadersOnly);
    if (KErrNone == err)
        {
        CleanupClosePushL(file);
        
        TFileText reader;
        reader.Set(file);
        err = reader.Seek(ESeekStart);
        if (KErrNone == err)
            {
            TBuf<KReadBufSize> readBuf;
            while(KErrNone == reader.Read(readBuf))
                {
                if (readBuf.Length() > 0)
                    {
                    TLex lex(readBuf);
                    lex.SkipSpace();
                    TInt language;
                    err = lex.Val(language);
                    if (KErrNone != err)
                        {
                        readBuf.Zero();
                        continue; // Read the next line
                        }
                    iDeviceSupportedLanguages.AppendL((TLanguage)language);
                    }
                readBuf.Zero();
                }
            }
        else
            {
            DEBUG_PRINTF3(_L("Reading %S failed with %d"), &KLanguagesIni, err);
            }
        
        CleanupStack::PopAndDestroy(&file);
        }
    else
        {
        DEBUG_PRINTF3(_L("Opening %S failed with %d"), &KLanguagesIni, err);
        }
    CleanupStack::PopAndDestroy(&fs);
    // If we are not able fetch the device languages, just parse for the current device language
    if (0 == iDeviceSupportedLanguages.Count())
        {
        iDeviceSupportedLanguages.AppendL(User::Language());
        }
    }
#endif

#ifndef SWI_TEXTSHELL_ROM
void CSisLauncherServer::StartDocumentL(RFile& aFile, TBool aWait)
    {
    DEBUG_CODE_SECTION(
    if (aWait)
        {
        DEBUG_PRINTF(_L8("Sis Launcher Server - Attempting to start document by filehandle, waiting for termination."));
        }
    else
        {
        DEBUG_PRINTF(_L8("Sis Launcher Server - Attempting to start document by filehandle, not waiting for termination."));
        }
    );

    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    TThreadId threadId; 
    User::LeaveIfError(apaSession.StartDocument(aFile, threadId));

    CleanupStack::PopAndDestroy(&apaSession);
    if (aWait)
        HandleShutdownL(threadId);
    }

void CSisLauncherServer::StartByMimeL(RFile& aFile, TDesC8& aMimeType, TBool aWait)
    {
    DEBUG_CODE_SECTION(
    if (aWait)
        {
        DEBUG_PRINTF2(_L8("Sis Launcher Server - Attempting to start document by mimetype '%S' (filehandle supplied), waiting for termination."),
            &aMimeType);
        }
    else
        {
        DEBUG_PRINTF2(_L8("Sis Launcher Server - Attempting to start document by mimetype '%S' (filehandle supplied), not waiting for termination."),
            &aMimeType);
        }
    );

    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    TThreadId threadId;
    TDataType dataType = TDataType(aMimeType);  
    User::LeaveIfError(apaSession.StartDocument(aFile, dataType, threadId));

    CleanupStack::PopAndDestroy(&apaSession);
    if (aWait)
        HandleShutdownL(threadId);
    }
    
void CSisLauncherServer::StartDocumentL(const TDesC& aFileName, TBool aWait)
    {
    DEBUG_CODE_SECTION(
    if (aWait)
        {
        DEBUG_PRINTF2(_L("Sis Launcher Server - Attempting to start document by filename '%S', waiting for termination."),
            &aFileName);
        }
    else
        {
        DEBUG_PRINTF2(_L("Sis Launcher Server - Attempting to start document by filename '%S', not waiting for termination."),
            &aFileName);
        }
    );

    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    TThreadId threadId;
    User::LeaveIfError(apaSession.StartDocument(aFileName, threadId));

    CleanupStack::PopAndDestroy(&apaSession);
    if (aWait)
        HandleShutdownL(threadId);
    }

void CSisLauncherServer::StartByMimeL(const TDesC& aFileName, TDesC8& aMimeType, TBool aWait)
    {
    DEBUG_CODE_SECTION(
    if (aWait)
        {
        DEBUG_PRINTF2(_L("Sis Launcher Server - Attempting to start document by filename '%S', waiting for termination."),
            &aFileName);
        }
    else
        {
        DEBUG_PRINTF2(_L("Sis Launcher Server - Attempting to start document by filename '%S', not waiting for termination."),
            &aFileName);
        }
    );
    DEBUG_PRINTF2(_L8("Sis Launcher Server - Supplied MIME type is '%S'."), &aMimeType);    

    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    TThreadId threadId;
    TDataType dataType = TDataType(aMimeType);
    User::LeaveIfError(apaSession.StartDocument(aFileName, dataType, threadId));

    CleanupStack::PopAndDestroy(&apaSession);
    if (aWait)
        HandleShutdownL(threadId);
    }

void CSisLauncherServer::ShutdownL(TUid aUid, TInt aTimeout)
    {
    // This method shuts down running exes matching the SID provided in aUid.
    // It first attempts a graceful shutdown, killing processes if a graceful
    // shutdown is not supported or fails to work within aTimeout microseconds.

    DEBUG_PRINTF2(_L8("Sis Launcher Server - Attempting to kill process with SID: 0x%08x."),
        aUid.iUid);

    TInt wgId=0;

    CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(iWsSession);
    CleanupStack::PushL(wgName);
    CApaWindowGroupName::FindByAppUid(aUid, iWsSession, wgId);
    
    while (wgId != KErrNotFound)
        {   
        wgName->ConstructFromWgIdL(wgId);
        if(wgName->RespondsToShutdownEvent())
            {
            TApaTask task(iWsSession);
            task.SetWgId(wgId);

            RThread thread;
            User::LeaveIfError(thread.Open(task.ThreadId()));
            CleanupClosePushL(thread);
            
            RProcess process;
            User::LeaveIfError(thread.Process(process));
            CleanupClosePushL(process);

            TRequestStatus processStatus;
            process.Logon(processStatus);
            
            task.SendSystemEvent(EApaSystemEventShutdown);

            RTimer timer;
            CleanupClosePushL(timer);
            TRequestStatus timerStatus;
            timer.CreateLocal();
            timer.After(timerStatus, aTimeout);

            User::WaitForRequest(processStatus,timerStatus);

            if (processStatus==KRequestPending)
                {
                // Failed to terminate gracefully, so kill the task.
                DEBUG_PRINTF(_L8("Sis Launcher Server - Process did not die before timeout. Forcibly killing it."));
                process.Kill(KErrNone);
                }
            else if (timerStatus==KRequestPending)
                {
                // Rendezvous completed so cancel timer
                timer.Cancel();
                }
            // Handle second request
            User::WaitForRequest(processStatus,timerStatus);
            CleanupStack::PopAndDestroy(3, &thread);
            }

        // See if there's another instance of this App running.
        CApaWindowGroupName::FindByAppUid(aUid, iWsSession, wgId);
        }
    CleanupStack::PopAndDestroy(wgName);
    }

void CSisLauncherServer::ShutdownL()
    {
    TInt wgId=0;
    
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(iWsSession);
    CleanupStack::PushL(wgName);
    TBuf<1> matchAny;
    matchAny.Append(KMatchAny);
    CApaWindowGroupName::FindByCaption(matchAny, iWsSession, wgId);
    while (wgId != KErrNotFound)
        {
        wgName->ConstructFromWgIdL(wgId);
        //DEF057706 - shut down hidden apps during uninstallation if
        // SH flag is specified
        if (!wgName->IsSystem() /* && !wgName->Hidden() */)
            {
            // leave if we cannot shutdown the app because its busy
            if(!wgName->IsBusy())
                {
                TApaTask task(iWsSession);
                task.SetWgId(wgId);
                // show shutdown dialog for this app?
                task.SendSystemEvent(EApaSystemEventShutdown);
                }
            else
                {
                // could not shutdown an app
                User::Leave(KErrInUse);
                }
            }
    
        // get next app to shutdown
        CApaWindowGroupName::FindByCaption(matchAny, iWsSession, wgId);
        }
    CleanupStack::PopAndDestroy(wgName);
    }

void CSisLauncherServer::NotifyNewAppsL(const RPointerArray<TDesC>& aFiles)
    {
    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    // UI frameworks advise ignoring the return code
    apaSession.ForceRegistration(aFiles);

    CleanupStack::PopAndDestroy();
    }

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void CSisLauncherServer::NotifyNewAppsL(const RPointerArray<Usif::CApplicationRegistrationData>& aApplicationRegistrationData)
    {
    RApaLsSession apaSession;
    User::LeaveIfError(apaSession.Connect());
    CleanupClosePushL(apaSession);

    // UI frameworks advise ignoring the return code
    // Proceeding even if force registration fails so that installation is not aborted
    apaSession.ForceRegistration(aApplicationRegistrationData);
    CleanupStack::PopAndDestroy();
    }
#endif
#endif

    
