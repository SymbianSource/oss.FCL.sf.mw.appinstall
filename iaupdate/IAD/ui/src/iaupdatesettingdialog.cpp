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

#include "iaupdatesettingdialog.h"
#include "iaupdateaccesspointhandler.h"  
#include "iaupdatestatuspanehandler.h"
#include "iaupdateapplication.h"
#include "iaupdate.hrh"
#include "iaupdateprivatecrkeys.h"
#include "iaupdatedebug.h"
#include <iaupdate.rsg>

#include <centralrepository.h>
#include <featmgr.h> 
#include <hlplch.h>


_LIT( KSWUPDATE_HLP_SETTINGS, "SWUPDATE_HLP_SETTINGS" ); 




// cenrep in emulator:
// copy 2000F85A.txt to '\epoc32\release\winscw\udeb\Z\private\10202be9\'
// delete 2000F85A.txt from 'epoc32\winscw\c\private\10202be9\persists'
//
// cenrep in hardware:
// copy 2000F85A.txt to '\epoc32\data\Z\private\10202be9'
//



/******************************************************************************
 * class CIAUpdateSettingDialog
 ******************************************************************************/


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::ShowDialogL
// 
// -----------------------------------------------------------------------------
TBool CIAUpdateSettingDialog::ShowDialogL()
	{
	//__UHEAP_MARK;
	
	CIAUpdateSettingDialog* dialog = CIAUpdateSettingDialog::NewL();

	TBool ret = dialog->ExecuteLD( R_IAUPDATE_SETTING_DIALOG );

   	//__UHEAP_MARKEND;
   	
    return ret;
	}


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::NewL
//
// -----------------------------------------------------------------------------
CIAUpdateSettingDialog* CIAUpdateSettingDialog::NewL()
    {
    CIAUpdateSettingDialog* self = new ( ELeave ) CIAUpdateSettingDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::CIAUpdateSettingDialog
// 
// -----------------------------------------------------------------------------
//
CIAUpdateSettingDialog::CIAUpdateSettingDialog()
	{
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::ConstructL()
    {
 	CAknDialog::ConstructL( R_IAUPDATE_SETTING_DIALOG_MENU );
    
    // get previous title so it can be restored
	iStatusPaneHandler = CIAUpdateStatusPaneHandler::NewL( iAvkonAppUi );
	iStatusPaneHandler->StoreOriginalTitleL();
    } 


// ----------------------------------------------------------------------------
// Destructor
//
// ----------------------------------------------------------------------------
//
CIAUpdateSettingDialog::~CIAUpdateSettingDialog()
    {
	delete iStatusPaneHandler;
	
    if (iAvkonAppUi)
    	{
    	iAvkonAppUi->RemoveFromStack( this );
    	}
    }


// ---------------------------------------------------------
// CIAUpdateSettingDialog::CreateCustomControlL
// ---------------------------------------------------------
//
SEikControlInfo CIAUpdateSettingDialog::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    
    switch ( aControlType )
        {
        case EAknCtLastControlId:
            {
            controlInfo.iControl = new (ELeave) CIAUpdateSettingItemList();
            break;
            }

        default:
            {
            break;
            }
        }
    return controlInfo;
    }


//------------------------------------------------------------------------------
// CIAUpdateSettingDialog::ActivateL
//
// Called by system when dialog is activated.
//------------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::ActivateL()
	{
    CAknDialog::ActivateL();

	// this cannot be in ConstructL which is executed before dialog is launched
	iAvkonAppUi->AddToStackL(this);
    }


//------------------------------------------------------------------------------
// CIAUpdateSettingDialog::GetHelpContext
//
//------------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::GetHelpContext( TCoeHelpContext& aContext ) const
	{
	aContext.iMajor = KUidIAUpdateApp;
    aContext.iContext = KSWUPDATE_HLP_SETTINGS;        
	}


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::HandleListBoxEventL
// 
// -----------------------------------------------------------------------------
void CIAUpdateSettingDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                  TListBoxEvent /*aEventType*/ )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::PreLayoutDynInitL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::PreLayoutDynInitL()
    {
    iList = (CIAUpdateSettingItemList*) ControlOrNull ( EIAUpdateSettingDialogList );
    
    iList->LoadSettingsL(); // from CAknSettingItemList
    
	iStatusPaneHandler->SetTitleL( R_IAUPDATE_SETTING_DIALOG_TITLE );
	iStatusPaneHandler->SetNaviPaneTitleL(KNullDesC);
    }


