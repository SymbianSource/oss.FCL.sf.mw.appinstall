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
* Description:    
*
*/



// INCLUDE FILES

#include "iaupdatedetailsdialog.h"
#include "iaupdatedialogutil.h"
#include "iaupdatebasenode.h"
#include "iaupdatenode.h"  
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
TBool CIAUpdateDetailsDialog::ShowDialogL( TIAUpdateDialogParam* aParam )
	{
	CIAUpdateDetailsDialog* dialog = CIAUpdateDetailsDialog::NewL( aParam );
	CleanupStack::PushL( dialog );

	dialog->ShowDialogL();

	CleanupStack::PopAndDestroy( dialog );
    
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::NewL
//
// -----------------------------------------------------------------------------
CIAUpdateDetailsDialog* CIAUpdateDetailsDialog::NewL( TIAUpdateDialogParam* aParam )
    {
    CIAUpdateDetailsDialog* self = new ( ELeave ) CIAUpdateDetailsDialog( aParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::CIAUpdateDetailsDialog
// 
// -----------------------------------------------------------------------------
//
CIAUpdateDetailsDialog::CIAUpdateDetailsDialog( TIAUpdateDialogParam* aParam )
	{
	iParam = aParam;
    }


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateDetailsDialog::ConstructL()
    {
    if ( !iParam->iNode )
        {
    	User::Leave( KErrNotFound );
        }
        
    TInt size = BufferSize();

 	iBuf = HBufC::NewL( size );
    } 



// ----------------------------------------------------------------------------
// Destructor
//
// ----------------------------------------------------------------------------
//
CIAUpdateDetailsDialog::~CIAUpdateDetailsDialog()
    {
    delete iBuf;
    }


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::BufferSize
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateDetailsDialog::BufferSize()
    {
    const TInt KExtraSize = 512;
    
    TPtrC ptr1 = iParam->iNode->Base().Name();
    TPtrC ptr2 = iParam->iNode->Base().Description();
    
    TInt len = ptr1.Length() + ptr2.Length() + KExtraSize;
    
    return len;
    } 


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::ConstructTextL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateDetailsDialog::ConstructTextL()
    {
    _LIT(KNewLine, "\n");

    TPtr ptr = iBuf->Des();
    
    //TPtrC name =         iParam->iNode->Base().Name();
    TPtrC description =  iParam->iNode->Base().Description();
    TIAUpdateVersion version = iParam->iNode->Base().Version();
    TInt contentSize =   iParam->iNode->Base().ContentSizeL();
    
    
    /*HBufC* hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_APP_NAME );
    ptr.Append( KOpeningBoldTag );
    ptr.Append( *hBuf );
    ptr.Append( KClosingBoldTag );
    CleanupStack::PopAndDestroy( hBuf );
    
    ptr.Append( KNewLine );
    ptr.Append( name );
    ptr.Append( KNewLine );
    ptr.Append( KNewLine );*/
    
    HBufC* hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_DESCRIPTION );
    ptr.Append( KOpeningBoldTag );
    ptr.Append( *hBuf );
    ptr.Append( KClosingBoldTag );
    CleanupStack::PopAndDestroy( hBuf );
    
    ptr.Append( KNewLine );
    ptr.Append( description );
    ptr.Append( KNewLine );
    ptr.Append( KNewLine );
    
    
    if( iParam->iNode->Type() != MIAUpdateNode::EPackageTypeServicePack )
        {
        hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_VERSION );
        ptr.Append( KOpeningBoldTag );
        ptr.Append( *hBuf );
        ptr.Append( KClosingBoldTag );
        CleanupStack::PopAndDestroy( hBuf );
        
        ptr.Append( KNewLine );
        hBuf = VersionTextLC( version );
        ptr.Append( *hBuf );
        ptr.Append( KNewLine );
        ptr.Append( KNewLine );
        CleanupStack::PopAndDestroy( hBuf );
        }
   
    
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


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::ShowDialogL
// 
// -----------------------------------------------------------------------------
TBool CIAUpdateDetailsDialog::ShowDialogL()
	{
	ConstructTextL();

	//HBufC* hBuf = StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_TITLE );
	IAUpdateDialogUtil::ShowMessageQueryL( iParam->iNode->Base().Name(), *iBuf );
	//CleanupStack::PopAndDestroy( hBuf );
    
    return ETrue;
	}


// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::FileSizeTextLC
// 
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateDetailsDialog::FileSizeTextLC( TInt aFileSize )
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

// -----------------------------------------------------------------------------
// CIAUpdateDetailsDialog::VersionTextLC
// 
// Version format is '%1N.%2N(%3N)' where
//
// %0N is major version number
// %1N is minor version number
// %2N is build number
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateDetailsDialog::VersionTextLC( TIAUpdateVersion aVersion )
    {
    const TInt KVersionSize = 64;
    
    TInt major = aVersion.iMajor;
    TInt minor = aVersion.iMinor;
    TInt build = aVersion.iBuild;
    
    TBuf<KVersionSize> buf1;
    TBuf<KVersionSize> buf2;
    
    HBufC* hBuf   = HBufC::NewLC( KVersionSize );
    TPtr ptr = hBuf->Des();
    
    HBufC* versionFormat = 
    StringLoader::LoadLC( R_IAUPDATE_DETAILS_DIALOG_VERSION_FORMAT );
   
    // replace  %0N with major number
    StringLoader::Format( buf1, versionFormat->Des(), 0, major );
    
    // replace  %1N with minor number
    StringLoader::Format( buf2, buf1, 1, minor );

    // replace  %2N with build number
    StringLoader::Format( ptr, buf2, 2, build );

    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    
    CleanupStack::PopAndDestroy( versionFormat );
    
    return hBuf;
    }

   
//  End of File  
