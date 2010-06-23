/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_APPMNGR2LOGPOPUPLIST_H
#define C_APPMNGR2LOGPOPUPLIST_H

#include <aknPopup.h>                   // CAknPopupList

/**
 *  Popup List for Installation Log
 *  CAppMngr2LogPopupList is actually a CAknPopupList that
 *  does not close when tapped outside the list area.
 */
class CAppMngr2LogPopupList : public CAknPopupList
    {
public:     // constructor and destructor
    static CAppMngr2LogPopupList* NewL( CEikListBox* aListBox,
            TInt aCbaResource, AknPopupLayouts::TAknPopupLayouts aType );
    ~CAppMngr2LogPopupList();

protected:  // from CAknPopupList
    /**
     * Overrides HandlePointerEventL function in base class.
     * Eats pointer events that would close the dialog.
     */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    };

#endif // C_APPMNGR2LOGPOPUPLIST_H

