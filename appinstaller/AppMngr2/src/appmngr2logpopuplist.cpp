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
* Description:   Popup list used to display installation log
*
*/


#include "appmngr2logpopuplist.h"       // CAppMngr2LogPopupList


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2LogPopupList::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2LogPopupList* CAppMngr2LogPopupList::NewL(
            CEikListBox* aListBox,
            TInt aCbaResource,
            AknPopupLayouts::TAknPopupLayouts aType )
    {
    CAppMngr2LogPopupList* self = new (ELeave) CAppMngr2LogPopupList;
    CleanupStack::PushL( self );
    self->ConstructL( aListBox, aCbaResource, aType );  // calls base class
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogPopupList::~CAppMngr2LogPopupList()
// ---------------------------------------------------------------------------
//
CAppMngr2LogPopupList::~CAppMngr2LogPopupList()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2LogPopupList::HandlePointerEventL()
// ---------------------------------------------------------------------------
//
void CAppMngr2LogPopupList::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if( Rect().Contains( aPointerEvent.iPosition ) )
            {
            // Process pointer event within dialog normally
            CAknPopupList::HandlePointerEventL( aPointerEvent );
            }
        else
            {
            // Pointer events that happen outside the dialog area close
            // CAknPopupList, hence they are passed to CEikBorderedControl.
            // Scrollbar drag, and kinetic scrolling need these events.
            CEikBorderedControl::HandlePointerEventL( aPointerEvent );
            }
        }
    }

