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
* Description:   Base class for AppInfo and PackageInfo arrays
*
*/


#ifndef C_APPMNGR2INFOARRAY_H
#define C_APPMNGR2INFOARRAY_H

#include <e32base.h>                    // CBase
#include <appmngr2infobase.h>           // CAppMngr2InfoBase
#include "appmngr2infoarrayobserver.h"  // MAppMngr2InfoArrayObserver


/**
 * CAppMngr2InfoArray contains list of installation files or installed apps.
 * CAppMngr2Model uses these arrays to hold data received from scanning
 * installation directories or application registeries. Application UI uses
 * these arrays (via CAppMngr2Model) to get info about displayed items.
 * Functions At() and Count() are used for this purpose. Changes in array
 * content are notified using interface MAppMngr2InfoArrayObserver, and
 * makes the Application UI to refresh displayed lists.
 *
 * A "cache" buffer is used to maintain old data while new data is collected. 
 * For example, new installation files found in scanning are stored in iArray,
 * and old content is used from iCache while the scanning is on-going and more
 * items are added to iArray. When the scanning completes, new content in
 * iArray is taken in use (old content in iCache is discarded) and UI is
 * notified about the change.
 * 
 * Notifications can be disabled temporarily, which means that cache is
 * locked in use temporarily. When notifications are disabled, new content
 * may be received to iArray as usual, but no notifications are sent to
 * observer until notifications are enabled again. CAppMngr2Model disables
 * the notifications while a plugin specific command is running, as it needs
 * to call CommandComplete() when the running command completes. Plugin
 * specific commands (like delete or uninstall) may launch new scanning that
 * change the array content. CAppMngr2Model maintains pointer iActiveItem
 * to the item that is running the plugin specific command, and this item
 * is actually stored in iArray or iCache. If array content could change
 * while the command is run, iActiveItem might not be valid any more after
 * the command is complete and calling iActiveItem->CommandComplete()
 * might raise a KERN-EXEC panic. 
 */

class CAppMngr2InfoArray : public CBase
    {
public:     // constructor and destructor
    CAppMngr2InfoArray( MAppMngr2InfoArrayObserver& aObserver );
    ~CAppMngr2InfoArray();

public:     // new functions
    CAppMngr2InfoBase* At( TInt aIndex ) const;
    TInt Count() const;
    void IncrementCacheUseL();
    void IncrementCacheUseStartingNewRoundL();
    void DecrementCacheUse();
    void DisableRefreshNotificationsL();
    void EnableRefreshNotificationsL();
    void AddItemInOrderL( CAppMngr2InfoBase* aInfo );
    void ImmediateDelete( CAppMngr2InfoBase* aInfo );

private:    // new functions
    void MoveItemsToCacheMaintainingOrderL();
    void MoveCachedItemsToArrayInOrderL();
    void NotifyObserver();
    TBool IsCacheUsed() const;

protected:  // data
    MAppMngr2InfoArrayObserver& iObserver;
    RPointerArray<CAppMngr2InfoBase> iArray;
    RPointerArray<CAppMngr2InfoBase> iCache;
    TLinearOrder<CAppMngr2InfoBase> iAlphabeticalOrder;
    TBool iForceCacheUse;
    TBool iForceCacheUseWhenAddingComplete;
    TBool iQuickRefreshes;
    TBool iArrayChangedObserverNeedsNotification;
    TInt iUseCache;
    };

#endif  // C_APPMNGR2INFOARRAY_H

