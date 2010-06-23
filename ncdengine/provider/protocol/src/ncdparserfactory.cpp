/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   NcdParserFactory implementation
*
*/


#include "ncdparserfactory.h"
#include "ncdparserimpl.h"

#include "catalogsdebug.h"

MNcdParser* NcdParserFactory::CreateParserL( 
    MNcdProtocolDefaultObserver* aDefaultObserver )
    {
    return CNcdParserImpl::NewL( aDefaultObserver );
    }

MNcdParser* NcdParserFactory::CreateParserLC( 
    MNcdProtocolDefaultObserver* aDefaultObserver )
    {
    MNcdParser* p = CNcdParserImpl::NewL( aDefaultObserver );
    CleanupDeletePushL( p );
    return p;
    }
