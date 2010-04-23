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
* Description:   Creates info objects asynchronously
*
*/


#include "appmngr2infomaker.h"          // CAppMngr2InfoMaker
#include "appmngr2infomakerobserver.h"  // MAppMngr2InfoMakerObserver
#include <appmngr2runtime.h>            // CAppMngr2Runtime


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2InfoMaker::CAppMngr2InfoMaker()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoMaker::CAppMngr2InfoMaker( CAppMngr2Runtime& aPlugin,
        MAppMngr2InfoMakerObserver& aObserver, RFs& aFs ) :
        CActive( CActive::EPriorityStandard ), iPlugin( aPlugin ), iObserver( aObserver ),
        iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoMaker::~CAppMngr2InfoMaker()
// ---------------------------------------------------------------------------
//
CAppMngr2InfoMaker::~CAppMngr2InfoMaker()
    {
    // derived classes must call Cancel()
    }

// ---------------------------------------------------------------------------
// CAppMngr2InfoMaker::RuntimeUid()
// ---------------------------------------------------------------------------
//
const TUid CAppMngr2InfoMaker::RuntimeUid() const
    {
    return iPlugin.RuntimeUid();
    }

