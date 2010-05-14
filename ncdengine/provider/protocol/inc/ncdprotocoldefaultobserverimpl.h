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
* Description:   MNcdProtocolDefaultObserverImpl declaration
*
*/


#ifndef CNCDPROTOCOLDEFAULTOBSERVER_H
#define CNCDPROTOCOLDEFAULTOBSERVER_H

#include "ncdprotocoldefaultobserver.h"

class MCatalogsContext;
class MNcdConfigurationManager;
class CNcdSubscriptionManager;

class CNcdProtocolDefaultObserver : public CBase,
                                    public MNcdProtocolDefaultObserver
    {
public:
    /**
     * 
     * parameters needed, at least:
     * - context
     * - conf manager: client spesific config, provider global config
     * - cookie handler: cookies from config protocol
     */
    static CNcdProtocolDefaultObserver* NewL( 
        MCatalogsContext& aContext,
        MNcdConfigurationManager& aConfigurationManager,
        CNcdSubscriptionManager& aSubscriptionManager,
        const TDesC& aServerUri );
    /**
     * 
     */
    ~CNcdProtocolDefaultObserver();


public: // New methods
    
    const MCatalogsContext& Context() const;

private:
    /**
     * Constructor
     */
    CNcdProtocolDefaultObserver( MCatalogsContext& aContext,
        MNcdConfigurationManager& aConfigurationManager,
        CNcdSubscriptionManager& aSubscriptionManager );
    /**
     * Constructor
     */
    void ConstructL( const TDesC& aServerUri );

private: // From MNcdProtocolDefaultObserver
  
private: 
    // From MNcdParserConfigurationProtocolObserver

    void ConfigurationBeginL( const TDesC& version, 
                              TInt expirationdelta );
    void ConfigurationQueryL( MNcdConfigurationProtocolQuery* aQuery );
    void ClientConfigurationL( 
        MNcdConfigurationProtocolClientConfiguration* aConfiguration );
    void ConfigurationDetailsL( 
        CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails );
    void ConfigurationActionRequestL(  
        MNcdConfigurationProtocolActionRequest* aActionRequest );
    void ConfigurationServerDetailsL( MNcdConfigurationProtocolServerDetails* aServerDetails );
    void ConfigurationErrorL( MNcdConfigurationProtocolError* aError );
    void ConfigurationEndL();

    // From MNcdParserInformationObserver
    void InformationL( MNcdPreminetProtocolInformation* aData );

    // From MNcdParserDataBlocksObserver
    void DataBlocksL( CArrayPtr<MNcdPreminetProtocolDataBlock>* aData );

    // From MNcdParserErrorObserver
    void ErrorL( MNcdPreminetProtocolError* aData );

    // From MNCdParserQueryObserver
    void QueryL( MNcdConfigurationProtocolQuery* aData );
    
    // From MNcdParserSubscriptionObserver
    virtual void ValidSubscriptionL(
        MNcdPreminetProtocolSubscription* aData );
    virtual void OldSubscriptionL( 
        MNcdPreminetProtocolSubscription* aData );

    

private: // data

    MCatalogsContext& iContext;
    MNcdConfigurationManager& iConfigurationManager;
    CNcdSubscriptionManager& iSubscriptionManager;
    HBufC* iServerUri;
    };

#endif

