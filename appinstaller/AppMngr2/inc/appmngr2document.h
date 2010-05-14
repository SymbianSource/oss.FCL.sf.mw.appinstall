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
* Description:   Declares document for application
*
*/


#ifndef C_APPMNGR2DOCUMENT_H
#define C_APPMNGR2DOCUMENT_H

#include <AknDoc.h>                     // CAknDocument

class CEikAppUi;


class CAppMngr2Document : public CAknDocument
    {
public:     // constructor and destructor
    static CAppMngr2Document* NewL( CEikApplication& aApp );
    ~CAppMngr2Document();

private:    // new functions
    CAppMngr2Document( CEikApplication& aApp );
    void ConstructL();

private:    // from CEikDocument
    CEikAppUi* CreateAppUiL();
    };

#endif  // C_APPMNGR2DOCUMENT_H

