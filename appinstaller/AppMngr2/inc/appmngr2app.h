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
* Description:   Declares main application class
*
*/


#ifndef C_APPMNGR2APP_H
#define C_APPMNGR2APP_H

#include <aknapp.h>                     // CAknApplication


class CAppMngr2App : public CAknApplication
    {
private:    // from CAknApplication
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
    };

#endif  // C_APPMNGR2APP_H

