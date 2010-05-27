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
* Description:   Contains CCatalogsInterfaceBase class implementation
*
*/


#include "catalogsinterfacebase.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsdebug.h"


CCatalogsInterfaceBase::CCatalogsInterfaceBase( CCatalogsInterfaceBase* aParent )
: CBase(),
  iParent( aParent )
    {
    
    }
  

CCatalogsInterfaceBase::~CCatalogsInterfaceBase()
    {
    DLTRACE(("this-ptr: %x", this));
    iInterfaces.ResetAndDestroy();    
    }


void CCatalogsInterfaceBase::SetParentL( CCatalogsInterfaceBase* aParent )
    {
    DLTRACEIN(("this-ptr: %x", this));
    
    if ( aParent == this )
        {
        // We do not accept this object to be its own parent.
        DLTRACEOUT(("Parent is same as the object itself"))
        return;
        }

    if ( iParent != NULL )
        {
        DLERROR(("Object already had a parent"));
        
        // This option can not be accepted, because problems would arise when
        // transferring interfaces from this class object to the new parent. Or,
        // if this class object should be made the top root, then it would not be
        // known which interfaces should be moved here.
        // Only parent knows the interfaces and children do not know their own.
        // So, we do not know which interfaces should be removed from the parent
        // and which should be left.
        
        DASSERT( EFalse );
        
        User::Leave( KErrArgument );
        }
    else if ( aParent == NULL )
        {
        DLTRACEOUT(("Nothing needs to be done, because NULL parent was given"));

        // Because the parent of this object is NULL and the new parent is NULL
        // then nothing needs to be done. In other words, this object remains to be
        // the top parent.
        
        return;
        }

    // This class object was top parent itself and the new parent seems to be acceptable.
    // Continue by moving interfaces from this class object to the new top parent.
        
    DLINFO(("New parent object was given"));

    // This class object was a top parent itself.
    // So, move all the interface elements from this list to the
    // list of the new parent.
    CCatalogsInterfaceIdentifier* interfaceIdentifier( NULL );
    TInt interfaceError( KErrNone );
    while ( iInterfaces.Count() > 0 )
        {
        interfaceIdentifier = iInterfaces[ 0 ];
        // Notice that new parent takes the ownership of the interface.
        // If the new parent is not the top parent, these interfaces will
        // be forwarded to the top parent automatically.
        // TRAP updates the error value to KErrNone everytime if no error occurs.
        TRAP( interfaceError, aParent->AddInterfaceL( interfaceIdentifier ) );
        if ( interfaceError != KErrNone )
            {
            DLERROR(("Could not add interface to the parent: %d", interfaceError));
            DASSERT( EFalse );
            // Because the ownership of the interface could not been transferred.
            // delete the interface here. There is nothing much else that we could do.
            delete interfaceIdentifier;
            interfaceIdentifier = NULL;
            }
        // Remove the interface from the list because it has been moved above.
        iInterfaces.Remove( 0 );
        }
    
    // The old parent value was NULL because this function is allower only for the
    // top parent. Update the parent pointer.
    iParent = aParent;

    // The top parent also keeps track of the reference counts of all its children.
    // Update the reference counts of the new parent.
    // Notice that the parent reference count always contains the sum of all the reference
    // counts of its children and their children etc. So, add this information to the new
    // parent.
    for ( TInt i = 0; i < iRefCount; ++i )
        {
        iParent->AddRef();
        }
    for ( TInt i = 0; i < iInternalRefCount; ++i )
        {
        iParent->InternalAddRef();
        }        

    DLTRACEOUT((""));
    }


void CCatalogsInterfaceBase::RemoveFromParent()
    {
    DLTRACEIN(("this-ptr: %x", this));
    
    if ( iParent == NULL )
        {
        DLTRACEOUT(("This is a top parent itself."));
        // Nothing to do here.
        return;
        }

    // This class object has a parent
    // Release old parent counter values according 
    // to the values this object has. Do not change the values
    // of this object.
    for ( TInt i = 0; i < iRefCount; ++i )
        {
        iParent->Release();
        }
    for ( TInt i = 0; i < iInternalRefCount; ++i )
        {
        iParent->InternalRelease();
        }
        
    // Set the parent to be NULL, because this will act as its own parent after this.
    iParent = NULL;

    DLTRACEOUT((""));
    }
 
    
void CCatalogsInterfaceBase::AddInterfaceL( CCatalogsInterfaceIdentifier* aInterface )
    {
    DLTRACE(("this-ptr: %x", this));

    if ( aInterface != NULL )
        {
        // All the interfaces exist in the top parent. So, if this object 
        // has parent, delegate query to it.
        if ( iParent != NULL )
            {
            iParent->AddInterfaceL( aInterface );
            }
        else
            {
            // This is a top parent.
            // If old interface of the same type exists, then remove
            // it because only one of the kind is accepted.
            RemoveInterface( aInterface->InterfaceId() );
            CleanupStack::PushL( aInterface );
            iInterfaces.AppendL( aInterface );
            CleanupStack::Pop( aInterface );
            }
        }
    }
    
    
void CCatalogsInterfaceBase::RemoveInterface( TInt aId )
    {
    DLTRACE(("this-ptr: %x", this));

    if ( iParent != NULL )
        {
        // All the interfaces exist in the top parent. So, if this 
        // has parent, delegate query to it.
        iParent->RemoveInterface( aId );
        }
    else
        {
        // This is a top parent
        for ( TInt i = 0; i < iInterfaces.Count(); ++i )
            {
            if ( iInterfaces[ i ]->InterfaceId() == aId )
                {
                delete iInterfaces[i];
                iInterfaces.Remove( i );
                break;
                }
            }        
        }    
    }

