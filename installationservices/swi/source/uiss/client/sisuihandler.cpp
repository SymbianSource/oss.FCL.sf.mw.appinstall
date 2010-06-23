/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <s32mem.h>
#include <pkixvalidationresult.h>

#include "uisscommand.h"
#include "sisuihandler.h"
#include "uiss.h"
#include "uisssession.h"
#include "writestream.h"
#include "swispubsubdefs.h"
#include <e32property.h> 
#include "progressbar.h"

namespace Swi
{

// RUiHandler

EXPORT_C TInt RUiHandler::Connect()
	{
	return CreateSession(KUissServerName, TVersion(KUissServerVersionMajor, KUissServerVersionMinor, KUissServerVersionBuild));
	}

EXPORT_C void RUiHandler::Close()
	{
	RSessionBase::Close();
	}

EXPORT_C void RUiHandler::ExecuteL(CUissCmd& aCmd)
	{
	TIpcArgs args = aCmd.IpcArgsL();
	User::LeaveIfError(SendReceive(aCmd.Message(), args));
	
	aCmd.UnmarshallArgumentsL();	
	}

EXPORT_C void RUiHandler::UpdateProgressBarL(const TAppInfo& aAppInfo, TInt aAmount)
	{
	if (aAmount==0)
		{
		return;
		}
	TInt progressAmount = aAmount;

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	_LIT(KProgressorPanicDescriptor, "UISSCLIENT:The progress bar value publisher has not been set!");
	__ASSERT_ALWAYS(iPublisher, User::Panic(KProgressorPanicDescriptor,KErrAbort));
	progressAmount = iPublisher->UpdateProgressBarValueL(aAmount);	
#endif	
	
	if (progressAmount != 0)
	    {
	    CHandleInstallEvent* event=CHandleInstallEvent::NewLC(aAppInfo, EEventUpdateProgressBar, progressAmount, KNullDesC);
	    ExecuteL(*event);
	        
	    if (!event->ReturnResult())
	        {
	        User::Leave(KErrCancel);
	        }
	    CleanupStack::PopAndDestroy(event);
	    }
	}
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RUiHandler::SetProgressBarValuePublisher(CProgressBarValuePublisher* aPublisher)
	{
	iPublisher = aPublisher;
	}
#endif

// CUissCmd

EXPORT_C CUissCmd::CUissCmd(TInt aMessage)
	: iMessage(aMessage), iArgumentsMarshalled(EFalse)
	{
	}

TInt CUissCmd::Message() const
	{
	return iMessage;	
	}

const TIpcArgs& CUissCmd::IpcArgsL()
	{
	if (!iArgumentsMarshalled)
		{
		iStream=CWriteStream::NewL();
		MarshallArgumentsL(iIpcArgs);
		iArgumentsMarshalled=ETrue;
		}
	return iIpcArgs;
	}
	
EXPORT_C CUissCmd::~CUissCmd()
	{
	delete iStream;
	}

EXPORT_C void CUissCmd::UnmarshallArgumentsL()
	{
	// default base class version which does not do any unmarshalling
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
// CProgressBarValuePublisher
CProgressBarValuePublisher::CProgressBarValuePublisher()
	{
	// empty
	}

CProgressBarValuePublisher::~CProgressBarValuePublisher()
	{
	// empty
	}


EXPORT_C CProgressBarValuePublisher* CProgressBarValuePublisher::NewL()
	{
	CProgressBarValuePublisher *self = new(ELeave) CProgressBarValuePublisher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CProgressBarValuePublisher::ConstructL()
	{
	// Initialize the value of the install progress bar property
	User::LeaveIfError(RProperty::Set(KUidInstallServerCategory, KUidSwiProgressBarValueKey, 0));
	}

EXPORT_C void CProgressBarValuePublisher::SetFinalProgressBarValue(TInt aValue)
	{
	iFinalProgressValue = aValue;
	}

EXPORT_C TInt CProgressBarValuePublisher::UpdateProgressBarValueL(TInt aValue)
	{
	iCurrentProgressValue += aValue;
	TUint percentage = (iFinalProgressValue <= 0) ? 100 : (iCurrentProgressValue * 100) / iFinalProgressValue;
	if ((percentage - iLastPercentCompletion) >= KProgressBarIncrement)
	    {
	    TInt amountCompleted = iCurrentProgressValue - iLastProgressValue; 
	    iLastProgressValue = iCurrentProgressValue;
		iLastPercentCompletion = percentage;
		User::LeaveIfError(RProperty::Set(KUidInstallServerCategory, KUidSwiProgressBarValueKey, percentage));
	    return amountCompleted;
	    }
	return 0;
	}	 
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
} // namespace Swi
