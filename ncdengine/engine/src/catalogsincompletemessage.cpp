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
* Description:  
*
*/


#include "catalogsincompletemessage.h"
#include "catalogsclientserverserversession.h"


// ======== MEMBER FUNCTIONS ========


CCatalogsIncompleteMessage::CCatalogsIncompleteMessage() :
    iMessageType( -1 )
    {
    }

void CCatalogsIncompleteMessage::ConstructL()
    {
    }

CCatalogsIncompleteMessage* CCatalogsIncompleteMessage::NewL()
    {
    CCatalogsIncompleteMessage* self = CCatalogsIncompleteMessage::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CCatalogsIncompleteMessage* CCatalogsIncompleteMessage::NewLC()
    {
    CCatalogsIncompleteMessage* self = 
        new( ELeave ) CCatalogsIncompleteMessage();
    //CleanupStack::PushL( self );
    TCleanupItem cleanupCObjectInfo( 
        CCatalogsClientServerServerSession::CleanupCObjectBasedClass,
                                 self );
    CleanupStack::PushL( cleanupCObjectInfo );    
    self->ConstructL();
    return self;
    }

CCatalogsIncompleteMessage::~CCatalogsIncompleteMessage()
    {
    delete iMessage;
    iMessage = NULL;
    delete iMessageWide;
    iMessageWide = NULL;
    }

void CCatalogsIncompleteMessage::SetMessageL( const TDesC8& aMessage, 
                                              TInt aReturnValue )
    {
    // Only one message can be stored at any one moment
    if ( iMessageType != -1 )
        {
        User::Leave( KErrGeneral );
        }
    iMessage = aMessage.AllocL();
    iReturnValue = aReturnValue;
    iMessageType = 8;
    }

void CCatalogsIncompleteMessage::SetMessageL( const TDesC16& aMessage, 
                                              TInt aReturnValue )
    {
    // Only one message can be stored at any one moment
    if ( iMessageType != -1 )
        {
        User::Leave( KErrGeneral );
        }
    iMessageWide = aMessage.AllocL();
    iReturnValue = aReturnValue;
    iMessageType = 16;
    }

TInt CCatalogsIncompleteMessage::ReturnValue() const
    {
    return iReturnValue;
    }

const TDesC8& CCatalogsIncompleteMessage::Message() const
    {
    return *iMessage; 
    }
const TDesC16& CCatalogsIncompleteMessage::MessageWide() const
    {
    return *iMessageWide; 
    }
