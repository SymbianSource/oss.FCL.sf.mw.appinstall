/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CAppMngr2SisxUnknownList class
*
*/


#include "appmngr2sisxunknownlist.h"    // CAppMngr2SisxUnknownList
#include <appmngr2debugutils.h>         // FLOG macros

const TInt KUIDLength = 8;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxUnknownList* CAppMngr2SisxUnknownList::NewL( TInt aDriveNumber )
    {
    CAppMngr2SisxUnknownList* self = NewLC( aDriveNumber );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxUnknownList* CAppMngr2SisxUnknownList::NewLC( TInt aDriveNumber )
    {
    CAppMngr2SisxUnknownList* self = new (ELeave) CAppMngr2SisxUnknownList;
    CleanupStack::PushL( self );
    self->ConstructL( aDriveNumber );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::~CAppMngr2SisxUnknownList()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxUnknownList::~CAppMngr2SisxUnknownList()
    {
    iUnknownPackages.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::PkgCount()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2SisxUnknownList::PkgCount() const
    {
    return iUnknownPackages.Count();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::PkgEntry()
// ---------------------------------------------------------------------------
//
Swi::CUninstalledPackageEntry& CAppMngr2SisxUnknownList::PkgEntry( TInt aIndex )
    {
    return *iUnknownPackages[ aIndex ];     // panics if aIndex out of range
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::FindPkgWithUID()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2SisxUnknownList::FindPkgWithUID( const TUid& aUid )
    {
    FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUID( 0x%08x )", aUid.iUid );

    TInt count = iUnknownPackages.Count();
    FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName, count = %d", count );
    for( TInt index = 0; index < count; index++ )
        {
        if( iUnknownPackages[ index ]->Uid() == aUid )
            {
            return index;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::FindPkgWithUIDName()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2SisxUnknownList::FindPkgWithUIDName( const TDesC& aUIDName )
    {
    FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName( %S )", &aUIDName );
    
    TInt count = iUnknownPackages.Count();
    FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName, count = %d", count );
    if( count && aUIDName.Length() >= KUIDLength )
        {
        // Take first 8 characters from the given UID name -> 12345678
        TPtrC name = aUIDName.Mid( 0, KUIDLength );
        FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName: name = %S", &name );

        // Find sis package that has matching UID
        for( TInt index = 0; index < count; index++ )
            {
            TUid packageUID = iUnknownPackages[ index ]->Uid();

            // UID name is returned as "[12345678]"
            TBuf<KMaxUidName> packageUIDName = packageUID.Name();
            FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName: %d 0x%08x %S",
                    index, packageUID.iUid, &packageUIDName );

            // Remove square brackets from UID name -> 12345678
            TPtrC uidName = packageUIDName.Mid( 1, KUIDLength );

            // If UID name is same we have a match
            if( name.Compare( uidName ) == KErrNone )
                {
                FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName: match, return %d",
                        index );
                return index;
                }
            }
        }
    FLOG( "CAppMngr2SisxUnknownList::FindPkgWithUIDName: return KErrNotFound" );
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::RemovePkgL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxUnknownList::RemovePkgL( TInt aPkgListIndex )
    {
    Swi::UninstalledSisPackages::RemoveL( *iUnknownPackages[ aPkgListIndex ] );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxUnknownList::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxUnknownList::ConstructL( TInt aDriveNumber )
    {
    Swi::UninstalledSisPackages::ListL( static_cast< TDriveNumber >( aDriveNumber ),
            iUnknownPackages );
    }

