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
* Description:   Interface MCatalogsBaseMessage declaration
*
*/


#ifndef M_CATALOGS_BASE_MESSAGE_H
#define M_CATALOGS_BASE_MESSAGE_H

#include "catalogssession.h"

class RFile;
class RFs;
class TSecurityPolicy;

/**
 *  An interface through which a message can be handled.
 *  This class should be implemented by CBase derived class.
 *
 *  This interface is used to pass the message information
 *  to a receiving object. The interface gives access to
 *  message handling functions. These functions include reading 
 *  input data, returning output data to the client and signal the
 *  client about message completion and success. Also a function to 
 *  obtain a message reciver registration interface is included to 
 *  allow registartion of new objects.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class MCatalogsBaseMessage
    {

public:

    /**
     * Returns the length of the sent input data.
     * NOTICE: The returned length is the length of the sent
     *         descriptor and so the  difference between 8 and 16 bit
     *         variants are taken into account.
     *
     * @since S60 ?S60_version
     * @return the input data length. Error code in error situations.
     */
    virtual TInt InputLength() const = 0;
    
    /**
     * Reads the sent input data
     *
     * @since S60 ?S60_version
     * @param aInputData a descriptor where the input data is
     *          to be written. The descriptor length can be obtained
     *          enquired by calling first the method InputLength.
     * @return Symbian error code.
     */
    virtual TInt ReadInput( TDes8& aInputData ) const = 0;

    /**
     * Reads the sent input data
     *
     * @since S60 ?S60_version
     * @param aInputData a descriptor where the input data is
     *          to be written. The descriptor length can be obtained
     *          enquired by calling first the method InputLength.
     * @return Symbian error code.
     */
    virtual TInt ReadInput( TDes16& aInputData ) const = 0;
 
    /**
     * Reads the handle of the object to whome this message is 
     * intended to. This is used primarily when unregistering
     * object from the session.
     *
     * @since S60 ?S60_version
     * @return Handle.
     */   
    virtual TInt Handle() const = 0;
    
    /**
     * Completes the message with the given status.
     * After this, releases implementor of this interface.
     * NOTICE: Do not use this message after this.
     *
     * @since S60 ?S60_version
     * @param aStatus message completion status
     */
    virtual void CompleteAndRelease( TInt aStatus ) const = 0;
    
    /**
     * Writes the output data to a client side output
     * buffer and completes the message with the status given.
     * After this, releases implementor of this interface.
     * NOTICE: Do not use this message after this.
     * NOTICE: If leave occurs, this message is NOT released and
     *         NOT completed.
     *
     * @since S60 ?S60_version
     * @param aOutputData a buffer that contains the output data
     * @param aStatus message completion status
     */
    virtual void CompleteAndReleaseL( const TDesC8& aOutputData,
                                      TInt aStatus ) = 0;

    /**
     * Writes the output data to a client side output
     * buffer and completes the message with the status given.
     * After this, releases implementor of this interface.
     * NOTICE: Do not use this message after this.
     * NOTICE: If leave occurs, this message is NOT released and
     *         NOT completed.
     *
     * @since S60 ?S60_version
     * @param aOutputData a buffer that contains the output data
     * @param aStatus message completion status
     */
    virtual void CompleteAndReleaseL( const TDesC16& aOutputData,
                                      TInt aStatus ) = 0;

    /**
     * Writes the output data to a client side output
     * buffer and completes the message with the status given.
     * After this, releases implementor of this interface.
     * NOTICE: Do not use this message after this.
     * NOTICE: If leave occurs, this message is NOT released and
     *         NOT completed.
     *
     * @since S60 ?S60_version
     * @param aOutputData a buffer that contains the output data
     * @param aStatus message completion status
     */
    virtual void CompleteAndReleaseL( TInt aOutputData,
                                      TInt aStatus ) = 0;

    

    /**
     * Returns the file handle to the client side
     * and completes the message with the status given.
     * After this, releases implementor of this interface.
     * NOTICE: Do not use this message after this.
     * NOTICE: If leave occurs, this message is NOT released and
     *         NOT completed.
     *
     * @since S60 ?S60_version
     * @param aFs File server session used to open the file.
     * @param aFile File handle that is shared with the client
     * @note The file server session must be shared with 
     * RFs::ShareProtected() before the file is opened with it.
     */
    virtual void CompleteAndReleaseL( RFs& aFs, RFile& aFile ) = 0;
                                      
    /**
     * Releases this message. This function is only intended for
     * special case usage when session has died and no communication
     * is allowed anymore. At that point, this function can be
     * used to release this message.
     * NOTICE: Do not use this message after this.
     *
     * @since S60 ?S60_version
     * @param aStatus message completion status
     */
    virtual void Release() = 0;

    /**
     * Checks if this message qualify the given security policies.
     *
     * @since S60 ?S60_version
     * @param aPolicy The policy is checked against the policy of this
     * message. If multiple capabilities are specified, all of them must 
     * be present in this message for the security check to succeed 
     * ('AND' relation).
     * @return TBool ETrue if the security check is success. Else EFalse.
     */
    virtual TBool CheckSecurityPolicy( const TSecurityPolicy& aSecurityPolicy ) = 0;
        
    /**
     * Returns the interface that gives access to registering
     * receiving objects and removing registered receivers
     *
     * @since S60 ?S60_version
     * @return a reference to the registering interface
     */
    virtual MCatalogsSession& Session() const = 0;

    /**
     * Function to inform that if this message is from given
     * session, then the client-side object (sender of this
     * message) has been lost. This means that this message
     * will not write anything to client-side or complete any
     * messages anymore even if CompleteAndRelease or
     * CompleteAndReleaseL are called. (this message is only
     * deleted at that point)
     *
     * If this message is not from the given session, nothing is
     * changed and normal functionality is received when
     * complete-functions are called.
     *
     * @since S60 ?S60_version
     * @param aSession Reference to the session in which the
     *                 message sender has died. This function
     *                 checks whether this message originates from
     *                 the same session or not and acts according
     *                 to it.
     * @return Returns ETrue if this message is from the given
     *         session, otherwise EFalse.
     */
    virtual TBool CounterPartLost(
        const MCatalogsSession& aSession ) = 0;

protected:

    /**
     * Destructor. Usage through this interface is not intended.
     *
     * @since S60 ?S60_version
     */    
    virtual ~MCatalogsBaseMessage() {}
    };


#endif // M_CATALOGS_BASE_MESSAGE_H
