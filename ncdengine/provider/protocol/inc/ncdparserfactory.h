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
* Description:   NcdParserFactory declaration
*
*/


#ifndef NcdPARSERFACTORY_H
#define NcdPARSERFACTORY_H

#include <e32base.h>

class MNcdParser;
class MNcdProtocolDefaultObserver;

/**
 * Factory to create a parser instance. 
 * This is the main entry point for protocol handling and parsing.
 */
class NcdParserFactory
    {
public:
    /**
     * Creates a parser instance.
     * 
     * @param aDefaultObserver Default observer for parser events
     * @return MNcdParser*
     */
    static MNcdParser* CreateParserL( 
        MNcdProtocolDefaultObserver* aDefaultObserver );
    /**
     * Creates a parser instance.
     *
     * @param aDefaultObserver Default observer for parser events
     * @return MNcdParser*
     */
    static MNcdParser* CreateParserLC( 
        MNcdProtocolDefaultObserver* aDefaultObserver );
private:
    NcdParserFactory();
    ~NcdParserFactory();
    };

#endif
