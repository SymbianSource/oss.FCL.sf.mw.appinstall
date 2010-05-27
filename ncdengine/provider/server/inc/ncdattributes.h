/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition of ncdattributes interface and
*                implementation class.
*
*/


#ifndef C_NCDATTRIBUTES_H
#define C_NCDATTRIBUTES_H


#include <e32base.h>

#include "ncdpanics.h"

class RWriteStream;
class RReadStream;

/**
 * Simple attribute container
 *
 * Used for setting, getting, storing and loading simple
 * attributes. Currently supports 16-bit descriptors and TInt32
 */
class CNcdAttributes : public CBase
    {
public:

    /**
     * Types of supported attributes
     */
    enum TAttributeType 
        {
        /**
         * Attribute type is not set
         */
        EAttributeTypeUndefined = 0,
        
        /**
         * Attribute is TInt32
         */
        EAttributeTypeInt32,
        
        /**
         * Attribute is a 16-bit descriptor
         */
        EAttributeTypeString16
        };
    
public:

    /**
     * @param aUpperLimit Upper attribute limit is NOT included in the range. Must be >= 0 and > aLowerLimit
     *
     */
    IMPORT_C static CNcdAttributes* NewL( TInt aUpperLimit, TInt aLowerLimit = 0 );
    
    /**
     * @note If given upperlimit differs from the one read from the stream then
     * the attribute range is adjusted for the given limit.
     * @leave KErrArgument if aLowerLimit differs from the lower limit read from the stream
     */
    IMPORT_C static CNcdAttributes* NewL( 
        RReadStream& aStream,
        TInt aUpperLimit, 
        TInt aLowerLimit = 0 );

    /**
     * Copy constructor
     */
    IMPORT_C static CNcdAttributes* NewL( const CNcdAttributes& aAttributes );


    /**
     * Destructor
     */
    IMPORT_C virtual ~CNcdAttributes();
    
public:
   
    /**
     * Lower limit getter
     */
    IMPORT_C TInt RangeLowerLimit() const;
    
    /**
     * Upper limit getter
     */
    IMPORT_C TInt RangeUpperLimit() const;

    /**
     * TInt32 attribute setter
     *
     * @param aAttribute Attribute
     * @param aValue Value     
     * @panic ENcdPanicIndexOutOfRange if aAttribute is not 
     * RangeLowerLimit() <= aAttribute < RangeUpperLimit()
     * @panic ENcdPanicInvalidArgument if the attribute type is not 
     * EAttributeTypeInt32 or EAttributeTypeUndefined
     */
    IMPORT_C void SetAttributeL( TInt aAttribute, TInt32 aValue );
    
    /**
     * 16-bit descriptor attribute setter
     *
     * @param aAttribute Attribute
     * @param aValue Value
     * @leave KErrNoMemory if memory runs out
     * @panic ENcdPanicIndexOutOfRange if aAttribute is not 
     * RangeLowerLimit() <= aAttribute < RangeUpperLimit()
     * @panic ENcdPanicInvalidArgument if the attribute type is not 
     * EAttributeTypeString16 or EAttributeTypeUndefined
     */
    IMPORT_C void SetAttributeL( TInt aAttribute, const TDesC& aValue );
    
    /**
     * 16-bit descriptor attribute getter
     *
     * @param aAttribute Attribute
     * @return Value of the attribute
     * @panic ENcdPanicIndexOutOfRange if aAttribute is not 
     * RangeLowerLimit() <= aAttribute < RangeUpperLimit()
     * @panic ENcdPanicInvalidArgument if the attribute type is not 
     * EAttributeTypeString16
     */
    IMPORT_C const TDesC& AttributeString16( TInt aAttribute ) const;
    
    
    /**
     * TInt32 attribute getter
     *
     * @param aAttribute Attribute
     * @return Value of the attribute
     * @panic ENcdPanicIndexOutOfRange if aAttribute is not 
     * RangeLowerLimit() <= aAttribute < RangeUpperLimit()
     * @panic ENcdPanicInvalidArgument if the attribute type is not 
     * EAttributeTypeInt32
     */
    IMPORT_C TInt32 AttributeInt32( TInt aAttribute ) const;


    /**
     * Attribute type getter
     * 
     * @param aAttribute
     * @return Attribute type
     * @panic ENcdPanicIndexOutOfRange if aAttribute is not 
     * RangeLowerLimit() <= aAttribute < RangeUpperLimit()
     */
    IMPORT_C TAttributeType AttributeType( TInt aAttribute ) const;
    
public:

    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    IMPORT_C void InternalizeL( RReadStream& aStream );

protected:

    // Union that contains all possible attribute types
    union TAttributeUnion 
        {
        HBufC* iString16;
        TInt32 iInt32;
        };
    
    // Attribute 
    struct TAttribute
        {
        TAttributeType iType;
        TAttributeUnion iAttribute;
        
        TAttribute() : iType( EAttributeTypeUndefined )
            {            
            iAttribute.iString16 = NULL;
            }            
        };


protected:
    
    CNcdAttributes( TInt aUpperLimit, TInt aLowerLimit );
    void ConstructL();
    void ConstructL( const CNcdAttributes& aAttributes );

    // Deletes all attributes, array size is reduced to 0
    void ClearAttributes();
    
    // Deletes all attributes and resets the array to have
    // room for the attribute range, array size == ArraySize()
    void ResetAttributesL();
    
    // Size needed for containing the attribute range
    TInt ArraySize() const;
    
    
    // Attribute externalization and internalization
    void ExternalizeAttributeL( 
        const TAttribute& aAttribute, RWriteStream& aStream ) const;

    TAttribute InternalizeAttributeL( RReadStream& aStream ) const;

    // Deletes attribute's internals
    void DeleteAttribute( TAttribute& aAttribute );
    
    // Copies attribute's internals and returns the copy
    TAttribute CopyAttributeL( const TAttribute& aAttribute ) const;

    // Indexing methods
    const TAttribute& Attribute( TInt aAttribute ) const;    
    TAttribute& Attribute( TInt aAttribute );
    
private:

    CNcdAttributes( const CNcdAttributes& );
    CNcdAttributes& operator=( const CNcdAttributes& );
    
private:
    
    TInt iLowerLimit;
    TInt iUpperLimit;
    
    // Attribute array
    RArray<TAttribute> iAttributes;
    };


#endif // M_NCDATTRIBUTES_H
