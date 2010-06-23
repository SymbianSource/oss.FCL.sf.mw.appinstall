/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <coemain.h>
#include <aknlists.h> 
#include <barsread.h> 
#include <e32property.h>
#include <aknutils.h>
#include <catalogslogger.rsg>

#include "catalogsloggerappview.h"
#include "catalogsdebugdefs.h"

CAknSettingItem* CCatalogsLoggerSettingList::CreateSettingItemL( TInt aIdentifier )
    {

    // Set/clear the setting value based on the corresponding bit
    // on the setting property

    TInt settingValue;
//    RProperty::Get( KCatalogsDebugPropertyCat, 
//                    ECatalogsLoggerSettingsProperty, 
//                    settingValue );
    settingValue = 0;
    
    iFlags[aIdentifier] = settingValue & ( 1 << aIdentifier );
    
    // Create the setting item
    CAknSettingItem* settingItem = NULL;
    settingItem = new (ELeave) 
                    CAknBinaryPopupSettingItem( aIdentifier, iFlags[aIdentifier] );
        
    return settingItem;
    }

void CCatalogsLoggerSettingList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    // Store the value
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
    ( *SettingItemArray() )[aIndex]->StoreL();

    // Read the current settings values from the property
    TInt settingBits;
//    RProperty::Get( KCatalogsDebugPropertyCat, 
//                    ECatalogsLoggerSettingsProperty, 
//                    settingBits );
    settingBits = 0;
    
    // Set/clear the bit corresoponding this setting value
    TInt settingId = ( *SettingItemArray())[aIndex]->Identifier();
    if( iFlags[settingId] )
        {
        settingBits |= ( 1 << settingId );
        }
    else
        {
        settingBits &= ~( 1 << settingId );
        }

    /*
    TInt err = RProperty::Set( KCatalogsDebugPropertyCat, 
                               ECatalogsLoggerSettingsProperty, 
                               settingBits );
    User::LeaveIfError( err );
    */
}

