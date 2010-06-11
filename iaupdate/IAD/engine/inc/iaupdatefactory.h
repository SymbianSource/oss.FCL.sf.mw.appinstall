/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#ifndef IAUPDATEFACTORY_H
#define IAUPDATEFACTORY_H

#include <e32cmn.h>

class MIAUpdateController;
class MIAUpdateControllerObserver;

/**
 * Provides static functions to create IA objects.
 */
namespace IAUpdateFactory
    {
    /** 
     * @param aFamilyUid Defines the uid for the client family.
     * @param aObserver Observer will be informed when the controller operations
     * progress.
     * @return MIAUpdateController* Update controller object that 
     * can be used through the given interface.
     * Ownership is transferred.
     */
    IMPORT_C MIAUpdateController* CreateControllerL( const TUid& aFamilyUid, 
                                                     MIAUpdateControllerObserver& aObserver );
    }

#endif // IAUPDATEFACTORY_H


