/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of the adorned filename handling utility functions
*
*/

#include "appregextractor.h"
#include "sishelper.h"

namespace Swi
{

/*static*/ CAppRegExtractor* CAppRegExtractor::NewLC(RFs& aFs, RPointerArray<Usif::CApplicationRegistrationData>& aApparcRegFileData )
    {
    CAppRegExtractor* self=new(ELeave) CAppRegExtractor(aFs, aApparcRegFileData);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
/*static*/ CAppRegExtractor* CAppRegExtractor::NewL(RFs& aFs, RPointerArray<Usif::CApplicationRegistrationData>& aApparcRegFileData )
    {
    CAppRegExtractor* self=NewLC(aFs, aApparcRegFileData);
    CleanupStack::Pop(self);
    return self;
    }
    
CAppRegExtractor::~CAppRegExtractor()
    {
    Deque();
    
    if (iManagedFileHandle)
        {
        iCurrentFile->Close();
        delete iCurrentFile;
        }
    
    iLauncher.Close();
    }

CAppRegExtractor::CAppRegExtractor(RFs& aFs, RPointerArray<Usif::CApplicationRegistrationData>& aApparcRegFileData)
    : CActive(EPriorityStandard), iFs(aFs), iApparcRegFileData(aApparcRegFileData)
    {
    CActiveScheduler::Add(this);
    }

void CAppRegExtractor::ConstructL()
    {
    User::LeaveIfError(iFs.ShareProtected());
    User::LeaveIfError(iLauncher.Connect());
    }

// public methods

void CAppRegExtractor::ExtractAppRegInfoSizeL(const TDesC& aFileName, TRequestStatus& aStatus)
    {
    DEBUG_PRINTF2(_L("CAppRegExtractor - ExtractAppRegInfoSizeL - extracting size of resource file '%S'"),&aFileName);
    iErrCode = KErrNone; 
    iManagedFileHandle=ETrue;
    iCancelled=EFalse;
    iClientStatus=&aStatus;
    if(iCurrentFile != NULL)
        delete iCurrentFile;
    iCurrentFile=new(ELeave) RFile;    
    User::LeaveIfError(iCurrentFile->Open(iFs, aFileName, EFileRead));
    *iClientStatus = KRequestPending;    
    iLauncher.AsyncParseResourceFileSizeL(*iCurrentFile, iStatus);
    SetActive();
    }


// CActive methods

void CAppRegExtractor::RunL()
    {
    //iStatus shall have the lenght of parsed object if it is greater than 0
    if (iStatus.Int() < KErrNone )
        {
        DEBUG_PRINTF2(_L("CAppRegExtractor - RunL - parsing failed with %d error"),iStatus.Int());
        User::LeaveIfError(FinishAppRegExtraction(iStatus.Int()));
        return;
        }
    
    if (iCancelled)
        {
        DEBUG_PRINTF(_L("CAppRegExtractor - RunL - User cancelled"));
        User::LeaveIfError(FinishAppRegExtraction(KErrCancel));
        return;
        }
    
    TInt size = iStatus.Int(); 
    Usif::CApplicationRegistrationData* applicationRegistrationData = 0;
    DEBUG_PRINTF2(_L("CAppRegExtractor - ParseResourceFileDataL - Invoking with size = %d"),size);
    applicationRegistrationData = iLauncher.AsyncParseResourceFileDataL(size);
    CleanupStack::PushL(applicationRegistrationData);
    iApparcRegFileData.AppendL(applicationRegistrationData);
    CleanupStack::Pop();
    FinishAppRegExtraction(KErrNone);
    // Extract the next chunk
    }

void CAppRegExtractor::DoCancel()
    {
    DEBUG_PRINTF(_L("CAppRegExtractor - DoCancel - User cancelled"));
    iCancelled=ETrue;
    if (iClientStatus)
        {      
        TInt err = FinishAppRegExtraction(KErrCancel);
        if (err != KErrNone)
            {
            User::RequestComplete(iClientStatus, KErrCancel);
            iClientStatus=NULL;
            }           
        }
    }
    
TInt CAppRegExtractor::RunError(TInt aError)
    {
    DEBUG_PRINTF(_L("CAppRegExtractor - DoError"));
    iCancelled = ETrue;
    if (iClientStatus)
        {     
        TInt err = FinishAppRegExtraction(aError);
        if (err != KErrNone)
            {
            User::RequestComplete(iClientStatus, aError);
            iClientStatus = NULL;
            }          
        }
    return KErrNone;
    }

TInt CAppRegExtractor::FinishAppRegExtraction(TInt aResult)
    {
    DEBUG_PRINTF2(_L("CAppRegExtractor - FinishAppRegExtraction - received result = %d"),aResult);
    if (iManagedFileHandle)
        {
        iCurrentFile->Close();
        delete iCurrentFile;
        iCurrentFile=0;
        iManagedFileHandle = EFalse;
        }
    iErrCode = aResult;

    // notify the user
    if (aResult == KErrCorrupt)
        {
        User::RequestComplete(iClientStatus, KErrNone);
        }
    else
        {
        User::RequestComplete(iClientStatus, aResult);
        }
    iClientStatus = NULL;
    return KErrNone;
    }
    
TInt CAppRegExtractor::GetErrorCode() const
    {
    return iErrCode;
    }

}

