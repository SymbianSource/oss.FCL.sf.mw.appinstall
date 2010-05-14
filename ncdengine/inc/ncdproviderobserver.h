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
	

#ifndef M_NCDPROVIDEROBSERVER_H
#define M_NCDPROVIDEROBSERVER_H

#include <e32cmn.h>
#include "catalogsarray.h"

class MNcdNode;

/**
 * Observer interface for provider generic events
 *
 * Provider users should implement this interface to be able to receive
 * notifications from the root node provider
 */
class MNcdProviderObserver
    {
    
public:

    /**
     * Called when information of expired nodes has been received. Users must
     * re-load these nodes to get updated data.
     *
     * @param aExpiredNodes A list of expired nodes in need of re-loading.
     */
    virtual void ForceExpirationInformationReceived(
        RCatalogsArray< MNcdNode >& aExpiredNodes ) = 0;
    };
    
#endif //  M_NCDPROVIDEROBSERVER_H
