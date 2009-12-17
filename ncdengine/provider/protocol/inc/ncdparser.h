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
* Description:   MNcdParser interface
*
*/


#ifndef NcdPARSER_H
#define NcdPARSER_H

#include <e32base.h>

class MNcdParserObserverBundle;
class MNcdProtocolDefaultObserver;
class CNcdSubParser;

/**
 * Parser interface.
 */
class MNcdParser 
    {
public:
    /**
     * Destructor
     */
    virtual ~MNcdParser() {}

    /**
     * Retrieves the observer bundle. The caller must set some 
     * observers prior to beginning the operation, or BeginL()-methods will fail.
     */
    virtual MNcdParserObserverBundle& Observers() const = 0;

    /**
     * Retrieves the default observer. All events not handled or handled by 
     * the client should be passed to the default observer.
     * It will further handle the events and free the memory.
     */
    virtual MNcdProtocolDefaultObserver& DefaultObserver() const = 0;

    /**
     * Sets the origin of data for this parser. This is passed to some
     * observers, as session observer.
     */
    virtual void SetOriginL( const TDesC& aOrigin ) = 0;

    /** 
     * Begin parsing in asynchronous manner. 
     * After this, data may fed to the parser.
     */
    virtual void BeginAsyncL() = 0;
    
    /**
     * Begin parsing in asynchronous manner using the given parser
     *
     * @param aSubParser Parser. If NULL, normal top level parser is used. Ownership is transferred.
     * @note Ownership is always transferred so aSubParser must NOT be deleted
     * by the calling code even if this method leaves
     */
    virtual void BeginAsyncL( CNcdSubParser* aSubParser ) = 0;
    
    /** 
     * Begin parsing in synchronous manner.
     * After this, data may fed to the parser.
     */
    virtual void BeginSyncL( ) = 0;
    
    /**
     * Begin parsing in asynchronous manner using the given parser
     *
     * @param aSubParser Parser. If NULL, normal top level parser is used. Ownership is transferred.
     * @note Ownership is always transferred so aSubParser must NOT be deleted
     * by the calling code even if this method leaves
     */
    virtual void BeginSyncL( CNcdSubParser* aSubParser ) = 0;

    /**
     * Parse some data.
     * @param aData UTF8 format XML data
     */
    virtual void ParseL( const TDesC8& aUTF8Data ) = 0;
    /**
     * Parse some data.
     * @param aData 16-bit unicode format XML data
     */
    virtual void ParseL( const TDesC16& aData ) = 0;
    /**
     * End parsing. 
     * This method must be called when all the data has been fed to the parser.
     * In synchronous mode, a callback will be immediately issued.
     * In asynchronous mode, a callback will be issued after all data
     * in the buffers has been processed.
     */
    virtual void EndL() = 0;
    /**
     * Cancel ongoing asynchronous parsing process.
     * A callback will not be issued.
     */
    virtual void CancelParsing() = 0;
    };


#endif
