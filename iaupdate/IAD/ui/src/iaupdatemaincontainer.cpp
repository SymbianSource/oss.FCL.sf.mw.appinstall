/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateMainContainer class 
*                member functions.
*
*/



//INCLUDES
#include <barsread.h>
#include <coemain.h>
#include <aknlists.h>
#include <badesca.h> // CDesCArray
#include <iaupdate.rsg>
#include <avkon.mbg>
#include <iaupdate.mbg>
#include <AknIconArray.h> 
#include <gulicon.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <e32property.h>
#include <featmgr.h>

#include "iaupdatefwnode.h"
#include "iaupdatemaincontainer.h"
#include "iaupdatecontainerobserver.h"
#include "iaupdateapplication.h"
#include "iaupdatenode.h"
#include "iaupdatebasenode.h"
#include "iaupdateversion.h"
#include "iaupdatefwnsmlpskeys.h"

//MACROS
_LIT( KTabulator, "\t" );
_LIT( KOne, "1");
_LIT( KTwo, "2");
_LIT( KSpace, " ");

//CONSTANTS
const TInt KKiloByte = 1024;
const TInt KMegaByte = 1024 * 1024;
const TInt KMaxShownInKiloBytes = 10 * KMegaByte;

_LIT( KSWUPDATE_HLP_MAIN, "SWUPDATE_HLP_MAIN" ); 

// MACROS
_LIT(KIAUpdateIconFileName, "IAUpdate.mif");

//CONST

const TUid KFOTAUid = {0x102072C4};

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateMainContainer* CIAUpdateMainContainer::NewL( 
                                         const TRect& aRect, 
                                         MIAUpdateContainerObserver& aObserver )
    {
    CIAUpdateMainContainer* self = CIAUpdateMainContainer::NewLC(aRect,aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::NewLC
// 
// -----------------------------------------------------------------------------
//
CIAUpdateMainContainer* CIAUpdateMainContainer::NewLC( 
                                          const TRect& aRect, 
                                          MIAUpdateContainerObserver& aObserver )
    {
    CIAUpdateMainContainer* self = new (ELeave) CIAUpdateMainContainer(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }
// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::CIAUpdateMainContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateMainContainer::CIAUpdateMainContainer(MIAUpdateContainerObserver& aObserver)
    : iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::~CIAUpdateMainContainer
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateMainContainer::~CIAUpdateMainContainer()
    {
	delete iListBox;
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateMainContainer::ConstructL(const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Create list box
    iListBox = new (ELeave) CAknDoubleGraphicStyleListBox();
    iListBox->SetContainerWindowL( *this );
    
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, R_IAUPDATE_UPDATES_LIST );
    iListBox->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy();

    iListBox->EnableStretching( EFalse );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    // Setup scroll bars
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );    

    // Create item text array
    iItemTextArray = new (ELeave) CDesCArrayFlat(2);    
    iListBox->Model()->SetItemTextArray( iItemTextArray );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray) ;
    
    HBufC* text = KNullDesC().AllocLC();    
    iListBox->View()->SetListEmptyTextL( *text );
    CleanupStack::PopAndDestroy( text );
   
    // Create icon array
    
    CFbsBitmap* bitmap = 0;
    CFbsBitmap* mask = 0;

    CArrayPtr<CGulIcon>* iconArray = new (ELeave) CAknIconArray( 3 );    
    CleanupStack::PushL( iconArray );
    
    AknIconUtils::CreateIconLC(
        bitmap, 
        mask, 
        AknIconUtils::AvkonIconFileName(), 
        EMbmAvkonQgn_prop_checkbox_on, 
        EMbmAvkonQgn_prop_checkbox_on_mask );
    CGulIcon* icon = CGulIcon::NewLC();
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    iconArray->AppendL( icon );
    
    CleanupStack::Pop( icon );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );

    AknIconUtils::CreateIconLC(
        bitmap, 
        mask, 
        AknIconUtils::AvkonIconFileName(), 
        EMbmAvkonQgn_prop_checkbox_off, 
        EMbmAvkonQgn_prop_checkbox_off_mask );
    icon = CGulIcon::NewLC();
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    iconArray->AppendL( icon );
    
    CleanupStack::Pop( icon );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    
    HBufC* iconFilePath = HBufC::NewLC( KDC_APP_BITMAP_DIR().Length() + 
                                        KIAUpdateIconFileName().Length() +2 );
    TPtr ptr = iconFilePath->Des();
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KIAUpdateIconFileName );
          
    TRAPD( err, CreateIconL( bitmap, 
                             mask, 
                             *iconFilePath, 
                             EMbmIaupdateQgn_indi_important_add, 
                             EMbmIaupdateQgn_indi_important_add_mask ) ); 
    if ( err == KErrNone ) 
        {
        CleanupStack::PushL( bitmap );
        CleanupStack::PushL( mask );
        }
    else if ( err == KErrEof )
        {
        // icon does not exist in Z drive. I.e IAD eclipsed over phase 1 version.
        // So icon to be retrieved from C drive. 
        ptr.Zero();
    	ptr.Append( _L("C:") );
    	ptr.Append( KDC_APP_BITMAP_DIR );
        ptr.Append( KIAUpdateIconFileName );
        AknIconUtils::CreateIconLC(
                      bitmap, 
                      mask, 
                      *iconFilePath, 
                      EMbmIaupdateQgn_indi_important_add, 
                      EMbmIaupdateQgn_indi_important_add_mask );
        }
    else
        {
        User::Leave( err );	
        }

    icon = CGulIcon::NewLC();
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    iconArray->AppendL( icon );
    
    CleanupStack::Pop( icon );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    CleanupStack::PopAndDestroy( iconFilePath );
    
    CleanupStack::Pop( iconArray ); 
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray );

    iListBox->SetListBoxObserver( this );
    
    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::HandleMarkCommandL
