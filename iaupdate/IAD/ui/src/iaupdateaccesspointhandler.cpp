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

#include "iaupdateaccesspointhandler.h"
#include "iaupdatedebug.h"
#include <iaupdate.rsg>

#include <featmgr.h>
#include <StringLoader.h>
// access point selection
#include <cmapplicationsettingsui.h>
#include <cmconnectionmethoddef.h>
#include <cmdestination.h>



 

/******************************************************************************
 * class CIAUpdateAccessPointHandler
 ******************************************************************************/
 
// -----------------------------------------------------------------------------
// CIAUpdateAccessPointHandler::NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateAccessPointHandler* CIAUpdateAccessPointHandler::NewL()
    {
    CIAUpdateAccessPointHandler* self = new( ELeave ) CIAUpdateAccessPointHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
    }


// -----------------------------------------------------------------------------
// Destructor
//
// -----------------------------------------------------------------------------
//
CIAUpdateAccessPointHandler::~CIAUpdateAccessPointHandler()
	{
	delete iCmUi;
	iCmManager.Close();
	}


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointHandler::CIAUpdateAccessPointHandler
//
// -----------------------------------------------------------------------------
//
CIAUpdateAccessPointHandler::CIAUpdateAccessPointHandler()
	{
	}


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateAccessPointHandler::ConstructL()
    {
    iCmManager.OpenL();
    iCmUi = CCmApplicationSettingsUi::NewL();

	iIdAppCsdSupport = EFalse;
	FeatureManager::InitializeLibL();
	if ( FeatureManager::FeatureSupported( KFeatureIdAppCsdSupport ) )
	    {
	    iIdAppCsdSupport = ETrue;
	    }
	FeatureManager::UnInitializeLib();
    }


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointHandler::ShowApSelectDialogL
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateAccessPointHandler::ShowApSelectDialogL( TInt& aItemUid, HBufC*& aItemName )
	{
    TBearerFilterArray filterArray;
    TCmSettingSelection selection;
    selection.iId = aItemUid;
    selection.iResult = CMManager::EDestination;
    
    TBool ret = iCmUi->RunApplicationSettingsL( 
        selection,
        CMManager::EShowDestinations | CMManager::EShowDefaultConnection, 
        filterArray );
    filterArray.Close();
	
	if ( ret )
		{
		aItemUid = selection.iId;
		if ( selection.iResult == CMManager::EDefaultConnection )
		    {
			GetDefaultConnectionLabelL( aItemName );
			}
	    else
	        {
	        GetApNameL( aItemUid, aItemName );	
	        }
		return EDialogSelect;
		}
    else
    	{
    	return EDialogCancel;
    	}
	}

	
// -----------------------------------------------------------------------------
// CIAUpdateAccessPointHandler::GetApNameL
//
// -----------------------------------------------------------------------------
//
TInt CIAUpdateAccessPointHandler::GetApNameL( TInt aItemUid, HBufC*& aItemName )
	{
	if ( aItemUid == KErrNotFound || aItemUid == 0 )
		{
		aItemName = KNullDesC().AllocL();
		return KErrNotFound;
		}

    RCmDestination dest = iCmManager.DestinationL( aItemUid );
    CleanupClosePushL( dest );
    HBufC* temp = dest.NameLC();    
    CleanupStack::Pop( temp );
	CleanupStack::PopAndDestroy( &dest ); 
	aItemName = temp;

	TInt ret = KErrNone;
	if ( aItemName->Length() == 0 ) 
		{
		ret = KErrNotFound;
		}
    
	return ret;
	}


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointHandler::GetDefaultConnectionLabelL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateAccessPointHandler::GetDefaultConnectionLabelL( HBufC*& aLabelText )
    {
    aLabelText = StringLoader::LoadLC( R_IAUPDATE_DEFAULT_CONNECTION );    
    CleanupStack::Pop( aLabelText );
    }

    
//  End of File  
