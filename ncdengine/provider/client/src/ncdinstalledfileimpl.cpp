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
* Description:   Contains CNcdInstalledFile class implementation
*
*/


#include "ncdinstalledfileimpl.h"

#include <bautils.h>

#include "ncdnodeinstallproxy.h"
#include "ncdinstallationservice.h"
#include "catalogsinterfaceidentifier.h"
#include "catalogsutils.h"
#include "ncdfileinfo.h"

#include "catalogsdebug.h"

// ======== PUBLIC MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CNcdInstalledFile::CNcdInstalledFile( 
    CNcdNodeInstallProxy& aParent,
    CNcdFileInfo* aInfo,
    TInt aFileIndex )
        : CNcdInstalledContent( aParent ),
    iInfo( aInfo ),
    iFileIndex( aFileIndex )    
    {
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CNcdInstalledFile::ConstructL()
    {
    DLTRACEIN(("this: %X", this));
    CNcdInstalledContent::ConstructL();
    
    // Info can't be NULL
    User::LeaveIfNull( iInfo );
    
    // Register the interface
    MNcdInstalledFile* interface( this );
    AddInterfaceL( 
        CCatalogsInterfaceIdentifier::NewL( interface, this,
            MNcdInstalledFile::KInterfaceUid ) );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CNcdInstalledFile* CNcdInstalledFile::NewL(
    CNcdNodeInstallProxy& aParent,
    CNcdFileInfo* aInfo,
    TInt aFileIndex )
    {
    CNcdInstalledFile* self = 
        CNcdInstalledFile::NewLC( aParent, aInfo, aFileIndex );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CNcdInstalledFile* CNcdInstalledFile::NewLC(
    CNcdNodeInstallProxy& aParent,
    CNcdFileInfo* aInfo,
    TInt aFileIndex )
    {
    CNcdInstalledFile* self = 
        new( ELeave ) CNcdInstalledFile( aParent, aInfo, aFileIndex );
    // Using PushL because the object does not have references yet
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CNcdInstalledFile::~CNcdInstalledFile()
    {
    DLTRACEIN(("this: %X", this));

    // Remove interfaces implemented by this class from the interface list.
    // So, the interface list is up to date when this class object is deleted.
    RemoveInterface( MNcdInstalledFile::KInterfaceUid );
    
    
    // Delete member variables here
    delete iInfo;
    }



// ---------------------------------------------------------------------------
// Is installed
// ---------------------------------------------------------------------------
//
TBool CNcdInstalledFile::IsInstalledL() const
    {    
    return iInfo->FilePath().Length() &&
        BaflUtils::FileExists( 
            ContentOwner().FileSession(), 
            iInfo->FilePath() );
    }
    

// ---------------------------------------------------------------------------
// Mime type getter
// ---------------------------------------------------------------------------
//
const TDesC& CNcdInstalledFile::MimeType() const
    {
    return iInfo->MimeType();
    }


// ---------------------------------------------------------------------------
// File opener
// ---------------------------------------------------------------------------
//
RFile CNcdInstalledFile::OpenFileL()
    {
    return ContentOwner().OpenFileL( iFileIndex );
    }