// Handles Mark/Unmark commnds
// -----------------------------------------------------------------------------
//
void CIAUpdateMainContainer::HandleMarkCommandL( TInt aCommand )
    {
    TInt index = iListBox->CurrentItemIndex();
       
    TBool markingAccepted = EFalse;
    if( !iListBox->View()->ItemIsSelected( index ) )                                                                                             
        {
        // aCommand is set to EAknCmdMark if current item is not selected.
        // That's because passed command id may be wrong when using touch UI 
        aCommand = EAknCmdMark;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
        markingAccepted = iObserver.MarkListItemL( ETrue, index );
        }
    else 
        {
        // aCommand is set to EAknCmdUnmark if current item is selected.
        // That's because passed command id may be wrong when using touch UI    
        aCommand = EAknCmdUnmark;
        markingAccepted = iObserver.MarkListItemL( EFalse, index ); 
        }
        
	if ( markingAccepted )
	    {
        AknSelectionService::HandleMarkableListProcessCommandL( aCommand, iListBox );
	    }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::RefreshL
// Refresh list
// -----------------------------------------------------------------------------
//
void CIAUpdateMainContainer::RefreshL( const RPointerArray<MIAUpdateAnyNode>& aNodes,
                                       TInt aError )
    {
    iItemTextArray->Reset();
        
    for( TInt i = 0; i < aNodes.Count(); ++i ) 
        {
        MIAUpdateAnyNode* node = aNodes[i];
        TInt sizeInBytes = node->Base().ContentSizeL();

        TInt size = 0;
        TBool shownInMegabytes = EFalse;        
        if ( sizeInBytes >= KMaxShownInKiloBytes )
            {
    	    shownInMegabytes = ETrue;
    	    size = sizeInBytes / KMegaByte;
    	    if ( sizeInBytes % KMegaByte != 0 )
    	        {
    	    	size++;
    	        }
            }
        else 
            {
    	    size = sizeInBytes / KKiloByte;
    	    if ( sizeInBytes % KKiloByte != 0 )
    	        {
    	    	size++;
    	        }  
            }
            
        HBufC* importanceDescription( NULL );        
        switch( node->Base().Importance() )
        {        
        case MIAUpdateBaseNode::EMandatory:
            {
            if ( shownInMegabytes )
                {
                importanceDescription = StringLoader::LoadLC(
                                   R_IAUPDATE_DES_REQUIRED_UPDATE_MB, size );
                }
            else 
                {
                importanceDescription = StringLoader::LoadLC(
                                   R_IAUPDATE_DES_REQUIRED_UPDATE_KB, size );   
                }  
            break;
            }
        
        
        case MIAUpdateBaseNode::ECritical:
            {
            TBool isNSU = EFalse;
            if( node->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                {
                MIAUpdateFwNode* fwnode = static_cast<MIAUpdateFwNode*>( node );          
                if ( fwnode->FwType() == MIAUpdateFwNode::EFotiNsu )
                    {
                    isNSU = ETrue;
                    }
                }

            if ( isNSU )
                {
                //for NSU firmware 
                importanceDescription = StringLoader::LoadLC(
                                       R_IAUPDATE_UPDATE_WITH_PC );
                }
            else if ( size == 0 )
                {
                //for FOTA firmware when size info is not provided by server
                importanceDescription = StringLoader::LoadLC(
                                                       R_IAUPDATE_DES_CRITICAL_UPDATE_NO_SIZE );
                }
            else
                {
                if ( shownInMegabytes )
                    {
                    importanceDescription = StringLoader::LoadLC(
                                       R_IAUPDATE_DES_CRITICAL_UPDATE_MB, size );
                    }
                else 
                    {
                    importanceDescription = StringLoader::LoadLC(
                                       R_IAUPDATE_DES_CRITICAL_UPDATE_KB, size );   
                    } 
                }
 
            break;
            }
    
        case MIAUpdateBaseNode::ERecommended:
            {
            if ( shownInMegabytes )
                {
            	importanceDescription = StringLoader::LoadLC(
            	                   R_IAUPDATE_DES_RECOMMENDED_UPDATE_MB, size );
                }
            else 
                {
                importanceDescription = StringLoader::LoadLC(
            	                   R_IAUPDATE_DES_RECOMMENDED_UPDATE_KB, size );	
                }  
            break;
            }
    
        case MIAUpdateBaseNode::ENormal:
            {
            if ( shownInMegabytes )
                {
            	importanceDescription = StringLoader::LoadLC(
            	                   R_IAUPDATE_DES_NORMAL_UPDATE_MB, size );
                }
            else 
                {
                importanceDescription = StringLoader::LoadLC(
            	                   R_IAUPDATE_DES_NORMAL_UPDATE_KB, size );	
                }  
            break;
            }

        default:
            {
            importanceDescription = KNullDesC().AllocLC();
            break;
            }
        }
        
        TPtr ptr = importanceDescription->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );    
        
        
        
        if ( node->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )  
            {       
            HBufC* name = node->Base().Name().AllocLC();
            HBufC* buffer = HBufC::NewLC( KOne.iTypeLength +
                                          KTabulator.iTypeLength + 
                                          name->Length() + 
                                          KTabulator.iTypeLength +
                                          importanceDescription->Length() );
            buffer->Des() = KOne();
            buffer->Des() += KTabulator();
            buffer->Des() += *name;
            buffer->Des() += KTabulator();
            buffer->Des() += *importanceDescription;        
            
            iItemTextArray->AppendL( *buffer );
            
            CleanupStack::PopAndDestroy( buffer );
            CleanupStack::PopAndDestroy( name );
            }
        
        if ( node->NodeType() == MIAUpdateAnyNode::ENodeTypeFw ) 
            {
            HBufC* firmwareHeader = StringLoader::LoadLC(R_IAUPDATE_MAIN_DEVICE_FW);
            HBufC* firmwarename = HBufC::NewLC( node->Base().Name().Length() +
                                                KSpace.iTypeLength + 
                                                firmwareHeader->Length() );
            firmwarename->Des() = node->Base().Name();
            firmwarename->Des() += KSpace();
            firmwarename->Des() += *firmwareHeader;
                     
            HBufC* buffer = HBufC::NewLC( KOne.iTypeLength +
                                          KTabulator.iTypeLength + 
                                          firmwarename->Length() + 
                                          KTabulator.iTypeLength +
                                          importanceDescription->Length() +
                                          KTabulator.iTypeLength +
                                          KTwo.iTypeLength );
            buffer->Des() = KOne();
            buffer->Des() += KTabulator();
            buffer->Des() += *firmwarename;
            buffer->Des() += KTabulator();
            buffer->Des() += *importanceDescription;        
            buffer->Des() += KTabulator();
            buffer->Des() += KTwo();
            
            iItemTextArray->AppendL( *buffer );
            CleanupStack::PopAndDestroy( buffer );
            CleanupStack::PopAndDestroy( firmwarename );
            CleanupStack::PopAndDestroy( firmwareHeader );
            }
        
        CleanupStack::PopAndDestroy( importanceDescription );
        }    
    
    if ( aNodes.Count() == 0 )
        {
    	HBufC* emptyText = NULL;
        if ( aError )
            {
            if ( aError == KErrCancel || aError == KErrAbort )
                {
        	    emptyText = KNullDesC().AllocLC();
                }
            else
                {
                emptyText = StringLoader::LoadLC( R_IAUPDATE_REFRESH_FAILED );	
                }
    	    }
        else
            {
            emptyText = StringLoader::LoadLC( R_IAUPDATE_TEXT_NO_UPDATES );	
            }
        
        iListBox->View()->SetListEmptyTextL( *emptyText );
        CleanupStack::PopAndDestroy( emptyText );
        iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );  
        }
    else
        {
        if ( iListBox->CurrentItemIndex() == KErrNotFound )
            {
           	iListBox->SetCurrentItemIndex( aNodes.Count() - 1 );
            }
        }
        
    iListBox->HandleItemAdditionL();
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::GetCurrentNode
// 
// -----------------------------------------------------------------------------
//
MIAUpdateAnyNode* CIAUpdateMainContainer::GetCurrentNode( 
                                    const RPointerArray<MIAUpdateAnyNode>& aNodes )    
    {
    MIAUpdateAnyNode* currentNode = NULL;
    
    TInt currentIndex = iListBox->CurrentItemIndex();
    
    if( currentIndex != -1 )
        {
        
        currentNode = aNodes[currentIndex];
        
        }
    return currentNode;
    }


// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::SetSelectedIndicesL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateMainContainer::SetSelectedIndicesL( 
                            const RArray<TInt>& aIndices )
    {    
    CArrayFixFlat<TInt>* indexes = new(ELeave) CArrayFixFlat<TInt>( aIndices.Count() + 1 );
    
    CleanupStack::PushL( indexes );
    
    for( TInt i = 0; i < aIndices.Count(); ++i )
        {
        indexes->AppendL( aIndices[i] );
        }
        
    iListBox->View()->SetSelectionIndexesL( indexes );
    
    UpdateMSKTextL();  
    CleanupStack::PopAndDestroy( indexes );
    }


// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::CleanAllSelectionL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateMainContainer::CleanAllSelection()
    {
    iListBox->View()->ClearSelection();
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::OfferKeyEventL
// Some key events are passed to the list componenent 
// -----------------------------------------------------------------------------
//       
TKeyResponse CIAUpdateMainContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                     TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;    


    if( FeatureManager::FeatureSupported( KFeatureIdSyncMlDmFota ) ) //NFUI
        {
        if (aKeyEvent.iScanCode == EStdKeyDevice0 
                || aKeyEvent.iScanCode == EStdKeyDevice3 
                || aKeyEvent.iScanCode ==   EStdKeyHash )
            {
         
            TBool value (EFalse);
            TInt err = RProperty::Get( KPSUidNSmlDMSyncApp, KFotaDLStatus, value );
            if (!err && value == 1)
                {
    
                TApaTaskList taskList(CEikonEnv::Static()->WsSession());
                TApaTask task=taskList.FindApp( KFOTAUid );
           
                if(task.Exists())
                    {
                    task.BringToForeground();
                    }
                return EKeyWasConsumed;
                }
            }
        }
 
    if( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                {                    
                response = iListBox->OfferKeyEventL(aKeyEvent, aType); 
                UpdateMSKTextL();   
                break;    
                }
            default:
                {
                break;
                }
             
            }
        }
    return response;        
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::SizeChanged
//  
// -----------------------------------------------------------------------------
//  
void CIAUpdateMainContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::CountComponentControls
//  
// -----------------------------------------------------------------------------
//  
TInt CIAUpdateMainContainer::CountComponentControls() const
    {
    if ( iListBox )
        {
    	return 1;
        }
    else
        {
        return 0;	
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::ComponentControl
//  
// -----------------------------------------------------------------------------
// 
CCoeControl* CIAUpdateMainContainer::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = 0;

    switch( aIndex )
        {
        case 0:
            control = iListBox;
            break;
        default:
            break;
        }
    return control;
    }


// ---------------------------------------------------------------------------
// CIAUpdateMainContainer::GetHelpContext
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateMainContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidIAUpdateApp;
    aContext.iContext = KSWUPDATE_HLP_MAIN;    
    }

