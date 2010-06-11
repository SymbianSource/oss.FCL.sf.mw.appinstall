/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   COsmDelay implementation
*
*/


#include "OsmDelay.h"

static TInt DelayTimerCallBack( TAny* /*aParam*/ )
    {
    CActiveScheduler::Stop();
    return 0;
    }

void COsmDelay::SynchronousWaitL( TInt aTime )
    {
    COsmDelay* self = NewL( TCallBack( DelayTimerCallBack, 0 ) );
    CleanupStack::PushL( self );
    self->After( aTime );
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( self );
    }


COsmDelay* COsmDelay::NewL( TCallBack aCallBack )
    {
    COsmDelay* self = new(ELeave) COsmDelay( aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

COsmDelay::COsmDelay( TCallBack aCallBack ) 
    : CTimer( EPriorityStandard ),
      iCallBack( aCallBack )
    {
    CActiveScheduler::Add( this );
    }
 
void COsmDelay::RunL()
    {
    iCallBack.CallBack();
    }


