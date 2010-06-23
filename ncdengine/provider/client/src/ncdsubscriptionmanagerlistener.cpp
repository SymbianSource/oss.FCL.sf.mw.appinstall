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
* Description:   Implementation of CNcdSubscriptionManagerListener class
*
*/


#include "ncdsubscriptionmanagerlistener.h"
#include "ncdsubscriptionmanagerproxy.h"
#include "catalogsclientserver.h"
#include "catalogsdebug.h"

CNcdSubscriptionManagerListener* CNcdSubscriptionManagerListener::NewL(
    CNcdSubscriptionManagerProxy& aParent ) 
    {
    DLTRACEIN((""));
    CNcdSubscriptionManagerListener* self = 
        new( ELeave ) CNcdSubscriptionManagerListener( aParent );        

    CActiveScheduler::Add( self );    
    return self;
    }
    
CNcdSubscriptionManagerListener::~CNcdSubscriptionManagerListener() 
    {
    Cancel();
    }
    
void CNcdSubscriptionManagerListener::Activate() 
    {
    DLTRACEIN((""));
    SetActive();
    }
    
CNcdSubscriptionManagerListener::CNcdSubscriptionManagerListener(
    CNcdSubscriptionManagerProxy& aParent )
    : CActive( EPriorityNormal ), iParent( aParent ) 
    {
    }
    
void CNcdSubscriptionManagerListener::RunL() 
    {
    DLTRACEIN((("iStatus: %d"), iStatus.Int()));
    if ( iStatus.Int() == KErrServerTerminated ) 
        {
        // Server was terminated, don't try to internalize subscriptions.
        DLINFO(("Server terminated!"));
        return;
        }
        
    iParent.SubscriptionsChangedL();
    }
    
void CNcdSubscriptionManagerListener::DoCancel() 
    {
    DLTRACEIN((""));    
    iParent.ClientServerSession().AsyncMessageSenderDown( iStatus ); 
    }
