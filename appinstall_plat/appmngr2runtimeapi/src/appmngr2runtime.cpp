/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class implementation for AppMngr2 plug-ins
*
*/


#include "appmngr2runtime.h"            // CAppMngr2Runtime
#include "appmngr2driveutils.h"         // TAppMngr2DriveUtils
#include "appmngr2debugutils.h"         // FLOG macros
#include <ecom/ecom.h>                  // REComSession
#include <eikenv.h>                     // CEikonEnv


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2Runtime* CAppMngr2Runtime::NewL( TUid aImplementationUid,
        MAppMngr2RuntimeObserver &aObserver )
    {
    FLOG( "CAppMngr2Runtime::NewL( 0x%08x )", aImplementationUid.iUid );
    TUid destructorIDKey;
    TAny* ptr = REComSession::CreateImplementationL( aImplementationUid,
            destructorIDKey, &aObserver );
    CAppMngr2Runtime* self = reinterpret_cast<CAppMngr2Runtime*>( ptr );
    if( self )
        {
        CleanupStack::PushL( self );
        self->iDtorIDKey = destructorIDKey;
        self->ConstructL( aImplementationUid );
        CleanupStack::Pop( self );
        }
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::~CAppMngr2Runtime()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2Runtime::~CAppMngr2Runtime()
    {
    FLOG( "CAppMngr2Runtime::~CAppMngr2Runtime( 0x%08x )", iRuntimeUid.iUid );
    REComSession::DestroyedImplementation( iDtorIDKey );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::RuntimeUid()
// ---------------------------------------------------------------------------
//
EXPORT_C const TUid CAppMngr2Runtime::RuntimeUid() const
    {
    return iRuntimeUid;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::AddNearestResourceFileL()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAppMngr2Runtime::AddNearestResourceFileL( const TDesC& aFileName )
    {
    TFileName* fullName = TAppMngr2DriveUtils::NearestResourceFileLC( aFileName,
            EikonEnv().FsSession() );
    TInt resourceId = EikonEnv().AddResourceFileL( *fullName );
    CleanupStack::PopAndDestroy( fullName );
    return resourceId;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::FullBitmapFileNameLC()
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CAppMngr2Runtime::FullBitmapFileNameLC( const TDesC& aBitmapFile )
    {
    HBufC* fullName = TAppMngr2DriveUtils::FullBitmapFileNameLC( aBitmapFile,
            EikonEnv().FsSession() );
    return fullName;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::EikonEnv()
// ---------------------------------------------------------------------------
//
EXPORT_C CEikonEnv& CAppMngr2Runtime::EikonEnv()
    {
    if( !iCachedEikonEnv )
        {
        iCachedEikonEnv = CEikonEnv::Static();
        }
    return *iCachedEikonEnv;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::Observer()
// ---------------------------------------------------------------------------
//
EXPORT_C MAppMngr2RuntimeObserver& CAppMngr2Runtime::Observer()
    {
    return iObserver;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::GetAdditionalDirsToScanL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2Runtime::GetAdditionalDirsToScanL( RFs& /*aFsSession*/,
        RPointerArray<HBufC>& /*aDirs*/ )
    {
    // no additional directories by default
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::CAppMngr2Runtime()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2Runtime::CAppMngr2Runtime( MAppMngr2RuntimeObserver &aObserver )
        : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2Runtime::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Runtime::ConstructL( TUid aImplementationUid )
    {
    iRuntimeUid = aImplementationUid;
    }

