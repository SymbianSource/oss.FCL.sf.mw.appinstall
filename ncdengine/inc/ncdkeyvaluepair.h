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
* Description:   Definition of key-value pair interface and implementation class
*
*/


#ifndef NCD_KEY_VALUE_PAIR_H
#define NCD_KEY_VALUE_PAIR_H

#include <e32base.h>

/**
 *  An interface for accessing a key - value pair object.
 *
 *  
 */
class MNcdKeyValuePair
    {
public:
    
    /**
     * Returns the key of the pair.
     * 
     * 
     * @return Key of the pair.
     */
    virtual const TDesC& Key() const = 0;

    /**
     * Returns the value of the pair.
     * 
     * 
     * @return Value of the pair.
     */
    virtual const TDesC& Value() const = 0;
    
#if 0    
    /**
     * Externalizes the pair to a stream
     *
     * @param aStream Target stream
     */
    virtual void ExternalizeL( RWriteStream& aStream ) const = 0;
    
    
    /**
     * Externalizes the pair to a descriptor
     *
     * @return Descriptor containing the pair
     */
    virtual HBufC8* ExternalizeLC() const = 0;
    
    
    /**
     * Internalizes the pair from a stream
     *
     * @param aStream Source stream
     */
    virtual void InternalizeL( RReadStream& aStream ) = 0;

#endif

    };

#endif // NCD_KEY_VALUE_PAIR_H
