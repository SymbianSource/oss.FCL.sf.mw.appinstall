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
* Description:   Contains CNcdPurchaseHistoryProxy class
*
*/


#ifndef C_NCDPURCHASEHISTORYPROXY_H
#define C_NCDPURCHASEHISTORYPROXY_H


#include <e32base.h>
#include <e32cmn.h>

#include "ncdinterfacebaseproxy.h"
#include "ncdpurchasehistory.h"


class CNcdOperationManagerProxy;


/**
 *  CPurchaseHistoryProxy provides functions to ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdPurchaseHistoryProxy : public CNcdInterfaceBaseProxy,
                                 public MNcdPurchaseHistory
    {

public:

    /**
     * NewL
     *
     * @return CNcdPurchaseHistoryProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdPurchaseHistoryProxy* NewL(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdOperationManagerProxy& aOperationManager );

    /**
     * NewLC
     *
     * @return CNcdPurchaseHistoryProxy* Pointer to the created object 
     * of this class.
     */
    static CNcdPurchaseHistoryProxy* NewLC(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdOperationManagerProxy& aOperationManager );

    /**
     * Destructor
     */
    virtual ~CNcdPurchaseHistoryProxy();

// From MNcdPurchaseHistory

    /**
     * @see MNcdPurchaseHistory::SavePurchaseL
     */
    void SavePurchaseL( const MNcdPurchaseDetails& aDetails,
                        TBool aSaveIcon );

    /**
     * @see MNcdPurchaseHistory::PurchaseIdsL
     */
    RArray<TUint> PurchaseIdsL( const MNcdPurchaseHistoryFilter& aFilter );

    /**
     * @see MNcdPurchaseHistory::PurchaseDetailsL
     */
    CNcdPurchaseDetails* PurchaseDetailsL( TUint aPurchaseId, 
        TBool aLoadIcon );

    /**
     * @see MNcdPurchaseHistory::RemovePurchaseL
     */
    void RemovePurchaseL( TUint aPurchaseId );
    
    /**
     * @see MNcdPurchaseHistory::EventCountL
     */
    TUint EventCountL();

protected:

    /**
     * Constructor
     */
    CNcdPurchaseHistoryProxy(
        MCatalogsClientServer& aSession, 
        TInt aHandle,
        CCatalogsInterfaceBase* aParent,
        CNcdOperationManagerProxy& aOperationManager );

    /**
     * ConstructL
     */
    virtual void ConstructL();

private:

    // Prevent if not implemented
    CNcdPurchaseHistoryProxy( const CNcdPurchaseHistoryProxy& aObject );
    CNcdPurchaseHistoryProxy& operator =
        ( const CNcdPurchaseHistoryProxy& aObject );

private: // data

    CNcdOperationManagerProxy& iOperationManager;

    };


#endif // C_NCDPURCHASEHISTORYPROXY_H