CCatalogsLoggerAppView* CCatalogsLoggerAppView::NewL(const TRect& aRect)
    {
    CCatalogsLoggerAppView* self = CCatalogsLoggerAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

CCatalogsLoggerAppView* CCatalogsLoggerAppView::NewLC(const TRect& aRect)
    {
    CCatalogsLoggerAppView* self = new (ELeave) CCatalogsLoggerAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

CCatalogsLoggerAppView::CCatalogsLoggerAppView()
    : iSettingsView( NULL )
    {
    }

CCatalogsLoggerAppView::~CCatalogsLoggerAppView()
    {
    delete iSettingsView;
    iSettingsView = NULL;
    delete iData;
    }

void CCatalogsLoggerAppView::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    SetRect(aRect);

    // Create settings view
//     iSettingsView = new( ELeave ) CCatalogsLoggerSettingList();
//     iSettingsView->SetContainerWindowL( *this );
//     iSettingsView->ConstructFromResourceL( R_CATALOGS_LOGGER_SETTING_ITEM_LIST );
//     iSettingsView->ActivateL();
    
    ActivateL();
    }

void CCatalogsLoggerAppView::Draw(const TRect& /*aRect*/) const
    {
    // Clear the screen, the list will draw itself after this
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect( Rect());
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbWhite );
    gc.SetPenColor( KRgbBlack );

    gc.Clear( drawRect );

    // Print some info

    const CFont* font = LatinBold19();
    gc.UseFont( font );

    TInt yOffset = Rect().iTl.iY + font->HeightInPixels();

    if( iLoggerStatus )
        {
        gc.DrawText( _L("Logging on"), TPoint( Rect().iTl.iX+2, yOffset ) );
        }
    else
        {
        gc.DrawText( _L("Logging off"), TPoint( Rect().iTl.iX+2, yOffset ) );
        }
    yOffset += font->HeightInPixels();
    TBuf<100> line;
    line.Copy(_L("Chunks: "));
    line.AppendNum( iLoggerNumber );
    line.Append(_L("  Offset: "));
    line.AppendNum( iLoggerOffset>>10 );
    gc.DrawText( line, TPoint( Rect().iTl.iX+2, yOffset ) );    yOffset += font->HeightInPixels();

    line.Copy(_L("Log drive: "));
    line.Append( iLoggerDrive == EDriveC ? 'C' : 'E' );

    TVolumeInfo info;
    iEikonEnv->FsSession().Volume( info, iLoggerDrive );
    line.Append(_L("   Free space: ")); 
    line.AppendNum( info.iFree/1024/1024 ); // megabytes

    gc.DrawText( line, TPoint( Rect().iTl.iX+2, yOffset ) );    yOffset += font->HeightInPixels();

    // Print some l0gzor

    font = LatinPlain12();
    gc.UseFont( font );

//     TLex18 lex;
//     if( iData && *iData != KNullDesC8 )
//         {
//         lex.Assign( *iData );
//         }
//     else
//         {
//         _LIT8(KHorror,"*not set*\n*not set*\n*not set*\n");
//         lex.Assign(KHorror);
//         }

    if( iData )
        {
        CArrayFixFlat<TPtrC>* lines = new(ELeave) CArrayFixFlat<TPtrC>( 32 );
        CleanupStack::PushL( lines );
        AknTextUtils::WrapToArrayL( iData->Des(), Rect().Width() - 6, *font, *lines );
    
        for( TInt i = 0; i < lines->Count(); i++ )
            {
            gc.DrawText( lines->At( i ), TPoint( Rect().iTl.iX+2, yOffset ) );
            yOffset += font->HeightInPixels();
            if( yOffset >= Rect().iBr.iY )
                {
                break;
                }
            }
        CleanupStack::PopAndDestroy( lines );
        }

//     lex.Mark();
//     while( 1 )
//         {
//         if( lex.Peek() == '\n' || lex.Eos() )
//             {
//             TPtrC8 marked = lex.MarkedToken();
//             HBufC* buffer = HBufC::NewLC( marked.Length() );
//             buffer->Des().Copy( marked );

//             CArrayFixFlat<TPtrC>* lines = new(ELeave) CArrayFixFlat<TPtrC>( 8 );
//             CleanupStack::PushL( lines );
//             AknTextUtils::WrapToArrayL( 
//                 buffer->Des(),
//                 Rect().Width() - 6,
//                 *font,
//                 *lines
//                 );

//             for( TInt i = 0; i < lines->Count(); i++ )
//                 {
//                 gc.DrawText( lines->At( i ), TPoint( Rect().iTl.iX+2, yOffset ) );
//                 yOffset += font->HeightInPixels();
//                 if( yOffset >= Rect().iBr.iY )
//                     {
//                     return;
//                     }
//                 }

//             CleanupStack::PopAndDestroy( lines );
//             CleanupStack::PopAndDestroy( buffer );
//             if( ! lex.Eos() )
//                 {
//                 lex.SkipAndMark(1);
//                 }
//             else
//                 {
//                 break;
//                 }
//             }
//         else
//             {
//             lex.Inc();
//             }
//         }
    }

TInt CCatalogsLoggerAppView::CountComponentControls() const
    {
    if( iSettingsView != NULL )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

CCoeControl* CCatalogsLoggerAppView::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iSettingsView;
        default:
            return NULL;
        }
    }

void CCatalogsLoggerAppView::SizeChanged()
    {         
    if ( iSettingsView )
        {
        iSettingsView->SetRect( Rect() );
        }
    }

TKeyResponse CCatalogsLoggerAppView::OfferKeyEventL( const TKeyEvent &aKeyEvent, TEventCode aType )
{
    if ( iSettingsView )
        {
        TKeyResponse response = iSettingsView->OfferKeyEventL( aKeyEvent, aType );

        DrawNow();
        return response;
        }
    return EKeyWasNotConsumed;
}

void CCatalogsLoggerAppView::SetLoggerStatus( TBool aStatus )
    {
    iLoggerStatus = aStatus;
    DrawDeferred();
    }
void CCatalogsLoggerAppView::SetLoggerNumber( TInt aNumber, TInt aOffset )
    {
    iLoggerNumber = aNumber;
    iLoggerOffset = aOffset;
    DrawDeferred();
    }

void CCatalogsLoggerAppView::SetLoggerDrive( TInt aDrive )
    {
    iLoggerDrive = aDrive;
    DrawDeferred();
    }

static void ReplaceChar( TPtr aDes, TChar aOldChar, TChar aNewChar )
    {
    for( TInt i = 0; i < aDes.Length(); ++i )
        {
        if ( aDes[ i ] == aOldChar )
            {
            aDes[ i ] = aNewChar;
            }
        }
    }

void CCatalogsLoggerAppView::SetData( TPtrC8 aData )
    {
    delete iData;
    iData = 0;
    iData = HBufC::NewL( aData.Length() );
    iData->Des().Copy( aData );
    ReplaceChar( iData->Des(), TChar('\t'), TChar(' ') );
    DrawDeferred();
    }

