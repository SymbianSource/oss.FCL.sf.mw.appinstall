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

#include "iaupdatestatuspanehandler.h"
#include "iaupdatenavipanehandler.h"
#include "iaupdatedebug.h"
#include "iaupdatedialogutil.h"

#include <akntitle.h>           // CAknTitlePane
#include <StringLoader.h>


/*******************************************************************************
 * class CIAUpdateStatusPaneHandler
 *******************************************************************************/

// -----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateStatusPaneHandler* CIAUpdateStatusPaneHandler::NewL( CAknAppUi* aAppUi )
    {
    CIAUpdateStatusPaneHandler* self = new (ELeave) CIAUpdateStatusPaneHandler( aAppUi );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }


// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CIAUpdateStatusPaneHandler::~CIAUpdateStatusPaneHandler()
    {
	TRAP_IGNORE( RestoreOriginalTitleL() );
	
	delete iNaviPaneHandler;
	delete iOriginalTitle;
    }


// -----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateStatusPaneHandler::ConstructL()
    {
    iNaviPaneHandler = new (ELeave) CIAUpdateNaviPaneHandler( iAppUi->StatusPane() );
    iOriginalTitle = KNullDesC().AllocL();
    }


// -----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::CIAUpdateStatusPaneHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateStatusPaneHandler::CIAUpdateStatusPaneHandler( CAknAppUi* aAppUi ) : iAppUi(aAppUi )
	{
	iOriginalTitleStored = EFalse;
	}


// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::GetTitleL
// 
// ----------------------------------------------------------------------------
//
TBool CIAUpdateStatusPaneHandler::GetTitleL( CAknAppUi* aAppUi, HBufC*& aTitle )
	{
	aTitle = NULL;

	CEikStatusPane* statusPane = aAppUi->StatusPane();
    if (statusPane && statusPane->PaneCapabilities( 
                            TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
		{
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        if ( titlePane->Text() ) 
			{
		    aTitle = titlePane->Text()->AllocL();
			return ETrue;
    		}
		}

	return EFalse;
	}


// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::SetTitleL
// 
// ----------------------------------------------------------------------------
//
TBool CIAUpdateStatusPaneHandler::SetTitleL( CAknAppUi* aAppUi, const TDesC& aTitle )
	{
	CEikStatusPane* statusPane = aAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities( 
                              TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
		{
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        if ( titlePane->Text() ) 
			{
	   		titlePane->SetTextL( aTitle );
			return ETrue;
			}
		}
	return EFalse;
	}


// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::SetNaviPaneTitleL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateStatusPaneHandler::SetNaviPaneTitleL( const TDesC& aTitle )
	{
	iNaviPaneHandler->SetNaviPaneTitleL( aTitle );
	}



// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::StoreOriginalTitleL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateStatusPaneHandler::StoreOriginalTitleL()
	{
	delete iOriginalTitle;
	iOriginalTitle = NULL;
	TBool ret = GetTitleL( iAppUi, iOriginalTitle );
	if ( !iOriginalTitle )
	    {
		iOriginalTitle = KNullDesC().AllocL();
	    }
	if ( ret )
		{
		iOriginalTitleStored = ETrue;
		}
	}


// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::RestoreOriginalTitleL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateStatusPaneHandler::RestoreOriginalTitleL()
	{
	if ( iOriginalTitleStored )
		{
		SetTitleL( iAppUi, *iOriginalTitle );
		}
	}


// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::SetTitleL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateStatusPaneHandler::SetTitleL( const TDesC& aText )
	{
	if ( iOriginalTitleStored )
		{
		SetTitleL( iAppUi, aText );
		}
	}


// ----------------------------------------------------------------------------
// CIAUpdateStatusPaneHandler::SetTitleL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateStatusPaneHandler::SetTitleL( TInt aResourceId )
	{
	HBufC* hBuf = StringLoader::LoadLC( aResourceId );
	SetTitleL( hBuf->Des() );
	CleanupStack::PopAndDestroy( hBuf );
	}

    
//  End of File  