// ---------------------------------------------------------------------------
// CIAUpdateMainContainer::HandleResourceChange
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateMainContainer::HandleResourceChange( TInt aType )
    {
	CCoeControl::HandleResourceChange( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawNow();
		}
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::UpdateMSKTextL
//  
// -----------------------------------------------------------------------------
//        
void CIAUpdateMainContainer::UpdateMSKTextL()
	{
	TInt index = iListBox->CurrentItemIndex();
	if (index < 0 )
	    {
		iObserver.SetMiddleSKTextL( EFalse, EFalse );
	    }
	else if ( iListBox->View()->ItemIsSelected( index ) )
	    {
		iObserver.SetMiddleSKTextL( ETrue, ETrue );
	    }
	else
	    {
	    iObserver.SetMiddleSKTextL( ETrue, EFalse );	
	    }    
	}


// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::HandleListBoxEventL
//  
// -----------------------------------------------------------------------------
//  
void CIAUpdateMainContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                  TListBoxEvent aEventType )
    {

    switch  ( aEventType )
        {
        case EEventItemSingleClicked:
        case EEventEnterKeyPressed:
            {
            TInt index = iListBox->CurrentItemIndex();
            if( iListBox->View()->ItemIsSelected( index ) )
                {
                HandleMarkCommandL( EAknCmdUnmark );
                }
            else
                {
                HandleMarkCommandL( EAknCmdMark );
                }            
            break;
            }        
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainContainer::CreateIconL
//  
// -----------------------------------------------------------------------------
// 
void CIAUpdateMainContainer::CreateIconL( CFbsBitmap*& aBitmap,
                                          CFbsBitmap*& aMask,
                                          const TDesC& aFileName,
                                          TInt aBitmapId,
                                          TInt aMaskId )
    {
	AknIconUtils::CreateIconLC( aBitmap,
                                aMask, 
                                aFileName,  
                                aBitmapId,
                                aMaskId );
    CleanupStack::Pop( aMask );
    CleanupStack::Pop( aBitmap );
    }
