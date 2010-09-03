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

#include <hbicon.h>

#include "bgcindicator.h" 

const char KTranslatorFileName[] = "swupdate";
const char KIconName[] = "qtg_large_swupdate";

//----------------------------------------------------------------------


// ----------------------------------------------------------------------------
// BgcIndicator::BgcIndicator
// @see bgcindicator.h
// ----------------------------------------------------------------------------
BgcIndicator::BgcIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::NotificationCategory,
        InteractionActivated), 
        mNrOfUpdates(0),
        mTranslator(0)
    {
  
    HbTranslator* mTranslator = new HbTranslator(KTranslatorFileName);
    
    }

// ----------------------------------------------------------------------------
// BgcIndicator::~BgcIndicator
// @see bgcindicator.h
// ----------------------------------------------------------------------------
BgcIndicator::~BgcIndicator()
    {
    if (mTranslator) 
        {
        delete mTranslator;
        mTranslator = 0;
        }
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
        TRAPD( err, StartIaupdateL() );
        if ( err != KErrNone )
            {
            // nothing to do 
            }
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
    case PrimaryTextRole: 
        {
        QString text("");
        if ( mNrOfUpdates == 0 )
            {
            // First time case
            text.append(hbTrId("txt_software_dblist_update_checking"));
            }
        else if ( mNrOfUpdates == 1 )
            {
            // one update available
            text.append(hbTrId("txt_software_dblist_update_available"));
            }
        else
            {
            // several updates available
            text.append(hbTrId("txt_software_dblist_updates_available"));
            }
        return text;        
        }
    case SecondaryTextRole:
        {
        QString text("");
        if ( mNrOfUpdates == 0 )
            {
            // First time case
            text.append(hbTrId("txt_software_dblist_val_not_activated"));
            }
        else 
            {
            // update(s) available
            text.append(hbTrId("txt_software_dblist_val_ln_update", mNrOfUpdates));
            }

        return text; 
        }
    case DecorationNameRole:
    case MonoDecorationNameRole:
        {
        return QString(KIconName);
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
            emit dataChanged();
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
// BgcIndicator::StartIaupdateL()
// ----------------------------------------------------------
void BgcIndicator::StartIaupdateL() const
    {
    
    const TUint KIADUpdateLauncherUid( 0x2001FE2F );
    _LIT(KIAUpdateLauncherExe, "iaupdatelauncher.exe" );
    _LIT8( KRefreshFromNetworkDenied, "1" );
    
    RWsSession ws;   
    User::LeaveIfError( ws.Connect() == KErrNone );
    TApaTaskList tasklist(ws);   
    TApaTask task = tasklist.FindApp( TUid::Uid( KIADUpdateLauncherUid ) );
    if ( task.Exists() )
        {
        task.BringToForeground();
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
    }

