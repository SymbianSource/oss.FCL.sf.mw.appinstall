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
* Description:   Contains CNcdInstalledApplication class implementation
*
*/


#include "ncdnodeinstallproxy.h"
#include "ncdinstalledapplicationimpl.h"
#include "ncdinstallationservice.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "catalogsdebug.h"


// ======== PUBLIC MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdInstalledApplication::CNcdInstalledApplication( 
    CNcdNodeInstallProxy& aParent,
    const TUid& aUid,
    MNcdInstalledContent::TInstalledContentType aType,
    TBool aUriExists )
        : CNcdInstalledContent( aParent, aType ),
        iUid( aUid ),
        iUriExists( aUriExists )
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CNcdInstalledApplication::ConstructL( 
    const TDesC& aVersion,
    const TDesC& aDocumentName )
    {
    DLTRACEIN(("this: %x, iUid: %x", this, iUid.iUid ));
    CNcdInstalledContent::ConstructL();
    
    // Register the interface
    MNcdInstalledApplication* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this,
            MNcdInstalledApplication::KInterfaceUid ) );
    
    TRAPD( err, TCatalogsVersion::ConvertL( iVersion, aVersion ) );        
    LeaveIfNotErrorL( err, KErrArgument, KErrGeneral );
    iDocumentName = aDocumentName.AllocL();
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdInstalledApplication* CNcdInstalledApplication::NewLC(
    CNcdNodeInstallProxy& aParent,
    const TUid& aUid,
    const TDesC& aVersion,
    const TDesC& aDocumentName,
    MNcdInstalledContent::TInstalledContentType aType,
    TBool aUriExists )
    {
    CNcdInstalledApplication* self = 
        new( ELeave ) CNcdInstalledApplication( 
            aParent, aUid, aType, aUriExists );
    // Using normal PushL because the object does not have any references yet
    CleanupStack::PushL( self );
    self->ConstructL( aVersion, aDocumentName );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdInstalledApplication::~CNcdInstalledApplication()
    {
    DLTRACEIN(("this: %x", this));

    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdInstalledApplication::KInterfaceUid );
    
    
    // Delete member variables here
    // Do not delete node and operation manager because
    // this object does not own them.
    delete iDocumentName;
    }


// ---------------------------------------------------------------------------
// Is installed
// ---------------------------------------------------------------------------
//
TBool CNcdInstalledApplication::IsInstalledL() const
    {
    DLTRACEIN((""));
    if ( !iUriExists ) 
        {
        DLTRACEOUT(("No URI, considered as installed"));
        return ETrue;
        }
        
    return ContentOwner().InstallationService().IsApplicationInstalledL( 
        iUid, iVersion ) >= ENcdApplicationInstalled;
    }
    

// ---------------------------------------------------------------------------
// UID getter
// ---------------------------------------------------------------------------
//
TUid CNcdInstalledApplication::Uid() const
    {
    DLTRACEIN(("UID: %x", iUid.iUid));
    return iUid;
    }


// ---------------------------------------------------------------------------
// Document name getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdInstalledApplication::DocumentName() const
    {
    DASSERT( iDocumentName );
    DLTRACEIN(( _L("document: %S"), iDocumentName ));
    return *iDocumentName;
    }