//------------------------------------------------------------------------------
// CIAUpdateSettingDialog::DynInitMenuPaneL
//
// Called by system before menu is shown.
//------------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::DynInitMenuPaneL(TInt aResourceID, CEikMenuPane* aMenuPane )
	{
    if( aResourceID == R_IAUPDATE_SETTING_DIALOG_MENU_PANE ) 
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    }


//------------------------------------------------------------------------------
// CIAUpdateSettingDialog::ProcessCommandL
//
// Handle commands from menu.
//------------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::ProcessCommandL(TInt aCommandId)
	{
    if ( MenuShowing() ) 
        {
        HideMenu();
        }


	switch ( aCommandId )
		{
		case EAknCmdHelp:
			{
			HlpLauncher::LaunchHelpApplicationL( 
			                     iEikonEnv->WsSession(), 
			                     iEikonEnv->EikAppUi()->AppHelpContextL() );
		    break;
			}
 
		case EAknCmdOpen:
			{
			EditItemL();
			break;
			}
		    
        case EAknCmdExit:
        case EEikCmdExit:
			{
			// close dialog and exit calling application
			iAvkonAppUi->ProcessCommandL( EAknCmdExit );
			break;
			}

		default:			
		    break;
		}
	}


//------------------------------------------------------------------------------
// CIAUpdateSettingDialog::OkToExitL
//
//------------------------------------------------------------------------------
//
TBool CIAUpdateSettingDialog::OkToExitL(TInt aButtonId)
	{
	if ( aButtonId == EEikBidCancel )
		{
		TRAP_IGNORE( SaveSettingsL() );  //potential leave trapped 
		                                 //because IAD is not closed if SaveSettingsL() leaves 
		return ETrue; // close dialog
		}

	if ( aButtonId == EAknSoftkeyOpen )
		{
		EditItemL();

		return EFalse; // leave dialog open
		}

	if ( aButtonId == EAknSoftkeyBack )
        {
		SaveSettingsL();
		
		return ETrue; // close dialog
		}

	return CAknDialog::OkToExitL(aButtonId);
	}



// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::OfferKeyEventL
// 
// ----------------------------------------------------------------------------
//
TKeyResponse CIAUpdateSettingDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if (aType == EEventKey)
		{
		switch (aKeyEvent.iCode)
			{
			case EKeyEscape:  // framework calls this when dialog must shut down
				{
				return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
        		}
			case EKeyUpArrow:
			case EKeyDownArrow:
			    {
    		    break;
			    }
            default:
				{
			    break;
				}
			}
		}

	return CAknDialog::OfferKeyEventL( aKeyEvent, aType);
	}


// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::HandleResourceChange
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::HandleResourceChange(TInt aType)
    {   
    if (aType == KEikDynamicLayoutVariantSwitch) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
    
        iList->HandleResourceChange( aType );
    
        CCoeControl::HandleResourceChange( aType );
		DrawDeferred();
	    return;
		}
		
    if ( aType == KAknsMessageSkinChange )
        {
        }
		
    CCoeControl::HandleResourceChange( aType );
    }


// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::EditItemL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::EditItemL()
	{
	TInt currentIndex = iList->ListBox()->CurrentItemIndex();
	TBool calledFromMenu = ETrue;
	
	iList->EditItemL( currentIndex, calledFromMenu );
	}


// -----------------------------------------------------------------------------
// CIAUpdateSettingDialog::SetVisibility
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::SetVisibility()
	{
	}


// -----------------------------------------------------------------------------
// CAspContentDialog::SetAllReadOnly
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::SetAllReadOnly()
	{
	}


