/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CSisxUIAppInfo 
*                class member functions.
*
*/


// INCLUDE FILES

#include <StringLoader.h>
#include <SWInstCommonUI.rsg>
#include <AknUtils.h>

#include "SisxUIAppInfo.h"
#include "CUIUtils.h"

_LIT( KSymbian, "Symbian OS\x2122" );
_LIT( KLRE, "\x202A" );
_LIT( KPDF, "\x202C" );
_LIT( KLeftParenthes, "(" );

using namespace SwiUI;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::CSisxUIAppInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSisxUIAppInfo::CSisxUIAppInfo( TBool aIsTrusted )
    : iIsTrusted( aIsTrusted )
    {
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::ConstructL( const Swi::CAppInfo& aInfo )
    {
    iName = aInfo.AppName().AllocL();
    iVendor = aInfo.AppVendor().AllocL();    
    iVersion = aInfo.AppVersion();      
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSisxUIAppInfo* CSisxUIAppInfo::NewL( const Swi::CAppInfo& aInfo, TBool aIsTrusted )
    {
    CSisxUIAppInfo* self = new ( ELeave ) CSisxUIAppInfo( aIsTrusted );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    CleanupStack::Pop( self );
    return self; 
    }
    
// Destructor
CSisxUIAppInfo::~CSisxUIAppInfo()
    {  
    FreeIterator();

    delete iName;
    delete iVendor;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::Name
// Return name of the package.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSisxUIAppInfo::Name() const
    {
    return *iName;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::SetNameL
// Set the name of the package
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::Vendor
// Return the package vendor. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CSisxUIAppInfo::Vendor() const
    {
    return *iVendor;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::SetVendorL
// Set the vendor of the package
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::SetVendorL( const TDesC& aVendor )
    {
    delete iVendor;
    iVendor = NULL;
    iVendor = aVendor.AllocL();    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::Version
// Return the package version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TVersion& CSisxUIAppInfo::Version() const
    {
    return iVersion;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::SetTrusted
// Set the trusted status of the package. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::SetTrusted( TBool aIsTrusted )
    {
    iIsTrusted = aIsTrusted;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::IsTrusted
// Indicates the trusted status of the package.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIAppInfo::IsTrusted() const
    {
    return iIsTrusted;
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::SetUid
// Sets the Uid of the package.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::SetUid( const TUid& aUid )
    {
    iUid.iUid = aUid.iUid;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::Uid
// Return the Uid of the package.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CSisxUIAppInfo::Uid() const
    {
    return iUid;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::PrepareIteratorL
// Prepares the iterator to be shown in details dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::PrepareIteratorL( const Swi::CAppInfo& /*aInfo*/ )
    {
    FreeIterator();

    iKeys = new( ELeave )CDesCArrayFlat( 6 );
    iValues = new( ELeave )CDesCArrayFlat( 6 );

    // Name
    SetFieldL( R_SWCOMMON_DETAIL_NAME, Name() );

    // Version
    HBufC* stringBuf = CommonUI::CUIUtils::ConstructVersionStringLC( Version().iMajor,
                                                                     Version().iMinor,
                                                                     Version().iBuild );    
    TPtr ptr = stringBuf->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
    HBufC* tmpBuf = HBufC::NewLC( KLRE().Length() + stringBuf->Length() + KPDF().Length() ); 
    TInt position = stringBuf->Find( KLeftParenthes() );
    if ( position >= 0 )
        {
        tmpBuf->Des() = stringBuf->Mid( 0, position );
        tmpBuf->Des() += KLRE();
        tmpBuf->Des() += stringBuf->Mid( position, stringBuf->Length() - position );
        tmpBuf->Des() += KPDF();
        }
    else
        {
        tmpBuf->Des() = *stringBuf;	
        }   
    SetFieldL( R_SWCOMMON_DETAIL_VERSION, *tmpBuf );
    CleanupStack::PopAndDestroy( tmpBuf );
    CleanupStack::PopAndDestroy( stringBuf );

     // Supplier
    if ( IsTrusted() )
        {        
        SetFieldL( R_SWCOMMON_DETAIL_SUPPLIER, Vendor() );
        }
    else
        {
        tmpBuf = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_UNKNOWN_SUPPLIER );
        SetFieldL( R_SWCOMMON_DETAIL_SUPPLIER, *tmpBuf );
        CleanupStack::PopAndDestroy( tmpBuf );        
        }  

    // size
    tmpBuf = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_UNKNOWN_SIZE );
    SetFieldL( R_SWCOMMON_DETAIL_APPSIZE, *tmpBuf );  
    CleanupStack::PopAndDestroy( tmpBuf );

    // Technology
    tmpBuf = HBufC::NewLC( KLRE().Length() + KSymbian().Length() + KPDF().Length() );
    tmpBuf->Des() = KLRE(); 
    tmpBuf->Des() += KSymbian();
    tmpBuf->Des() += KPDF();
    SetFieldL( R_SWCOMMON_DETAIL_TECHNOLOGY, *tmpBuf ); 
    CleanupStack::PopAndDestroy( tmpBuf );  

    // Type
    tmpBuf = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_APPLICATION );    
    SetFieldL( R_SWCOMMON_DETAIL_TYPE, *tmpBuf );
    CleanupStack::PopAndDestroy( tmpBuf );    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::FreeIterator
// Frees the memory reserved for the iterator.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::FreeIterator()     
    {
    if ( iKeys )
        {        
        iKeys->Reset();
        }
    delete iKeys;
    iKeys = NULL;    

    if ( iValues )
        {        
        iValues->Reset();    
        }    
    delete iValues;
    iValues = NULL;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::HasNext
// Indicates if the iterator has any more fields left. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSisxUIAppInfo::HasNext() const
    {
    return ( ( iKeys->Count() ) > iCurrentIndex );    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::Next
// Get the next field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::Next( TPtrC& aKey, TPtrC& aValue )
    {
    aKey.Set( ( *iKeys )[iCurrentIndex] );
    aValue.Set( ( *iValues )[iCurrentIndex] );
    iCurrentIndex++;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::Reset
// Reset the iterator. After calling this, Next() will return the first value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::Reset()
    {
    iCurrentIndex = 0;    
    }

// -----------------------------------------------------------------------------
// CSisxUIAppInfo::SetFieldL
// Sets a new field for the iterator with given parameter values.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSisxUIAppInfo::SetFieldL( TInt aResourceId, const TDesC& aValue )
    {
     HBufC* temp = StringLoader::LoadLC( aResourceId );
     iKeys->AppendL( temp->Des() );
     CleanupStack::PopAndDestroy( temp );
     iValues->AppendL( aValue );
    }


//  End of File  
