/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The main header file of background timer
*
*/




#ifndef IAUPDATEBGREMINDERTIMER_H
#define IAUPDATEBGREMINDERTIMER_H

#include <e32base.h>
#include "iaupdatebginternalfilehandler.h"


// INCLUDE FILES 
class MIAUpdateBGReminderTimerCallBack
    {
    public:
        virtual void ReminderTimerCallBack() = 0;
    };


class CIAUpdateBGReminderTimer : public CTimer
            
    {
    public:

         static CIAUpdateBGReminderTimer* NewL( MIAUpdateBGReminderTimerCallBack* aCallback );  
        
         virtual ~CIAUpdateBGReminderTimer();   
          
         void StartReminderTimerL( TTimeIntervalMinutes& aTimetoWait );
         
         void CancelReminderTimerL();
         
    private:
    
        CIAUpdateBGReminderTimer( MIAUpdateBGReminderTimerCallBack* aCallback );

        void ConstructL();  
        
        void RunL();
  
        void DoCancel();

        TInt RunError(TInt aError);
        
        void StartL( TTimeIntervalMinutes aWaitTime );
        
        TTimeIntervalMicroSeconds32 ConvertToMicroseconds( TTimeIntervalMinutes aInterval );

    private:

        TTime iNextRemindTime;
        TInt iRuns;
        CIAUpdateBGInternalFileHandler* iInternalFile;
        MIAUpdateBGReminderTimerCallBack* iCallback; 
    };        

#endif //IAUPDATEBGREMINDERTIMER_H