// ----------------------------------------------------------------------------
// CIAUpdateSettingDialog::SaveSettingsL
// 
// ----------------------------------------------------------------------------
//
void CIAUpdateSettingDialog::SaveSettingsL()
	{
    iList->StoreSettingsL();  // from CAknSettingItemList
    
    CRepository* cenrep = CRepository::NewLC( KCRUidIAUpdateSettings );
    TInt err = cenrep->StartTransaction( CRepository::EReadWriteTransaction );
    User::LeaveIfError( err );
    cenrep->CleanupCancelTransactionPushL();

    
    TInt num = iList->Attribute( EAccessPoint );
    if ( num == 0 )
        {
        // O means default destination. Let's save it as -1 so that we know later on 
        // whether cenrep really contained the default or just nothing.
        num = -1;
        }
    err = cenrep->Set( KIAUpdateAccessPoint, num );
    User::LeaveIfError( err );

    num = iList->Attribute( EAutoUpdateCheck );
    err = cenrep->Set( KIAUpdateAutoUpdateCheck, num );
    User::LeaveIfError( err );
    
    num = iList->Attribute( ERoamingWarning );
    err = cenrep->Set( KIAUpdateRoamingWarning, num );
    User::LeaveIfError( err );
    
    TUint32 ignore = KErrNone;
    User::LeaveIfError( cenrep->CommitTransaction( ignore ) );
    CleanupStack::PopAndDestroy(); // CleanupCancelTransactionPushL()
    CleanupStack::PopAndDestroy( cenrep );
	}
	
	



/******************************************************************************
 * class CIAUpdateSettingItemList
 ******************************************************************************/


// -----------------------------------------------------------------------------
// CIAUpdateSettingItemList::CreateSettingItemL
// 
// -----------------------------------------------------------------------------
//
CAknSettingItem* CIAUpdateSettingItemList::CreateSettingItemL( TInt aSettingId )
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSettingItemList::CreateSettingItemL begin");
    CRepository* cenrep = CRepository::NewLC( KCRUidIAUpdateSettings );
    CAknSettingItem* item = NULL;
    
    switch ( aSettingId )
        {
        case EIAUpdateSettingAccessPoint:
            {
            User::LeaveIfError( cenrep->Get( KIAUpdateAccessPoint, iAccessPoint ) );
            IAUPDATE_TRACE_1("[IAUPDATE] CIAUpdateSettingItemList::CreateSettingItemL iAccessPoint: %d", iAccessPoint);
            if ( iAccessPoint == -1 )
                {
                // -1 was just our private representation in cenrep of the default destination  
                // Let's handle it as it was 0
                iAccessPoint = 0;
                }
            item = CIAUpdateAccessPointSettingItem::NewL( aSettingId, iAccessPoint );
            break;
            }

        case EIAUpdateSettingAutoUpdateCheck:
            {
            User::LeaveIfError( cenrep->Get( KIAUpdateAutoUpdateCheck, iAutoUpdateCheck ) );
            item = new (ELeave) CAknEnumeratedTextPopupSettingItem
                               ( aSettingId, iAutoUpdateCheck );
            break;
            }

        case EIAUpdateSettingRoamingWarning:
            {
            User::LeaveIfError( cenrep->Get( KIAUpdateRoamingWarning, iRoamingWarning ) );
            item = new (ELeave) CAknBinaryPopupSettingItem
                               ( aSettingId, iRoamingWarning );
            break;
            }

        default:
            {
            item = new (ELeave) CAknSettingItem( aSettingId );
            break;
            }
        }
    CleanupStack::PopAndDestroy( cenrep ); 
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateSettingItemList::CreateSettingItemL end");
    return item;
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingItemList::LoadSettingsL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingItemList::LoadSettingsL()
    {
    CAknSettingItemList::LoadSettingsL();
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingItemList::EditItemL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingItemList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
    }

// -----------------------------------------------------------------------------
// CIAUpdateSettingItemList::SetAttribute
// 
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CIAUpdateSettingItemList::Attribute
// 
// -----------------------------------------------------------------------------
//
TInt CIAUpdateSettingItemList::Attribute( TInt aKey )
    {
    TInt ret = KErrNotFound;
    
    switch ( aKey )
        {
        case CIAUpdateSettingDialog::EAccessPoint:
            {
            ret = iAccessPoint;
            break;
            }
        case CIAUpdateSettingDialog::EAutoUpdateCheck:
            {
            ret = iAutoUpdateCheck;
            break;
            }
        case CIAUpdateSettingDialog::ERoamingWarning:
            {
            ret = iRoamingWarning;
            break;
            }
        default:
            {
            IAUpdateDialogUtil::Panic( KErrNotSupported );
            break;
            }
        }
        
    return ret;
    }


