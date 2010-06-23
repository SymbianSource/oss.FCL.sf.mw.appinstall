/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "catalogsasyncmediator.h"
#include "catalogsdebug.h"

CCatalogsAsyncMediator::CCatalogsAsyncMediator(
    TRequestStatus& aStatus,
    TCallBack aCallback ) 
    : CActive( EPriorityStandard ),
      iClientStatus( aStatus ),
      iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    aStatus = KRequestPending;
    }
    
    
CCatalogsAsyncMediator::~CCatalogsAsyncMediator()
    {
    DLTRACEIN((""));
    Cancel();
    }

void CCatalogsAsyncMediator::Activate()
    {
    DLTRACEIN((""));
    SetActive();
    }
    
void CCatalogsAsyncMediator::RunL()
    {
    DLTRACEIN(("iStatus: %d, calling Callback", iStatus.Int() ))
    // CallBack may lead to deletion of this object so
    // we take local copies of the variables
    TRequestStatus* status = &iClientStatus;
    TInt ownError = iStatus.Int();
    
    TInt err = iCallback.CallBack();
    
    if ( err == KErrNone ) 
        {     
        err = ownError;
        }
        
    DLTRACE(("Complete client status: %x", status ));
    
    User::RequestComplete( status, ownError );
    }
    
    
void CCatalogsAsyncMediator::DoCancel()    
    {
    DLTRACEIN((""));
    // Please, don't cancel before the real request
    // has been completed
    }
    
    
TInt CCatalogsAsyncMediator::RunError( TInt aError )
    {
    DLTRACEIN(("aError: %d", aError));
    return aError;
    }
