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
* Description:   ?Description
*
*/


#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <e32property.h>
#include <e32cmn.h>
#include <apgtask.h>
#include <W32STD.H>


#include <catalogslogger.rsg>
#include "catalogslogger.pan"
#include "catalogsloggerappui.h"
#include "catalogsloggerappview.h"
#include "catalogslogger.hrh"
#include "catalogsloggermsgqueueobserver.h"
#include "catalogsdebugdefs.h"
#include "osmdelay.h"

const TInt KTimerDelay = 1*1000000; // 1 s
const TInt KStarupActionDelay = 500000; // 500 ms

static TInt TimerCallBack( TAny* aAppUi )
    {
    reinterpret_cast<CCatalogsLoggerAppUi*>( aAppUi )->TimerEvent();
    return 0;
    }
static TInt StartupActionCallBack( TAny* aAppUi )
    {
    reinterpret_cast<CCatalogsLoggerAppUi*>( aAppUi )->StartupTimerEvent();
    return 0;
    }

// ConstructL is called by the application framework
void CCatalogsLoggerAppUi::ConstructL()
    {
    BaseConstructL();
    iDelay = COsmDelay::NewL( TCallBack( TimerCallBack, this ) );    
    iStartupActionDelay = COsmDelay::NewL( TCallBack( StartupActionCallBack, this ) ); 
    
    iAppView = CCatalogsLoggerAppView::NewL( ClientRect() );    

    AddToStackL( iAppView );
    
    // The logger message handler
    iMsgObserver = CCatalogsLoggerMsgQueueObserver::NewL();
    iMsgObserver->SetEnableFlags( 
        ECatalogsDebugFlagEnableError |
        ECatalogsDebugFlagEnableWarning |
        ECatalogsDebugFlagEnableTrace |
        ECatalogsDebugFlagEnableInfo );
    iMsgObserver->StartLogging();
    
    iAppView->SetLoggerStatus( ETrue );
    
    iDelay->After( KTimerDelay );
    iStartupActionDelay->After( KStarupActionDelay );
    }

CCatalogsLoggerAppUi::CCatalogsLoggerAppUi()                              
    {
	// no implementation required
    }

CCatalogsLoggerAppUi::~CCatalogsLoggerAppUi()
    {
    delete iDelay;
    delete iStartupActionDelay;
    if (iAppView)
        {
        RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;

        delete iMsgObserver;
        iMsgObserver = NULL;
        }
    }

// handle any menu commands
void CCatalogsLoggerAppUi::HandleCommandL(TInt aCommand)
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case ECatalogsLoggerCommandStart:
            {
            iMsgObserver->StartLogging();
            iAppView->SetLoggerStatus( ETrue );
            
            }
            break;

        case ECatalogsLoggerCommandStop:
            {
            iMsgObserver->StopLogging();
            iAppView->SetLoggerStatus( EFalse );
            }
            break;
                
        case ECatalogsLoggerCommandSendFile:
            {
            iMsgObserver->SendFileL();
            }
            break;
        case ECatalogsLoggerCommandClearLog:
            {
            TInt err = iMsgObserver->ClearLog();

            if ( err != KErrNone )
                {
                // Show error note
                }
            }
            break;

#ifndef __SERIES60_30__
        // handle events from Active applications-menu option
        case EAknCmdTaskSwapper:
            {
            // do nothing here
            break;
            }
#endif
        case ECatalogsLoggerCommandBackground:
        	{
        	TApaTask task(iEikonEnv->WsSession( ));
        	task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
        	task.SendToBackground();        	
        	}
        	break;
        default:
            Panic( ECatalogsLoggerBasicUi );
            break;
        }
    }

void CCatalogsLoggerAppUi::TimerEvent()
    {
    iAppView->SetLoggerNumber( iMsgObserver->FunCounter(),
                               iMsgObserver->CurrentChunkOffset() );
    iAppView->SetLoggerDrive( iMsgObserver->LogDrive() );
    iAppView->SetData( iMsgObserver->LastData( 1024 ) );
    iDelay->After( KTimerDelay );
    }

void CCatalogsLoggerAppUi::StartupTimerEvent()
    {
    // After starup, send application directly to background.
    // This removes the need to do that manually.
    TRAP_IGNORE( HandleCommandL( ECatalogsLoggerCommandBackground ); );
    }

void CCatalogsLoggerAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane)
    {
    if( aResourceId == R_CATALOGSLOGGER_MENU )
        {
        if( iMsgObserver->IsLogging() )
            {
            // Running already, disable start.
            aMenuPane->SetItemDimmed( ECatalogsLoggerCommandStart, ETrue );
            }
        else
            {
            // not running, disable stop
            aMenuPane->SetItemDimmed( ECatalogsLoggerCommandStop, ETrue );
            }
        }
    }
