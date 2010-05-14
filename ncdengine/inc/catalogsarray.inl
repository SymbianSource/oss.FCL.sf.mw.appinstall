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
* Description:   Contains definition of Catalogs interface pointer array 
*
*/


#ifndef R_CATALOGS_ARRAY_INL
#define R_CATALOGS_ARRAY_INL

template< class T >
RCatalogsArray< T >::RCatalogsArray() 
    : RPointerArray<T>()
    {
    }


template< class T >
RCatalogsArray< T >::RCatalogsArray( TInt aGranularity ) 
    : RPointerArray<T>( aGranularity )
    {
    }

/**
 *  Empties the array and releases the referenced objects.
 *
 *  It frees all memory allocated to the array and resets the internal state so
 *  that it is ready to be reused. The function also calls Release() on all of
 *  the objects whose pointers are contained by the array.
 *  This array object can be allowed to go out of scope after a call to this
 *  function.
 */
template< class T >
void RCatalogsArray< T >::ResetAndDestroy()
    {
    for( TInt i=0; i<RPointerArray< T >::Count(); i++ )
        {
        (*this)[i]->Release();
        }
    RPointerArray< T >::Reset();
    }

#endif // R_CATALOGS_ARRAY_INL
