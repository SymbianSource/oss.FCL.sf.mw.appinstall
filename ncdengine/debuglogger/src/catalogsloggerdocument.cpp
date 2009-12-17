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


#include "catalogsloggerappui.h"
#include "catalogsloggerdocument.h"

CCatalogsLoggerDocument* CCatalogsLoggerDocument::NewL(CEikApplication& aApp)
    {
    CCatalogsLoggerDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CCatalogsLoggerDocument* CCatalogsLoggerDocument::NewLC(CEikApplication& aApp)
    {
    CCatalogsLoggerDocument* self = new (ELeave) CCatalogsLoggerDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CCatalogsLoggerDocument::ConstructL()
    {
	// no implementation required
    }    

CCatalogsLoggerDocument::CCatalogsLoggerDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }

CCatalogsLoggerDocument::~CCatalogsLoggerDocument()
    {
	// no implementation required
    }

CEikAppUi* CCatalogsLoggerDocument::CreateAppUiL()
    {
    CEikAppUi* appUi = new (ELeave) CCatalogsLoggerAppUi;
    return appUi;
    }

