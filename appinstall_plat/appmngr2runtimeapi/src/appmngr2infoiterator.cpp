/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements MCUIInfoIterator API for CCUIDetailsDialog
*
*/


#include "appmngr2infoiterator.h"       // CAppMngr2InfoIterator
#include "appmngr2infobase.h"           // CAppMngr2InfoBase
#include <badesca.h>                    // CDesC8Array
#include <StringLoader.h>               // StringLoader
#include <SWInstCommonUI.rsg>           // Resource IDs

const TInt KGranularity = 8;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::CAppMngr2InfoIterator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2InfoIterator::CAppMngr2InfoIterator( CAppMngr2InfoBase& aInfo,
        TAppMngr2InfoType aInfoType ) : iInfo( aInfo ), iInfoType( aInfoType )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::BaseConstructL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::BaseConstructL()
    {
    iKeys = new ( ELeave ) CDesCArrayFlat( KGranularity );
    iValues = new ( ELeave ) CDesCArrayFlat( KGranularity );
    SetAllFieldsL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::~CAppMngr2InfoIterator()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2InfoIterator::~CAppMngr2InfoIterator()
    {
    if( iKeys )
        {
        iKeys->Reset();
        delete iKeys;
        }
    if( iValues )
        {
        iValues->Reset();
        delete iValues;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::HasNext()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAppMngr2InfoIterator::HasNext() const
    {
    TInt keysCount = iKeys->Count();
    return ( keysCount > 0 && keysCount > iCurrentIndex );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::Next()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::Next( TPtrC& aKey, TPtrC& aValue )
    {
    aKey.Set( ( *iKeys )[ iCurrentIndex ] );
    aValue.Set( ( *iValues )[ iCurrentIndex ] );
    iCurrentIndex++;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::Reset()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::Reset()
    {
    iCurrentIndex = 0;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::SetFieldL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::SetFieldL( TInt aResourceId, const TDesC& aValue )
    {
    HBufC* fieldName = StringLoader::LoadLC( aResourceId );
    iKeys->AppendL( fieldName->Des() );
    CleanupStack::PopAndDestroy( fieldName );
    iValues->AppendL( aValue );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::SetAllFieldsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::SetAllFieldsL()
    {
    SetFieldL( R_SWCOMMON_DETAIL_NAME, iInfo.Name() );
    SetStatusL();
    SetLocationL();
    SetFieldL( R_SWCOMMON_DETAIL_APPSIZE, iInfo.Details() );
    SetOtherFieldsL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::SetStatusL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::SetStatusL()
    {
    HBufC* status = NULL;
    if( iInfoType == EAppMngr2StatusInstalled )
        {
        status = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_INSTALLED );
        }
    else // iInfoType is EAppMngr2StatusNotInstalled
        {
        status = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_NOT_INSTALLED );
        }
    SetFieldL( R_SWCOMMON_DETAIL_STATUS, *status );
    CleanupStack::PopAndDestroy( status );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::SetLocationL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::SetLocationL()
    {
    TChar driveChar;
    RFs::DriveToChar( iInfo.LocationDrive(), driveChar );
    const TInt KSingleLetter = 1;
    TBuf<KSingleLetter> driveLetter;
    driveLetter.Append( driveChar );

    HBufC* memory = NULL;
    if( iInfo.Location() == EAppMngr2LocationMemoryCard )
        {
        memory = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_MMC, driveLetter );
        }
    else if( iInfo.Location() == EAppMngr2LocationMassStorage )
        {
        memory = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_MASS_STORAGE, driveLetter );
        }
    else
        {
        memory = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_DEVICE, driveLetter );
        }
    SetFieldL( R_SWCOMMON_DETAIL_LOCATION, *memory );
    CleanupStack::PopAndDestroy( memory );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoIterator::SetOtherFieldsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoIterator::SetOtherFieldsL()
    {
    // empty default implementation
    }

