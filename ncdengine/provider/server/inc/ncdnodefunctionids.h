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
* Description:   Contains enumeration values for the node functions
*
*/
	

#ifndef NCD_NODE_FUNCTION_IDS_H
#define NCD_NODE_FUNCTION_IDS_H

namespace NcdNodeFunctionIds
    {

    /**
     * TNcdNodeFunctionNumber should be used when proxies are requesting services by
     * calling ReceiveMessageL function of this class.
     */
    enum TNcdNodeFunctionNumber
        {
        
        // Node functions ids

        /**
         *
         */
        ENcdRootNodeHandle,
        
        /**
         *
         */
        ENcdSearchRootNodeHandle,
        
        /**
         *
         */
        ENcdNodeHandle,
        
        /**
         * This means that a temporary node item should be creted.
         */
        ENcdTemporaryNodeItemHandle,
        
        /**
         *
         */
        ENcdTemporaryNodeItemWithMetaDataHandle,
        
        /**
         * This means that a temporary node folder should be created.
         */
        ENcdTemporaryNodeFolderHandle,
        
        /**
         *
         */
        ENcdTemporaryNodeFolderWithMetaDataHandle,
        
        /**
         * This means that a temporary bundle folder should be created.
         */
        ENcdTemporaryBundleFolderHandle,
        
        /**
         *
         */
        ENcdTemporaryBundleFolderWithMetaDataHandle,

        /**
         *
         */
        ENcdLinkHandle,

        /**
         *
         */
        ENcdMetadataHandle,

        /**
         *
         */
        ENcdDisclaimerHandle,

        /**
         *
         */
        ENcdIconHandle,

        /**
         *
         */
        ENcdScreenshotHandle,

        /**
         *
         */
        ENcdSkinHandle,

        /**
         *
         */
        ENcdUriContentHandle,
        
        /**
         *
         */
        ENcdContentInfoHandle,
        
        /**
         *
         */
        ENcdUserDataHandle,
        
        /**
         *
         */
        ENcdPreviewHandle,
        
        /**
         *
         */
        ENcdUpgradeHandle,
        
        /**
         *
         */
        ENcdDependencyHandle,
        
        /**
         *
         */
        ENcdNodeSeenHandle,
        
        /**
         *
         */
        ENcdNodeSeenFolderHandle,
        
        /**
         *
         */
        ENcdCreateTemporaryOrSupplierNode,
        
        /**
         *
         */
        ENcdCreateTemporaryNodeIfMetadataExists,
        
        /**
         *
         */
        ENcdUserData,

        /**
         *
         */
        ENcdSetUserData,

        /**
         *
         */
        ENcdClearUserData,
        
        /**
         *
         */
        ENcdRemoveFromDb,
        


        // Node install function numbers
        /**
         * 
         */
        ENcdInstallHandle,
         
        /**
         * 
         */
        ENcdInstallOpenFile,
 
        /**
         * 
         */
        ENcdSetApplicationInstalled,
 
       
        /**
         * Gets all the data for the proxy in one descriptor.
         */
        ENcdInternalize,
        
        /**
         *
         */
        ENcdSubscriptionGroupIdentifiers,
        
        /**
         *
         */
        ENcdListenerEnrollment,

        /**
         * 
         */
        ENcdSubscriptionIconData,        
            
        // Node metadata function numbers           

        /**
         *
         */
        ENcdIconData,

        /**
         *
         */
        ENcdScreenshotData,



        /**
         *
         */
        ENcdInternalizePurchaseHistory,

        /**
         *
         */
        ENcdInternalizePurchaseMeans,
                
        /**
         * Gets all the purchase option ids.
         */
        ENcdPurchaseOptionIds,

        

        /**
         *
         */
        ENcdDownloadHandle,


        /**
         *
         */        
        ENcdPreviewOpenFile,


        /**
         *
         */
        ENcdIsPreviewLoaded,
        
        /**
         *
         */
        ENcdIsPurchaseSupported,
        
        // Node seen function numbers
        
        /**
         *
         */
        ENcdNodeSeenSetSeen,
        
        // Node seen folder function numbers
        
        /**
         *
         */
        ENcdNodeSeenFolderSetContentsSeen,
        
        /**
         *
         */
        ENcdNodeSeenFolderNewCount,
        
        /**
         *
         */
        ENcdNodeSeenFolderNewNodes,


        // Purchase history function numbers

        /**
         *
         */
        ENcdPurchaseHistorySavePurchase,
        
        /**
         *
         */
        ENcdPurchaseHistorySavePurchaseWithOldIcon,

        /**
         *
         */
        ENcdPurchaseHistoryRemovePurchase,

        /**
         *
         */
        ENcdPurchaseHistoryGetPurchaseIds,

        /**
         *
         */
        ENcdPurchaseHistoryGetPurchase,
        
        /**
         * Gets the purchase details without the icon
         */
        ENcdPurchaseHistoryGetPurchaseNoIcon,

        /**
         *
         */
        ENcdPurchaseHistoryEventCount,
        
        /**
         *
         */
        ENcdFilesExist,


        // Server report manager functions
        
        /**
         *
         */
        ENcdServerReportManagerSetReportingMethod,
        
        /**
         *
         */
        ENcdServerReportManagerReportingMethod,

        /**
         *
         */
        ENcdServerReportManagerSetReportingStyle,
        
        /**
         *
         */
        ENcdServerReportManagerReportingStyle,

        /**
         *
         */
        ENcdServerReportManagerNodeSetAsInstalled,


        // Misc functions

        /**
         * Gets the class id of the node.
         * This way the parent proxy class may decide
         * what kind of proxy class it should create for this
         * communicable node (for example folder or item)
         */
        ENcdClassId,

                      
        /**
         * When proxy does not require services of this node it may call 
         * release function which inform that this node may delete itself.
         * For example, destructor of the node proxy may use this.
         */
        ENcdRelease,
         
         /**
          * Clear all search result nodes.
          */
        ENcdClearSearchResults,
        
                

        // Other

        /**
         *
         */        
        ENcdIsCapabilitySupported,

        /**
         *
         */        
        ENcdMoreInfoHandle,
        
        
        /**
         *
         */
        ENcdIsTransparentChildExpired

        };


    /**
     * 
     * 
     */
    enum TNcdOperationManagerFunctionNumber
        {
        /**
         * 
         */
        ENcdOperationManagerCreateLoadNodeOperation,
        
        /**
         * 
         */
        ENcdOperationManagerCreateLoadNodeChildrenOperation,
        
        /**
         *
         */
        ENcdOperationManagerCreateDownloadOperation,

        /**
         * 
         */
        ENcdOperationManagerCreatePurchaseOperation,

        
        ENcdOperationManagerCreateLoadRootNodeOperation,
        
        ENcdOperationManagerCreateLoadBundleNodeOperation,
        
        ENcdOperationManagerCreateInstallOperation,

        ENcdOperationManagerCreateSilentInstallOperation,

        ENcdOperationManagerCreateRightsObjectOperation,
        
        ENcdOperationManagerCreateSubscriptionOperation,
        
        ENcdOperationManagerRestoreContentDownloads,
        
        ENcdOperationManagerCreateCreateAccessPointOperation,
        
        ENcdOperationManagerCreateSendHttpRequestOperation,
        
        ENcdOperationManagerCreateServerReportOperation
        };
    
    /**
     *
     */    
    enum TNcdFavoriteManagerFunctionNumber 
        {
        /**
         * Remove favorite node.
         */
        ENcdFavoriteManagerRemoveFavorite,
        
        /**
         * Add favorite node.
         */
        ENcdFavoriteManagerAddFavorite,
        
        /**
         * Set (or remove) disclaimer for favorite node.
         */
        ENcdFavoriteManagerSetDisclaimer,
        
        /**
         * Get the disclaimer handle.
         */
        ENcdFavoriteManagerDisclaimerHandle
        };
    }	
	
#endif //  NCD_NODE_FUNCTION_IDS_H
