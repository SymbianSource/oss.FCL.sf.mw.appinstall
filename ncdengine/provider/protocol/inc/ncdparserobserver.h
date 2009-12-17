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
* Description:   MNCDParserObserver declaration
*
*/


#ifndef NCDPARSEROBSERVER_H
#define NCDPARSEROBSERVER_H

#include <e32base.h>


class MNcdConfigurationProtocolActionRequest;
class MNcdConfigurationProtocolClientConfiguration;
class MNcdConfigurationProtocolDetail;
class MNcdConfigurationProtocolError;
class MNcdConfigurationProtocolError;
class MNcdConfigurationProtocolQuery;
class MNcdConfigurationProtocolServerDetails;
class MNcdPreminetProtocolCookie;
class MNcdPreminetProtocolDataBlock;
class MNcdPreminetProtocolDataEntity;
class MNcdPreminetProtocolDataEntity;
class MNcdPreminetProtocolError;
class MNcdPreminetProtocolFolderRef;
class MNcdPreminetProtocolInformation;
class MNcdPreminetProtocolItemRef;
class MNcdPreminetProtocolPurchaseInformation;
class MNcdPreminetProtocolPurchaseProcessed;
class MNcdPreminetProtocolSubscription;

/**
 * Main parser observer interface for watching parser events.
 */
class MNcdParserObserver
    {
public:
    /**
    * Called when an error occurs during parsing. The parsing
    * will not continue after this.
    * The error code should tell things such as:
    * - syntax error
    * - protocol error
    */
    virtual void ParseError( TInt aErrorCode ) = 0;

    /**
     * This method is called after the user has called MNCDParser::EndL()
     * and all data has been processed. 
     */
    virtual void ParseCompleteL( TInt aError ) = 0;
    };


/**
 * Observer for entities received from the parser.
 */
class MNcdParserEntityObserver
    {
public:
    /**
     * Folder ref entity.
     * @param aData Ownership is transfered.
     */
    virtual void FolderRefL( MNcdPreminetProtocolFolderRef* aData ) = 0;
    /**
     * Folder metadata entity.
     * @param aData Ownership is transfered.
     */
    virtual void FolderDataL( MNcdPreminetProtocolDataEntity* aData ) = 0;
    /**
     * Item ref entity.
     * @param aData Ownership is transfered.
     */
    virtual void ItemRefL( MNcdPreminetProtocolItemRef* aData ) = 0;
    /**
     * Item metadata entity.
     * @param aData Ownership is transfered.
     */
    virtual void ItemDataL( MNcdPreminetProtocolDataEntity* aData ) = 0;
    };

/**
 * Observer for purchase information received from the parser.
 */
class MNcdParserPurchaseObserver
    {
public:
    /**
     * Purchase information.
     * @param aData Ownership is transfered.
     */
    virtual void InformationL( 
        MNcdPreminetProtocolPurchaseInformation* aData ) = 0;

    /**
     * Processed purchase data.
     * @param aData Ownership is transfered.
     */
    virtual void ProcessedL( 
        MNcdPreminetProtocolPurchaseProcessed* aData ) = 0;
    };

/**
 * Observer for subscription information received from the parser.
 */
class MNcdParserSubscriptionObserver
    {
public:
    /**
     * Purchase information.
     * @param aData Ownership is transfered.
     */
    virtual void ValidSubscriptionL( 
        MNcdPreminetProtocolSubscription* aData ) = 0;

    /**
     * Processed purchase data.
     * @param aData Ownership is transfered.
     */
    virtual void OldSubscriptionL( 
        MNcdPreminetProtocolSubscription* aData ) = 0;
    };

/**
 * Observer for session information.
 */ 
class MNcdParserSessionObserver
    {
public:
    /**
     * Session with an id has been received.
     * @param aSessionId
     * @param aServerURI Server where this session originated.
     * @param aNameSpace
     */
    virtual void SessionL( const TDesC& aSessionId,
                           const TDesC& aServerURI,
                           const TDesC& aNameSpace ) = 0;
    };

class MNcdParserInformationObserver
    {
public:
    virtual void InformationL( 
        MNcdPreminetProtocolInformation* aData ) = 0;
    };

class MNcdParserDataBlocksObserver
    {
public:
    virtual void DataBlocksL( 
        CArrayPtr<MNcdPreminetProtocolDataBlock>* aData ) = 0;
    };

