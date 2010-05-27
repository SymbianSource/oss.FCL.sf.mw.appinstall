/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of CAppMngr2WidgetInfoIterator
*
*/


#include "appmngr2widgetinfoiterator.h" // CAppMngr2WidgetInfoIterator
#include "appmngr2widgetappinfo.h"      // CAppMngr2WidgetAppInfo
#include <StringLoader.h>               // StringLoader
#include <SWInstCommonUI.rsg>           // R_SWCOMMON_DETAIL_VALUE_WIDGET


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2WidgetInfoIterator::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetInfoIterator* CAppMngr2WidgetInfoIterator::NewL(
        CAppMngr2InfoBase& aWidget, TAppMngr2InfoType aInfoType )
    {
    CAppMngr2WidgetInfoIterator* self =
            new ( ELeave ) CAppMngr2WidgetInfoIterator( aWidget, aInfoType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetInfoIterator::~CAppMngr2WidgetInfoIterator()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetInfoIterator::~CAppMngr2WidgetInfoIterator()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetInfoIterator::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetInfoIterator::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetInfoIterator::SetOtherFieldsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetInfoIterator::SetOtherFieldsL()
    {
    HBufC* type = NULL;
    type = StringLoader::LoadLC( R_SWCOMMON_DETAIL_VALUE_WIDGET );
    SetFieldL( R_SWCOMMON_DETAIL_TYPE , *type);
    CleanupStack::PopAndDestroy( type );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetInfoIterator::CAppMngr2WidgetInfoIterator()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetInfoIterator::CAppMngr2WidgetInfoIterator(
        CAppMngr2InfoBase& aWidget, TAppMngr2InfoType aInfoType ) :
            CAppMngr2InfoIterator( aWidget, aInfoType )
    {
    }

