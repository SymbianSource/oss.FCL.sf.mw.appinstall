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
* Description:   Definition of CNcdPurchaseHistoryDb
*
*/


#ifndef C_NCDPURCHASEHISTORYIMPL_H
#define C_NCDPURCHASEHISTORYIMPL_H

#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>

#include "catalogscommunicable.h"
#include "ncdnodefunctionids.h"

class CNcdPurchaseHistoryFilter;
class CNcdPurchaseDetails;
class MCatalogsBaseMessage;
class RFs;

/**
 * Purchase history.
 */
class CNcdPurchaseHistoryDb : public CCatalogsCommunicable
    {
public: // Enumerations

    /**
     * Column numbers for rowsets.
     */
    enum TPurchaseColumns
        {
        EPurchaseId = 1,
        EEventId,
        EClientUid,
        ENamespace,
        EEntityId,
        EItemName,
        EItemPurpose,
        ECatalogName,
        EDownloadInfo,
        EPurchaseOptionId,
        EPurchaseOptionName,
        EPurchaseOptionPrice,
        EFinalPrice,
        EPaymentMethodName,
        EPurchaseTime,
        EDownloadedFiles,
        EFileInstallInfos,
        EIcon,
        EDownloadAccessPoint,
        EDescription,
        EVersion,
        EServerUri,
        EItemType,
        ETotalContentSize,
        EOriginNodeId,
        ELastOperationTime,
        ELastOperationErrorCode,
        EHasIcon,
        EAttributes
        };

    /**
     * Sorting order of the purchases. ENewestFirst order sorts purchases
     * from newest to oldest. EOldestFirst order sorts purchases from oldest
     * to newest. ENone will not sort at all and results may come in any
     * order.
     */
    enum TSortingOrder
        {
        ENewestFirst, EOldestFirst, ENone
        };

public: // Construction & destruction

    /**
     * Constructor.
     *
     * @param aDbFilename Filename of the database.
     * @return CNcdPurchaseHistoryDb* Purchase history.
     */
    static CNcdPurchaseHistoryDb* NewL(
        const TDesC& aDbFilename );
    
    /**
     * Constructor.
     *
     * @param aDbFilename Filename of the database.
     * @return CNcdPurchaseHistoryDb* Purchase history.
     */
    static CNcdPurchaseHistoryDb* NewLC(
        const TDesC& aDbFilename );

    /**
     * Destructor.
     */
    ~CNcdPurchaseHistoryDb();

public: // From CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );

private: // Construction

    /**
     * Private constructor.
     * 
     * @param aDbFilename Filename of the database.
     */
    void ConstructL( const TDesC& aDbFilename );
    
    /**
     * Private constructor.
     */
    CNcdPurchaseHistoryDb();

public: // New functions

    /**
     * Save purchase details into the purchase database.
     * 
     * @since S60 v3.2
     * @param aPurchase Details that will be saved.
     * @param aSaveIcon ETrue as default, which informs that the icon 
     * should be saved. This will replace an old icon if it existed. 
     * So, if an old icon was to be kept, EFalse should be used.
     */
    void SavePurchaseL( CNcdPurchaseDetails& aPurchase, 
                        TBool aSaveIcon = ETrue );
     
    /**
     * Remove purchase from the purchase history.
     *
     * @since S60 v3.2
     * @param aPurchaseId ID of the purchase to be removed.
     */
    void RemovePurchaseL( TUint aPurchaseId );

    /**
     * Get purchase IDs from purchase history.
     *
     * @since S60 v3.2
     * @param aFilter Filter used to get certain purchase IDs from the
     * purchase history.
     * @param aSortingOrder Sorting order used in returned array.
     * @return Array of purchase IDs. Can be empty.
     */
    RArray<TUint> PurchaseIdsL(
        const CNcdPurchaseHistoryFilter& aFilter,
        const TSortingOrder aSortingOrder );

    /**
     * Get purchase details from purchase history.
     *
     * @since S60 v3.2
     * @param aPurchaseId ID of the purchase. PurchaseIdsL function can be
     * used to get purchase IDs.
     * @param aLoadIcon If EFalse, the icon is not loaded. ETrue by default
     * @return Purchase details. Ownership is transferred to the caller.
     * @throw KErrNotFound if purchase is not found.
     */
    CNcdPurchaseDetails* PurchaseL( TUint aPurchaseId, 
        TBool aLoadIcon = ETrue );

    /**
     * Get count of events occurred to the purchase history.
     *
     * @since S60 v3.2
     * @return TUint Count of purchase events.
     */
    TUint EventCountL();
    

    /**
     * Ensures that purchase details have the correct amount of
     * filepaths and install infos.
     * 
     * Filepaths are added/removed so that DownloadFiles().MdcaCount() equals
     * DownloadInfoCount(). Added filepaths are empty
     * 
     * Install infos are removed so that DownloadInfoCount() is always greater
     * than or equal to InstallInfoCount()
     * 
     * @param aDetails Details to validate
     */
    static void ValidatePurchaseDetailsL( CNcdPurchaseDetails& aDetails );        

    
private: // New private functions

    /**
     * This function handles save purchase request.
     *
     * @since S60 v3.2
     * @param aMessage Message.
     * @param aSaveIcon ETrue as default, which informs that the icon 
     * should be saved. This will replace an old icon if it existed. 
     * So, if an old icon was to be kept, EFalse should be used.
     */
    void SavePurchaseRequestL( MCatalogsBaseMessage& aMessage,
                               TBool aSaveIcon = ETrue );

    /**
     * This function handles remove purchase request.
     *
     * @since S60 v3.2
     * @param aMessage Message.
     */
    void RemovePurchaseRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * This function handles purchase IDs request.
     *
     * @since S60 v3.2
     * @param aMessage Message.
     */
    void GetPurchaseIdsRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * This function handles get purchase request.
     *
     * @since S60 v3.2
     * @param aMessage Message.
     * @param aLoadIcon ETrue if the icon should be loaded from the database
     */
    void GetPurchaseRequestL( MCatalogsBaseMessage& aMessage, 
        TBool aLoadIcon );

    /**
     * This function handles event count request.
     *
     * @since S60 v3.2
     * @param aMessage Message.
     */
    void EventCountRequestL( MCatalogsBaseMessage& aMessage );

    /**
     * This function handles release request.
     *
     * @since S60 v3.2
     * @param aMessage Message.
     */
    void ReleaseRequest( MCatalogsBaseMessage& aMessage );

    /**
     * Insert new purchase into the purchase history.
     *
     * @since S60 v3.2
     * @param aPurchase Purchase.
     * @param aNewEventCount New purchase event count.
     */
    void NewPurchaseL(
        CNcdPurchaseDetails& aPurchase,
        TUint aNewEventCount );

    /**
     * Update details of old purchase.
     *
     * @since S60 v3.2
     * @param aView Database view used in update.
     * @param aPurchase Purchase.
     * @param aNewEventCount New purchase event count.
     * @param aSaveIcon ETrue if the icon data should be saved
     */
    void UpdatePurchaseL(
        RDbRowSet& aView,
        CNcdPurchaseDetails& aPurchase,
        TUint aNewEventCount,
        TBool aSaveIcon );

    /**
     * Set database view with new values.
     *
     * @since S60 v3.2
     * @param aView Database view to be set.
     * @param aPurchase New purchase details. 
     * @param aNewEventCount New purchase event count.
     * @param aSaveIcon ETrue if the icon data should be saved
     */
    void SetPurchaseViewL(
        RDbRowSet& aView,
        CNcdPurchaseDetails& aPurchase,
        TUint aNewEventCount,
        TBool aSaveIcon );
    
    /**
     * Set count of events occurred to the purchase history.
     *
     * @since S60 v3.2
     * @param aEventCount New purchase event count.
     */
    void SetEventCountL( TUint aEventCount );
    
    /**
     * Checks if purchase exists in the database.
     *
     * @since S60 v3.2
     * @param aPurchaseId ID of the purchase.
     * @return ETrue if purchase existed in the database.
     */
    TBool PurchaseExistsL( TUint aPurchaseId );

private: // Data members

    // Name of the database.
    HBufC* iDbFilename;
    // File server session.
    RFs iFs;
    // Database.
    RDbNamedDatabase iDatabase;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    };

#endif // C_NCDPURCHASEHISTORYIMPL_H
