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
* Description:   AppMngr2 GS Installation Files plugin implementation
*
*/


#ifndef C_APPMNGR2GSINSTFILESPLUGIN_H
#define C_APPMNGR2GSINSTFILESPLUGIN_H

#include <gsplugininterface.h>          // CGSPluginInterface
#include <AknServerApp.h>               // MAknServerAppExitObserver
#include <ConeResLoader.h>              // RConeResourceLoader
#include <apadoc.h>

class CAknNullService;

const TInt KAppMngr2InstFilesUidValue = 0x10282D8D;
const TUid KAppMngr2InstFilesPluginUid = { KAppMngr2InstFilesUidValue };


/**
 * CAppMngrInstFilesPlugin launches embedded AppMngr2 application to show
 * Installation Files view. It is an EGSItemTypeSettingDialog type GS plug-in
 * and hence the GS framework calls HandleSelection() instead of DoActivate().
 * CAknView functionality is not supported although the base class is derived
 * from CAknView via CGSPluginInterface.
 */
class CAppMngr2GsInstFilesPlugin : public CGSPluginInterface,
                        public MAknServerAppExitObserver
    {
public:     // constructors and destructor
    static CAppMngr2GsInstFilesPlugin* NewL( TAny* aInitParams );
    ~CAppMngr2GsInstFilesPlugin();

public:     // from CGSPluginInterface
    void GetCaptionL( TDes& aCaption ) const;
    void HandleSelection( const TGSSelectionTypes aSelectionType );
    TGSListboxItemTypes ItemType();
    CGulIcon* CreateIconL( const TUid aIconType );
    TInt PluginProviderCategory() const;

public:     // from CAknView, via CGSPluginInterface
    TUid Id() const;

protected:  // from CAknView, via CGSPluginInterface
    void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
            const TDesC8& aCustomMessage );
    void DoDeactivate();

private:    // new functions
    CAppMngr2GsInstFilesPlugin();
    void ConstructL();
    void OpenLocalizedResourceFileL( const TDesC& aResourceFileName,
            RConeResourceLoader& aResourceLoader );
    void LaunchAppManagerL();
    void EmbedAppL( const TUid& aAppUid );

protected:  // data
    RConeResourceLoader iResources;
    CAknNullService* iNullService;
    CApaDocument* iEmbedded;
    };

#endif  // C_APPMNGR2GSINSTFILESPLUGIN_H