// -----------------------------------------------------------------------------
// CIAUpdateSettingItemList::CheckSettings
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateSettingItemList::CheckSettings()
    {
    if ( iAutoUpdateCheck != EIAUpdateSettingValueDisable &&
         iAutoUpdateCheck != EIAUpdateSettingValueDisableWhenRoaming &&
         iAutoUpdateCheck != EIAUpdateSettingValueEnable )
        {
        iAutoUpdateCheck = EIAUpdateSettingValueEnable;
        }

    if ( iRoamingWarning != EIAUpdateSettingValueOff &&
         iRoamingWarning != EIAUpdateSettingValueOn )
        {
        iRoamingWarning = EIAUpdateSettingValueOn;
        }
    }



/******************************************************************************
 * class CIAUpdateAccessPointSettingItem
 ******************************************************************************/

// -----------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::NewL
//
// -----------------------------------------------------------------------------
//
CIAUpdateAccessPointSettingItem* CIAUpdateAccessPointSettingItem::NewL(
                   TInt aSettingId, TInt& aAccessPointId )
    {
    CIAUpdateAccessPointSettingItem* item = 
    new (ELeave) CIAUpdateAccessPointSettingItem( aSettingId, aAccessPointId );
    CleanupStack::PushL(item);
    item->ConstructL();
    CleanupStack::Pop(item);
    return item;
	}

// -----------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::CIAUpdateAccessPointSettingItem
//
// -----------------------------------------------------------------------------
//
CIAUpdateAccessPointSettingItem::CIAUpdateAccessPointSettingItem( 
                                 TInt aSettingId, TInt& aAccessPointId )
:   CAknSettingItem( aSettingId ), 
    iAccessPointId ( aAccessPointId )
    {
    }


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::~CIAUpdateAccessPointSettingItem
// 
// -----------------------------------------------------------------------------
//
CIAUpdateAccessPointSettingItem::~CIAUpdateAccessPointSettingItem()
    {
    delete iSettingText;
    delete iApHandler;
    }


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIAUpdateAccessPointSettingItem::ConstructL()
    {
    iApHandler = CIAUpdateAccessPointHandler::NewL();
     
    if ( iAccessPointId == 0 )
        {
    	iApHandler->GetDefaultConnectionLabelL( iSettingText );
        }
    else
        {
        TRAPD( err, iApHandler->GetApNameL( iAccessPointId, iSettingText ) );
        if ( err == KErrNotFound )
            { // destination stored by IAD may be removed
            iAccessPointId = 0;
            iApHandler->GetDefaultConnectionLabelL( iSettingText );
            }
        else
            {
            User::LeaveIfError( err );
            }
        }
    } 


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::EditItemL
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateAccessPointSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    EditAccessPointItemL();
    }


// -----------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::SettingTextL
// 
// -----------------------------------------------------------------------------
//
const TDesC& CIAUpdateAccessPointSettingItem::SettingTextL()
    {
    if ( !iSettingText )
        {
    	return CAknSettingItem::SettingTextL();
        }
    else if ( iSettingText->Length() == 0 )
        {
        return CAknSettingItem::SettingTextL();
        }
        
    return *iSettingText;
    }


//------------------------------------------------------------------------------
// CIAUpdateAccessPointSettingItem::EditAccessPointItemL
//
//------------------------------------------------------------------------------
//	
TBool CIAUpdateAccessPointSettingItem::EditAccessPointItemL()
    {
	TInt itemUid = iAccessPointId;
	
	TInt ret = KErrNone;
	
	delete iSettingText;
	iSettingText = NULL;
	TRAPD( err, ret = iApHandler->ShowApSelectDialogL( itemUid, iSettingText ) );
	
	if ( err != KErrNone )
	    {
	    iAccessPointId = KErrNotFound;
	    UpdateListBoxTextL(); // from CAknSettingItem
	    return EFalse;
	    }
	
	if ( ret == CIAUpdateAccessPointHandler::EDialogSelect )
		{
		iAccessPointId = itemUid;
 	    UpdateListBoxTextL(); // from CAknSettingItem
	    return ETrue;
		}
		
	return EFalse;
    }

	
//  End of File  
