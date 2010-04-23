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
* Description:   ?description
*
*/


#ifndef C_CATALOGS_INCOMPLETE_MESSAGE_H
#define C_CATALOGS_INCOMPLETE_MESSAGE_H

#include "e32base.h"


/**
 *  Class used to represent an incomplete message
 *
 *  
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsIncompleteMessage : public CObject
    {


public:

    static CCatalogsIncompleteMessage* NewL();

    static CCatalogsIncompleteMessage* NewLC();

    virtual ~CCatalogsIncompleteMessage();

    /**
     * A function to set the message and error value of the message.
     * 8-bit variant.
     * NOTICE: Only one message can be set. Leaves if one is already
     *         set.
     *
     * @since S60 ?S60_version
     */
    void SetMessageL( const TDesC8& aMessage, TInt aReturnValue );

    /**
     * A function to set the message and error value of the message.
     * 16-bit variant.
     * NOTICE: Only one message can be set. Leaves if one is already
     *         set.
     *
     * @since S60 ?S60_version
     */
    void SetMessageL( const TDesC16& aMessage, TInt aReturnValue );

    /**
     * A function to get the error value of the message.
     *
     * @since S60 ?S60_version
     * @return Symbian error code received along the message.
     */
    TInt ReturnValue() const;
    
    /**
     * A function to get the the message if it is 8-bit.
     *
     * @since S60 ?S60_version
     * @return Actual message.
     */    
    const TDesC8& Message() const;

    /**
     * A function to get the the message if it is 16-bit.
     *
     * @since S60 ?S60_version
     * @return Actual message.
     */    
    const TDesC16& MessageWide() const;    
    
protected:


private:

    CCatalogsIncompleteMessage();
    void ConstructL();


private: // data

    /**
     * Descriptor that contains message-part of this incomplete message
     * if it is 8-bit.
     * Own.
     */
    HBufC8* iMessage;
    /**
     * Descriptor that contains message-part of this incomplete message
     * if it is 16-bit.
     * Own.
     */
    HBufC16* iMessageWide;
    /**
     * Symbian error-code that contains completion value of this message.
     */
    TInt iReturnValue;
    /**
     * Integer to represent message type.
     */    
    TInt iMessageType;

    };


#endif // C_CATALOGS_INCOMPLETE_MESSAGE_H
