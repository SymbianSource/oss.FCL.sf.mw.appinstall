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
* Description:    
*
*/



// INCLUDE FILES

#include "iaupdatestatusdialog.h"
#include "iaupdatedialogutil.h"
#include "iaupdatedebug.h"

#include <iaupdate.rsg>
#include <aknmessagequerydialog.h>      // CAknMessageQueryDialog
#include <StringLoader.h>
#include <AknUtils.h>



/*******************************************************************************
 * class CIAUpdateStatusDialog
 *******************************************************************************/

/*
A message query can contain a link text which can be highlighted and opened.
When a query is opened and it has a link text in the beginning (visible without
scrolling the query) the link is not highlighted. When a link becomes visible by
scrolling the query, the link is highlighted.
There can be multiple link texts in the query. It is highly recommended, not to
use more than 3 link texts in one query.
A link text can open for example a help view or another message query.
The default softkeys are:
SK1 OK text.softkey.ok

SK1 changes to View text.softkey.view when a link text is highlighted.
SK2 does not have a default text.
*/


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::ShowDialogL
// 
// -----------------------------------------------------------------------------
TInt CIAUpdateStatusDialog::ShowDialogL( TIAUpdateDialogParam* aParam )
	{
	CIAUpdateStatusDialog* dialog = CIAUpdateStatusDialog::NewL( aParam );
	CleanupStack::PushL( dialog );

	TInt ret = dialog->ShowDialogL();

	CleanupStack::PopAndDestroy( dialog );
    
    return ret;
	}


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::NewL
//
// -----------------------------------------------------------------------------
CIAUpdateStatusDialog* CIAUpdateStatusDialog::NewL( TIAUpdateDialogParam* aParam )
    {
    CIAUpdateStatusDialog* self = new ( ELeave ) CIAUpdateStatusDialog( aParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::CIAUpdateStatusDialog
// 
// -----------------------------------------------------------------------------
//
CIAUpdateStatusDialog::CIAUpdateStatusDialog( TIAUpdateDialogParam* aParam )
	{
	iParam = aParam;
    }


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateStatusDialog::ConstructL()
    {
    } 


// ----------------------------------------------------------------------------
// Destructor
//
// ----------------------------------------------------------------------------
//
CIAUpdateStatusDialog::~CIAUpdateStatusDialog()
    {
    delete iBuf;
    }


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::ConstructTextL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateStatusDialog::ConstructTextL()
    {
    _LIT(KNewLine, "\n");
    
    const TInt KBufSize = 1024;

    delete iBuf;
    iBuf = NULL;
    iBuf = HBufC::NewL( KBufSize );
    TPtr ptr = iBuf->Des();
    
    HBufC* hBuf = NULL;

    
    if ( iParam->iCountSuccessfull == 0 && iParam->iCountCancelled == 0 &&
         iParam->iCountFailed == 0 )
        {
        hBuf = StringLoader::LoadLC( 
        R_IAUPDATE_STATUS_DIALOG_SUCCESSFUL, iParam->iCountSuccessfull );
        
        ptr.Append( *hBuf );
        ptr.Append( KNewLine );
        CleanupStack::PopAndDestroy( hBuf );

        hBuf = StringLoader::LoadLC( R_IAUPDATE_STATUS_DIALOG_TEXT_LINK );
        ptr.Append( KOpeningLinkTag );
        ptr.Append( *hBuf );
        ptr.Append( KClosingLinkTag );
        CleanupStack::PopAndDestroy( hBuf );
        return;
        }
    
    
        
    if ( iParam->iCountSuccessfull != 0 )
        {
        if ( iParam->iCountSuccessfull == 1 )
            {
            hBuf = StringLoader::LoadLC( R_IAUPDATE_STATUS_DIALOG_SUCCESSFUL_ONE );
            }
        else
            {
            hBuf = StringLoader::LoadLC( 
            R_IAUPDATE_STATUS_DIALOG_SUCCESSFUL, iParam->iCountSuccessfull );
            }
        ptr.Append( *hBuf );
        ptr.Append( KNewLine );
        CleanupStack::PopAndDestroy( hBuf );
        }
    
    if ( iParam->iCountCancelled != 0 )
        {
        if ( iParam->iCountCancelled == 1 )
            {
            hBuf = StringLoader::LoadLC( R_IAUPDATE_STATUS_DIALOG_CANCELLED_ONE );
            }
        else
            {
            hBuf = StringLoader::LoadLC( 
            R_IAUPDATE_STATUS_DIALOG_CANCELLED, iParam->iCountCancelled  );
            }
        ptr.Append( *hBuf );
        ptr.Append( KNewLine );
        CleanupStack::PopAndDestroy( hBuf );
        }

    if ( iParam->iCountFailed != 0 )
        {
        if ( iParam->iCountFailed == 1 )
            {
            hBuf = StringLoader::LoadLC( R_IAUPDATE_STATUS_DIALOG_FAILED_ONE );
            }
        else
            {
            hBuf = StringLoader::LoadLC( 
            R_IAUPDATE_STATUS_DIALOG_FAILED, iParam->iCountFailed );
            }
    
        ptr.Append( *hBuf );
        ptr.Append( KNewLine );
        CleanupStack::PopAndDestroy( hBuf );
        }
    
    if ( iParam->iShowCloseAllText )
        {
    	hBuf = StringLoader::LoadLC( R_IAUPDATE_CLOSE_ALL );
    	ptr.Append( *hBuf );
    	ptr.Append( KNewLine );
        CleanupStack::PopAndDestroy( hBuf );
        }


    hBuf = StringLoader::LoadLC( R_IAUPDATE_STATUS_DIALOG_TEXT_LINK );
    ptr.Append( KOpeningLinkTag );
    ptr.Append( *hBuf );
    ptr.Append( KClosingLinkTag );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
    CleanupStack::PopAndDestroy( hBuf );
    } 


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::ShowDialogL
// 
// -----------------------------------------------------------------------------
TInt CIAUpdateStatusDialog::ShowDialogL()
	{
	ConstructTextL();
	
	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *iBuf );
	dlg->PrepareLC( R_IAUPDATE_MESSAGE_QUERY );

	HBufC* title = StringLoader::LoadLC( R_IAUPDATE_STATUS_DIALOG_TITLE );
	CAknPopupHeadingPane* headingPane = dlg->Heading();
	headingPane->SetTextL( *title );
	CleanupStack::PopAndDestroy( title );

	TCallBack LinkCallback( HandleLink, this );
	dlg->SetLink( LinkCallback );
	
	return ( dlg->RunLD() );
	}


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::HandleLink
//
// Used as a callback function in message query.
// -----------------------------------------------------------------------------
//
TInt CIAUpdateStatusDialog::HandleLink( TAny* aPtr )
    {
    TInt err = KErrNone;
        
    CIAUpdateStatusDialog* self = static_cast<CIAUpdateStatusDialog*>(aPtr);
    
    TRAP( err, self->HandleLinkL() );
    return err;    
    }


// -----------------------------------------------------------------------------
// CIAUpdateStatusDialog::HandleLink
//
// Used as a callback function in message query.
// -----------------------------------------------------------------------------
//
void CIAUpdateStatusDialog::HandleLinkL()
    {
    if ( iParam->iLinkObserver )
        {
        iParam->iLinkObserver->TextLinkL( 0 );
        }
    }

    
//  End of File  
