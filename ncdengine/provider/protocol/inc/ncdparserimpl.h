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
* Description:   CNcdParserImpl declaration
*
*/


#ifndef NcdPARSERIMPL_H
#define NcdPARSERIMPL_H

#include <xml/parser.h>
#include <badesca.h>


#include "ncdparser.h"
#include "ncdparserobserver.h"
#include "ncdsubparser.h"

class CNcdParserObserverBundleImpl;
class MNcdProtocolDefaultObserver;

/**
 * Parser implementation class.
 */
class CNcdParserImpl : //public CBase,
    public CActive,
    public MNcdParser,
    public MNcdSubParserObserver
   
    {
public:
    /**
     * Constructor
     *
     * @param Default observer. Ownership is transferred to parser
     * @return CNcdParserImpl*
     */
    static CNcdParserImpl* NewL( 
        MNcdProtocolDefaultObserver* aDefaultObserver );
    /**
     * Destructor
     */
    virtual ~CNcdParserImpl();
    
protected:
    /**
     * Constructor
     */
    CNcdParserImpl( MNcdProtocolDefaultObserver* aDefaultObserver );
    /**
     * Constructor
     */
    void ConstructL();

private: // From CActive

    /**
     * @see CActive::DoCancel()
     */
    void DoCancel();
    /**
     * @see CActive::RunL()
     */
    void RunL();
    /**
     * @see CActive::RunError()
     */
    TInt RunError( TInt aError );
       
       
public: // From MNcdParser
    /**
     * @see MNcdParser::Observers()
     */
    MNcdParserObserverBundle& Observers() const;
    /**
     * @see MNcdParser::DefaultObserver()
     */
    MNcdProtocolDefaultObserver& DefaultObserver() const;
    
    /**
     * @see MNcdParser::BeginAsyncL()
     */     
    void BeginAsyncL();

    /**
     * @see MNcdParser::BeginAsyncL()
     */     
    void BeginAsyncL( CNcdSubParser* aSubParser );

    /**
     * @see MNcdParser::BeginSyncL()
     */
    void BeginSyncL();
    
    /**
     * @see MNcdParser::BeginSyncL()
     */
    void BeginSyncL( CNcdSubParser* aSubParser );
    /**
     * @see MNcdParser::ParseL()
     */
    void ParseL( const TDesC8& aData );
    /**
     * @see MNcdParser::ParseL()
     */
    void ParseL( const TDesC16& aData );
    /**
     * @see MNcdParser::EndL()
     */
    void EndL();
    /**
     * @see MNcdParser::CancelParsing()
     */
    void CancelParsing();

    /**
     * @see MNcdParser::SetOriginL
     */
    void SetOriginL( const TDesC& aOrigin );


public: // From MNcdParserObserver

    /**
     * @see MNcdSubParserObserver::SubParserFinishedL
     */
    void SubParserFinishedL( const TDesC8& aTag, TInt aErrorCode );

private:

    void BeginL( CNcdSubParser* aSubParser );

protected:
    /**
     * Observer for parser events
     */
    CNcdParserObserverBundleImpl* iObservers;

    /**
     * Default observer for parser events
     * Owned.
     */
    MNcdProtocolDefaultObserver* iDefaultObserver;
        
    /**
     * Subparser used for further parsing
     */
    CNcdSubParser* iSubParser;
    
    /**
     * XML parser instance
     */
    Xml::CParser* iXmlParser;
    
    /**
     * Data buffer
     */
    CBufBase* iInputBuffer;

    /**
     * Set to ETrue by user when all data has been fed to the parser.
     * After processing the data, observer is called to indicate comlpetion.
     */
    TBool iPleaseFinish;
    /**
     * Status flag to indicate running in synchronous or asynchronous mode.
     */
    TBool iSynchronous;

    /**
     * Status flag indicating cancellation. The parsing should be stopped.
     */
    TBool iCancelled;

    /**
     * Origin data field, set according to SetOriginL(). Passed to some
     * observers.
     */
    HBufC* iOrigin;
    
private:
    HBufC8* iFeedBuffer;
    
    
    };

#endif
