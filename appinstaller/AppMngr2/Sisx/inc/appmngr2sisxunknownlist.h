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
* Description:   Utility class to list and remove unknown (not installed)
*                sisx packages from removable media.
*
*/


#ifndef C_APPMNGR2SISXUNKNOWNLIST_H
#define C_APPMNGR2SISXUNKNOWNLIST_H

#include <e32base.h>                    // CBase
#include <swi/pkgremover.h>             // Swi::CUninstalledPackageEntry

// Private folder path containing pre-installed applications
_LIT( KAppMngr2PreInstalledPath, ":\\private\\10202dce\\" );


/**
 * Class for listing and removing uninstalled PA and PP type sisx packages
 * from removable media. Unknown (not installed) sisx packages are read using
 * Swi::UninstalledSisPackages::ListL() funtion. Basically this is just a
 * container class for unknown package list array.
 */
class CAppMngr2SisxUnknownList : public CBase
    {
public:     // constructors and destructor
    static CAppMngr2SisxUnknownList* NewL( TInt aDriveNumber );
    static CAppMngr2SisxUnknownList* NewLC( TInt aDriveNumber );
    ~CAppMngr2SisxUnknownList();

public:     // new functions
    TInt PkgCount() const;
    Swi::CUninstalledPackageEntry& PkgEntry( TInt aIndex );
    TInt FindPkgWithUID( const TUid& aUid );
    TInt FindPkgWithUIDName( const TDesC& aUIDName );   // aUIDName can be e.g. "20001234.SISX"
    void RemovePkgL( TInt aIndex );

private:    // new functions
    void ConstructL( TInt aDriveNumber );

private:     // data
    RPointerArray<Swi::CUninstalledPackageEntry> iUnknownPackages;
    };

#endif      // C_APPMNGR2SISXUNKNOWNLIST_H

