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
* Description:   This module contains the implementation of CIAUpdateHistoryView 
*                class  member functions.
*
*/



#include <aknViewAppUi.h> 
#include <eikmenup.h> 
#include <eikspane.h> 
#include <akntitle.h> 
#include <akncontext.h>
#include <StringLoader.h> 
#include <iaupdate.rsg>

#include "iaupdatehistoryview.h"
#include "iaupdatehistorycontainer.h"
#include "iaupdatehistory.h"
#include "iaupdateappui.h"
#include "iaupdate.hrh"


// Standard construction sequence
CIAUpdateHistoryView* CIAUpdateHistoryView::NewL( MIAUpdateHistory& aHistory )
    {
    CIAUpdateHistoryView* self = 
        CIAUpdateHistoryView::NewLC( aHistory );
    CleanupStack::Pop(self);
    return self;
    }

CIAUpdateHistoryView* CIAUpdateHistoryView::NewLC( MIAUpdateHistory& aHistory )
    {
    CIAUpdateHistoryView* self = 
        new( ELeave ) CIAUpdateHistoryView( aHistory );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CIAUpdateHistoryView::CIAUpdateHistoryView( MIAUpdateHistory& aHistory ) 
: CAknView(),
  iHistory( aHistory )
    {
	// no implementation required
    }

CIAUpdateHistoryView::~CIAUpdateHistoryView()
    {
    delete iContainer;
    }

void CIAUpdateHistoryView::ConstructL()
    {
    // Initializes the view with the given resources
    BaseConstructL( R_IAUPDATE_HISTORY_VIEW );
    }

    
void  CIAUpdateHistoryView::RefreshL()
    {
    if( !iContainer )
        {    
        // Create the container if it does not already exist.
        // Also, add it to the app ui control stack 
        iContainer = CIAUpdateHistoryContainer::NewL( ClientRect() );
        AppUi()->AddToStackL( iContainer );
        }

    UpdateStatusPaneL();

    // Container does the actual refreshing by using the current history
    // information.
    // Also, refresh the history first, because it may have changed since
    // last time.
    History().RefreshL();
    iContainer->RefreshL( History() );
    }

    
TUid CIAUpdateHistoryView::Id() const
    {
    return TUid::Uid( EIAUpdateHistoryViewId );
    }


void CIAUpdateHistoryView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand ) 
        {
        // All the exit type of softkey commands will be interpret as
        // the exit from the history view.
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EAknSoftkeyClose:
            aCommand = EIAUpdateCmdHistoryViewExit; 
            break;
            
        default:
            break;
        }

    // Let the app ui handle all the commands
    AppUi()->HandleCommandL( aCommand );
    }
    
    
void  CIAUpdateHistoryView::DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
                                         TUid /*aCustomMessageId*/, 
                                         const TDesC8& /*aCustomMessage*/ )
    {
    // Refresh everything.
    RefreshL();
    }

void CIAUpdateHistoryView::DoDeactivate()
    {
    if ( iContainer )
        {
        // Remove container from the control stack and delete
        // the container.
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    if ( static_cast<CIAUpdateAppUi*>(AppUi())->ShowStatusDialogAgain() )
        {
    	TRAP_IGNORE( static_cast<CIAUpdateAppUi*>(AppUi())->ShowStatusDialogDeferredL() );
        }
    }


MIAUpdateHistory& CIAUpdateHistoryView::History() const
    {
    return iHistory;
    }


void CIAUpdateHistoryView::UpdateStatusPaneL()
    {
    // Also update title pane text
    // Gets a pointer to the status pane. 
    // Notice, that the ownership is not transferred here.
    CEikStatusPane* statusPane = 
                 static_cast< CAknAppUi* >( AppUi() )->StatusPane();
          
    // Check if title pane is in current layout.
    TBool isTitlePaneInLayout( 
        statusPane->
            PaneCapabilities( 
                TUid::Uid( EEikStatusPaneUidTitle ) ).
                    IsInCurrentLayout() );

    // Change title text if title pane is in the curent layout.
    if ( isTitlePaneInLayout )
        {
        // Fetch pointer to the title pane control. Notice, that the ownership is not
        // transferred here.
        CAknTitlePane* titlePane = 
            static_cast< CAknTitlePane* >( statusPane->ControlL(
                                           TUid::Uid( EEikStatusPaneUidTitle ) ) );
        HBufC* text = StringLoader::LoadLC( R_IAUPDATE_TEXT_TITLE_PANE_HISTORY );
        titlePane->SetTextL( *text );
        CleanupStack::PopAndDestroy( text );        
        }         
    }
    
