/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AppMngr2 GS (General Settings) Settings plug-in Container
*
*/


#ifndef C_APPMNGR2GSSETTINGSCONTAINER_H
#define C_APPMNGR2GSSETTINGSCONTAINER_H

#include <coecntrl.h>                   // CCoeControl
#include <badesca.h>                    // CDesCArray
#include "appmngr2.hrh"                 // KAppMngr2AppUidValue

class CEikTextListBox;
class CAknSettingStyleListBox;

const TUid KAppMngr2AppUid = { KAppMngr2AppUidValue };

// Items in R_APPMNGR2_SETTING_ARRAY resource array
enum KAppMngr2SettingArrayItems
    {
    EAppMngr2SettingSwInstall = 0,
    EAppMngr2SettingOcsp = 1,
    EAppMngr2SettingOcspUrl = 2
    };

// Items in R_APPMNGR2_OCSP_CHECK_VALUE_ARRAY resource array
enum KAppMngr2OcspCheckValueArrayItems
    {
    EAppMngr2OcspCheckValueArrayMustPass = 0,
    EAppMngr2OcspCheckValueArrayOn = 1,
    EAppMngr2OcspCheckValueArrayOff = 2
    };


class CAppMngr2GSSettingsContainer : public CCoeControl
    {
public:     // constructors and destructor
    static CAppMngr2GSSettingsContainer* NewL( const TRect& aRect );
    ~CAppMngr2GSSettingsContainer();
    
public:     // new functions
    CAknSettingStyleListBox* ListBox() const;
    void UpdateListBoxContentL();
    
public:     // from CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private:    // new functions
    CAppMngr2GSSettingsContainer();
    void ConstructL( const TRect& aRect );
    void LoadIconsL();

private:    // from CCoeControl
    void SizeChanged();
    void FocusChanged( TDrawNow aDrawNow );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex ) const;
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    void HandleResourceChange( TInt aType );
    
private:    // data
    CDesCArray* iItems;
    CDesCArray* iOcspCheckValueArray;
    CDesCArray* iAllowUntrustedValueArray;
    CAknSettingStyleListBox* iListbox;
    };


#endif  // C_APPMNGR2GSSETTINGSCONTAINER_H

