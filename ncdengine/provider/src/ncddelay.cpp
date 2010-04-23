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
* Description:   Implementation of CNcdDelay class.
*
*/


#include "ncddelay.h"

CNcdDelay* CNcdDelay::NewL( TCallBack aCallBack )
    {
    CNcdDelay* self = new ( ELeave ) CNcdDelay( aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcdDelay::CNcdDelay( TCallBack aCallBack ) 
    : CTimer( EPriorityStandard ),
      iCallBack( aCallBack )
    {
    CActiveScheduler::Add( this );
    }
 
void CNcdDelay::RunL()
    {
    iCallBack.CallBack();
    }
