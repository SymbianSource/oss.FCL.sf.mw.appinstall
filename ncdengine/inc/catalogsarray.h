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


#ifndef R_CATALOGS_ARRAY_H
#define R_CATALOGS_ARRAY_H

#include <e32cmn.h>

/**
 *  Array class for MCatalogsBase-derived interface pointers. Can be operated
 *  like ordinary RPointerArray array.
 *
 *  
 */
template< class T >
class RCatalogsArray : public RPointerArray< T >
    {
public:
    
    RCatalogsArray();
    
    RCatalogsArray( TInt aGranularity );
        
    /**
     * Empties the array and releases the referenced objects.
     *
     * It frees all memory allocated to the array and resets the internal state so
     * that it is ready to be reused. The function also calls Release() on all of
     * the objects whose pointers are contained by the array.
     * This array object can be allowed to go out of scope after a call to this
     * function.
     *
     * 
     * @see ResetAndRelease()
     */
    void ResetAndDestroy();


    /**
     * Empties the array and releases the referenced objects.
     *
     * It frees all memory allocated to the array and resets the internal state so
     * that it is ready to be reused. The function also calls Release() on all of
     * the objects whose pointers are contained by the array.
     * This array object can be allowed to go out of scope after a call to this
     * function.
     *
     * @note Same as ResetAndDestroy(), can be used instead if a more descriptive
     *       terminology for the operation is desired (contained objects will not
     *       necessarily be destroyed, but their Release() method is called).
     *
     * 
     * @see ResetAndDestroy()
     */
    inline void ResetAndRelease()
        {
        ResetAndDestroy();
        }

    };

#include "catalogsarray.inl"

#endif // R_CATALOGS_ARRAY_H
