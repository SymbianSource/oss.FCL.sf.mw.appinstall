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
* Description:   This file contains the implementation of 
*                CCUICertificateDetailsDialog class member functions.
*
*/


// INCLUDE FILES
//#include <aknmessagequerydialog.h>
//#include <StringLoader.h>
#include <SWInstCommonUI.rsg>
//#include <AknUtils.h>
#include <hash.h>                  

#include "CUICertificateDetailsDialog.h"
#include "CUIDetailsDialog.h"

using namespace SwiUI::CommonUI;

// CONSTANTS
_LIT( KNextLine, "\n" );
_LIT( KNextNextLine, "\n\n" );
_LIT( KHexFormat, "%02X" );
const TInt KMaxLengthTextSerialNumberFormatting = 3;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::CCUICertificateDetailsDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCUICertificateDetailsDialog::CCUICertificateDetailsDialog()
    {
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCUICertificateDetailsDialog::ConstructL()
    {
   
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCUICertificateDetailsDialog* CCUICertificateDetailsDialog::NewL()

    {
    CCUICertificateDetailsDialog* self = new ( ELeave ) CCUICertificateDetailsDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// Destructor
CCUICertificateDetailsDialog::~CCUICertificateDetailsDialog()
    {
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::AddFieldLC
// Adds a field (header and value) to the dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUICertificateDetailsDialog::AddFieldLC( HBufC*& aMessage,
                                               TInt aHeaderResourceId,
                                               const TDesC& aValue )
    {
    /*
    HBufC* headerString = StringLoader::LoadLC( aHeaderResourceId );
    HBufC* newMessage;
    
    if ( aMessage->Length() > 0 )
        {
        newMessage = HBufC::NewL( headerString->Length() +
	                          aValue.Length() +
                                  aMessage->Length() + 
                                  3 ); // \n\n, \n
        
        TPtr ptr( newMessage->Des() );
        ptr += *aMessage;        
        ptr += KNextNextLine;        
        ptr += *headerString;
        ptr += KNextLine;
        ptr += aValue;
        }
    else
        {
        newMessage = HBufC::NewL( headerString->Length() + aValue.Length() + 1 ); // \n

        TPtr ptr( newMessage->Des() );                
        ptr += *headerString;
        ptr += KNextLine;
        ptr += aValue;
        }

    CleanupStack::PopAndDestroy( headerString );
    CleanupStack::PopAndDestroy( aMessage );  
  
    aMessage = newMessage;    
    CleanupStack::PushL( aMessage );
    */  
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::ExecuteLD
// Executes the dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUICertificateDetailsDialog::ExecuteLD( const CCUICertificateInfo& aCertInfo )
    {
    /*
    CleanupStack::PushL( this );
    
    // Valid from
    HBufC* validFrom = DateToStringLC( aCertInfo.ValidFromL() );

    // Valid until
    HBufC* validTo = DateToStringLC( aCertInfo.ValidToL() );
    
    // Serial
    TPtrC8 serialPtr( aCertInfo.SerialNumberL() );
    HBufC* serial = HBufC::NewLC( serialPtr.Length() * 2 );
    TBuf<KMaxLengthTextSerialNumberFormatting> buf;
    
    for ( TInt index = 0; index < serialPtr.Length(); index++ )
        {
        buf.Format( KHexFormat, serialPtr[index] ); 
        serial->Des().Append( buf );
        }

    // Fingerprint   
    HBufC* fingerprint = HBufC::NewLC( aCertInfo.FingerprintL().Length() * 5 );
    DevideToBlocks( aCertInfo.FingerprintL(), fingerprint->Des() );
    
    // Fingerprint (MD5)
    HBufC* fingerprint_md = HBufC::NewLC( aCertInfo.FingerprintL().Length() * 5 );
    if ( aCertInfo.EncodingL() != KNullDesC8 )
        {
    	CMD5* md5 = CMD5::NewL();
        CleanupStack::PushL( md5 );
        DevideToBlocks( md5->Hash( aCertInfo.EncodingL() ), fingerprint_md->Des() );
        CleanupStack::PopAndDestroy( md5 );
        }
    
    HBufC* message = HBufC::NewLC( 0 );

    // Costruct the fields
    AddFieldLC( message, R_SWCOMMON_SEC_ISSUER, aCertInfo.IssuerNameL() ); 
    AddFieldLC( message, R_SWCOMMON_SEC_SUBJECT, aCertInfo.SubjectNameL() );
    AddFieldLC( message, R_SWCOMMON_SEC_VALID_FROM, *validFrom );
    AddFieldLC( message, R_SWCOMMON_SEC_VALID_UNTIL, *validTo );
    AddFieldLC( message, R_SWCOMMON_SEC_SERIAL, *serial );
    AddFieldLC( message, R_SWCOMMON_SEC_FINGERPRINT, *fingerprint );
    if ( fingerprint_md->Length() > 0 )
        {
    	AddFieldLC( message, R_SWCOMMON_SEC_FINGERPRINT_MD, *fingerprint_md );
        }
    

    // Create and show the message query dialog
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *message );
    dlg->PrepareLC( R_SWCOMMON_SEC_DETAILS_DIALOG );
    dlg->RunLD();

    CleanupStack::PopAndDestroy( 7 );  // message, fingerprint_md, fingerprint, serial, validTo, validFrom
    */
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::DateToString
// Converts date to local format.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCUICertificateDetailsDialog::DateToStringLC( const TDateTime& aDate )
    {
    /*
    // Create date descriptors.
    // We use Avkon date format string to format the date into correct format.
    HBufC* dateFormatString = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
    
    TBuf<30> timeString;    
    TTime time( aDate );
        
    time.FormatL( timeString, *dateFormatString );
    CleanupStack::PopAndDestroy( dateFormatString );   
    HBufC* tmp = timeString.AllocLC();

    TPtr ptr = tmp->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 

    return tmp;   
    */
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::DevideToBlocks
// Devides input descriptor to blocks.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUICertificateDetailsDialog::DevideToBlocks( const TDesC8& aInput, TPtr aOutput )
    {
    const TInt KBlockLength = 2;    
    TInt blockIndex = 0;
    _LIT( KBlockSeparator, " " );

    for ( TInt index = 0 ; index < aInput.Length() ; index++ )
        {
        if ( blockIndex == KBlockLength )
            {
            aOutput.Append( KBlockSeparator );
            blockIndex = 0;
            }
        aOutput.AppendNumFixedWidthUC( (TUint)(aInput[index]), EHex, 2 );
        ++blockIndex;
        }
    }
//  End of File
