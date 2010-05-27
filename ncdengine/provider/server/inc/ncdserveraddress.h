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
* Description:   CNcdServerAddress declaration
*
*/


#ifndef C_NCDSERVERADDRESS_H
#define C_NCDSERVERADDRESS_H

#include <e32base.h>

#include "ncdstoragedataitem.h"


/**
 * Utility class for handling server addresses with validities
 */
class CNcdServerAddress : public CBase, public MNcdStorageDataItem
    {
public:

    /**
     * Creates a new CNcdServerAddress-object
     *
     * @param aAddress Address
     * @param aValidity Validity as absolute home time
     * @note If aValidity is 0, validity is not used for
     * evaluating whether the address is valid or not. The effect is 
     * the same as after SetUseValidity( EFalse ) has been used
     */
    static CNcdServerAddress* NewL( const TDesC& aAddress, 
        const TInt64& aValidity );

    static CNcdServerAddress* NewL( RReadStream& aStream );
    
    /**
     * 
     */

    static CNcdServerAddress* NewLC( const TDesC& aAddress, 
        const TInt64& aValidity );


    /**
     * Destructor
     */
    virtual ~CNcdServerAddress();


    /**
     * Address getter
     */
    const TDesC& Address() const;
    
    
    /**
     * Validity setter
     *
     * @param aValidity New validity value
     */
    void SetValidity( const TInt64& aValidity );
    
    
    /**
     * Validity getter
     */    
    TInt64 Validity() const;
       

    /**
     * Validity checker
     *
     * Compares the validity of the address to current home time
     *
     * @return ETrue if the address is valid
     */        
    TBool IsValid() const;
    
    
    /**
     * Sets whether validity value is used when evaluating the
     * validity of the address
     *
     * @param aUseValidity If ETrue, validity value is used
     */
    void SetUseValidity( TBool aUseValidity );
    

public: // MNcdStorageDataItem

    void ExternalizeL( RWriteStream& aStream );
    void InternalizeL( RReadStream& aStream );
        
protected:

    CNcdServerAddress( const TInt64& aValidity );
    void ConstructL( const TDesC& aAddress );
    
    
    // Prevent copy constructor and assignment operator
    CNcdServerAddress( 
        const CNcdServerAddress& );
        
    CNcdServerAddress& operator=(const CNcdServerAddress& );
    
    
private: // data

    HBufC* iAddress;
    TInt64 iValidity;
    TBool  iUseValidity;
    };

#endif // C_NCDSERVERADDRESS_H