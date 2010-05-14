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
* Description:   Declaration of CCatalogsBaseMessageImpl
*
*/


#ifndef C_CATALOGS_BASE_MESSAGE_IMPL_H
#define C_CATALOGS_BASE_MESSAGE_IMPL_H

#include "catalogsbasemessage.h"

class RMessage2;
class CCatalogsClientServerServerSession;

/**
 *  Catalogs base message implementation
 *
 *  @see MCatalogsBaseMessage 
 *  The implementation is based on encapsulation of 
 *  an RMessage2 object and delegation of methods to this instance.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CCatalogsBaseMessageImpl : public CBase, public MCatalogsBaseMessage
    {

public:

    static CCatalogsBaseMessageImpl* NewL( 
        const RMessage2& aMessage,
        CCatalogsClientServerServerSession& aSession,
        TInt aHandle );

    static CCatalogsBaseMessageImpl* NewLC(
        const RMessage2& aMessage,
        CCatalogsClientServerServerSession& aSession,
        TInt aHandle );

    
// from base class MCatalogsBaseMessage

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::InputLength
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    TInt InputLength() const;
    
    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::ReadInput
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    TInt ReadInput( TDes8& aInputData ) const;

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::ReadInput
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    TInt ReadInput( TDes16& aInputData ) const;
    
    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::Handle
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    TInt Handle() const;
    
    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CompleteAndRelease
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    void CompleteAndRelease( TInt aStatus ) const;
    
    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CompleteAndReleaseL
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    void CompleteAndReleaseL( const TDesC8& aOutputData, 
                              TInt aStatus );

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CompleteAndReleaseL
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    void CompleteAndReleaseL( TInt aOutputData, 
                              TInt aStatus );

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CompleteAndReleaseL
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    void CompleteAndReleaseL( const TDesC16& aOutputData, 
                              TInt aStatus );

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CompleteAndReleaseL
     */
    void CompleteAndReleaseL( RFs& aFs, 
                              RFile& aFile ); 
    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::Release
     *
     * @since S60 ?S60_version
     */
    void Release();

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CheckSecurityPolicy
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    TBool CheckSecurityPolicy( const TSecurityPolicy& aSecurityPolicy );
        
    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::Session
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    MCatalogsSession& Session() const;

    /**
     * From MCatalogsBaseMessage.
     * @see MCatalogsBaseMessage::CounterPartLost
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     */
    TBool CounterPartLost( const MCatalogsSession& aSession );

protected:

    virtual ~CCatalogsBaseMessageImpl();

private:

    /**
     * Constructor
     * 
     * @param aMessage the message.
     * @param aSession a reference to an object
     *        which is the session in which this message is used 
     * 
     */
    CCatalogsBaseMessageImpl( const RMessage2& aMessage,
                              CCatalogsClientServerServerSession& aSession,
                              TInt aHandle );
                              
    void ConstructL();

private: // data

    /**
     * the encapsulated message object that does
     * the real message handling
     * NOTICE: This has to be a copy. Otherwise the current
     *         message of the session or server seems to be used
     *         which is not desired. 
     */
    const RMessage2 iMessage;
    /**
     * session in which this message is used
     */
    CCatalogsClientServerServerSession& iSession;
    /**
     * handle to the receiver of this message
     */    
    TInt iHandle;
    /**
     * boolean value which tells whether the sender of this message
     * has died or not and if completion messages can act normally
     * or not. If ETrue, no messages are sent to client-side anymore.
     */        
    TBool iSenderLost;
    };

#endif // C_CATALOGS_BASE_MESSAGE_IMPL
