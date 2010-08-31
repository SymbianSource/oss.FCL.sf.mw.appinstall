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
* Description:    
*
*/



#ifndef IAUPDATEBGCONST_H
#define IAUPDATEBGCONST_H


//CONST
//#define _DEMO

#ifdef _DEMO
const TInt KDelayForUserToAcceptDisclaimer = 1; //10; //10 minutes
const TInt KCheckingUpdateInterval = 4; //Set in Cenrep by default 4 mins
const TInt KUpdateAvailableReminderInterval = 1; //60*24*7; //one week in minutes
const TInt KFirstTimeDialogDelay = 1; //60*24*2; //two days in minutes
const TInt KFirstTimeDialogReminderInterval = 4; //60*24*7*4; //four weeks in minutes
const TInt HalfHourInMinuts = 5;//30;
const TInt KFirstSecondRetryInterval = 1; //30mins
const TInt KThirdFourthRetryInterval = 2; //one day in minutes
#else

const TInt KDelayForUserToAcceptDisclaimer = 10; //10 minutes
const TInt HalfHourInMinuts = 30;
const TInt KFirstSecondRetryInterval = 30; //30mins
const TInt KThirdFourthRetryInterval = 24*60; //one day in minutes
#endif //_DEMO

const TInt StartNow = 0;
const TInt KMaxRetry = 4; //Max times of retry when refresh fails for some reason
const TInt StartAfterOneMin = 1; //1mins



#endif //IAUPDATEBGCONST_H


