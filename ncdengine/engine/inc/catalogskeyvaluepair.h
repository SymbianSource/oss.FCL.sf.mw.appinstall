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
* Description:   ?Description
*
*/


#ifndef C_CATALOGSKEYVALUEPAIR_H
#define C_CATALOGSKEYVALUEPAIR_H

// Includes
#include <e32base.h>

class RReadStream;
class RWriteStream;

/**
 * Key - Value pair
 */
class CCatalogsKeyValuePair : public CBase
    {
public: // Construction & destruction

    /**
     * Static constructor.
     *
     * @param   aKey    Key for the pair.
     * @param   aValue  Valuefor the pair.
     *
     * @return  The created object.
     */
    static CCatalogsKeyValuePair* NewL( const TDesC16& aKey, 
        const TDesC16& aValue );

    
    /**
     * Static constructor. Leaves a pointer on the cleanup stack.
     *
     * @param   aKey    Key for the pair.
     * @param   aValue  Valuefor the pair.
     *
     * @return  The created object.
     */
    static CCatalogsKeyValuePair* NewLC( const TDesC16& aKey, 
        const TDesC16& aValue );

    
    /**
     * Static constructor.
     *
     * @param   aKey    Key for the pair.
     * @param   aValue  Valuefor the pair.
     *
     * @return  The created object.
     */
    static CCatalogsKeyValuePair* NewL( const TDesC8& aKey, 
        const TDesC8& aValue );

    
    /**
     * Static constructor. Leaves a pointer on the cleanup stack.
     *
     * @param   aKey    Key for the pair.
     * @param   aValue  Valuefor the pair.
     *
     * @return  The created object.
     */
    static CCatalogsKeyValuePair* NewLC( const TDesC8& aKey, 
        const TDesC8& aValue );


    /**
     * Creates a copy of the key-value -pair
     * 
     * @param aOther Pair to copy
     * @return A copy of the given pair
     */
    static CCatalogsKeyValuePair* NewL( const CCatalogsKeyValuePair& aOther );
    

    /**
     * Creates a copy of the key-value -pair and pushes it to the cleanup stack
     * 
     * @param aOther Pair to copy
     * @return A copy of the given pair
     */
    static CCatalogsKeyValuePair* NewLC( 
        const CCatalogsKeyValuePair& aOther );


    /**
     * Internalizes a key-value -pair from a stream
     * 
     * @param aStream Source stream
     * @return A pair
     */
    static CCatalogsKeyValuePair* NewL( RReadStream& aStream );
    
    /**
     * Internalizes a key-value -pair from a stream and pushes it to the
     * cleanup stack.
     * 
     * @param aStream Source stream
     * @return A pair
     */
    static CCatalogsKeyValuePair* NewLC( RReadStream& aStream );

    /**
     * Destructor.
     */
    ~CCatalogsKeyValuePair();


private:    // Construction & destruction
    
    CCatalogsKeyValuePair();
    
    void ConstructL( const TDesC16& aKey, const TDesC16& aValue );
    void ConstructL( const TDesC8& aKey, const TDesC8& aValue );


public:     // New methods

    /**
     * Returns the key of the pair.
     *
     * @return  Reference descriptor containing the key.
     */
    const TDesC8& Key() const;
    
    
    /**
     * Returns the value of the pair.
     *
     * @return  Reference descriptor containing the value.
     */
    const TDesC8& Value() const;


    /**
     * Returns the key of the pair.
     *
     * @return  Reference descriptor containing the key.
     */
    HBufC* KeyLC() const;

    /**
     * Returns the key of the pair.
     *
     * @return  Reference descriptor containing the key.
     */
    HBufC* KeyL() const;


    /**
     * Returns the value of the pair.
     *
     * @return  Reference descriptor containing the value.
     */
    HBufC* ValueLC() const;
    
    
    /**
     * Returns the value of the pair.
     *
     * @return  Reference descriptor containing the value.
     */
    HBufC* ValueL() const;

    /**
     * Comparison method. Compares the keys of two pairs to 
     * determine equivalence.
     *
     * @return  Boolean, ETrue if pairs match.
     */
    static TBool MatchByKey( const CCatalogsKeyValuePair& aFirst, 
                             const CCatalogsKeyValuePair& aSecond );
                             
                             
    /**
     * Externalize
     *
     * @param aStream Target stream
     */                             
    void ExternalizeL( RWriteStream& aStream ) const;
    
    
    /**
     * Internalize
     *
     * @param aStream Source stream
     */
    void InternalizeL( RReadStream& aStream );                             

private:    // Data

    HBufC8* iKey;
    HBufC8* iValue;
    };

#endif // C_CATALOGSKEYVALUEPAIR_H

