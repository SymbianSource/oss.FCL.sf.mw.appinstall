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
* Description:   This module contains the implementation of 
*                CIAUpdateHistoryContainer class member functions.
*
*/




#include <coemain.h>
#include <aknlists.h> // For the listbox
#include <barsread.h> // TResourceReader
#include <avkon.mbg>
#include <AknIconArray.h> 
#include <gulicon.h>
#include <AknsUtils.h>
#include <StringLoader.h> 
#include <e32std.h>
#include <data_caging_path_literals.hrh>
#include <iaupdate.rsg>
#include <iaupdate.mbg>

#include "iaupdatehistorycontainer.h"
#include "iaupdatehistory.h"
#include "iaupdatehistoryitem.h"
#include "iaupdateutils.h"


// MACROS
_LIT(KIAUpdateIconFileName, "IAUpdate.mif");
// Format for TTime
// See: C++ API reference » Date and Time Handling » Using TTime 
_LIT( KDateFormat, "%*D%*M%Y%/0%4%/1%5" );
_LIT( KTimeFormat, "%-B%:0%J%:1%T%+B" );
_LIT( KIconIndexFormat, "%d" );
_LIT( KTabulator, "\t" );

//CONSTANTS
// Item list granularity
const TInt KListGranularity( 8 );
// There will be two images. One for error and for success.
const TInt KListIconGranularity( 2 );
const TInt KTimeLength( 48 );
// Icon index length is always one because an index is either zero or one
const TInt iconIndexLength( 1 );

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateHistoryContainer* CIAUpdateHistoryContainer::NewL( const TRect& aRect )
    {
    CIAUpdateHistoryContainer* self = 
        CIAUpdateHistoryContainer::NewLC( aRect );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::NewLC
// 
// -----------------------------------------------------------------------------
//
CIAUpdateHistoryContainer* CIAUpdateHistoryContainer::NewLC( const TRect& aRect )
    {
    CIAUpdateHistoryContainer* self = 
        new (ELeave) CIAUpdateHistoryContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::CIAUpdateHistoryContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateHistoryContainer::CIAUpdateHistoryContainer() 
: CCoeControl()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::~CIAUpdateHistoryContainer
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateHistoryContainer::~CIAUpdateHistoryContainer()
    {
    delete iListBox;
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateHistoryContainer::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Create list box
    iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox();
    iListBox->SetContainerWindowL( *this );
    
    TResourceReader reader;
    // Inserts reader into the cleanupstack.
    iEikonEnv->CreateResourceReaderLC( reader, R_IAUPDATE_HISTORY_LIST );
    iListBox->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy();

    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    // Setup scroll bars
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->
        ScrollBarFrame()->
            SetScrollBarVisibilityL( 
                CEikScrollBarFrame::EOff, 
                CEikScrollBarFrame::EOff );    
    // Create item text array
    iItemTextArray = new( ELeave ) CDesCArrayFlat( KListGranularity );    
    iListBox->Model()->SetItemTextArray( iItemTextArray );
    // Ownership of the array is transferred.
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
        
    HBufC* text = StringLoader::LoadLC( R_IAUPDATE_NO_UPDATES_DONE );    
    iListBox->View()->SetListEmptyTextL( *text );
    CleanupStack::PopAndDestroy( text );

    // Create icon array
    
    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );

    CArrayPtr<CGulIcon>* iconArray = 
        new( ELeave ) CAknIconArray( KListIconGranularity );    
    CleanupStack::PushL( iconArray );
    
    HBufC* iconFilePath = HBufC::NewLC( KDC_APP_BITMAP_DIR().Length() + 
                                        KIAUpdateIconFileName().Length() );
    TPtr ptr = iconFilePath->Des();
    ptr.Append( KDC_APP_BITMAP_DIR );
    ptr.Append( KIAUpdateIconFileName );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
        
    // This method puts both aBitmap and aMask in the cleanup stack.
    AknsUtils::CreateIconLC(
        skin,
        KAknsIIDQgnPropCaleAccepted,
        bitmap, 
        mask, 
        *iconFilePath, 
        EMbmIaupdateQgn_prop_cale_accepted, 
        EMbmIaupdateQgn_prop_cale_accepted_mask );
    CGulIcon* icon = CGulIcon::NewLC();
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    iconArray->AppendL( icon );
    CleanupStack::Pop( icon );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );

    // This method puts both aBitmap and aMask in the cleanup stack.
    AknsUtils::CreateIconLC(
        skin,
        KAknsIIDQgnPropSwupdFailed,
        bitmap, 
        mask, 
        *iconFilePath, 
        EMbmIaupdateQgn_prop_swupd_failed, 
        EMbmIaupdateQgn_prop_swupd_failed_mask);
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

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::RefreshL
// 
// -----------------------------------------------------------------------------
//    
void CIAUpdateHistoryContainer::RefreshL( MIAUpdateHistory& aHistory )
    {
    iItemTextArray->Reset();
    
    MIAUpdateHistoryItem* item( NULL ); 
    const RPointerArray< MIAUpdateHistoryItem >& items =
        aHistory.Items();
    TInt iconIndex( 0 );
    for( TInt i = 0; i < items.Count(); ++i ) 
        {
        item = items[i];                
        iconIndex = 0;
        if ( item->LastOperationErrorCode() != KErrNone
             || item->StateL() != MIAUpdateHistoryItem::EInstalled )
            {
            // If there was an error in the last operation
            // or if the node has not been installed, 
            // then use the second icon that is for
            // error situations.
            iconIndex = 1;
            }
        HBufC* nodeStateDescription = NodeStateDescriptionLC( *item );
        TPtr ptr = nodeStateDescription->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
 
        HBufC* name = item->Name().AllocLC();
        TBuf<iconIndexLength> iconIndexBuf;
        iconIndexBuf.Format( KIconIndexFormat, iconIndex );

        HBufC* buffer = HBufC::NewLC( iconIndexBuf.Length() +
                                      KTabulator.iTypeLength + 
                                      name->Length() + 
                                      KTabulator.iTypeLength +
                                      nodeStateDescription->Length() );  

        buffer->Des() = iconIndexBuf;
        buffer->Des() += KTabulator();
        buffer->Des() += *name;
        buffer->Des() += KTabulator();
        buffer->Des() += *nodeStateDescription;        
         
        iItemTextArray->AppendL( *buffer );
                 
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PopAndDestroy( name );
        CleanupStack::PopAndDestroy( nodeStateDescription );
        }    
    
    if ( items.Count() > 0 )
        {
    	iListBox->
        ScrollBarFrame()->
            SetScrollBarVisibilityL( 
                CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto ); 
        }
    else
        {
        iListBox->
        ScrollBarFrame()->
            SetScrollBarVisibilityL( 
                CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOff );	
        }     
    iListBox->HandleItemAdditionL();        
    }


// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::OfferKeyEventL
// 
// -----------------------------------------------------------------------------
//
TKeyResponse CIAUpdateHistoryContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                        TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;    

    if( aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            // Up and down arrow actions are delegated to the listbox
            case EKeyUpArrow:
            case EKeyDownArrow:
                response = iListBox->OfferKeyEventL( aKeyEvent, aType );        
                break;
                
            case EKeyOK:
                // Nothing to do here
                break;
                
            default:
                // Nothing to do here
                break;
            }
        }

    return response;        
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::SizeChanged
// 
// -----------------------------------------------------------------------------
//
void CIAUpdateHistoryContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::CountComponentControls
// 
// -----------------------------------------------------------------------------
//
TInt CIAUpdateHistoryContainer::CountComponentControls() const
    {
    return 1;
    }

