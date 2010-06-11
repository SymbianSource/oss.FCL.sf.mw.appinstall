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
* Description:   Contains CNcdInstalledContent class implementation
*
*/


#include "ncdinstalledcontentimpl.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"
#include "ncdnodeinstallproxy.h"


// ======== PUBLIC MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdInstalledContent::CNcdInstalledContent( 
    CNcdNodeInstallProxy& aParent,
    TInstalledContentType aType )
    : CCatalogsInterfaceBase( NULL ),
    iOwner( aParent ),
    iContentType( aType )
    {
    
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CNcdInstalledContent::ConstructL()
    {
    DLTRACEIN((""));
    // Register the interface
    MNcdInstalledContent* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this, 
            MNcdInstalledContent::KInterfaceUid ) );
            
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdInstalledContent::~CNcdInstalledContent()
    {
    DLTRACEIN(("this-ptr: %x", this));
    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdInstalledContent::KInterfaceUid );    
    DLTRACEOUT(("this-ptr: %x", this));
    }



// ---------------------------------------------------------------------------
// Is launchable
// ---------------------------------------------------------------------------
//
TBool CNcdInstalledContent::IsLaunchable() const
    {
    return iLaunchable;
    }


// ---------------------------------------------------------------------------
// Content type getter
// ---------------------------------------------------------------------------
//
MNcdInstalledContent::TInstalledContentType 
    CNcdInstalledContent::ContentType() const
    {
    return iContentType;
    }



// ---------------------------------------------------------------------------
// Set launchable
// ---------------------------------------------------------------------------
//
void CNcdInstalledContent::SetLaunchable( TBool aLaunchable )
    {
    DLTRACEIN(("Launchable: %d", aLaunchable));
    iLaunchable = aLaunchable;
    }



// ---------------------------------------------------------------------------
// Set launchable
// ---------------------------------------------------------------------------
//
TInt CNcdInstalledContent::AddRef()
    {
    DLTRACEIN(("this-ptr: %x", this));
    
    TInt count = CCatalogsInterfaceBase::AddRef();
    
    // Ensure that NodeInstall-proxy stays alive as long as necessary
    // The owner should not be deleted before this class object's 
    // reference count is back to zero. The owner should
    // handle the deletion of this class object separately for example 
    // when its destructor is called.
    if ( count > 1 ) 
        {
        DLTRACE(("Adding a ref to NodeInstall, count: %d", count));
        ContentOwner().InternalAddRef();
        }
    DLTRACEOUT(("this-ptr: %x, count: %d", this, count));
    return count;
    }

// ---------------------------------------------------------------------------
// Set launchable
// ---------------------------------------------------------------------------
//
TInt CNcdInstalledContent::Release()
    {
    DLTRACEIN(("this-ptr: %x", this));
    
    TInt count = CCatalogsInterfaceBase::Release();

    // The owner should not be deleted before this class object's 
    // reference count is back to zero. The owner should
    // handle the deletion of this class object separately for example 
    // when its destructor is called.
    if ( count >= 1 ) 
        {
        DLTRACE(("Releasing a ref from NodeInstall, count: %d", count));
        ContentOwner().InternalRelease();
        }
        
    return count;
    }


// ---------------------------------------------------------------------------
// Get the owner of this class object
// ---------------------------------------------------------------------------
//
CNcdNodeInstallProxy& CNcdInstalledContent::ContentOwner() const
    {
    return iOwner;
    }
