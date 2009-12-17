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
* Description:   Catalogs Engine callback interface definition
*
*/
	

#ifndef M_CATALOGS_ENGINE_OBSERVER_H
#define M_CATALOGS_ENGINE_OBSERVER_H

#include <e32cmn.h>

/**
 * Observer interface for catalogs engine events
 *
 * Engine users should implement this interface to be able to receive
 * notifications from the catalogs engine
 *
 * 
 */
class MCatalogsEngineObserver
    {
    
public:

    /**
     * Called when the Catalogs Engine is about to be updated. The client
     * must stop using the Engine, closing its session(s) and deleting its
     * Catalogs Engine objects ASAP.
     */
    virtual void CatalogsEngineShutdown() = 0;

    /**
     * Called when a Catalogs OTA update is available. Only the client
     * authorized for Catalogs OTA updates will receive this callback.
     *
     * @param aTarget Update target.
     * @param aId Additional update target id, such as application UID or
     *  skin ID.
     * @param aVersion Update version.
     * @param aUri Uri for downloading the update package.
     * @param aForce ETrue if the update should be forced (user is not
     *  asked), EFalse otherwise.
     */
    virtual void CatalogsUpdateNotification( 
        const TDesC& aTarget,
        const TDesC& aId,
        const TDesC& aVersion,
    	const TDesC& aUri,
        TBool aForce ) = 0;

    /**
     * Called when connection status changes. This is called when client-
     * related network activity starts and stops.
     * 
     * @param aConnectionActive Connection status. ETrue if connection is active,
     * EFalse otherwise.
     */
    virtual void CatalogsConnectionEvent( TBool aConnectionActive ) = 0;
    };
    
#endif //  M_CATALOGS_ENGINE_OBSERVER_H