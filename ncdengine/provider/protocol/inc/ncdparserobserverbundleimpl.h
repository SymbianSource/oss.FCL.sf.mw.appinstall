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


#ifndef NCDPARSEROBSERVERBUNDLEIMPL_H
#define NCDPARSEROBSERVERBUNDLEIMPL_H

#include "ncdparserobserver.h"

class MNcdProtocolDefaultObserver;

class CNcdParserObserverBundleImpl : public CBase,
                                     public MNcdParserObserverBundle
    {
public:
    static CNcdParserObserverBundleImpl* NewL(
        MNcdProtocolDefaultObserver* aDefaultObserver );
        
    ~CNcdParserObserverBundleImpl();
private:
    CNcdParserObserverBundleImpl( 
        MNcdProtocolDefaultObserver* aDefaultObserver );
    void ConstructL();

public: // From MNcdParserObserverBundle
    void SetParserObserver( 
        MNcdParserObserver* aObserver );
    MNcdParserObserver* ParserObserver() const;

    void SetEntityObserver( 
        MNcdParserEntityObserver* aObserver );
    MNcdParserEntityObserver* EntityObserver() const;

    void SetPurchaseObserver( 
        MNcdParserPurchaseObserver* aObserver );
    MNcdParserPurchaseObserver* PurchaseObserver() const;

    void SetSubscriptionObserver( 
        MNcdParserSubscriptionObserver* aObserver );
    MNcdParserSubscriptionObserver* SubscriptionObserver() const;

    void SetSessionObserver( 
        MNcdParserSessionObserver* aObserver );
    MNcdParserSessionObserver* SessionObserver() const;

    void SetConfigurationProtocolObserver( MNcdParserConfigurationProtocolObserver* aObserver );
    MNcdParserConfigurationProtocolObserver* ConfigurationProtocolObserver() const;

    void SetInformationObserver( MNcdParserInformationObserver* aObserver );
    MNcdParserInformationObserver* InformationObserver() const;

    void SetDataBlocksObserver( MNcdParserDataBlocksObserver* aObserver );
    MNcdParserDataBlocksObserver* DataBlocksObserver() const;

    void SetErrorObserver( MNcdParserErrorObserver* aObserver );
    MNcdParserErrorObserver* ErrorObserver() const;

    void SetQueryObserver( MNcdParserQueryObserver* aObserver );
    MNcdParserQueryObserver* QueryObserver() const;

    const TDesC& SessionOrigin() const;
public:
    void SetSessionOriginL( const TDesC& aSessionOrigin );
private:
    MNcdParserObserver* iObserver;
    MNcdParserEntityObserver* iEntityObserver;
    MNcdParserPurchaseObserver* iPurchaseObserver;
    MNcdParserSubscriptionObserver* iSubscriptionObserver;
    MNcdParserSessionObserver* iSessionObserver;
    MNcdParserInformationObserver* iInformationObserver;
    MNcdParserDataBlocksObserver* iDataBlocksObserver;
    MNcdParserConfigurationProtocolObserver* iConfigurationProtocolObserver;
    MNcdParserErrorObserver* iErrorObserver;
    MNcdParserQueryObserver* iQueryObserver;
    HBufC* iSessionOrigin;
    };

#endif
