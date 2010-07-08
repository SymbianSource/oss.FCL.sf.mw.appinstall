/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistryAccess - server and implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <usif/sts/sts.h>
#include "sisregistryaccess_server_session.h"
#include "sisregistrypackage.h"
#include "sisregistryaccess_server_session.h"
#include "sisregistryaccess_common.h"
#include "dessisdataprovider.h"
#include "siscontroller.h"
#include "application.h"
#include "userselections.h"
#include "sislauncherclient.h"

/////////////////////// Utility functions //////////////////////////////
void StartTimer(TTime& aTimer)
	{
	aTimer.HomeTime();
	}

TInt StopTimer(TTime aStartTimer)
	{
	TTime endTime;
	endTime.HomeTime();
		
	TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(aStartTimer);
	TInt actualDuration = I64INT(duration.Int64())/1000; // in millisecond
	return actualDuration;
	}

//////////////////////// Server implementation /////////////////////////

CServer2* CSisRegistryAccessServer::NewLC()
	{
	CSisRegistryAccessServer* self=new(ELeave) CSisRegistryAccessServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSisRegistryAccessServer::CSisRegistryAccessServer()
	:CServer2(0, ESharableSessions)
	{
	}

CSisRegistryAccessServer::~CSisRegistryAccessServer()
	{
	iRegistry.Close();
	}

void CSisRegistryAccessServer::ConstructL()
	{
	StartL(KSisRegistryAccessServerName);
	User::LeaveIfError(iRegistry.Connect());
	}

CSession2* CSisRegistryAccessServer::NewSessionL(const TVersion&, const RMessage2&) const
	{
	return new(ELeave) CSisRegistryAccessSession();
	}

void CSisRegistryAccessServer::AddSession()
	{
	++iSessionCount;
	}

void CSisRegistryAccessServer::DropSession()
	{
	--iSessionCount;
	if(!iSessionCount)
		CActiveScheduler::Stop();
	}


//////////////////////// Session implementation /////////////////////////
CSisRegistryAccessSession::CSisRegistryAccessSession()
	{
	}

void CSisRegistryAccessSession::CreateL()
	{
	Server().AddSession();
	}

CSisRegistryAccessSession::~CSisRegistryAccessSession()
	{
	Server().DropSession();
	}

void CSisRegistryAccessSession::ServiceL(const RMessage2& aMessage)
	{
    TSisRegistryAccessMessages f = static_cast<TSisRegistryAccessMessages>(aMessage.Function());
    TTime timer;
    StartTimer(timer);
    TInt err(0);
    if(f == EAddAppRegInfo)
        {
        TUint regFileNameLen = aMessage.GetDesLengthL(0);
        HBufC* regFileName = HBufC::NewLC(regFileNameLen);
        TPtr namePtr = regFileName->Des();
        aMessage.ReadL(0, namePtr);
        TUid appUid = TUid::Uid(aMessage.Int1());
        Swi::RSisRegistrySession sisRegistryWritableSession;
        CleanupClosePushL(sisRegistryWritableSession);
        TRAP(err, sisRegistryWritableSession.Connect());
        TRAP(err, sisRegistryWritableSession.AddAppRegInfoL(*regFileName););
        CleanupStack::PopAndDestroy(2, regFileName);
        }
    else if(f == ERemoveAppRegInfo)
        {
        TUint regFileNameLen = aMessage.GetDesLengthL(0);
        HBufC* regFileName = HBufC::NewLC(regFileNameLen);
        TPtr namePtr = regFileName->Des();
        aMessage.ReadL(0, namePtr);
        TUid appUid = TUid::Uid(aMessage.Int1());
        Swi::RSisRegistrySession sisRegistryWritableSession;
        CleanupClosePushL(sisRegistryWritableSession);
        TRAP(err, sisRegistryWritableSession.Connect());
        TRAP(err, sisRegistryWritableSession.RemoveAppRegInfoL(*regFileName););
        CleanupStack::PopAndDestroy(2, regFileName);
        }
    else
        {
        // create transaction on SCR for mutable operations
        Server().RegistrySession().CreateTransactionL();
        // SWI regsitry needs a transaction to cover updates to logs during package add/update/removal
        Usif::RStsSession stsSession;
        TInt64 transactionID = stsSession.CreateTransactionL();
        CleanupClosePushL(stsSession);
        
        switch (f)
            {
            case EAddEntry:
            case EUpdateEntry:
                {
                TInt len = aMessage.GetDesLengthL(0);
                HBufC8 *controllerData = HBufC8::NewLC(len);
                TPtr8 ptrControllerData(controllerData->Des());
                aMessage.ReadL(0, ptrControllerData);
                
                // create the controller
                Swi::CDesDataProvider *desProvider = Swi::CDesDataProvider::NewLC(ptrControllerData);
                Swi::Sis::CController *controller = Swi::Sis::CController::NewLC(*desProvider, Swi::Sis::EAssumeType);
                // setup the application
                Swi::CApplication *app = Swi::CApplication::NewLC();
                app->SetInstall(*controller);
                // since the attributes chosen by the user don't have impact on the adding/updating entry tests,
                // they are set with the constant values.
                app->UserSelections().SetLanguage(ELangEnglish);
                app->UserSelections().SetDrive(2);
                
                if(EAddEntry == f)
                    {
                    TRAP(err, Server().RegistrySession().AddEntryL(*app, ptrControllerData, transactionID));
                    }
                else
                    {
                    TRAP(err, Server().RegistrySession().UpdateEntryL(*app, ptrControllerData, transactionID));
                    }
                CleanupStack::PopAndDestroy(4, controllerData); // controllerData, desProvider, controller, app
                break;
                }
            case EDeleteEntry:
                {
                // Get Uid and index
                // Augmentation index - argument 0
                TInt packageIndex = aMessage.Int0();
    
                // Uid (ipc argument 1)
                TUid uid;
                TPckg<TUid> packageUid(uid);
                aMessage.ReadL(1, packageUid);
                
                // Invoke SisRegistry server API using SisRegistryClient
                Swi::CSisRegistryPackage* package = Swi::CSisRegistryPackage::NewLC(uid, KNullDesC, KNullDesC); // For the delete API, the name and the vendor of the package are irrelevant
                package->SetIndex(packageIndex);
                TRAP(err, Server().RegistrySession().DeleteEntryL(*package, transactionID));
    
                CleanupStack::PopAndDestroy(package);
                break;
                }
            default:
                {
                PanicClient(aMessage, EPanicIllegalFunction);
                break;
                }
            }
        
        stsSession.CommitL();
        CleanupStack::PopAndDestroy(&stsSession);
        Server().RegistrySession().CommitTransactionL();
        }	
	TPckg<TInt> timePckg(StopTimer(timer));
	if(EDeleteEntry == f)
		aMessage.WriteL(2, timePckg);
	else
		aMessage.WriteL(1, timePckg);

	aMessage.Complete(err);
	}

void CSisRegistryAccessSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError == KErrBadDescriptor)
		PanicClient(aMessage, EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}



