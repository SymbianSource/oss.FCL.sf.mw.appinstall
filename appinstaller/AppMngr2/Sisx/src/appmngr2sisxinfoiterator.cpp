/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CAppMngr2SisxInfoIterator
*
*/


#include "appmngr2sisxinfoiterator.h"   // CAppMngr2SisxInfoIterator
#include "appmngr2sisxappinfo.h"        // CAppMngr2SisxAppInfo
#include <StringLoader.h>               // StringLoader
#include <SWInstCommonUI.rsg>           // Resource IDs
#include <AknUtils.h>                   // AknTextUtils

_LIT( KSymbian, "Symbian OS\x2122" );
_LIT( KLRE, "\x202A" );
_LIT( KPDF, "\x202C" );
_LIT( KLeftParenthes, "(" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxInfoIterator* CAppMngr2SisxInfoIterator::NewL(
        CAppMngr2InfoBase& aSisx, TAppMngr2InfoType aInfoType )
    {
    CAppMngr2SisxInfoIterator* self =
            new ( ELeave ) CAppMngr2SisxInfoIterator( aSisx, aInfoType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::~CAppMngr2SisxInfoIterator()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxInfoIterator::~CAppMngr2SisxInfoIterator()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxInfoIterator::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::SetAllFieldsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxInfoIterator::SetAllFieldsL()
    {
    SetFieldL( R_SWCOMMON_DETAIL_NAME, iInfo.Name() );
    
    if( iInfoType == EAppMngr2StatusInstalled )
        {
        SetVersionL();
        SetSupplierL();
        }
    
    SetStatusL();
    SetLocationL();
    SetFieldL( R_SWCOMMON_DETAIL_APPSIZE, iInfo.Details() );

    // Technology
    HBufC* tmpBuf = HBufC::NewLC( KLRE().Length() + KSymbian().Length() + KPDF().Length() );
    tmpBuf->Des() = KLRE();
    tmpBuf->Des() += KSymbian();
    tmpBuf->Des() += KPDF();
    SetFieldL( R_SWCOMMON_DETAIL_TECHNOLOGY, *tmpBuf );
    CleanupStack::PopAndDestroy( tmpBuf );

    SetTypeL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::SetVersionL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxInfoIterator::SetVersionL()
    {
    CAppMngr2SisxAppInfo* sisxInfo = reinterpret_cast<CAppMngr2SisxAppInfo*>( &iInfo );
    HBufC* version = sisxInfo->Version().Name().AllocLC();
    
    TPtr versionPtr = version->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( versionPtr );
    HBufC* tmpBuf = HBufC::NewLC( KLRE().Length() + version->Length() + KPDF().Length() );
    TInt position = version->Find( KLeftParenthes() );
    if( position >= 0 )
        {
        TPtr bufferPtr = tmpBuf->Des();
        bufferPtr.Copy( version->Mid( 0, position ) );
        bufferPtr.Append( KLRE() );
        bufferPtr.Append( version->Mid( position, version->Length() - position ) );
        bufferPtr.Append( KPDF() );
        }
    else
        {
        tmpBuf->Des() = *version;
        }    
    SetFieldL( R_SWCOMMON_DETAIL_VERSION, *tmpBuf );
    
    CleanupStack::PopAndDestroy( 2, version );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::SetSupplierL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxInfoIterator::SetSupplierL()
    {
    CAppMngr2SisxAppInfo* sisxInfo = reinterpret_cast<CAppMngr2SisxAppInfo*>( &iInfo );
    if( sisxInfo->IsTrusted() )
        {
        SetFieldL( R_SWCOMMON_DETAIL_SUPPLIER, sisxInfo->Vendor() );
        }
    else
        {
        HBufC* unknown = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_UNKNOWN_SUPPLIER );
        SetFieldL( R_SWCOMMON_DETAIL_SUPPLIER, *unknown );
        CleanupStack::PopAndDestroy( unknown );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::SetTypeL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxInfoIterator::SetTypeL()
    {
    HBufC* type = NULL;
    type = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_APPLICATION );
    SetFieldL( R_SWCOMMON_DETAIL_TYPE , *type);
    CleanupStack::PopAndDestroy( type );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxInfoIterator::CAppMngr2SisxInfoIterator()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxInfoIterator::CAppMngr2SisxInfoIterator(
        CAppMngr2InfoBase& aSisx, TAppMngr2InfoType aInfoType ) :
            CAppMngr2InfoIterator( aSisx, aInfoType )
    {
    }

