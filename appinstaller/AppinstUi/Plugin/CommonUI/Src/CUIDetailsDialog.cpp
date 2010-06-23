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
* Description:   This file contains the implementation of CSisxUIInstall
*                class member functions.
*
*/


// INCLUDE FILES
//#include <aknmessagequerydialog.h>
//#include <aknlistquerydialog.h> // remove
//#include <StringLoader.h>
#include <bautils.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <SWInstCommonUI.rsg>
#include <pathinfo.h>
//#include <avkon.rsg>
#include <DRMHelper.h>

#include "CUIDetailsDialog.h"
#include "CUICertificateDetailsDialog.h"
#include "CUIUtils.h"

using namespace SwiUI::CommonUI;

// LOCAL CONSTANTS AND MACROS

_LIT( KNextLine, "\n" );
_LIT( KLinkTagOpen, "<AknMessageQuery Link>" );
_LIT( KLinkTagClose, "</AknMessageQuery Link>" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::CCUIDetailsDialog
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCUIDetailsDialog::CCUIDetailsDialog()
//    : iCbaResourceId( R_AVKON_SOFTKEYS_OK_EMPTY )
    {
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::CCUIDetailsDialog
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCUIDetailsDialog::CCUIDetailsDialog( TInt aCbaResourceId )
    : iCbaResourceId( aCbaResourceId )
    {
    }
// -----------------------------------------------------------------------------
// CCUIDetailsDialog::CCUIDetailsDialog
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCUIDetailsDialog::CCUIDetailsDialog( TInt aCbaResourceId, TCUIDetailsHeader aHeaderType )
    : iCbaResourceId( aCbaResourceId ), iHeaderType( aHeaderType )
    {
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::ConstructL()
    {    
    iCoeEnv = CEikonEnv::Static();    
    if ( !iCoeEnv )
        {
        User::Leave( KErrGeneral );        
        }

    TFileName fileName;
    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KCUIResourceFileName );
    iResourceFileOffset = CUIUtils::LoadResourceFileL( fileName, iCoeEnv );    
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUIDetailsDialog* CCUIDetailsDialog::NewL()
    {
    CCUIDetailsDialog* self = new ( ELeave ) CCUIDetailsDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUIDetailsDialog* CCUIDetailsDialog::NewL( TInt aCbaResourceId )
    {
    CCUIDetailsDialog* self = new ( ELeave ) CCUIDetailsDialog( aCbaResourceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUIDetailsDialog* CCUIDetailsDialog::NewL( TInt aCbaResourceId,
                                                     TBool aWithInstallHeader )
    {
    CCUIDetailsDialog* self = new ( ELeave ) CCUIDetailsDialog( aCbaResourceId,
                                ( aWithInstallHeader ? EInstallHeader : EDetailsHeader ) );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCUICertificateDetailsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCUIDetailsDialog* CCUIDetailsDialog::NewL( TInt aCbaResourceId,
                                                     TCUIDetailsHeader aHeaderType )
    {
    CCUIDetailsDialog* self = new ( ELeave ) CCUIDetailsDialog( aCbaResourceId,
                                                                aHeaderType  );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
EXPORT_C CCUIDetailsDialog::~CCUIDetailsDialog()
    {
    delete iMessageArray;    
   
    if( iResourceFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResourceFileOffset );
        }

    iCommonCertificates.Close();
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ExecuteLD
// Displays the dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCUIDetailsDialog::ExecuteLD( MCUIInfoIterator& aIterator )
    {
    return ShowDialogL( aIterator );
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ExecuteLD
// Displays the dialog with given certificates.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCUIDetailsDialog::ExecuteLD( MCUIInfoIterator& aIterator,
                                            const RPointerArray<CCUICertificateInfo>& aCertInfos )
    {
    // Copy the certificates
    for ( TInt index = 0; index < aCertInfos.Count(); index++ )
        {
        iCommonCertificates.Append( aCertInfos[index] );        
        }

    return ShowDialogL( aIterator );    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ExecuteLD
// Displays the dialog with given certificates.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCUIDetailsDialog::ExecuteLD( MCUIInfoIterator& aIterator,
                                            RFile& aFile )
    {
    iFile = aFile;
    iFileSet = ETrue;

    return ShowDialogL( aIterator );
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ExecuteLD
// Displays the dialog with given certificates.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCUIDetailsDialog::ExecuteLD( MCUIInfoIterator& aIterator,
                                            const RPointerArray<CCUICertificateInfo>& aCertInfos,
                                            RFile& aFile )
    {
    iFile = aFile;
    iFileSet = ETrue;

    return ExecuteLD( aIterator, aCertInfos );    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ShowDialogL
// Helper to show the details dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCUIDetailsDialog::ShowDialogL( MCUIInfoIterator& aIterator )
    {
    TInt result( 0 );
/*
    CleanupStack::PushL( this );

    PopulateArrayL( aIterator );
    HBufC* message = GetMessageDescriptorLC();        
    
    // Create and show the message query dialog
    TCallBack certCallback( ShowCertificates, this );    
    TCallBack drmCallback( ShowDrm, this );
    
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *message );
    dlg->PrepareLC( R_SWCOMMON_DETAILS_DIALOG );
#ifdef RD_COMBINE_INSTALLATION_QUERIES
    HBufC* header = NULL;
    switch( iHeaderType )
        {
        case EInstallAndDownloadHeader:
            header = StringLoader::LoadLC( R_SWCOMMON_HEADER_DOWNLOAD );
            dlg->QueryHeading()->SetTextL( *header );
            CleanupStack::PopAndDestroy( header );
            break;
            
        case EInstallHeader:
            header = StringLoader::LoadLC( R_SWCOMMON_HEADER_INSTALL );
            dlg->QueryHeading()->SetTextL( *header );
            CleanupStack::PopAndDestroy( header );
            break;
            
        case EDetailsHeader:
        default:
            break;
        }
#endif //RD_COMBINE_INSTALLATION_QUERIES
    dlg->ButtonGroupContainer().SetCommandSetL( iCbaResourceId );
    
    if ( iCommonCertificates.Count() > 0 )
        {
        dlg->SetLink( certCallback );
        }
    if ( iFileSet )
        {        
        dlg->SetLink( drmCallback );
        }    
    
    result = dlg->RunLD();
    CleanupStack::PopAndDestroy( 2 ); // message, this
*/
    return result;    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::SetFieldL
// Sets a field in the details dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::SetFieldL( CDesCArray& aItemArray,
                                   const TDesC& aHeading, 
                                   const TDesC& aValue )
    {
    HBufC* temp = HBufC::NewLC( aHeading.Length() + aValue.Length() + 3 );
    TPtr ptr( temp->Des() );
    
    if ( aItemArray.Count() > 0 )
        {
        ptr += KNextLine;
        ptr += KNextLine;        
        }
    
    ptr += aHeading;        
    ptr += KNextLine;
    if ( aValue.Length() > 0 )
        {
        ptr += aValue;
        }    
    aItemArray.AppendL( *temp );    
    CleanupStack::PopAndDestroy( temp );    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::PopulateArrayL
// Helper to populate the field array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::PopulateArrayL( MCUIInfoIterator& aIterator )
    {
    delete iMessageArray;    
    iMessageArray = NULL;    
    iMessageArray = new (ELeave) CDesCArrayFlat( 8 );

    // Construct the text to be shown
    aIterator.Reset();    
    while ( aIterator.HasNext() )
        {
        TPtrC heading;
        TPtrC value;
        
        aIterator.Next( heading, value );
        SetFieldL( *iMessageArray, heading, value );        
        }
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::GetMessageDescriptorLC
// Helper to get descriptor containing the fields of the dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCUIDetailsDialog::GetMessageDescriptorLC()
    {
    /*
    TInt size( 0 ); 
    TInt index( 0 );

    HBufC* type = StringLoader::LoadLC( R_SWCOMMON_DETAIL_TYPE );    
    HBufC* typeString = HBufC::NewLC( type->Length() + 1);
    typeString->Des().Copy( *type );
    typeString->Des().Append( KNextLine() );    

    HBufC* message = HBufC::NewLC( size );    
    
    for ( index = 0; index < iMessageArray->Count(); index++ )
        {       
        HBufC* tmp = message;
        message = message->ReAllocL( tmp->Length() + (*iMessageArray)[index].Length() );
        CleanupStack::Pop( tmp );
        CleanupStack::PushL( message );
        
        message->Des().Append( (*iMessageArray)[index] );
        // If this is the type field we need to add possible certificates and
        // drm details
        if ( (*iMessageArray)[index].Find( *typeString ) == 2 )
            {
            if ( iCommonCertificates.Count() > 0 )
                {
                AddCertificatesLC( message );                
                }            
            if ( iFileSet )
                {
                AddDrmLC( message );                
                }            
            }        
        }    

    CleanupStack::Pop( message );
    CleanupStack::PopAndDestroy( 2, type );
    CleanupStack::PushL( message );    
   
    return message;
    */
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::AddCertificatesLC
// Helper to add certificate information to query text.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::AddCertificatesLC( HBufC*& aMessage )
    {
    /*
    HBufC* certHeading = StringLoader::LoadLC( R_SWCOMMON_DETAIL_CERTIFICATES );
    HBufC* certificateLink = LoadLinkLC( R_SWCOMMON_DETAIL_VALUE_VIEW_CERT );
    HBufC* newString = HBufC::NewL( certHeading->Length() + 
                                    aMessage->Length() + 
                                    certificateLink->Length() + 
                                    3 );

    TPtr ptr( newString->Des() );
    ptr += *aMessage;        
    ptr += KNextLine;
    ptr += KNextLine;
    ptr += *certHeading;
    ptr += KNextLine;
    ptr += *certificateLink;
   
    CleanupStack::PopAndDestroy( 3, aMessage );
  
    aMessage = newString;
    CleanupStack::PushL( aMessage ); 
    */
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::AddDrmLC
// Helper to add DRM information to query text.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::AddDrmLC( HBufC*& aMessage )
    {
    /*
    HBufC* drmHeading = StringLoader::LoadLC( R_SWCOMMON_DETAIL_DRM );
    HBufC* drmLink = LoadLinkLC( R_SWCOMMON_DETAIL_VALUE_VIEW_DRM );
    HBufC* newString = HBufC::NewL( drmHeading->Length() + 
                                    aMessage->Length() + 
                                    drmLink->Length() + 
                                    3 );

    TPtr ptr( newString->Des() );
    ptr += *aMessage;        
    ptr += KNextLine;
    ptr += KNextLine;
    ptr += *drmHeading;
    ptr += KNextLine;
    ptr += *drmLink;
    
    CleanupStack::PopAndDestroy( 3, aMessage );
  
    aMessage = newString;
    CleanupStack::PushL( aMessage ); 
    */
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ShowCertificates
// Used as a callback function in message query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCUIDetailsDialog::ShowCertificates( TAny* ptr )
    {
    TInt err( KErrNone );    
    CCUIDetailsDialog* self = static_cast<CCUIDetailsDialog*>(ptr);
    TRAP( err, self->DoShowCertificatesL() );
    return err;    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::ShowDrm
// Used as a callback function in message query.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCUIDetailsDialog::ShowDrm( TAny* ptr )
    {
    TInt err( KErrNone );    
    CCUIDetailsDialog* self = static_cast<CCUIDetailsDialog*>(ptr);
    TRAP( err, self->DoShowDrmL() );
    return err;    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::DoShowCertificatesL
// Show CCUICertificateInfo certificates.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::DoShowCertificatesL()
    {
    /*
    if ( iCommonCertificates.Count() > 1 )
        {
        CDesCArrayFlat *itemArray = new( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( itemArray );

        for ( TInt index = 0; index < iCommonCertificates.Count(); index++ )
            {
            itemArray->AppendL( iCommonCertificates[index]->IssuerNameL() );            
            }        

        TInt selection( 0 );
        TInt dlgResult( EAknSoftkeyOk );
        
        while ( dlgResult )
            {
            // First show a dialog where user can select the certificate, which
            // will be viewed in more detail
            CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selection );
            dlg->PrepareLC( R_SWCOMMON_SEC_SELECTION_DIALOG );
            dlg->SetItemTextArray( itemArray );
            dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
            dlg->ListBox()->SetCurrentItemIndex( selection );            

            dlgResult = dlg->RunLD();
            if ( dlgResult )
                {
                // Show details about the selected certificate
                CCUICertificateDetailsDialog* certDlg = 
                    CCUICertificateDetailsDialog::NewL();
                
                certDlg->ExecuteLD( *( iCommonCertificates[selection] ) );    
                }            
            }        

        CleanupStack::PopAndDestroy( itemArray );
        }   
    else if ( iCommonCertificates.Count() == 1 )
        {
        CCUICertificateDetailsDialog* certDlg = 
            CCUICertificateDetailsDialog::NewL();
                
        certDlg->ExecuteLD( *( iCommonCertificates[0] ) );    
        }  
    */      
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::DoShowDrmL
// Show detailed drm info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCUIDetailsDialog::DoShowDrmL()
    {
    CDRMHelper* helper = CDRMHelper::NewLC( *iCoeEnv );
    helper->LaunchDetailsViewEmbeddedL( iFile );   
    CleanupStack::PopAndDestroy( helper );    
    }

// -----------------------------------------------------------------------------
// CCUIDetailsDialog::LoadLinkLC
// Helper to construct a message query link from resource string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCUIDetailsDialog::LoadLinkLC( TInt aResourceId )
    {
    /*
    HBufC* link = StringLoader::LoadLC( aResourceId );
    HBufC* tmp = link->ReAllocL( link->Length() + KLinkTagOpen().Length() + KLinkTagClose().Length() );
    
    CleanupStack::Pop( link );
    link = tmp;
    CleanupStack::PushL( link );
    
    link->Des().Insert(0, KLinkTagOpen);
    link->Des().Append( KLinkTagClose );   

    return link;   
    */ 
    return NULL;
    }

//  End of File  
