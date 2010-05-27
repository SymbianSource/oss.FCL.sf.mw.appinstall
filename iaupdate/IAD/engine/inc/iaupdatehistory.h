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
* Description:   MIAUpdateHistory
*
*/



#ifndef IA_UPDATE_HISTORY_H
#define IA_UPDATE_HISTORY_H

#include <e32cmn.h>

class MIAUpdateHistoryItem;


/**
 * MIAUpdateHistory interface provides history items.
 *
 * @since S60 v3.2
 */
class MIAUpdateHistory
    {

public: 
    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~MIAUpdateHistory() { }
    
    /**
     * @note Array items are arranged in following order
     *       1. Items whose updates have not been completed 
     *          correctly are shown first.
     *       2. Items having a same status are arranged so, 
     *          that latest item is shown first.
     * @return const RPointerArray< MIAUpdateHistoryItem >& Reference to
     * the array that contains update history items.
     *
     * @since S60 v3.2
     */
    virtual const RPointerArray< MIAUpdateHistoryItem >& Items() const = 0;

    /**
     * Resets and updates the history.
     *
     * @note History item array is cleared and old items are
     * deleted. Then, history is updated with the current items.
     * If Items-function has been used to get the array 
     * reference, it should be noted that the array content is
     * changed.
     *
     * @since S60 v3.2
     */
    virtual void RefreshL() = 0; 

    };

#endif // IA_UPDATE_HISTORY_H
