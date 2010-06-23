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
* Description:   Base class implementation for items displayed in AppMngr2
*
*/


#include "appmngr2infobase.h"           // CAppMngr2InfoBase
#include "appmngr2runtime.h"            // CAppMngr2Runtime
#include "appmngr2common.hrh"           // Default icon indexes
//#include <appmngr2.rsg>                 // Resource IDs
#include <AknUtils.h>                   // DisplayTextLanguageSpecificNumberConversion
#include <StringLoader.h>               // StringLoader
#include <barsread.h>                   // TResourceReader
#include <e32math.h>                    // Math::Round

const TUint KKiloByte = KKilo;
const TUint KMegaByte = KKilo*KKiloByte;
const TUint KGigaByte = KKilo*KMegaByte;
const TInt KDecimalPlaces = 0;


// ======== LOCAL FUNCTIONS =========

// ---------------------------------------------------------------------------
// DivideAndReturnRoundedInt()
// ---------------------------------------------------------------------------
//
TInt DivideAndReturnRoundedInt( TInt64 aDivident, TUint aDivisor )
    {
    // Ignored possible math errors returned by Math::Round() and Math::Int().
    TReal rounded = 0;
    Math::Round( rounded, I64REAL( aDivident ) / aDivisor, KDecimalPlaces );
    if( rounded < 1 )
        {
        rounded = 1;
        }
    TInt32 final = 0;
    Math::Int( final, rounded );
    return final;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2InfoBase::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::~CAppMngr2InfoBase()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase::~CAppMngr2InfoBase()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::Runtime()
// ---------------------------------------------------------------------------
//
EXPORT_C CAppMngr2Runtime& CAppMngr2InfoBase::Runtime() const
    {
    return iRuntime;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::SpecificIconL()
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CAppMngr2InfoBase::SpecificIconL() const
    {
    User::Leave( KErrNotSupported );    // should not get here
    return NULL;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::IndicatorIconIndex()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAppMngr2InfoBase::IndicatorIconIndex() const
    {
    if( iLocation == EAppMngr2LocationMemoryCard )
        {
        return EAppMngr2IconQgnIndiAmInstMmcAdd;
        }
    if( iLocation == EAppMngr2LocationMassStorage )
        {
        return EAppMngr2IconQgnIndiFmgrMsAdd;
        }
    return EAppMngr2NoIndicatorIcon;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::SpecificIndicatorIconL()
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CAppMngr2InfoBase::SpecificIndicatorIconL() const
    {
    User::Leave( KErrNotSupported );    // should not get here
    return NULL;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::IsShowOnTop()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAppMngr2InfoBase::IsShowOnTop() const
    {
    return iShowOnTop;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::GetMenuItemsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoBase::GetMenuItemsL(
        RPointerArray<CEikMenuPaneItem::SData>& /*aMenuCmds*/ )
    {
    // no item specific menu items by default
    }


// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::SupportsGenericCmd()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAppMngr2InfoBase::SupportsGenericCommand( TInt /*aCmdId*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::GetMiddleSoftkeyCommandL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoBase::GetMiddleSoftkeyCommandL(
        TInt& /*aResourceId*/, TInt& /*aCommandId*/ )
    {
    // no item specific middle softkey command by default
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::SizeStringWithUnitsL()
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CAppMngr2InfoBase::SizeStringWithUnitsL( TInt64 aSizeInBytes )
    {
    HBufC* dispString = NULL;
    if( aSizeInBytes <= KMegaByte )
        {
        TInt kiloBytes = DivideAndReturnRoundedInt( aSizeInBytes, KKiloByte );
        // Temporary fix until appmngr2pluginapi is removed
        // dispString = StringLoader::LoadLC( R_SWINS_UNIT_KILOBYTE, kiloBytes );
        _LIT( KKiloByte, "%N kB" );
        dispString = KKiloByte().AllocLC();
        }
    else if( aSizeInBytes <= KGigaByte )
        {
        TInt megaBytes = DivideAndReturnRoundedInt( aSizeInBytes, KMegaByte );
        // Temporary fix until appmngr2pluginapi is removed
        // dispString = StringLoader::LoadLC( R_SWINS_UNIT_MEGABYTE, megaBytes );
        _LIT( KMegaByte, "%N MB" );
        dispString = KMegaByte().AllocLC();
        }
    else // aSizeInBytes > KGigaByte
        {
        TInt gigaBytes = DivideAndReturnRoundedInt( aSizeInBytes, KGigaByte );
        // Temporary fix until appmngr2pluginapi is removed
        // dispString = StringLoader::LoadLC( R_SWINS_UNIT_GIGABYTE, gigaBytes );
        _LIT( KGigaByte, "%N GB" );
        dispString = KGigaByte().AllocLC();
        }
    
    TPtr ptr = dispString->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
    
    CleanupStack::Pop( dispString );
    return dispString;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::ReadMenuItemDataFromResourceL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CAppMngr2InfoBase::ReadMenuItemDataFromResourceL(
        TInt aResourceId, CEikMenuPaneItem::SData& aMenuItemData )
    {
    TResourceReader reader;
    iRuntime.EikonEnv().CreateResourceReaderLC( reader, aResourceId );

    aMenuItemData.iCommandId = reader.ReadInt32();
    aMenuItemData.iCascadeId = reader.ReadInt32();
    aMenuItemData.iFlags = reader.ReadInt32();
    aMenuItemData.iText.Copy( reader.ReadTPtrC() );
    aMenuItemData.iExtraText = reader.ReadTPtrC();
    // bmpfile, bmpid, bmpmask, and extension ignored 

    CleanupStack::PopAndDestroy();  // resource reader
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::Location()
// ---------------------------------------------------------------------------
//
EXPORT_C TAppMngr2Location CAppMngr2InfoBase::Location() const
    {
    return iLocation;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::LocationDrive()
// ---------------------------------------------------------------------------
//
EXPORT_C TDriveUnit CAppMngr2InfoBase::LocationDrive() const
    {
    return iLocationDrive;
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoBase::CAppMngr2InfoBase()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoBase::CAppMngr2InfoBase( CAppMngr2Runtime& aRuntime,
        RFs& aFsSession ) : iFs( aFsSession ), iRuntime( aRuntime )
    {
    }

