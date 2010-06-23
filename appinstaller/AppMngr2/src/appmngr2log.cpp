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
* Description:   Shows installation log in a popup list dialog
*
*/


#include "appmngr2log.h"                // CAppMngr2Log
#include "appmngr2logpopuplist.h"       // CAppMngr2LogPopupList
#include "appmngr2logdatabase.h"        // CAppMngr2LogDatabase
#include "appmngr2logdatabaseentry.h"   // CAppMngr2LogDatabaseEntry
#include <appmngr2.rsg>                 // Resource IDs
#include <aknlists.h>                   // CAknDoublePopupMenuStyleListBox
#include <StringLoader.h>               // Stingloader

const TInt KMaxNumberOfShownLogItems = 50;
const TInt KInstDateTimeTextLength = 64;
const TInt KInstActionTextLength = 64;
const TInt KInstLogLineTextLen = KInstDateTimeTextLength +
    KInstActionTextLength + SwiUI::KMaxLogNameLength;

// Date and action separated by " - "
_LIT( KSeparator, " - " );
_LIT( KVersionPrefix, " (" );
_LIT( KVersionSuffix, ")" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2Log::CAppMngr2Log()
// ---------------------------------------------------------------------------
//
CAppMngr2Log::CAppMngr2Log( CEikonEnv& aEikEnv ) : iEikEnv( aEikEnv )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2Log::NewLC()
// ---------------------------------------------------------------------------
//
CAppMngr2Log* CAppMngr2Log::NewLC()
    {
    CAppMngr2Log* self = new (ELeave) CAppMngr2Log( *( CEikonEnv::Static() ) );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Log::~CAppMngr2Log()
// ---------------------------------------------------------------------------
//
CAppMngr2Log::~CAppMngr2Log()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2Log::ShowInstallationLogL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Log::ShowInstallationLogL()
    {    
    CEikFormattedCellListBox* listBox = new (ELeave) CAknDoublePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    // AppMngr version of Avkon popup list with heading
    CAppMngr2LogPopupList* popupList = CAppMngr2LogPopupList::NewL(
            listBox, R_AVKON_SOFTKEYS_OK_EMPTY__OK, AknPopupLayouts::EMenuDoubleWindow );
    CleanupStack::PushL( popupList );
    
    // Set title text
    HBufC* title = StringLoader::LoadLC( R_INST_LOG_HEADER );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    // Set listbox properties and create item array
    listBox->ConstructL( popupList, EAknListBoxViewerFlags );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto );
    MDesCArray* itemList = listBox->Model()->ItemTextArray();
    CDesCArray* itemArray = static_cast<CDesCArray*>( itemList );    
    ConstructLogArrayL( itemArray );

    // Set empty text
    CListBoxView* view = listBox->View();
    HBufC* emptyText = StringLoader::LoadLC( R_QTN_SWINS_LOG_EMPTY );
    view->SetListEmptyTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );
    
    // Lauch popup list
    popupList->ExecuteLD();

    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( listBox );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Log::ConstructLogArrayL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Log::ConstructLogArrayL( CDesCArray* aItemArray )
    {
    CAppMngr2LogDatabase* log = CAppMngr2LogDatabase::NewL();
    CleanupStack::PushL( log );

    const RPointerArray<CAppMngr2LogDatabaseEntry>& logEntries = log->Entries();
    
    // Read KMaxNumberOfShownLogItems first log entries, ignore the rest 
    TInt length = logEntries.Count();
    TInt diff = length - KMaxNumberOfShownLogItems;

    // Add entries from the latest log item (reverse order) into item array
    for( TInt index = length - 1; index >= diff && index >= 0; index-- )
        {
        if( logEntries[ index ] )
            {
            const CAppMngr2LogDatabaseEntry& entry = *( logEntries[ index ] );
    
            HBufC* buffer = HBufC::NewMaxLC( KInstLogLineTextLen );
            TPtr listboxLine = buffer->Des();
            listboxLine.Zero();
    
            // First row in double list item contains application name
            listboxLine.Append( entry.Name() );
            listboxLine.Append( KVersionPrefix );
            listboxLine.Append( entry.Version() );
            listboxLine.Append( KVersionSuffix );
            listboxLine.Append( KColumnListSeparator );
            
            // Second row contains date and action type (install/uninstall/partial)
            SetLogDateTimeColumnL( listboxLine, entry.Time() );
            listboxLine.Append( KSeparator );
            SetLogActionTextColumnL( listboxLine, entry.Action() );
    
            aItemArray->AppendL( listboxLine );
            CleanupStack::PopAndDestroy( buffer );
            }
        }
        
    CleanupStack::PopAndDestroy( log );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Log::SetLogActionTextColumnL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Log::SetLogActionTextColumnL( TDes& aListboxLine,
        const SwiUI::TLogTaskAction& aAction )
    {
    TBuf<KInstActionTextLength> action;
    switch( aAction )
        {
        case SwiUI::ELogTaskActionInstall:
            iEikEnv.ReadResourceL( action, R_INST_INSTALL_EVENT );
            break;
        case SwiUI::ELogTaskActionUninstall:
            iEikEnv.ReadResourceL( action, R_INST_REMOVE_EVENT );
            break;
        // no action for R_INST_PARTIAL_INSTALL_EVENT, not supported
        default:
            break;
        }
    aListboxLine.Append( action );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Log::SetLogDateTimeColumnL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Log::SetLogDateTimeColumnL( TDes& aListboxLine,
        const TTime& aDateTime )
    {
    TBuf<KInstDateTimeTextLength> tempBuf;
    TBuf<KInstDateTimeTextLength> dateFormat;
    iEikEnv.ReadResourceL( dateFormat, R_QTN_DATE_USUAL_WITH_ZERO );   
    aDateTime.FormatL( tempBuf, dateFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempBuf );
    aListboxLine.Append( tempBuf );
    }

