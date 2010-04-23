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
* Description:   This module contains the implementation of CIAUpdateUiConfigData
*                class member functions.
*
*/



#include <e32std.h>
#include <centralrepository.h>

#include "iaupdateuiconfigdata.h"
#include "iaupdateprivatecrkeys.h"
#include "iaupdatedebug.h"

CIAUpdateUiConfigData* CIAUpdateUiConfigData::NewL()
    {
    CIAUpdateUiConfigData* self =
        CIAUpdateUiConfigData::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CIAUpdateUiConfigData* CIAUpdateUiConfigData::NewLC()
    {
    CIAUpdateUiConfigData* self =
        new( ELeave ) CIAUpdateUiConfigData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CIAUpdateUiConfigData::CIAUpdateUiConfigData()
: CBase()
    {
    }


void CIAUpdateUiConfigData::ConstructL()
    {
    IAUPDATE_TRACE("[IAUPDATE]  CIAUpdateUiConfigData::ConstructL begin");
    CRepository* cenrep = CRepository::NewL( KCRUidIAUpdateSettings ); 
    CleanupStack::PushL( cenrep );
    User::LeaveIfError( cenrep->Get( KIAUpdateGridRefreshDeltaHours , iGridRefreshDeltaHours ) );
    User::LeaveIfError( cenrep->Get( KIAUpdateQueryHistoryDelayHours, iQueryHistoryDelayHours ) );
    IAUPDATE_TRACE("[IAUPDATE]  CIAUpdateUiConfigData::ConstructL 3");
    CleanupStack::PopAndDestroy( cenrep );
    }


CIAUpdateUiConfigData::~CIAUpdateUiConfigData()  
    {
    }

TInt CIAUpdateUiConfigData::GridRefreshDeltaHours() const
    {
    return iGridRefreshDeltaHours;
    }

TInt CIAUpdateUiConfigData::QueryHistoryDelayHours() const
    {
    return iQueryHistoryDelayHours;
    }
