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
* Description:   ?description
*
*/


#ifndef C_CATALOGSLOGGERDOCUMENT_H
#define C_CATALOGSLOGGERDOCUMENT_H


#include <akndoc.h>

class CCatalogsLoggerAppUi;
class CEikApplication;

class CCatalogsLoggerDocument : public CAknDocument
    {
public:
    
    static CCatalogsLoggerDocument* NewL(CEikApplication& aApp);

    static CCatalogsLoggerDocument* NewLC(CEikApplication& aApp);
    
    ~CCatalogsLoggerDocument();

public: // from CAknDocument
    CEikAppUi* CreateAppUiL();

private:
    
    void ConstructL();

    CCatalogsLoggerDocument(CEikApplication& aApp);

    };


#endif // C_CATALOGSLOGGERDOCUMENT_H
