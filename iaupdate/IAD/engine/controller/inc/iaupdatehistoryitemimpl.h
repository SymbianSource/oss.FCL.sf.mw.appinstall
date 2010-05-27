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
* Description:   CIAUpdateHistoryItem
*
*/



#ifndef IA_UPDATE_HISTORY_ITEM_IMPL_H
#define IA_UPDATE_HISTORY_ITEM_IMPL_H

#include <e32base.h>
#include "iaupdatehistoryitem.h"

class MNcdPurchaseDetails;
class MNcdProvider;

/**
 * CIAUpdateHistoryItem provides history item.
 *
 * @since S60 v3.2
 */
class CIAUpdateHistoryItem : public CBase,
                             public MIAUpdateHistoryItem
    {

public: 
    
    /**
     * @param aDetails Purchase details of this item.
     * @param aProvider NCD Engine provider can be used to get
     * history information of nodes.
     * @return CIAUpdateHistoryItem*
     * @exception Leaves with KErrArgument if aDetails is NULL.
     * Leaves with system wide error code.
     *
     * @since S60 v3.2
     */
    static CIAUpdateHistoryItem* NewL( MNcdPurchaseDetails* aDetails,
                                       MNcdProvider& aProvider );

    /**
     * @see CIAUpdateHistoryItem::NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateHistoryItem* NewLC( MNcdPurchaseDetails* aDetails,
                                        MNcdProvider& aProvider );
    
    
    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateHistoryItem();


    /**
     * @return MNcdPurchaseDetails& Purchase details of this item.
     * Details are gotten from the purchase history.
     */
    MNcdPurchaseDetails& Details() const;

    /**
     * @return MNcdProvider& Provider is used to get the history 
     * information from the NCD Engine.
     */
    MNcdProvider& Provider() const;


public: // MIAUpdateHistoryItem

    /**
     * @see MIAUpdateHistoryItem::Name
     */
    virtual const TDesC& Name() const;
    
    /**
     * @see MIAUpdateHistoryItem::Version
     */
    virtual const TDesC& Version() const;

    /**
     * @see MIAUpdateHistoryItem::LastOperationTime
     */
    virtual TTime LastOperationTime() const;

    /**
     * @see MIAUpdateHistoryItem::LastOperationErrorCode
     */
    virtual TInt LastOperationErrorCode() const;

    /**
     * @see MIAUpdateHistoryItem::StateL
     */
    virtual TIAHistoryItemState StateL() const;
    

private:

    /**
     * @see CIAUpdateHistoryItem::NewL
     */
    CIAUpdateHistoryItem( MNcdProvider& aProvider );

    /** 
     * ConstructL
     *
     * @see CIAUpdateHistoryItem::NewL
     */
    virtual void ConstructL( MNcdPurchaseDetails* aDetails );


    // Prevent these
    CIAUpdateHistoryItem( const CIAUpdateHistoryItem& aObject );
    CIAUpdateHistoryItem& operator =( const CIAUpdateHistoryItem& aObject );


private: // data

    // Owned.
    MNcdPurchaseDetails* iDetails; 
    
    MNcdProvider& iProvider;
        
    };

#endif // IA_UPDATE_HISTORY_ITEM_IMPL_H
