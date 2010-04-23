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
* Description:   Client string localizer interface.
*
*/
	

#ifndef M_NCD_CLIENT_LOCALIZER_H
#define M_NCD_CLIENT_LOCALIZER_H

#include <e32cmn.h>

/**
 * Callback interface for client string localization.
 *
 * NCD provider users (clients) must implement this interface and register
 * it in order to get a chance to translate server-originated
 * localization keys into user viewable strings.
 *
 * 
 */
class MNcdClientLocalizer
    {
    
public:

    /**
     * Called when a localization key needs to be translated into a user
     * viewable string.
     *
     * 
     * @param aLocalizationKey Localization key string as received from server.
     * @return Pointer to heap descriptor containing the localized string
     *  corresponding to the specified localization key. Ownership is
     *  transferred. Returns a NULL pointer if the localization key is not
     *  known.
     */
    virtual HBufC* LocalizeString( const TDesC& aLocalizationKey ) = 0;
    
    };
    
#endif //  M_NCD_CLIENT_LOCALIZER_H
