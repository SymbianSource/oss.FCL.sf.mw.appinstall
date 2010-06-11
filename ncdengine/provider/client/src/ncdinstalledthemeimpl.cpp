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
* Description:   Contains CNcdInstalledTheme class implementation
*
*/


#include "ncdnodeinstallproxy.h"
#include "ncdinstalledthemeimpl.h"
#include "ncdinstallationservice.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


// ======== PUBLIC MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdInstalledTheme::CNcdInstalledTheme( 
    CNcdNodeInstallProxy& aParent,
    HBufC* aTheme  )
        : CNcdInstalledContent( aParent ),
    iTheme( aTheme )
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CNcdInstalledTheme::ConstructL()
    {
    DLTRACEIN(("this: %x", this));
    CNcdInstalledContent::ConstructL();
    
    // Theme name cannot be NULL but it can be empty
    User::LeaveIfNull( iTheme );
    
    // Register the interface
    MNcdInstalledTheme* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this,
            MNcdInstalledTheme::KInterfaceUid ) );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdInstalledTheme* CNcdInstalledTheme::NewL(
    CNcdNodeInstallProxy& aParent,
    HBufC* aTheme )
    {
    CNcdInstalledTheme* self = 
        CNcdInstalledTheme::NewLC( aParent, aTheme );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdInstalledTheme* CNcdInstalledTheme::NewLC(
    CNcdNodeInstallProxy& aParent,
    HBufC* aTheme )
    {
    CNcdInstalledTheme* self = 
        new( ELeave ) CNcdInstalledTheme( aParent, aTheme );
    // Using PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdInstalledTheme::~CNcdInstalledTheme()
    {
    DLTRACEIN(("this: %x", this));

    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdInstalledTheme::KInterfaceUid );
    
    
    // Delete member variables here
    delete iTheme;
    }



// ---------------------------------------------------------------------------
// Is installed
// ---------------------------------------------------------------------------
//
TBool CNcdInstalledTheme::IsInstalledL() const
    {
    return ContentOwner().InstallationService().IsThemeInstalledL( *iTheme );
    }
    

// ---------------------------------------------------------------------------
// Theme name getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdInstalledTheme::Theme() const
    {
    return *iTheme;
    }


