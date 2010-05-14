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
* Description:   Implementation of CNcdAttributes
*
*/


#include <s32strm.h>

#include "ncdattributes.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"

// Checks that the given attribute value is in range
#define ASSERT_ATTRIBUTE_IN_RANGE( attribute )                          \
    NCD_ASSERT_ALWAYS( ( RangeLowerLimit() <= (attribute) ) &&          \
        ( (attribute) < RangeUpperLimit() ), ENcdPanicIndexOutOfRange );

// Checks that the given type can be written to the attribute
#define ASSERT_ATTRIBUTE_WRITABLE( attribute, writetype )               \
    NCD_ASSERT_ALWAYS(                                                  \
        attribute.iType == writetype ||                                 \
        attribute.iType == EAttributeTypeUndefined,                     \
        ENcdPanicInvalidArgument );

// Checks that the given type can be read from the attribute
#define ASSERT_ATTRIBUTE_READABLE( attribute, readtype )                \
    NCD_ASSERT_ALWAYS(                                                  \
        attribute.iType == readtype,                                    \
        ENcdPanicInvalidArgument );
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C CNcdAttributes* CNcdAttributes::NewL( 
    TInt aUpperLimit, 
    TInt aLowerLimit )
    {
    CNcdAttributes* self = new( ELeave ) CNcdAttributes( 
        aUpperLimit, aLowerLimit );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C CNcdAttributes* CNcdAttributes::NewL( 
    RReadStream& aStream,
    TInt aUpperLimit, 
    TInt aLowerLimit )
    {
    CNcdAttributes* self = new( ELeave ) CNcdAttributes( 
        aUpperLimit, aLowerLimit );
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C CNcdAttributes* CNcdAttributes::NewL( 
    const CNcdAttributes& aAttributes )
    {
    CNcdAttributes* self = new( ELeave ) CNcdAttributes( 
        aAttributes.RangeUpperLimit(), 
        aAttributes.RangeLowerLimit() );
    CleanupStack::PushL( self );
    self->ConstructL( aAttributes );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//        
EXPORT_C CNcdAttributes::~CNcdAttributes()
    {
    DLTRACEIN((""));
    ClearAttributes();
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C TInt CNcdAttributes::RangeLowerLimit() const
    {
    return iLowerLimit;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C TInt CNcdAttributes::RangeUpperLimit() const
    {
    return iUpperLimit;
    }


// ---------------------------------------------------------------------------
// TInt32 Attribute setter
// L-method mainly in order to be consistent with the string setter and
// also in preparation for changing the asserts to leaves
// ---------------------------------------------------------------------------
//        
EXPORT_C void CNcdAttributes::SetAttributeL( TInt aAttribute, TInt32 aValue )
    {
    DLTRACEIN(("aAttribute: %d, aValue: %d", aAttribute, aValue));
    
    TAttribute& attrib = Attribute( aAttribute );
    
    ASSERT_ATTRIBUTE_WRITABLE( attrib, EAttributeTypeInt32 );
    
    attrib.iType = EAttributeTypeInt32;
    attrib.iAttribute.iInt32 = aValue;
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C void CNcdAttributes::SetAttributeL( 
    TInt aAttribute, 
    const TDesC& aValue )
    {
    DLTRACEIN(( _L("aAttribute: %d, aValue: %S"), aAttribute, &aValue ));
    
    TAttribute& attrib = Attribute( aAttribute );
    
    ASSERT_ATTRIBUTE_WRITABLE( attrib, EAttributeTypeString16 );
    
    HBufC* tempValue = aValue.AllocL();
    
    delete attrib.iAttribute.iString16;
    attrib.iAttribute.iString16 = tempValue;
    
    attrib.iType = EAttributeTypeString16;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C const TDesC& CNcdAttributes::AttributeString16( 
    TInt aAttribute ) const
    {
    DLTRACEIN(("aAttribute: %d", aAttribute));
    
    const TAttribute& attrib = Attribute( aAttribute );
    
    if ( attrib.iType == EAttributeTypeUndefined ) 
        {
        DLTRACEOUT(("Not set"));
        return KNullDesC;
        }
        
    ASSERT_ATTRIBUTE_READABLE( attrib, EAttributeTypeString16 );
    
    DASSERT( attrib.iAttribute.iString16 );
    DLTRACEOUT(( _L("value: %S"), 
        attrib.iAttribute.iString16 ));
        
    return *attrib.iAttribute.iString16;
    }
     

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C TInt32 CNcdAttributes::AttributeInt32( TInt aAttribute ) const
    {
    DLTRACEIN(("aAttribute: %d", aAttribute));
    
    const TAttribute& attrib = Attribute( aAttribute );

    if ( attrib.iType == EAttributeTypeUndefined ) 
        {
        DLTRACEOUT(("Not set"));
        return 0;
        }
    
    ASSERT_ATTRIBUTE_READABLE( attrib, EAttributeTypeInt32 );
    
    DLTRACEOUT(("value: %d", attrib.iAttribute.iInt32 ));
    return attrib.iAttribute.iInt32;    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C CNcdAttributes::TAttributeType CNcdAttributes::AttributeType( 
    TInt aAttribute ) const
    {
    DLTRACEIN(("aAttribute: %d", aAttribute));
    return Attribute( aAttribute ).iType;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C void CNcdAttributes::ExternalizeL( RWriteStream& aStream ) const
    {
    DLTRACEIN((""));
    aStream.WriteInt32L( iLowerLimit );
    aStream.WriteInt32L( iUpperLimit );
    
    DLTRACE(("Externalizing %d attributes", iAttributes.Count() ));
    DASSERT( iAttributes.Count() == ArraySize() );
    for ( TInt i = 0; i < iAttributes.Count(); ++i ) 
        {
        ExternalizeAttributeL( iAttributes[i], aStream );
        }
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
EXPORT_C void CNcdAttributes::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));    
    TInt lowerLimit = aStream.ReadInt32L();
    
    // Lower limits are not allowed to change
    if ( lowerLimit != iLowerLimit ) 
        {
        DLERROR(("Lower limits don't match, leaving"));
        User::Leave( KErrArgument );
        }
    
    // Ensure that upperlimit is in the allowed range
    NCD_ASSERT_ALWAYS( iUpperLimit > 0 && iUpperLimit > iLowerLimit, 
        ENcdPanicIndexOutOfRange );
    
    // Read stored upper limit, this will be overridden by the
    // upper limit given for NewL
    TInt upperLimit = aStream.ReadInt32L();
    
    // Reset attribute array to correct amount of attributes
    ResetAttributesL();
    
    // Read either all values from the stream or only those that fit 
    // the current limits.
    TInt count = Min( upperLimit, iUpperLimit ) - iLowerLimit;
    
    DLTRACE(("Internalizing %d attributes", count));
    for ( TInt i = 0; i < count; ++i )
        {
        iAttributes[i] = InternalizeAttributeL( aStream );
        }
    
    TInt leftOvers = upperLimit - iUpperLimit;
    // Check if some attributes were not read from the stream    
    if ( leftOvers ) 
        {
        DLTRACE(("Reading and deleting %d leftover attributes", leftOvers));
        while ( leftOvers-- ) 
            {
            TAttribute attribute = InternalizeAttributeL( aStream );
            DeleteAttribute( attribute );
            }
        }
    DLTRACEOUT(("Attributes internalized successfully" ));
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
CNcdAttributes::CNcdAttributes( TInt aUpperLimit, TInt aLowerLimit ) 
    : iLowerLimit( aLowerLimit ), 
      iUpperLimit( aUpperLimit )
    {
    }
    

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//        
void CNcdAttributes::ConstructL()
    {
    DLTRACEIN(("Upper limit: %d, lower limit: %d", 
        iUpperLimit, iLowerLimit ));
        
    NCD_ASSERT_ALWAYS( iUpperLimit > 0 && iUpperLimit > iLowerLimit, 
        ENcdPanicIndexOutOfRange );

    ResetAttributesL();
    }


// ---------------------------------------------------------------------------
// ConstructL for copying the attributes
// ---------------------------------------------------------------------------
//        
void CNcdAttributes::ConstructL( const CNcdAttributes& aAttributes )
    {
    DLTRACEIN((""));
    TInt count = ArraySize();
    DASSERT( count == aAttributes.iAttributes.Count() );
    
    iAttributes.ReserveL( count );
    for ( TInt i = 0; i < count; ++i )
        {
        iAttributes.Append( CopyAttributeL( aAttributes.iAttributes[i] ) );
        }
    }


// ---------------------------------------------------------------------------
// Clear attribute array
// ---------------------------------------------------------------------------
//        
void CNcdAttributes::ClearAttributes()
    {
    DLTRACEIN((""));
    for ( TInt i = 0; i < iAttributes.Count(); ++i ) 
        {
        DeleteAttribute( iAttributes[i] );
        }
    iAttributes.Reset();
    }


// ---------------------------------------------------------------------------
// Reset attribute array
// ---------------------------------------------------------------------------
//        
void CNcdAttributes::ResetAttributesL()
    {
    DLTRACEIN((""));
    ClearAttributes();
    
    TInt size = ArraySize();
    DLTRACE(("Resetting the array with %d attributes", size));
    iAttributes.ReserveL( size );
    TAttribute attrib;
    
    while( size-- ) 
        {
        iAttributes.Append( attrib );
        }    
    DLTRACEOUT(("Array size: %d", iAttributes.Count() ));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
TInt CNcdAttributes::ArraySize() const
    {
    return iUpperLimit - iLowerLimit;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdAttributes::ExternalizeAttributeL( 
    const TAttribute& aAttribute, RWriteStream& aStream ) const    
    {
    DLTRACEIN((""));    
    ExternalizeEnumL( aAttribute.iType, aStream );
    switch( aAttribute.iType ) 
        {
        case EAttributeTypeInt32:
            {
            DLTRACE(("Int"));
            aStream.WriteInt32L( aAttribute.iAttribute.iInt32 );
            break;
            }

        case EAttributeTypeString16:
            {
            DLTRACE(("String"));
            ExternalizeDesL( *aAttribute.iAttribute.iString16, aStream );
            break;
            }
        
        case EAttributeTypeUndefined:
            {
            DLTRACE(("Undefined"));
            // Nothing to do
            break;
            }
            
        default:
            NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
        }        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
CNcdAttributes::TAttribute CNcdAttributes::InternalizeAttributeL( 
    RReadStream& aStream ) const    
    {
    DLTRACEIN((""));  
    TAttribute attribute;
    InternalizeEnumL( attribute.iType, aStream );
    
    switch( attribute.iType ) 
        {
        case EAttributeTypeInt32:
            {
            attribute.iAttribute.iInt32 = aStream.ReadInt32L();
            break;
            }

        case EAttributeTypeString16:
            {
            InternalizeDesL( attribute.iAttribute.iString16, aStream );
            break;
            }
        
        case EAttributeTypeUndefined:
            {
            // Nothing to do
            break;
            }
            
        default:
            NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
        }        
    DLTRACEOUT(("Read attribute of type: %d", attribute.iType ));
    return attribute; 
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CNcdAttributes::DeleteAttribute( TAttribute& aAttribute ) 
    {
    DLTRACEIN((""));
    if ( aAttribute.iType == EAttributeTypeString16 ) 
        {
        delete aAttribute.iAttribute.iString16;
        aAttribute.iAttribute.iString16 = NULL;
        }    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
CNcdAttributes::TAttribute CNcdAttributes::CopyAttributeL( 
    const TAttribute& aAttribute ) const
    {
    DLTRACEIN((""));
    TAttribute copy;
    copy.iType = aAttribute.iType;
    
    switch( aAttribute.iType ) 
        {
        case EAttributeTypeInt32:
            {
            copy.iAttribute.iInt32 = aAttribute.iAttribute.iInt32;
            break;
            }

        case EAttributeTypeString16:
            {
            if ( aAttribute.iAttribute.iString16 ) 
                {                
                copy.iAttribute.iString16 = 
                    aAttribute.iAttribute.iString16->AllocL();
                }
            break;
            }
        
        case EAttributeTypeUndefined:
            {
            // Nothing to do
            break;
            }
            
        default:
            NCD_ASSERT_ALWAYS( 0, ENcdPanicIndexOutOfRange );
        } 
    return copy;       
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
const CNcdAttributes::TAttribute& CNcdAttributes::Attribute( 
    TInt aAttribute ) const
    {
    DLTRACEIN(("aAttribute: %d", aAttribute));
    ASSERT_ATTRIBUTE_IN_RANGE( aAttribute );
    return iAttributes[ aAttribute - iLowerLimit ];
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
CNcdAttributes::TAttribute& CNcdAttributes::Attribute( TInt aAttribute )
    {
    DLTRACEIN(("aAttribute: %d", aAttribute));
    ASSERT_ATTRIBUTE_IN_RANGE( aAttribute );
    return iAttributes[ aAttribute - iLowerLimit ];
    }
