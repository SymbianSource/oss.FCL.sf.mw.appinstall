/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of background checker
*
*/


#include    <e32std.h>
#include    <centralrepository.h>
#include    "iaupdatebgremindertimer.h"
#include    "iaupdateprivatecrkeys.h"
#include    "iaupdatebgconst.h"

// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::NewL()
// ----------------------------------------------------------
CIAUpdateBGReminderTimer* CIAUpdateBGReminderTimer::NewL(MIAUpdateBGReminderTimerCallBack* aCallback)
    {
    CIAUpdateBGReminderTimer* self = new(ELeave)CIAUpdateBGReminderTimer(aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    

// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::ConstructL()
// ----------------------------------------------------------
void CIAUpdateBGReminderTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    iInternalFile = CIAUpdateBGInternalFileHandler::NewL();
    }
    
    
// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::~CIAUpdateBGReminderTimer()
// ----------------------------------------------------------
CIAUpdateBGReminderTimer::~CIAUpdateBGReminderTimer()
    {
    Cancel();
    delete iInternalFile;
    }


// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::CIAUpdateBGReminderTimer()
// ----------------------------------------------------------
CIAUpdateBGReminderTimer::CIAUpdateBGReminderTimer(MIAUpdateBGReminderTimerCallBack* aCallback):CTimer( EPriorityStandard ), iCallback ( aCallback )
    {   
    }


// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::RunL()
// ----------------------------------------------------------
void CIAUpdateBGReminderTimer::RunL()
    {
    
    if( iRuns == 0 )
        {
        //timer expired call the callback
        iCallback->ReminderTimerCallBack();
        }
    else
        {
        TTime currenttime;
        currenttime.UniversalTime();
                 
        if ( currenttime >= iNextRemindTime )
            {
            StartL( StartNow );
            }
        else
            {
            iInternalFile->ReadControllerDataL();
            TTime nextremindtime = iInternalFile->NextRemindTime();
            TTimeIntervalMinutes timetowait;
            currenttime.MinutesFrom( nextremindtime, timetowait );
            StartL( timetowait );
            }                           
         }  
    }
  
// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::DoCancel()
// ----------------------------------------------------------
void CIAUpdateBGReminderTimer::DoCancel()
    {
    }
// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::RunError()
// ----------------------------------------------------------
TInt CIAUpdateBGReminderTimer::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::StartReminderTimerL()
// ----------------------------------------------------------
void CIAUpdateBGReminderTimer::StartReminderTimerL(TTimeIntervalMinutes& aTimetoWait )
    {
    if ( IsActive() )
        {
        Cancel();
        }
    
    //show after one week
    StartL( aTimetoWait );    
    }


// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::CancelReminderTimerL()
// ----------------------------------------------------------
void CIAUpdateBGReminderTimer::CancelReminderTimerL()
    {    
    Cancel();
    }


// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::StartL()
// ----------------------------------------------------------
void CIAUpdateBGReminderTimer::StartL( TTimeIntervalMinutes aWaitTime )
    {   
    Cancel();
    /*
    RTimer::After maximum delay is 35 minutes, 47 seconds this is because it is 32bit integer. 
    Thats why aWaitTime is divided to 30min runs.  
    */
    iRuns = 0;
    if(aWaitTime.Int() > HalfHourInMinuts )
        {
        iRuns = aWaitTime.Int() / HalfHourInMinuts;
        TTimeIntervalMinutes halfhour( HalfHourInMinuts );    
        After( ConvertToMicroseconds( halfhour ) );
        }
    else{
        After( ConvertToMicroseconds( aWaitTime ) );
        }

    }

// ----------------------------------------------------------
// CIAUpdateBGReminderTimer::ConvertToMicroseconds()
// ----------------------------------------------------------
TTimeIntervalMicroSeconds32 CIAUpdateBGReminderTimer::ConvertToMicroseconds( TTimeIntervalMinutes aInterval )
    {
    return aInterval.Int()*60*1000*1000;
    }

// End of file

