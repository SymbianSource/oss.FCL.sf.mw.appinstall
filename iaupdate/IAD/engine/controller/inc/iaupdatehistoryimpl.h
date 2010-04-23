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
* Description:   CIAUpdateHistory
*
*/



#ifndef IA_UPDATE_HISTORY_IMPL_H
#define IA_UPDATE_HISTORY_IMPL_H


#include <e32base.h>
#include "iaupdatehistory.h"

class MNcdProvider;
class MNcdPurchaseHistory;
class MNcdPurchaseDetails;


/**
 * CIAUpdateHistory provides history items.
 *
 * Notice, the item list is still empty after this class
 * object is created. RefreshL-function has to be called 
 * to get the history items from the purchase history. Also,
 * RefreshL can be used later in the code to refresh the item list.
 *
 * @since S60 v3.2
 */
class CIAUpdateHistory : public CBase,
                         public MIAUpdateHistory
    {

public: 
    
    /**
     * @param aFamilyUid FamilUid describes the family of the
     * update items.
     * @param aProvider Provider is used to get the purchase history
     * data.
     * @return CIAUpdateHistory*
     *
     * @since S60 v3.2
     */
    static CIAUpdateHistory* NewL( const TUid& aFamilyUid,
                                   MNcdProvider& aProvider );

    /**
     * @see CIAUpdateHistory::NewL
     */
    static CIAUpdateHistory* NewLC( const TUid& aFamilyUid,
                                    MNcdProvider& aProvider );
    
    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateHistory();
    
    /**
     * @see MIAUpdateHistory::Items
     */
    virtual const RPointerArray< MIAUpdateHistoryItem >& Items() const;

    /**
     * @see MIAUpdateHistory::RefreshHistoryL
     */
    virtual void RefreshL();
    

protected:

    /**
     * @return MNcdProvider& Provider is used to get the history 
     * information from the NCD Engine.
     */
    MNcdProvider& Provider();
    
    /**
     * @return MNcdPurchaseHistory& Purchase history class object that
     * can be used to get the history items.
     */
    MNcdPurchaseHistory& History();

    /**
     * @return const TUid& Family uid that is used for the history items.
     */
    const TUid& FamilyUid() const;
    
    
private:

    /**
     * @see CIAUpdateHistory::NewL
     */
    CIAUpdateHistory( const TUid& aFamilyUid,
                      MNcdProvider& aProvider );

    /**
     * ConstructL
     */    
    void ConstructL();


    // Prevent these
    CIAUpdateHistory( const CIAUpdateHistory& aObject );
    CIAUpdateHistory& operator =( const CIAUpdateHistory& aObject );


    // Creates a MIAUpdateHistoryItem object if necessary and 
    // inserts that item into the correct place of the item 
    // array.
    void InsertItemL( MNcdPurchaseDetails* aDetails );

    // This function is used to check if the item type is accepted
    // and if the item should be included into the history.
    TBool AcceptItem( const CNcdPurchaseDetails& aItem ) const;


private: // data

    TUid iFamilyUid;

    MNcdProvider& iProvider;

    // Owned.
    // Notice that this cannot be deleted but the Release has to 
    // be called.
    MNcdPurchaseHistory* iHistory;

    RPointerArray< MIAUpdateHistoryItem > iItems;    
        
    };

#endif // IA_UPDATE_HISTORY_IMPL_H
