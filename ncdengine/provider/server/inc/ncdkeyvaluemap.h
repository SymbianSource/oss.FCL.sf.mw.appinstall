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
* Description:   CNcdKeyValueMap declaration
*
*/


#ifndef C_NCDKEYVALUEMAP_H
#define C_NCDKEYVALUEMAP_H

#include "e32base.h"

class RWriteStream;
class RReadStream;
class CNcdKeyValuePair;

/**
* Map implementation
*/
class CNcdKeyValueMap : public CBase
    {
    public:
    
        typedef TInt KeyValueIndex;
    
    public:
    
        /**
        * Creator
        */
        static CNcdKeyValueMap* NewL();

        /**
        * Creator
        */
        static CNcdKeyValueMap* NewLC();

        
        /**
        * Copy constructor
        */
        static CNcdKeyValueMap* NewL( const CNcdKeyValueMap& aOther );
        
        /**
        * Cloner
        */
        CNcdKeyValueMap* CloneL() const;
        
        /**
        * Destructor
        */
        ~CNcdKeyValueMap();
        
    public:
         
        /**
        * Adds a key-value pair
        *
        * @note Always adds a new pair even if the key already exists. 
        * ValueByKey and KeyExists -methods return the oldest instance of the key
        * 
        * @param aKey Key name
        * @param aValue Data
        */
        void AddL( const TDesC& aKey, const TDesC& aValue );
        
        
        /**
         * Adds a key-value pair.
         *
         * @note Always adds a new pair even if the key already exists. 
         * ValueByKey and KeyExists -methods return the oldest instance of the key
         *
         * @param aPair Pair to add. The ownership of the pair is transferred
         * to the CNcdKeyValueMap if the operation is successful.
         * @note If a leave occurs, it is the responsibility of the caller to
         * delete the pair.
         */
        void AddL( CNcdKeyValuePair* aPair );
        
        
        
        /**
         * Adds a new or replaces an old key-value pair.
         *
         * If the key already exists, the old value is replaced with the new one
         *
         * @param aPair Pair to add. The ownership of the pair is transferred
         * to the CNcdKeyValueMap if the operation is successful.
         * @note If a leave occurs, it is the responsibility of the caller to
         * delete the pair.
         */
        void ReplaceL( CNcdKeyValuePair* aPair );
        
        
        /** 
        * Removes key-value pair.
        *
        * @note If the key exists more than once, then the oldest 
        * instance is removed.
        * 
        * @param aKey Key
        * @return KErrNotFound if the pair was not found
        */
        TInt Remove( const TDesC& aKey );
           
            
        /**
        * Returns an array of all key-value pairs
        *
        * @return Array of headers
        */
        RPointerArray<CNcdKeyValuePair>& Pairs();
        

        /**
        * Returns an array of all key-value pairs
        *
        * @return Array of headers
        */
        const RPointerArray<CNcdKeyValuePair>& Pairs() const;
                       
        
        /**
        * Searches for the value that matches the key
        * 
        * @param aPair Wanted key
        * @return Header
        * @exception KErrNotFound if a matching value was not found
        */        
        const TDesC& ValueByKeyL( const TDesC& aKey ) const;
        
        
        KeyValueIndex KeyExists( const TDesC& aKey ) const;
        
        const TDesC& ValueByIndex( const KeyValueIndex& aIndex ) const;
        
        
        KeyValueIndex PairExists( const CNcdKeyValuePair& aPair ) const;
        
        
        /**
         * Deletes all key-value pairs
         */
        void ResetAndDestroy();
    
        
        /**
         * Resets the array but doesn't delete the pairs
         */
        void Reset();
        

        /**
         * Appends a map.
         *
         * Simply appends key-value -pairs from the given map to this
         * map. No key collision checks are made.
         *
         * @param aMap Map 
         */
        void AppendL( const CNcdKeyValueMap& aMap );
        
        
        /**
         * Externalizes the map
         *
         * @param aStream Target stream
         */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
         * Externalizes the map to a RBuf8
         *
         * @param aTarge Target buffer.
         */
        void ExternalizeL( RBuf8& aTarget ) const;
        
        /**
         * Internalizes the map
         *
         * @param aStream Source stream
         */
        void InternalizeL( RReadStream& aStream );
        
           
    private:

        /**
        * Constructor
        */
        CNcdKeyValueMap();

        /**
        * Copy constructor
        */
        CNcdKeyValueMap( const CNcdKeyValueMap& aOther );
    
        /**
        * 2nd phase copy constructor
        */
        void ConstructL( const CNcdKeyValueMap& aOther );
            
    private:
    
        /**
        * Searches for the pair that matches the key in the given 
        * pair
        * 
        * @param aPair Key-value pair that has the wanted key set
        * @return A value
        * @note aPair is popped from the Cleanupstack and deleted
        * @exception KErrNotFound if a matching header was not found
        */
        const TDesC& FindValueL( CNcdKeyValuePair* aPair ) const;

    
    private:
    
        RPointerArray<CNcdKeyValuePair> iPairs;
        
    };

#endif // C_NCDKEYVALUEMAP_H