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


#ifndef C_CATALOGSLOGGERAPPLICATION_H
#define C_CATALOGSLOGGERAPPLICATION_H

#include <aknapp.h>

class CCatalogsLoggerApplication : public CAknApplication
    {
public:  // from CAknApplication

    TUid AppDllUid() const;

protected: // from CAknApplication
    
    CApaDocument* CreateDocumentL();
    };

#endif // C_CATALOGSLOGGERAPPLICATION_H
