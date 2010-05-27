/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AppMngr2 Settings plug-in for GS (General Settings)
*
*/


#ifndef C_APPMNGR2GSSETTINGSPLUGIN_H
#define C_APPMNGR2GSSETTINGSPLUGIN_H

#include <gsplugininterface.h>          // CGSPluginInterface
#include <eiklbo.h>                     // MEikListBoxObserver
#include <ConeResLoader.h>              // RConeResourceLoader

class CAppMngr2GSSettingsContainer;

const TInt KAppMngr2GSSettingsUidValue = 0x101F9A11;
const TUid KAppMngr2GSSettingsUid  = { KAppMngr2GSSettingsUidValue };


class CAppMngr2GSSettingsPlugin : public CGSPluginInterface, public MEikListBoxObserver
    {
public:     // constructors and destructor
    static CAppMngr2GSSettingsPlugin* NewL( TAny* aInitParams );
    ~CAppMngr2GSSettingsPlugin();

public:     // from CGSPluginInterface
    void GetCaptionL( TDes& aCaption ) const;
    CGulIcon* CreateIconL( const TUid aIconType );
    TInt PluginProviderCategory() const;

public:     // from CAknView, via CGSPluginInterface
    TUid Id() const;
    void HandleCommandL( TInt aCommand );
    void HandleViewRectChange();
    void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane );

public:     // from MEikListBoxObserver
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

protected:  // from CAknView, via CGSPluginInterface
    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
    void DoDeactivate();
    void HandleForegroundEventL( TBool aForeground );

private:    // new functions
    CAppMngr2GSSettingsPlugin();
    void ConstructL();

    void HandleListBoxSelectionL();
    void ShowSettingPageL( TInt aSettingIndex );
    void SetPermissionL( TInt aSettingIndex, TInt aNewValueIndex, TPtrC16 aUrlText );
    TInt MapOCSPIndex( TInt aValue );

private:    // data
    CAppMngr2GSSettingsContainer* iContainer;
    RConeResourceLoader iResources;
    TVwsViewId iPrevViewId;
    };

#endif  // C_APPMNGR2GSSETTINGSPLUGIN_H

