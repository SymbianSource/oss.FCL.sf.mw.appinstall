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
* Description:  
*
*/


#include "catalogsloggerdocument.h"
#include "catalogsloggerapplication.h"

static const TUid KUidCatalogsLoggerApp = {0x0B5C26D8};

CApaDocument* CCatalogsLoggerApplication::CreateDocumentL()
    {  
    CApaDocument* document = CCatalogsLoggerDocument::NewL( *this );
    return document;
    }

TUid CCatalogsLoggerApplication::AppDllUid() const
    {
    return KUidCatalogsLoggerApp;
    }

