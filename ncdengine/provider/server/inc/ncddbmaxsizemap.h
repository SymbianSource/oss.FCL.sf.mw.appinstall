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
* Description:   Contains CNcdDbMaxSizeMap class
*
*/


#ifndef NCD_DB_MAX_SIZE_MAP
#define NCD_DB_MAX_SIZE_MAP

/**
 * This class object acts as a map that has the client uid as the key
 * and the db byte size as the value.
 */

#include <e32base.h>
#include <e32cmn.h>

class CNcdDbMaxSizeMap : public CBase
    {
    
public:

    /** 
     * @param aDefaultMaxSize
     * @return CNcdDbMaxSizeMap* Pointer to the created object 
     * of this class.
     */
    static CNcdDbMaxSizeMap* NewL( const TInt aDefaultMaxSize );

    /** 
     * @param aDefaultMaxSize
     * @return CNcdDbMaxSizeMap* Pointer to the created object 
     * of this class.
     */
    static CNcdDbMaxSizeMap* NewLC( const TInt aDefaultMaxSize );


    /**
     * Destructor
     */
    virtual ~CNcdDbMaxSizeMap();


    /**
     * Sets the value that corresponds to the given key.
     * If the key already exists. Then the corresponding value
     * will be reset. If the key does not exist. Then the new
     * key value pair is created.
     *
     * @return KErrNone if value was set. Else some system error.
     */
    TInt SetValue( const TUid& aKey,
                   TInt aValue );
    

    /**
     * @return TInt the value that corresponds the given key.
     * If the value corresponding the key was not found from the
     * map, then the default value given for the constructor is 
     * returned.
     */                   
    TInt Value( const TUid& aKey ) const;
    
    
protected:

    /** 
     * Constructor
     *
     * @param aDefaultMaxSize
     */ 
    CNcdDbMaxSizeMap( const TInt aDefaultMaxSize );
    
    
    /**
     * ConstructL
     */
    virtual void ConstructL();

            
private:
    
    // Prevent these if not implemented
    CNcdDbMaxSizeMap( const CNcdDbMaxSizeMap& aObject );
    CNcdDbMaxSizeMap& operator =( const CNcdDbMaxSizeMap& aObject );


    // Searches for the given key. And returns the index of that key.
    // Returns KElementWasNotFound if the key was not found.
    TInt KeyIndex( const TUid& aKey ) const;


private: // data

    // Constant value that is used if the search for the key or value
    // did not find any actual value.
    const TInt KElementWasNotFound;

    // The default max size that is used if the key is not found
    // and some value should be returned.
    const TInt iDefaultMaxSize;


    // The key can not be changed
    RArray<const TUid> iKeyArray;
    
    // The value of may be changed.
    RArray<TInt> iValueArray;

    };
    
#endif // NCD_DB_MAX_SIZE_MAP