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
* Description:    
*
*/



// INCLUDE FILES

#include "iaupdatefwdetailsdialog.h"
#include "iaupdatedialogutil.h"
#include "iaupdatebasenode.h"
#include "iaupdatefwnode.h"
#include "iaupdatedebug.h"
#include <iaupdate.rsg>

#include <aknmessagequerydialog.h>      // CAknMessageQueryDialog

#include <StringLoader.h>

// CONSTANTS
const TInt KKiloByte = 1024;
const TInt KMegaByte = 1024 * 1024;
const TInt KMaxShownInKiloBytes = 10 * KMegaByte;


/*******************************************************************************
 * class CIAUpdateDetailsDialog
 *******************************************************************************/


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::ShowDialogL
// 
// -----------------------------------------------------------------------------
TBool CIAUpdateFwDetailsDialog::ShowDialogL( MIAUpdateFwNode* aFwNode )
	{
	CIAUpdateFwDetailsDialog* dialog = CIAUpdateFwDetailsDialog::NewL( aFwNode );
	CleanupStack::PushL( dialog );

	dialog->ShowDialogL();

	CleanupStack::PopAndDestroy( dialog );
    
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::NewL
//
// -----------------------------------------------------------------------------
CIAUpdateFwDetailsDialog* CIAUpdateFwDetailsDialog::NewL( MIAUpdateFwNode* aFwNode )
    {
    CIAUpdateFwDetailsDialog* self = new ( ELeave ) CIAUpdateFwDetailsDialog( aFwNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::CIAUpdateFwDetailsDialog
// 
// -----------------------------------------------------------------------------
//
CIAUpdateFwDetailsDialog::CIAUpdateFwDetailsDialog( MIAUpdateFwNode* aFwNode )
	{
	iFwNode = aFwNode;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwDetailsDialog::ConstructL()
    {
    TInt size = BufferSize();
 	iBuf = HBufC::NewL( size );
    } 



// ----------------------------------------------------------------------------
// Destructor
//
// ----------------------------------------------------------------------------
//
CIAUpdateFwDetailsDialog::~CIAUpdateFwDetailsDialog()
    {
    delete iBuf;
    }


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::BufferSize
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateFwDetailsDialog::BufferSize()
    {
    const TInt KExtraSize = 512;
    
    TPtrC ptr1 = iFwNode->Base().Name();
    TPtrC ptr2 = iFwNode->Base().Description();
    
    TInt len = ptr1.Length() + ptr2.Length() + KExtraSize;
    
    return len;
    } 


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::ConstructTextL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateFwDetailsDialog::ConstructTextL()
    {
    _LIT(KNewLine, "\n");

    TPtr ptr = iBuf->Des();
 
    TInt contentSize =   iFwNode->Base().ContentSizeL();
     
    
    HBufC* hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_DESCRIPTION );
    ptr.Append( KOpeningBoldTag );
    ptr.Append( *hBuf );
    ptr.Append( KClosingBoldTag );
    CleanupStack::PopAndDestroy( hBuf );
    
    
    HBufC* description = StringLoader::LoadLC( R_IAUPDATE_FW_DESCRIPTION );
    ptr.Append( KNewLine );
    ptr.Append( *description );
    ptr.Append( KNewLine );
    ptr.Append( KNewLine );
    CleanupStack::PopAndDestroy( description );
    
    if ( iFwNode->FwVersion1().Length() > 0 )
        { 
        hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_VERSION );        
        ptr.Append( KOpeningBoldTag );
        ptr.Append( *hBuf );
        ptr.Append( KClosingBoldTag );
        CleanupStack::PopAndDestroy( hBuf );

        ptr.Append( KNewLine );
        ptr.Append( iFwNode->FwVersion1() );
        ptr.Append( KNewLine );
        ptr.Append( KNewLine );
        }
    
    
    if( contentSize > 0 && (iFwNode->FwType() == MIAUpdateFwNode::EFotaDp2) )
        {
        hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_FILE_SIZE );
        ptr.Append( KOpeningBoldTag );
        ptr.Append( *hBuf );
        ptr.Append( KClosingBoldTag );
        CleanupStack::PopAndDestroy( hBuf );
        
        ptr.Append( KNewLine );
        
        hBuf = FileSizeTextLC( contentSize );

        ptr.Append( *hBuf );
        CleanupStack::PopAndDestroy( hBuf );
        }
   
    } 


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::ShowDialogL
// 
// -----------------------------------------------------------------------------
TBool CIAUpdateFwDetailsDialog::ShowDialogL()
	{
	ConstructTextL();
	HBufC* firmwarename = iFwNode->Base().Name().AllocLC();
	IAUpdateDialogUtil::ShowMessageQueryL( *firmwarename, *iBuf );
    CleanupStack::PopAndDestroy( firmwarename );
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdateFwDetailsDialog::FileSizeTextLC
// 
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateFwDetailsDialog::FileSizeTextLC( TInt aFileSize )
	{
    TInt resourceId = 0;
	TInt size = 0;
	
	if ( aFileSize >= KMaxShownInKiloBytes )
	    {
	    resourceId = R_IAUPDATE_DETAILS_DIALOG_SIZE_MEGABYTE;
	    size = aFileSize / KMegaByte;
   	    if ( aFileSize % KMegaByte != 0 )
	        {
	        size++;
	        }
	    }
	else
	    {
	    resourceId = R_IAUPDATE_DETAILS_DIALOG_SIZE_KILOBYTE;
	    size = aFileSize / KKiloByte;
   	    if ( aFileSize % KKiloByte != 0 )
	        {
	        size++;
	        }	
	    }
	    
	HBufC* sizeAsString = StringLoader::LoadLC( resourceId, size );
	TPtr ptr = sizeAsString->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
    return sizeAsString;
	}


   
//  End of File  
