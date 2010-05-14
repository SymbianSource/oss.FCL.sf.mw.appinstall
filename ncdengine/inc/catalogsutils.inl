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
* Description:   Contains definition of catalogsutils functions 
*
*/


#include "catalogsdebug.h"

// ---------------------------------------------------------------------------
// IntToDes16
// 64-bit specialization is in catalogsutils.cpp
// ---------------------------------------------------------------------------
//   
template<typename T>
void IntToDes16( const T& aInt, TDes& aDes )
    {
    DLTRACEIN(("aDes.MaxLength() = %d", aDes.MaxLength() ));
    DASSERT( aDes.MaxLength() >= sizeof(T)/2 );    
    *(( T* )aDes.Ptr()) = aInt;        
    }
    

// ---------------------------------------------------------------------------
// Des16ToInt
// 64-bit specialization is in catalogsutils.cpp
// ---------------------------------------------------------------------------
//   
template<typename T>    
void Des16ToInt( const TDesC& aDes, T& aInt )
    {
    DLTRACEIN(( _L("Des: %S"), &aDes ));
    aInt = *(( T* )aDes.Ptr());
    DLTRACEOUT(( "Result: %d", aInt ));
    }
    



// Template class CleanupResetAndDestroy
template <typename T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
/**
Creates a TCleanupItem for the specified object.

The cleanup operation is the private static function Close() of this class.

@param aRef The object for which a TCleanupItem is to be constructed.
*/
	{CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));}




template <typename T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
/**
The cleanup operation to be performed.

@param aPtr A pointer to the object for which clean up is to be performed. 
            The implementation calls Close() on this object.
*/
	{(STATIC_CAST(T*,aPtr))->ResetAndDestroy();}




// See header file e32base.h for in-source comment.
template <typename T>
inline void CleanupResetAndDestroyPushL(T& aRef)
	{CleanupResetAndDestroy<T>::PushL(aRef);}
	

// Template class CleanupInternalRelease
template <typename T>
inline void CleanupInternalRelease<T>::PushL(T& aRef)
/**
Creates a TCleanupItem for the specified object.

The cleanup operation is the private static function Close() of this class.

@param aRef The object for which a TCleanupItem is to be constructed.
*/
	{CleanupStack::PushL(TCleanupItem(&InternalRelease,&aRef));}


template <typename T>
void CleanupInternalRelease<T>::InternalRelease(TAny *aPtr)
/**
The cleanup operation to be performed.

@param aPtr A pointer to the object for which clean up is to be performed. 
            The implementation calls Close() on this object.
*/
	{(STATIC_CAST(T*,aPtr))->InternalRelease();}
	

template <typename T>
inline void CleanupInternalReleasePushL(T& aRef)
	{CleanupInternalRelease<T>::PushL(aRef);}

template <typename T>
void ResetAndCloseArray( RPointerArray<T>& aArray )
    {
    TInt count( aArray.Count() );    
    while ( count-- )
        {
        aArray[count]->Close();                    
        }
    aArray.Reset();
    }


template <typename T>
void ResetAndCloseArray( RArray<T>& aArray )
    {
    TInt count( aArray.Count() );    
    while ( count-- )
        {
        aArray[count].Close();                    
        }
    aArray.Reset();
    }
    
    
template <typename Casted, typename T>
void ResetAndDestroyWithCast( RPointerArray<T>& aArray )
    {
    TInt count = aArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        Casted* info =
            static_cast<Casted*>( aArray[i] );
        delete info;
        info = NULL;
        }
    aArray.Reset();        
    }
    

template <typename T>
void DeleteFromArray( RPointerArray<T>& aArray, TInt aIndex )
    {
    delete aArray[ aIndex ];
    aArray.Remove( aIndex );
    }

template <typename Casted, typename T>
void DeleteFromArray( RPointerArray<T>& aArray, TInt aIndex )
    {
    delete static_cast<Casted*>( aArray[ aIndex ] );
    aArray.Remove( aIndex );    
    }

    
template <typename EnumType>
void InternalizeEnumL( EnumType& aTarget, RReadStream& aReadStream )
    {
    aTarget = static_cast<EnumType>( aReadStream.ReadInt32L() );
    }


template <typename EnumType>
void ExternalizeEnumL( const EnumType& aSource, RWriteStream& aWriteStream )
    {
    aWriteStream.WriteInt32L( aSource );
    }
    

template <typename T>
TBool IsOneOf( 
    const T& aData, 
    const T& aComp1, 
    const T& aComp2 )
    {
    return aData == aComp1 || 
           aData == aComp2;
    }


template <typename T>
TBool IsOneOf( 
    const T& aData, 
    const T& aComp1, 
    const T& aComp2, 
    const T& aComp3 )
    {
    return IsOneOf( aData, aComp1, aComp2 ) || 
           aData == aComp3;
    }


template <typename T>
TBool IsOneOf( 
    const T& aData, 
    const T& aComp1, 
    const T& aComp2, 
    const T& aComp3, 
    const T& aComp4 )
    {
    return aData == aComp1 || 
           aData == aComp2 ||
           aData == aComp3 ||
           aData == aComp4;
    }


template <typename T>
void ReleasePtr( T*& aObject )
    {
    if ( aObject ) 
        {
        aObject->Release();
        aObject = NULL;
        }
    }


template <typename T>
void ClosePtr( T*& aObject )
    {
    if ( aObject ) 
        {
        aObject->Close();
        aObject = NULL;
        }
    }


template <typename T>
void DeletePtr( T*& aObject )
    {
    delete aObject;
    aObject = NULL;
    }

