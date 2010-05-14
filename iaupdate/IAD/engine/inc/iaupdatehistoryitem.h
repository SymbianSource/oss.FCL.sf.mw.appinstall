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
* Description:   MIAUpdateHistoryItem
*
*/



#ifndef IA_UPDATE_HISTORY_ITEM_H
#define IA_UPDATE_HISTORY_ITEM_H

#include <e32std.h>
#include <e32cmn.h>

/**
 * MIAUpdateHistory interface provides history item.
 *
 * @since S60 v3.2
 */
class MIAUpdateHistoryItem
    {

public: 

    /**
     * TIAHistoryItemState
     * Describes what operations has been completed
     * to the history item.
     * The state information can be used together with the
     * information about the last error code, to decide what operations
     * have been completed succesfully.
     */
    enum TIAHistoryItemState
        {
        /**
         * Nothing has been done to the item.
         * This should never be the case, because item is in
         * purchase history only after the purchase has been succefully
         * finished.
         */
        EUnknownState,
        
        /**
         * Item has been purchased successfully, but not downlaoded yet.
         */
        EPurchased,
        
        /**
         * Item has been downloaded succesfully, but not installed yet.
         */
        EDownloaded,
        
        /**
         * Item has been installed succesfully. So, the whole process has been
         * completed.
         */          
        EInstalled
        };


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~MIAUpdateHistoryItem() { }


    /**
     * @return
     *
     * @since S60 v3.2
     */
    virtual const TDesC& Name() const = 0;
    
    /**
     * @return
     *
     * @since S60 v3.2
     */
    virtual const TDesC& Version() const = 0;

    /**
     * @return
     *
     * @since S60 v3.2
     */
    virtual TTime LastOperationTime() const = 0;

    /**
     * @return 
     *
     * @since S60 v3.2
     */
    virtual TInt LastOperationErrorCode() const = 0;

    /**
     * The state information can be used together with the
     * information about the last error code, to decide what operations
     * have been completed succesfully.
     *
     * @return 
     *
     * @since S60 v3.2
     */
    virtual TIAHistoryItemState StateL() const = 0;

    };

#endif // IA_UPDATE_HISTORY_ITEM_H
