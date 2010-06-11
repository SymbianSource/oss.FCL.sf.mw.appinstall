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
* Description:   Contains MCatalogsBase interface
*
*/


#ifndef M_CATALOGS_BASE_H
#define M_CATALOGS_BASE_H

#include <e32cmn.h>
#include "catalogsinterfaceids.h"

/**
 *  This interface provides functions for querying the interfaces that
 *  a class implements. Also, functions for reference counter are provided.
 *  The reference counter is used to keep track of the number of users
 *  of the implemented class. 
 *
 *  
 */
class MCatalogsBase
    {
    
public:
 
    /**
     * KInterfaceUid defines the identifier for this interface.
     * Child interfaces should define their own unique values for their
     * KInterfaceUid.
     *
     * 
     */
    enum { KInterfaceUid = ECatalogsBaseUid };


    /**
     * This function is used to test if this object contains an implementation of
     * the interface of the given type.
     *
     * If the object contains the interface, a pointer to it is returned, and
     * the reference count is increased by one. Otherwise, NULL is returned.
     *
     * @note This interface does not allow the user to delete object directly 
     * (destructor is protected). The destruction of this object should be done 
     * by calling Release-function.
     *
     * A template shorthand version is provided here, making the usage easier.
     * An example of using the template method:
     * MWantedInterface* object = 
     *      someObjectInheritedFromMCatalogBase->QueryInterfaceL< MWantedInterface >();
     *
     * 
     * @return Pointer to the object's interface, if the object implements the given
     *  interface, NULL otherwise. Counted, Release() must be called after use.
     * @see MCatalogsBase::QueryInterfaceLC
     */
    template< class T >
    inline T* QueryInterfaceL()
        {
        const MCatalogsBase* self = this;
        const TAny* interface = self->QueryInterfaceL( T::KInterfaceUid );
        return const_cast< T* >( static_cast< const T* >( interface ) );
        }
    
    /**
     * This is const version of QueryInterfaceL.
     * 
     */
    template< class T >
    inline const T* QueryInterfaceL() const
        {      
        return static_cast< const T* >( QueryInterfaceL( T::KInterfaceUid ) );
        }

    /**
     * Like QueryInterfaceL, tries to retrieve an interface of specified type
     * from the object. If the interface pointer is returned, it is appended
     * to the cleanup stack, so that a following CleanupStack::PopAndDestroy()
     * will call Release() on the interface pointer appropriately.
     *
     * @return Pointer to the object's interface, if the object implements
     * the given interface, or NULL if this object does not implement the
     * given interface. If NULL is returned then the object is not pushed to
     * the cleanupstack. 
     *
     * 
     * @return Pointer to the object's interface, if the object implements the given
     *  interface, NULL otherwise. Returned non-NULL pointer reference is counted, must
     *  be released with a call to CleanupStack::PopAndDestroy().
     * @see MCatalogsBase::QueryInterfaceL
     */ 
    template< class T >
    inline T* QueryInterfaceLC()
        {
        T* interface( QueryInterfaceL< T >() );
        if ( interface )
            {
            // Do not push NULL object into the cleanup stack.
            CleanupReleasePushL( *interface );        
            }
        return interface;
        }

    /**
     * This is const version of QueryInterfaceLC.
     * 
     */
    template< class T >
    inline const T* QueryInterfaceLC() const
        {
        const T* interface( QueryInterfaceL< const T >() );
        if ( interface )
            {
            // Do not push NULL object into the cleanup stack.
            CleanupReleasePushL( *const_cast<T*>( interface ) );        
            }
        return interface;
        }
    
    /**
     * Increments the reference counter.
     *
     * @note QueryInterfaceL() already increases the value by one
     *  if interface check was successfull. This function may be used
     *  if user knows that the interface is used in multiple places and
     *  Release() is called accordingly.
     *
     * 
     * @return New reference count value. For debug purposes only.
     */
    virtual TInt AddRef() const = 0;		
    
    /**
     * Releases a reference. Deletes the object if the reference count
     * reaches 0. 
     * Notice that destructor is defined as protected. So, the object of
     * this interface can be deleted using this function and only after 
     * reference count is zero.
     *
     * 
     * @return Reference count after release. For debug purposes only.
     */
    virtual TInt Release() const = 0;
    
    
protected:

    /**
     * QueryInterfaceL
     *
     * This function is used to test if this object contains an implementation of
     * the interface of the given type.
     * If the object contains the interface, then the reference count is increased
     * by one.
     *
     * 
     * @param aInterfaceType TInt that specifies the interface that this object should implement.
     * @return Pointer to this object if the object implements given interface 
     * or NULL if this object does not implement the given interface. Counted, Release() must
     * be called after use.
     */
    virtual const TAny* QueryInterfaceL( TInt aInterfaceType ) const = 0;
    

    /**
     * The destructor of an interface is set virtual to make sure that
     * the destructors of derived classes are called appropriately when the
     * object is destroyed.
     *
     * Destructors for MCatalogsBase and all derived interfaces are defined as
     * protected to prevent direct use of delete on interface pointers (Release()
     * must be called instead of delete).
     *
     * 
     */
    virtual ~MCatalogsBase() {}

    };

#endif // M_CATALOGS_BASE_H
