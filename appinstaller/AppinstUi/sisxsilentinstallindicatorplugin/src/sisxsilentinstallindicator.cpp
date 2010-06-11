/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hb/hbcore/hbtranslator.h>
#include <hbicon.h>
#include "sisxsilentinstallindicator.h" 

const char KSifUiDefaultApplicationIcon[] = "qtg_large_application.svg";

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicator::SisxSilentInstallIndicator
// @see sisxsilentinstallindicator.h
// ----------------------------------------------------------------------------
SisxSilentInstallIndicator::SisxSilentInstallIndicator( 
	const QString &indicatorType) :
    	HbIndicatorInterface( indicatorType,    				
    		HbIndicatorInterface::NotificationCategory,
    		InteractionActivated),
    	mUpdateValue(0),
    	mIsInstallProcess(1) // Set installer mode as default.
    {
    }

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicator::~SisxSilentInstallIndicator
// @see sisxsilentinstallindicator.h
// ----------------------------------------------------------------------------
SisxSilentInstallIndicator::~SisxSilentInstallIndicator()
    {
    }

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicator::handleInteraction
// @see sisxsilentinstallindicator.h
// ----------------------------------------------------------------------------
bool SisxSilentInstallIndicator::handleInteraction(InteractionType type)
    {
    bool handled = false;
    
    if (type == InteractionActivated) 
        {       
        handled = true;           
        emit deactivate(); 
        }
    
    return handled;
    }

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicator::indicatorData
// @see sisxsilentinstallindicator.h
// ----------------------------------------------------------------------------
QVariant SisxSilentInstallIndicator::indicatorData(int role) const
{       
switch(role)
    {
    case PrimaryTextRole: 
        {
        // Set text to first line of indicator.
        QString text("");
        // Check which mode is on.
        if ( mIsInstallProcess  )
            { 
            text.append(QString("Installing"));            
            }       
        else
            {
            text.append(QString("Finalizing installations"));            
            }     
        return text;        
        }
    case SecondaryTextRole:
        {
        // Set text to second line of indicator.
        QString text("");        
        text.append(QString("%1 %").arg(mUpdateValue));      
        return text; 
        }    
    case DecorationNameRole:
    case MonoDecorationNameRole:
        {
        // Get icon for the indicator.
        QString iconName(KSifUiDefaultApplicationIcon);
        return iconName;
        }
    default: 
        return QVariant();      
    }
}

// ----------------------------------------------------------------------------
// SisxSilentInstallIndicator::prepareDisplayName
// @see sisxsilentinstallindicator.h
// ----------------------------------------------------------------------------
bool SisxSilentInstallIndicator::handleClientRequest( RequestType type, 
                                                      const QVariant &parameter)
    {
    bool handled(false);
    
    switch (type) 
        {
        case RequestActivate:
            {
            // Read client percent value to float.
            mUpdateValue = parameter.toInt();
            
            // If client send -1 insted of percent value (0-100) we need
            // to switch to uninstaller mode.
            if (mUpdateValue == -1)
                {
                mIsInstallProcess = false;
                mUpdateValue = 0;
                }           
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

// EOF
