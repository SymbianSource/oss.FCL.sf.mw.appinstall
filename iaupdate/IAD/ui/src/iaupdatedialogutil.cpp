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

#include "iaupdatedialogutil.h"
#include "iaupdatedebug.h"
#include <iaupdate.rsg>

#include <aknmessagequerydialog.h>      // CAknMessageQueryDialog
#include <aknnotewrappers.h>    // CAknInformationNote
#include <StringLoader.h>


/*******************************************************************************
 * class TIAUpdateDialogParam
 *******************************************************************************/


// -------------------------------------------------------------------------------
// TIAUpdateDialogParam::TIAUpdateDialogParam
//
// -------------------------------------------------------------------------------
//
TIAUpdateDialogParam::TIAUpdateDialogParam()
 : iCountSuccessfull( KErrNotFound ), iCountCancelled( KErrNotFound ),
   iCountFailed( KErrNotFound ), iResourceId( KErrNotFound ), iNode( NULL ), 
   iLinkObserver( NULL )
    {
    }


/*******************************************************************************
 * class IAUpdateDialogUtil
 *******************************************************************************/


//------------------------------------------------------------------------------
// IAUpdateDialogUtil::ShowMessageQueryL
//
//------------------------------------------------------------------------------
//
void IAUpdateDialogUtil::ShowMessageQueryL(const TDesC& aTitle, const TDesC& aText)
	{
	TPtrC ptr = aText;
	CAknMessageQueryDialog* query = CAknMessageQueryDialog::NewL( ptr );
	query->PrepareLC( R_IAUPDATE_MESSAGE_QUERY );

	if ( aTitle.Length() != 0 )
		{
		CAknPopupHeadingPane* headingPane = query->Heading();
		headingPane->SetTextL( aTitle );
		}
	query->RunLD();
	}


//------------------------------------------------------------------------------
// IAUpdateDialogUtil::ShowMessageQueryL
//
//------------------------------------------------------------------------------
//
void IAUpdateDialogUtil::ShowMessageQueryL( const TDesC& aTitle, TInt aResource )
	{
	HBufC* hBuf = StringLoader::LoadLC( aResource );
	ShowMessageQueryL( aTitle, hBuf->Des() );
	CleanupStack::PopAndDestroy( hBuf );
	}


//------------------------------------------------------------------------------
// IAUpdateDialogUtil::ShowInformationQueryL
//
//------------------------------------------------------------------------------
//
void IAUpdateDialogUtil::ShowInformationQueryL(const TDesC& aText)
	{
	CAknQueryDialog* queryDialog = new (ELeave) CAknQueryDialog;
	queryDialog->ExecuteLD(R_IAUPDATE_INFORMATION_QUERY, aText);
	}


//------------------------------------------------------------------------------
// IAUpdateDialogUtil::ShowInformationQueryL
//
//------------------------------------------------------------------------------
//
void IAUpdateDialogUtil::ShowInformationQueryL(TInt aResource)
	{
	HBufC* hBuf = StringLoader::LoadLC(aResource);
	ShowInformationQueryL( hBuf->Des() );
	CleanupStack::PopAndDestroy( hBuf );
	}

//------------------------------------------------------------------------------
// IAUpdateDialogUtil::ShowConfirmationQueryL
//
//------------------------------------------------------------------------------
//
TInt IAUpdateDialogUtil::ShowConfirmationQueryL( const TDesC& aText, 
                                          TInt aSoftkeyResourceId )
	{
	CAknQueryDialog* queryDialog = CAknQueryDialog::NewL( 
	                               CAknQueryDialog::EConfirmationTone );

    queryDialog->PrepareLC( R_IAUPDATE_CONFIRMATION_QUERY );
    queryDialog->SetPromptL( aText );
    queryDialog->ButtonGroupContainer().SetCommandSetL( aSoftkeyResourceId );
    return ( queryDialog->RunLD() );
	}

//------------------------------------------------------------------------------
// IAUpdateDialogUtil::ShowConfirmationQueryL
//
//------------------------------------------------------------------------------
//
TInt IAUpdateDialogUtil::ShowConfirmationQueryL( TInt aResource, 
                                          TInt aSoftkeyResourceId )
	{
	HBufC* hBuf = StringLoader::LoadLC( aResource) ;
	TInt ret = ShowConfirmationQueryL( *hBuf, aSoftkeyResourceId );
	CleanupStack::PopAndDestroy( hBuf );
	return ret;
	}




// -----------------------------------------------------------------------------
// IAUpdateDialogUtil::Panic
//
// -----------------------------------------------------------------------------
//
void IAUpdateDialogUtil::Panic( TInt aReason ) 
    {
	_LIT(KPanicCategory, "IAUpdateDialogUtil");
	
	User::Panic(KPanicCategory, aReason); 
    }
    
//  End of File  
