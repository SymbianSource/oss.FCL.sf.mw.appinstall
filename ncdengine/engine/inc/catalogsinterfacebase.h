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
* Description:   Contains CCatalogsInterfacebase class
*
*/


#ifndef CATALOGS_INTERFACE_BASE_H
#define CATALOGS_INTERFACE_BASE_H


#include <e32base.h>
#include <e32cmn.h>

#include "catalogsbase.h"
#include "catalogsinterfaceids.h"


class CCatalogsInterfaceIdentifier;


/**
 * This class contains list of the interface objects that this class object 
 * contains or implements. Also, reference counters are provided so that
 * the user may check if some of the interfaces are used.
 *
 * @note Two kind of counter informations are provided here. 
 * One is for the top parent reference counter. 
 * Another is class object specific (My-functions).
 * The top parent counter contains the sum of all the reference counts of 
 * its children and their children, etc.. 
 * My-reference counter functions give the child reference 
 * counts for that specific class object and its children, and their children,
 * etc.. 
 * Notice also that when the My-reference counter info is asked for a parent, 
 * the parent specific information contains also the total count of references
 * including child reference information. This should be adequate because some
 * classes may implement multiple interfaces and thus have the same counter 
 * for them.
 */
class CCatalogsInterfaceBase: public CBase,
                              public virtual MCatalogsBase
    {
public:

    /**
     * Destructor
     *
     * @note Only deletes the register list info elements that are owned 
     * by this class. But, does not delete the actual interface elements.
     * So, the destructor of the child of this class has to delete
     * separately the objects that it owns.
     */
    virtual ~CCatalogsInterfaceBase();


    /**
     * Sets the new parent for this class object.
     *
     * If the given parent is same as this,
     * then nothing is done. 
     
     * The reference counters of the new parent 
     * are increased according to the counters of this object.
     *
     * @note This function can be called only to the class object that
     * is top parent itself at the moment. If this function is called
     * to the class object that is not a top parent, then function leaves
     * with KErrArgument. The reason for this is that only top parent knows
     * all the registered interfaces and only that parent may update these
     * interfaces to the new top parent. Also, notice that aParent does not
     * need to be a top parent, because it will forward the interfaces
     * to its own top parent.
     *
     * @note If the given parent contains interfaces that this class object
     * contains, then interfaces pointers of the parent will be replaced
     * be the interfaces of this class object. 
     *
     * @param aParent The new parent of this class object.
     * If NULL is given then this class object does not have a parent.
     * Ownership is not transferred.
     * @exception Leaves with KErrArgument if this class object is not
     * a top parent when this function is called.
     */
    void SetParentL( CCatalogsInterfaceBase* aParent );

    /**
     * This function is used to remove this class object from the ownership of its
     * parents. This function only affects the reference counters of the parents.
     * The parent of this object is set to NULL by this function. In other words this
     * class object will be a top parent itself after this function has finished.
     *
     * @note WARNING! Use this function with great care. This function only
     * releases parents' reference counts that have been reserved by this class object and its
     * children. Interfaces are not released. If this class object will be deleted
     * before its parent, the parent may contain obsolete interface references if
     * they are not separately removed from the top parents interface list. Use of those
     * obsolete references will most likely crash the program.
     *
     * @note Even if the total reference count of this object is zero after removing it from the parent, 
     * this object will not be deleted in this function. So, the user has to call delete for this object 
     * separately if the object should be deleted in that situation.
     */
    void RemoveFromParent(); 
     
    /**
     * Adds the given interface object into the interface register list. 
     * If the interface already exists in the list, the old interface 
     * object is removed and replaced by the new object.
     *
     * @param aInterface Interface infromation that is inserted to the
     * interface register list. The list is used to check if the object
     * implements an interface that is requested by using QueryInterface
     * functions. Ownership is transferred.
     */
    void AddInterfaceL( CCatalogsInterfaceIdentifier* aInterface );
    
    /**
     * Finds the interface from the interface register list
     * according to the given interface id and removes
     * the interface.
     *
     * @param aId Interface identifier that is used to remove the
     * registered interface from the interface list.
     */
    void RemoveInterface( TInt aId );


    /**
     * @note This information is gotten from the top parent
     * (if the parent is different object that this class object).
     *
     * @return The total number of references to the object
     * This contains the internal and external counts.
     */
    TInt TotalRefCount() const;

    
    /**
     * @note This updates the information contained in the parent(s)
     * (if the parent is different object that this class object).
     *
     * @return The number of internal references to the object
     * after the addition.
     * Internal reference counts are meant to be used when the
     * counter info is want to be hidden from the API user.
     */
    TInt InternalAddRef();

    /**
     * @note This updates the information contained in the parent(s)
     * (if the parent is different object that this class object)
     * in addition to the counter information contained for this
     * class object.
     *
     * @return The number of internal references to the object
     * after the release.
     * Internal reference counts are meant to be used when the
     * counter info is want to be hidden from the API user.
     */
    TInt InternalRelease();

    /**
     * @note This gives the information contained in the top parent
     * (if the parent is different object that this class object).
     *
     * @return The number of internal references to the object.
     * Internal reference counts are meant to be used when the
     * counter info is want to be hidden from the API user.
     */
    TInt InternalRefCount() const;


    /**
     * @note This information is this class object specific.
     * (does not forward the request to the top parent).
     *
     * @return The total number of references to the object
     * This contains the internal and external counts.
     */
    TInt MyTotalRefCount() const;
        
    /**
     * @note This information is this class object specific
     * (does not forward the request to the top parent).
     *
     * @return The number of API references to the object.
     */
    TInt MyRefCount() const;

    /**
     * @note This information is this class object specific
     * (does not forward the request to the top parent).
     *
     * @return The number of internal references to the object.
     * Internal reference counts are meant to be used when the
     * counter info is want to be hidden from the API user.
     */
    TInt MyInternalRefCount() const;


public: // MCatalogsBase

    /**
     * @see MCatalogsBase::AddRef
     */
    virtual TInt AddRef() const;
    
    
    /**
     * @see MCatalogsBase::Release
     */
    virtual TInt Release() const;
    
    
    /**
     * @see MCatalogsBase::RefCount
     */
    virtual TInt RefCount() const;


protected: // MCatalogsBase

    /**
     * @see MCatalogsBase::QueryInterfaceL
     */
    virtual const TAny* QueryInterfaceL( TInt aInterfaceType ) const;


protected:

    /**
     * Constructor
     *
     * Because this class is meant to be a parent instead
     * of an object class itself. The constructor is protected.
     * So, the objects should be created from the child classes.
     *
     * @param aParent If NULL this object does not have parent.
     * Ownership is not transferred.
     */
    CCatalogsInterfaceBase( CCatalogsInterfaceBase* aParent );
 

private:
    // Prevent
    CCatalogsInterfaceBase( const CCatalogsInterfaceBase& aObject );
    CCatalogsInterfaceBase& operator =( const CCatalogsInterfaceBase& aObject );


private: // data

    // If NULL then this class object does not have parent.
    // Notice that the parent here does not necessarily mean same as the
    // owner of the class. The parent means the object that contains
    // the registered interfaces and reference counts of all the objects
    // that have set the parent as their parent.
    CCatalogsInterfaceBase* iParent;
    
    // This counter knows how many times the interfaces of this class
    // has been queried from ui.
    // Notice, that the total counter is set to zero when an object is created.
    // When Release is called, the object will be deleted if the ref count
    // is zero or less.
    mutable TInt iRefCount;
    TInt iInternalRefCount;
    
    // This list will contain pointers to the interfaces
    // that are identified by interface uid.
    // QueryInterface-functions check the right pointers
    // from this list.
    RPointerArray<CCatalogsInterfaceIdentifier> iInterfaces;     

    };

#endif // CATALOGS_INTERFACE_BASE_H