// -----------------------------------------------------------------------------
// CIAUpdateHistoryContainer::ComponentControl
// 
// -----------------------------------------------------------------------------
//
CCoeControl* CIAUpdateHistoryContainer::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control( NULL );

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
// CIAUpdateHistoryContainer::HandleResourceChange
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateHistoryContainer::HandleResourceChange( TInt aType )
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
// CIAUpdateHistoryContainer::NodeStateDescriptionLC
// 
// -----------------------------------------------------------------------------
//
HBufC* CIAUpdateHistoryContainer::NodeStateDescriptionLC( const MIAUpdateHistoryItem& aItem )
    {
    HBufC* stateDescription( NULL );

    MIAUpdateHistoryItem::TIAHistoryItemState state( aItem.StateL() );
    TInt errorCode( aItem.LastOperationErrorCode() );

    HBufC* timeDes = HBufC::NewLC( KTimeLength );
    TPtr tmpDes = timeDes->Des();
    
    // Get the current local time
    TTime currentTime( 0 );
    currentTime.HomeTime();

    TTime lastOperationTime = aItem.LastOperationTime();
    // Get the local time from the unviversal time.
    TLocale locale;
    lastOperationTime += locale.UniversalTimeOffset();
    
    if ( lastOperationTime - locale.UniversalTimeOffset() == 0 )
        {
        // Because the origianal universal operation time was zero, the given time
        // is not correct. Set the time string empty. So, we will not give wrong information.
        tmpDes.Copy( KNullDesC() );
        }
    else if ( currentTime.DayNoInYear() != lastOperationTime.DayNoInYear()
         || currentTime.DaysFrom( lastOperationTime ).Int() > 0 )
        {
        // Day has changed.
        // Just to be sure, other checking is done because year may have also changed.
        lastOperationTime.FormatL( tmpDes, KDateFormat() );       
        }
    else
        {
        // Same day
        lastOperationTime.FormatL( tmpDes, KTimeFormat() );       
        }      
    
    if ( errorCode == KErrNoMemory )
        {
        // Last operation could not be completed because we were out of memory.
        // Show this error text.
        stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_UPDATE_NO_MEMORY,
                                                 *timeDes ); 
        }
    else 
        {
        switch( state )
            {
            case MIAUpdateHistoryItem::EInstalled:
                if ( errorCode == KErrNone )
                    {
                    // Because item is installed successfully, everything has gone ok.
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_UPDATE_SUCCESSFULL,
                                                             *timeDes );                    
                    }
                else if ( errorCode == KErrCancel )
                    {
                    // For some reason, the state says that an item has been installed,
                    // but the error code is cancel. In this case, trust the error code.
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_INSTALL_CANCELLED,
                                                             *timeDes );
                    }
                else
                    {
                    // If everything goes ok, then the item should be installed.
                    // So, something has gone wrong because we are here. So,
                    // give the install failed message
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_INSTALL_FAILED,
                                                             *timeDes );
                    }                
                break;

            case MIAUpdateHistoryItem::EDownloaded:
                if ( errorCode == KErrCancel )
                    {
                    // Because item has been downloaded but the error code is cancel
                    // this means that the install has been cancelled
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_INSTALL_CANCELLED,
                                                             *timeDes );
                    }
                else
                    {
                    // If everything goes ok, then the item should be installed.
                    // So, something has gone wrong because we are here. So,
                    // give the install failed message
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_INSTALL_FAILED,
                                                             *timeDes );
                    }
                break;

            case MIAUpdateHistoryItem::EPurchased:
                if ( errorCode == KErrCancel )
                    {
                    // Because item has been purchased but the error code is cancel
                    // this means that the download has been cancelled
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_DOWNLOAD_CANCELLED,
                                                             *timeDes );
                    }
                else
                    {
                    // If everything goes ok, then the item should be installed.
                    // So, something has gone wrong because we are here. So,
                    // give the download failed message
                    stateDescription = StringLoader::LoadLC( R_IAUPDATE_TEXT_DOWNLOAD_FAILED,
                                                             *timeDes );
                    }
                break;
                                
            default:
                // We should never come here.
                stateDescription = KNullDesC().AllocLC();
                break;
            }        
        }
   
    CleanupStack::Pop( stateDescription );
    CleanupStack::PopAndDestroy( timeDes );
    CleanupStack::PushL( stateDescription );
         
    return stateDescription;
    }


