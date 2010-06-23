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
* Description:   CLangName class contains language id amd name
*
*/


#include "SisxUILangName.h"             // CLangName
#include <barsread.h>                   // TResourceReader


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CLangName::NewL()
// ---------------------------------------------------------------------------
//
CLangName* CLangName::NewL( TResourceReader& aReader )
    {
    CLangName* self = new (ELeave) CLangName;
    CleanupStack::PushL( self );
    self->ConstructFromResourceL( aReader );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CLangName::~CLangName()
// ---------------------------------------------------------------------------
//
CLangName::~CLangName()
    {
    delete iName;
    }

// ---------------------------------------------------------------------------
// CLangName::Id()
// ---------------------------------------------------------------------------
//
TLanguage CLangName::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// CLangName::Name()
// ---------------------------------------------------------------------------
//
TPtrC CLangName::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// CLangName::CLangName()
// ---------------------------------------------------------------------------
//
CLangName::CLangName()
    {
    }

// ---------------------------------------------------------------------------
// CLangName::ConstructFromResourceL()
// ---------------------------------------------------------------------------
//
void CLangName::ConstructFromResourceL( TResourceReader& aReader )
    {
    iId = static_cast< TLanguage >( aReader.ReadInt16() );
    iName = aReader.ReadHBufC16L();
    }
