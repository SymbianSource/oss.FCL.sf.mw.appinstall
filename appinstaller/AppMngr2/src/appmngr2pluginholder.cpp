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
* Description:   Container class to hold Runtime plugin and related data
*
*/


#include "appmngr2pluginholder.h"       // CAppMngr2PluginHolder
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2debugutils.h>         // FLOG macros
#include <AknIconArray.h>               // CAknIconArray

const TInt KIconGranularity = 8;
const TInt KDataTypeGranularity = 8;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::CAppMngr2PluginHolder()
// ---------------------------------------------------------------------------
//
CAppMngr2PluginHolder::CAppMngr2PluginHolder( CAppMngr2Runtime* aRuntime )
    {
    iRuntime = aRuntime;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::~CAppMngr2PluginHolder()
// ---------------------------------------------------------------------------
//
CAppMngr2PluginHolder::~CAppMngr2PluginHolder()
    {
    delete iRuntime;
    delete iDataTypes;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::
// ---------------------------------------------------------------------------
//
CAppMngr2Runtime& CAppMngr2PluginHolder::Runtime()
    {
    return *iRuntime;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::
// ---------------------------------------------------------------------------
//
CDataTypeArray& CAppMngr2PluginHolder::DataTypes()
    {
    return *iDataTypes;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::
// ---------------------------------------------------------------------------
//
void CAppMngr2PluginHolder::LoadIconsL( CAknIconArray& aIconArray )
    {
    CAknIconArray* iconArray = new ( ELeave ) CAknIconArray( KIconGranularity );
    CleanupStack::PushL( iconArray );
    
    // Use temporary array to load icons from plugin. This prevents
    // plugin from messing icons that other plugins have loaded.
    TRAP_IGNORE( iRuntime->LoadIconsL( *iconArray ) );    
    iIconIndexBase = aIconArray.Count();
    iIconCount = iconArray->Count();
    
    // Insert icons in reverse order so that they can be removed
    // from the iconArray while inserting in aIconArray. Icons must
    // be inserted in the right position to maintain icon indexes
    // (the same icon order as in iconArray).
    TInt insertPosition = iIconIndexBase;
    for( TInt index = iIconCount - 1; index >= 0; index-- )
        {
        // copies CGulIcon from iconArray to aIconArray
        aIconArray.InsertL( insertPosition, iconArray->At( index ) );
        iconArray->Delete( index );
        }
    CleanupStack::PopAndDestroy( iconArray );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::
// ---------------------------------------------------------------------------
//
void CAppMngr2PluginHolder::FetchDataTypesL()
    {
    if( iDataTypes )
        {
        delete iDataTypes;
        iDataTypes = NULL;
        }
    iDataTypes = new (ELeave) CDataTypeArray( KDataTypeGranularity );
    iRuntime->GetSupportedDataTypesL( *iDataTypes );
    FLOG( "CAppMngr2PluginHolder::FetchDataTypesL: iDataTypes->Count() = %d",
            iDataTypes->Count() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::
// ---------------------------------------------------------------------------
//
TInt CAppMngr2PluginHolder::IconIndexBase()
    {
    return iIconIndexBase;
    }

// ---------------------------------------------------------------------------
// CAppMngr2PluginHolder::
// ---------------------------------------------------------------------------
//
TInt CAppMngr2PluginHolder::IconIndexMax()
    {
    return iIconIndexBase + iIconCount;
    }