class MNcdParserErrorObserver
    {
public:
    virtual void ErrorL( 
        MNcdPreminetProtocolError* aData ) = 0;
    };

class MNcdParserQueryObserver
    {
public:
    virtual void QueryL( 
        MNcdConfigurationProtocolQuery* aData ) = 0;
    };


/**
 * Observer for configuration protocol information.
 */ 
class MNcdParserConfigurationProtocolObserver
    {
public:
    virtual void ConfigurationBeginL( const TDesC& version, 
                                      TInt expirationdelta ) = 0;
    virtual void ConfigurationQueryL( MNcdConfigurationProtocolQuery* aQuery ) = 0;

    virtual void ClientConfigurationL(
        MNcdConfigurationProtocolClientConfiguration* aConfiguration ) = 0;
    virtual void ConfigurationDetailsL( CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails ) = 0;
    virtual void ConfigurationActionRequestL( MNcdConfigurationProtocolActionRequest* aActionRequest ) = 0;
    virtual void ConfigurationErrorL( MNcdConfigurationProtocolError* aError ) = 0;
    virtual void ConfigurationServerDetailsL( MNcdConfigurationProtocolServerDetails* aServerDetails ) = 0;
    virtual void ConfigurationEndL() = 0;
    };

class MNcdParserObserverBundle
    {
public:
    virtual ~MNcdParserObserverBundle() {}

    /**
     * Sets the main observer. 
     * This is mandatory prior to beginning the parsing!
     * @param aObserver
     */ 
    virtual void SetParserObserver( MNcdParserObserver* aObserver ) = 0;
    /**
     * Retrieves the observer pointer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserObserver* ParserObserver() const = 0;

    /**
     * Sets the entity observer.
     * @param aObserver
     */
    virtual void SetEntityObserver( MNcdParserEntityObserver* aObserver ) = 0;
    /**
     * Retrieves the entity observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserEntityObserver* EntityObserver() const = 0;

    /**
     * Sets the purchase observer.
     * @param aObserver
     */
    virtual void SetPurchaseObserver( MNcdParserPurchaseObserver* aObserver ) = 0;
    /**
     * Retrieves the purchase observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserPurchaseObserver* PurchaseObserver() const = 0;

    /**
     * Sets the subscription observer.
     * @param aObserver
     */
    virtual void SetSubscriptionObserver( MNcdParserSubscriptionObserver* aObserver ) = 0;
    /**
     * Retrieves the subscription observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserSubscriptionObserver* SubscriptionObserver() const = 0;

    /**
     * Sets the session observer.
     * @param aObserver
     */
    virtual void SetSessionObserver( MNcdParserSessionObserver* aObserver ) = 0;
    
    /**
     * Retrieves the observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserSessionObserver* SessionObserver() const = 0;

    /**
     * Sets the information observer.
     * @param aObserver
     */
    virtual void SetInformationObserver( MNcdParserInformationObserver* aObserver ) = 0;
    
    /**
     * Retrieves the observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserInformationObserver* InformationObserver() const = 0;
    
    /**
     * Sets the data blocks observer.
     * @param aObserver
     */
    virtual void SetDataBlocksObserver( MNcdParserDataBlocksObserver* aObserver ) = 0;
    
    /**
     * Retrieves the observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserDataBlocksObserver* DataBlocksObserver() const = 0;
    
    /**
     * Sets the configuration protocol observer.
     * @param aObserver
     */
    virtual void SetConfigurationProtocolObserver( MNcdParserConfigurationProtocolObserver* aObserver ) = 0;
    /**
     * Retrieves the observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserConfigurationProtocolObserver* ConfigurationProtocolObserver() const = 0;
    
    /**
     * Sets the error observer.
     * @param aObserver
     */
    virtual void SetErrorObserver( MNcdParserErrorObserver* aObserver ) = 0;
    /**
     * Retrieves the observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserErrorObserver* ErrorObserver() const = 0;
    
    /**
     * Sets the query observer.
     * @param aObserver
     */
    virtual void SetQueryObserver( MNcdParserQueryObserver* aObserver ) = 0;
    /**
     * Retrieves the observer.
     * @return Observer pointer or null.
     */
    virtual MNcdParserQueryObserver* QueryObserver() const = 0;
    
    /**
     * Retrieves the origin of the current session.
     * @return Server URI or KNullDesC if not available.
     */
    virtual const TDesC& SessionOrigin() const = 0;
    };

#endif

