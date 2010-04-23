/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <w32std.h>
#include <apgtask.h>
#include <apacmdln.h>
#include <xqservicerequest.h>

// #include "debugtraces.h"
#include "bgcindicator.h" 

//----------------------------------------------------------------------


// ----------------------------------------------------------------------------
// BgcIndicator::BgcIndicator
// @see bgcindicator.h
// ----------------------------------------------------------------------------
BgcIndicator::BgcIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::GroupPriorityHigh,
        InteractionActivated),
        mNrOfUpdates(0)
    {
    }

// ----------------------------------------------------------------------------
// BgcIndicator::~BgcIndicator
// @see bgcindicator.h
// ----------------------------------------------------------------------------
BgcIndicator::~BgcIndicator()
    {
    }

// ----------------------------------------------------------------------------
// BgcIndicator::handleInteraction
// @see bgcindicator.h
// ----------------------------------------------------------------------------
bool BgcIndicator::handleInteraction(InteractionType type)
    {
    bool handled = false;
    
    if (type == InteractionActivated) 
        {
        StartIaupdateL();
        handled = true;   
        
        emit deactivate(); 
        }
    return handled;
    }

// ----------------------------------------------------------------------------
// BgcIndicator::indicatorData
// @see bgcindicator.h
// ----------------------------------------------------------------------------
QVariant BgcIndicator::indicatorData(int role) const
{
    
switch(role)
    {
    case TextRole: 
        {
        QString text("");
        if ( mNrOfUpdates == 1 )
            text.append(QString("Updates available"));
        else
            text.append(QString("Check for updates?"));
        return text;        
        }
    case SecondaryTextRole:
        {
        QString text("Tap to view");
        return text; 
        }
    case IconNameRole:
        {
        // QString iconName("z:/resource/messaging/message.svg");
        QString iconName("c:/qgn_note_swupdate_notification.svg");
        return iconName;
        }
    default: 
        return QVariant();      
    }
}

// ----------------------------------------------------------------------------
// BgcIndicator::prepareDisplayName
// @see bgcindicator.h
// ----------------------------------------------------------------------------
bool BgcIndicator::handleClientRequest( RequestType type, 
        const QVariant &parameter)
    {
    bool handled(false);
    switch (type) {
        case RequestActivate:
            {
            mNrOfUpdates = parameter.toInt();
            handled =  true;
            }
            break;
        case RequestDeactivate:
            {
            emit deactivate();
            }
            break;
        default:
            break;
    }

    return handled;
    }

// ----------------------------------------------------------
// CIAUpdateBGTimer::StartIaupdateL()
// ----------------------------------------------------------
void BgcIndicator::StartIaupdateL() const
    {
    
    const TUint KIADUpdateLauncherUid( 0x2001FE2F );
    _LIT(KIAUpdateLauncherExe, "iaupdatelauncher.exe" );
    _LIT8( KRefreshFromNetworkDenied, "1" );
    
    //FLOG("[bgchecker] StartIaupdateL() begin");
    RWsSession ws;   
    User::LeaveIfError( ws.Connect() == KErrNone );
    TApaTaskList tasklist(ws);   
    TApaTask task = tasklist.FindApp( TUid::Uid( KIADUpdateLauncherUid ) );
    if ( task.Exists() )
        {
        //FLOG("[bgchecker] StartIaupdateL() IAD launcher process found");
        task.BringToForeground();
        //FLOG("[bgchecker] StartIaupdateL() after task.BringToForeground()");
        ws.Close();
        }
    else 
        {
        ws.Close();
        RProcess process;
        CleanupClosePushL( process );
        User::LeaveIfError( 
              process.Create( KIAUpdateLauncherExe, KNullDesC ) );
              
        CApaCommandLine* commandLine = CApaCommandLine::NewLC();
        commandLine->SetDocumentNameL( KNullDesC );
        commandLine->SetExecutableNameL( _L("iaupdatelauncher.exe") );
        commandLine->SetTailEndL( KRefreshFromNetworkDenied );
        commandLine->SetProcessEnvironmentL( process );
        CleanupStack::PopAndDestroy( commandLine );
       
        process.Resume();
        CleanupStack::PopAndDestroy( &process );    
        }
    //FLOG("[bgchecker] StartIaupdateL() end");
    }

