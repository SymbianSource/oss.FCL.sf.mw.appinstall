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



#ifndef IA_UPDATE_ERROR_CODES_H
#define IA_UPDATE_ERROR_CODES_H

#include "e32def.h"

/**
 * IAUpdateErrorCodes namespace contains iaupdate engine
 * error codes.
 */
namespace IAUpdateErrorCodes
    {
    /**
     * This means that during the startup the database cache
     * was cleared for some reason. Still the iaupdate can
     * continue its actions, but the refresh operation is
     * recommended. Because fatal error did not occur,
     * error code is set positive.
     */
    const TInt KErrCacheCleared( 1 );
    }

#endif // IA_UPDATE_ERROR_CODES_H