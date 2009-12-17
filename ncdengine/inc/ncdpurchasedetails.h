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
* Description:   Definition of MNcdPurchaseDetails
*
*/


#ifndef M_NCD_PURCHASE_DETAILS_H
#define M_NCD_PURCHASE_DETAILS_H

#include <e32base.h>
#include <bamdesca.h>
#include "ncditempurpose.h"

class MNcdPurchaseDownloadInfo;
class MNcdPurchaseInstallInfo;

/**
 *  Purchase details.
 *
 *  
 */
class MNcdPurchaseDetails
    {

public:

    virtual ~MNcdPurchaseDetails()
        {
        }

    /**
     * State of the purchase.
     *
     * 
     */
    enum TState
        {

        /** Payment made, but not downloaded nor installed. */
        EStatePurchased,

        /** Payment made and content downloaded, but not installed. */
        EStateDownloaded,

        /** Payment made, content downloaded and installed. */
        EStateInstalled
 
        };

    /**
     * Type of the purchased item.
     */
    enum TItemType
        {
        
        /** Item has unknown type. */
        EUnknown = 0,

        /** This is item. */
        EItem = 1,

        /** This is folder. */
        EFolder = 2
        
        };

    
    /**
     * Additional attributes
     *
     * These are retrieved with AttributeString and AttributeInt32
     *
     * Adding new attributes does not cause a binary break but removing
     * does
     *
     */
    enum TPurchaseAttribute
        {
        /** 
         * Content UID received in a protocol response
         * 
         * Type: Int32
         */
        EPurchaseAttributeContentUid = 0,
        
        /**
         * Content MIME type received in a protocol response
         * 
         * Type: String
         */
        EPurchaseAttributeContentMimeType,

        /**
         * Internal attribute value, DO NOT USE
         *
         * @note Add new attributes before this
         */
        EPurchaseAttributeInternal
        };
    
    /**
     * Get purchase state.
     *
     * @note When an item has been installed, its state will remain
     * as EStateInstalled in the purhcase detail even if the content 
     * was uninstalled.
     *
     * 
     * @return Current state of the purchase.
     */
    virtual TState State() const = 0;

    /**
     * Get client UID.
     *
     * 
     * @return Client UID.
     */
    virtual TUid ClientUid() const = 0;
    
    /**
     * Get namespace.
     *
     * 
     * @return Namespace.
     */
    virtual const TDesC& Namespace() const = 0;
    
    /**
     * Get entity ID.
     *
     * 
     * @return Entity ID.
     */
    virtual const TDesC& EntityId() const = 0;
    
    /**
     * Get item name.
     *
     * 
     * @return Item name.
     */
    virtual const TDesC& ItemName() const = 0;

    /**
     * Get item purpose.
     *
     * 
     * @return Item purpose. Bit field of TNcdItemPurpose flags.
     * @see TNcdItemPurpose
     */
    virtual TUint ItemPurpose() const = 0;
    
    /**
     * Get catalog source name.
     *
     * 
     * @return Catalog source name.
     */
    virtual const TDesC& CatalogSourceName() const = 0;
    
    /**
     * Get download infos.
     *
     * @note Only valid for purchases in EStatePurchased state.
     *
     * 
     * @return Download infos.
     */
    virtual TArray< MNcdPurchaseDownloadInfo* > DownloadInfoL() const = 0;
    
    /**
     * Get purchase option ID.
     *
     * 
     * @return Purchase option ID.
     */
    virtual const TDesC& PurchaseOptionId() const = 0;
    
    /**
     * Get purchase option name.
     *
     * 
     * @return Purchase option name.
     */
    virtual const TDesC& PurchaseOptionName() const = 0;
    
    /**
     * Get purchase option price.
     *
     * 
     * @return Purchase option price.
     */
    virtual const TDesC& PurchaseOptionPrice() const = 0;
    
    /**
     * Get final price.
     *
     * 
     * @return Final price.
     */
    virtual const TDesC& FinalPrice() const = 0;
    
    /**
     * Get payment method name.
     *
     * 
     * @return Payment method name.
     */
    virtual const TDesC& PaymentMethodName() const = 0;
    
    /**
     * Get time of purchase.
     * The time is universal time, not local time.
     * 
     * @return Purchase time.
     */
    virtual TTime PurchaseTime() const = 0;
    
    /**
     * Get file name(s) of the downloaded file(s).
     *
     * @note Only valid for purchases in EStateDownloaded state.
     *
     * 
     * @return Array of file names.
     */
    virtual const MDesCArray& DownloadedFiles() const = 0;
    
    /**
     * Get file installation infos.
     *
     * @note Only valid for purchases in EStateInstalled state.
     *
     * 
     * @return File installation info array.
     */
    virtual TArray< MNcdPurchaseInstallInfo* > InstallInfoL() const = 0;

    /**
     * Get icon.
     *
     * 
     * @return Icon data.
     */
    virtual const TDesC8& Icon() const = 0;
    
    /**
     * Checks whether this purchase detail has an icon.
     *
     * @note This flag's intention is to indicate whether this purchase detail has
     * an icon saved in the purhcase history. This does not indicate whether the
     * actual icon data has been loaded or not.
     * 
     * @return ETrue if icon is available, EFalse if not.
     */
    virtual TBool HasIcon() const = 0;
    
    /**
     * Get access point used in download process.
     *
     * 
     * @return Access point.
     */
    virtual const TDesC& DownloadAccessPoint() const = 0;
    
    /**
     * Get description.
     *
     * 
     * @return Description.
     */
    virtual const TDesC& Description() const = 0;

    /**
     * Get version.
     *
     * 
     * @return Version.
     */
    virtual const TDesC& Version() const = 0;

    /**
     * Get server URI.
     *
     * 
     * @return Server URI.
     */
    virtual const TDesC& ServerUri() const = 0;

    /**
     * Get item type.
     *
     * 
     * @return Item type.
     */
    virtual TItemType ItemType() const = 0;
    
    /**
     * Get the total size of the content of the item.
     *
     * 
     * @return The size, or 0 if not defined.
     */
    virtual TInt TotalContentSize() const = 0;
    
    /**
     * Getter for origin node id.
     *
     * 
     * @return The id.
     */
    virtual const TDesC& OriginNodeId() const = 0;

    /**
     * Getter for the time of last operation that has been directed 
     * to the purchase item. The time is an universal time.
     *
     * 
     * @return TTime Universal time of the last operation.
     */
    virtual TTime LastOperationTime() const = 0;
    
    /**
     * Getter for the last operation error code that has been set
     * by the NCD Engine during operations or by the user of the API.  
     *
     * @note The state gives the current state of the purchase details.
     * If the error code is KErrNone, then the state of the purchase detail
     * describes the state after a successfull operation.
     * If the error code differs from KErrNone, then an error has occurred
     * during an operation. The error code and the state can be used together 
     * to conclude what kind of operation failed. 
     * If the state is EStatePurchased after an error occurred, 
     * then there are two possibilities:
     * either a purchase operation has failed or a download operation has failed.
     * If the state is EStateDownloaded after an error occurred, then install operation 
     * has failed.
     * If the state is EStateInstalled, then installing has succesfully completed 
     * and the error code should be KErrNone. If EStateInstalled is given with some 
     * other error code, this means that the installation was success
     * but some repository information could not be set correctly.
     * 
     *
     * @return TInt Error code of the last operation.
     */
    virtual TInt LastOperationErrorCode() const = 0;
        

    /**
     * String attribute getter
     *
     * @param aAttribute Attribute
     * @return Parameter string or KNullDesC if not set
     *     
     * @panic ENcdPanicIndexOutOfRange if aAttribute is invalid
     * @panic ENcdPanicInvalidArgument if the attribute type is not string
     */
    virtual const TDesC& AttributeString( 
        TPurchaseAttribute aAttribute ) const = 0;

    
    /**
     * Integer attribute getter
     *
     * @param aAttribute Attribute
     * @return Parameter integer or 0 if not set
     *
     * @panic ENcdPanicIndexOutOfRange if aAttribute is invalid
     * @panic ENcdPanicInvalidArgument if the attribute type is not TInt32
     */
    virtual TInt32 AttributeInt32( 
        TPurchaseAttribute aAttribute ) const = 0;
    
    };

#endif // M_NCD_PURCHASE_DETAILS_H