TInt CCatalogsInterfaceBase::TotalRefCount() const
    {
    DLTRACE(("this-ptr: %x", this));

    if ( iParent != NULL )
        {
        // Parent has all the information
        return iParent->TotalRefCount();
        }
    else
        {
        // This is a top parent. So, return the information.
        return RefCount() + InternalRefCount();        
        }
    }

TInt CCatalogsInterfaceBase::InternalRefCount() const
    {
    DLTRACEIN(("this-ptr: %x", this));

    if ( iParent != NULL )
        {
        // Parent has all the information.
        return iParent->InternalRefCount();
        }
    else
        {
        DLTRACEOUT(( "InternalRefCount: %d", iInternalRefCount ));
        // This is a top parent
        return iInternalRefCount;        
        }
    }

TInt CCatalogsInterfaceBase::InternalAddRef()
    {
    DLTRACE(("this-ptr: %x", this));

    // Increase the reference counter for all the
    // objects starting from this object to the top parent.
    iInternalRefCount++;
    
    if ( iParent != NULL )
        {
        // Parent has all the information
        return iParent->InternalAddRef();
        }
    else
        {
        DLTRACE(( "InternalRefCount: %d", iInternalRefCount ));
        // This is a top parent.
        return iInternalRefCount;            
        }
    }
    
TInt CCatalogsInterfaceBase::InternalRelease()
    {
    DLTRACE(("this-ptr: %x", this));
    DASSERT( iInternalRefCount > 0 );

    // Descrease the reference counter for all the
    // objects starting from this object to the top parent.
    if ( iInternalRefCount > 0 )
        {
        // Do not let the counter to go below zero.
        --iInternalRefCount;            
        }
        
    if ( iParent != NULL )
        {
        // Parent has all the information
        return iParent->InternalRelease();
        }
    else
        {
        // This is parent.
        if ( TotalRefCount() > 0 )
            {
            DLTRACE(( "InternalRefCount: %d", iInternalRefCount ));
            return iInternalRefCount;
            }
        else
            {
            DLTRACE(("Deleting this: %x", this));
            // No references are left for this object.
            delete this;
            return 0;
            }            
        }
    }

TInt CCatalogsInterfaceBase::MyTotalRefCount() const
    {
    DLTRACE(("this-ptr: %x", this));

    return MyRefCount() + MyInternalRefCount();        
    }

TInt CCatalogsInterfaceBase::MyRefCount() const
    {
    DLTRACE(("this-ptr: %x", this));

    return iRefCount;        
    }

TInt CCatalogsInterfaceBase::MyInternalRefCount() const
    {
    DLTRACE(("this-ptr: %x", this));
    
    return iInternalRefCount;        
    }


// MCatalogsBase functions
    
 
TInt CCatalogsInterfaceBase::AddRef() const
    {    

    // Increase the reference counter for all the
    // objects starting from this object to the top parent.
    iRefCount++;

    if ( iParent != NULL )
        {
        // Parent has all the info.
        DLTRACE(( "this-ptr: %x, RefCount: %d", this, iRefCount ));
        return iParent->AddRef();
        }
    else
        {
        DLTRACE(( "this-ptr: %x, RefCount: %d", this, iRefCount ));
        return iRefCount;        
        }
    }

TInt CCatalogsInterfaceBase::Release() const
    {    
    DASSERT( iRefCount > 0 );

    // Descrease the reference counter for all the
    // objects starting from this object to the top parent.
    if ( iRefCount > 0 )
        {
        // Do not let the counter to go below zero.
        --iRefCount;            
        }
        
    if ( iParent != NULL )
        {
        DLTRACE((("this-ptr: %x, RefCount: %d"), this, iRefCount ));
        // Parent has all the info.
        return iParent->Release();        
        }
    else
        {
        // This is parent
        if ( TotalRefCount() > 0 )
            {
            DLTRACE(( "RefCount: %d, this: %x", iRefCount, this ));
            return iRefCount;
            }
        else
            {
            DLTRACE(("Deleting this: %x", this));
            // No references are left for this object.
            delete this;
            return 0;
            }        
        }
    }

TInt CCatalogsInterfaceBase::RefCount() const
    {    
    if ( iParent != NULL )
        {
        // Parent has all the info
        return iParent->RefCount();
        }
    else
        {
        DLTRACE(( "RefCount: %d", iRefCount ));
        // This is parent
        return iRefCount;    
        }
    }


const TAny* CCatalogsInterfaceBase::QueryInterfaceL( TInt aInterfaceType ) const
    {
    DLTRACEIN(( "Interface: %d, this-ptr: %x", aInterfaceType, this ));

    // All the interfaces exist in the parent. So, if this 
    // has parent, delegate query to it.
    if ( iParent != NULL )
        {
        // The parent has all the info
        return iParent->QueryInterfaceL( aInterfaceType );
        }
    else
        {
        // This is parent.
        // Check the interfaces from the list and return the interface
        // that matches the given type
        for ( TInt i = 0; i < iInterfaces.Count(); ++i )
            {
            if ( iInterfaces[ i ]->InterfaceId() == aInterfaceType )
                {
                // Match was found
                const TAny* interface( iInterfaces[ i ]->InterfaceObject() );
                const CCatalogsInterfaceBase* interfaceBase( iInterfaces[ i ]->InterfaceBaseObject() );                

                // Because interface is returned, the ref count has to be increased.
                interfaceBase->AddRef();

                DLTRACEOUT(( "Interface found: %X", interface ));
                // Return interface, not interfaceBase, because interfaceBase is a pointer to
                // the base class. They may have different pointer because of "public virtual <class>"
                // inheritance.
                return interface;            
                }
            }

        // Interface was not found
        DLTRACEOUT( ( "return NULL" ) );        
        return NULL;        
        }
    }


