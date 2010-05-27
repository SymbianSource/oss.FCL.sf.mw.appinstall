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
* Description:   Contains NCD interface identifiers
*
*/


#ifndef NCD_INTERFACE_IDS_H
#define NCD_INTERFACE_IDS_H

/**
 *  NCD provider external interface id definitions.
 *
 *  
 */

enum TNcdInterfaceId
    {
    /**
     * Interface id of MNcdProvider.
     *
     * @see MNcdProvider
     */
    ENcdProviderUid = 1000,      

    /**
     * Interface id of MNcdNode.
     *
     * @see MNcdNode
     */
    ENcdNodeUid = 1001,

    /**
     * Interface id of MNcdNodeContainer.
     *
     * @see MNcdNodeContainer
     */
    ENcdNodeContainerUid = 1002,

    /**
     * Interface id of MNcdOperation.
     *
     * @see MNcdOperation
     */
    ENcdOperationUid = 1003,

    /**
     * Interface id of MNcdLoadNodeOperation.
     *
     * @see MNcdLoadNodeOperation
     */
    ENcdLoadNodeOperationUid = 1004,

    /**
     * Interface id of MNcdNodeUserData.
     *
     * @see MNcdNodeUserData
     */
    ENcdNodeUserDataUid = 1005,

    /**
     * Interface id of MNcdNodeSearch.
     *
     * @see MNcdNodeSearch
     */
    ENcdNodeSearchUid = 1006,

    /**
     * Interface id of MNcdSearchOperation.
     *
     * @see MNcdSearchOperation
     */
    ENcdSearchOperationUid = 1007,

    /**
     * Interface id of MNcdQuery.
     *
     * @see MNcdQuery
     */
    ENcdQueryUid = 1008,

    /**
     * Interface id of MNcdQueryItem.
     *
     * @see MNcdQueryItem
     */
    ENcdQueryItemUid = 1009,

    /**
     * Interface id of MNcdQueryTextItem.
     *
     * @see MNcdQueryTextItem
     */
    ENcdQueryTextItemUid = 1011,

    /**
     * Interface id of MNcdQueryNumericItem.
     *
     * @see MNcdQueryNumericItem
     */
    ENcdQueryNumericItemUid = 1012,

    /**
     * Interface id of MNcdQueryPinCodeItem.
     *
     * @see MNcdQueryPinCodeItem
     */
    ENcdQueryPinCodeItemUid = 1013,

    /**
     * Interface id of MNcdQuerySelectionItem.
     *
     * @see MNcdQuerySelectionItem
     */
    ENcdQuerySelectionItemUid = 1014,

    /**
     * Interface id of MNcdDownloadOperation.
     *
     * @see MNcdDownloadOperation
     */
    ENcdDownloadOperationUid = 1015,

    /**
     * Interface id of MNcdInstallOperation.
     *
     * @see MNcdInstallOperation
     */
    ENcdInstallOperationUid = 1016,

    /**
     * Interface id of MNcdNodeContent.
     *
     * @see MNcdNodeContent
     */
    ENcdNodeContentUid = 1017,

    /**
     * Interface id of MNcdNodeDependency.
     *
     * @see MNcdNodeDependency
     */
    ENcdNodeDependencyUid = 1018,

    /**
     * Interface id of MNcdNodeDownload.
     *
     * @see MNcdNodeDownload
     */
    ENcdNodeDownloadUid = 1019,

    /**
     * Interface id of MNcdNodeIcon.
     *
     * @see MNcdNodeIcon
     */
    ENcdNodeIconUid = 1020,

    /**
     * Interface id of MNcdNodeInstall.
     *
     * @see MNcdNodeInstall
     */
    ENcdNodeInstallUid = 1021,

    /**
     * Interface id of MNcdNodePreview.
     *
     * @see MNcdNodePreview
     */
    ENcdNodePreviewUid = 1022,

    /**
     * Interface id of MNcdNodePurchase.
     *
     * @see MNcdNodePurchase
     */
    ENcdNodePurchaseUid = 1023,

    /**
     * Interface id of MNcdNodeMetadata.
     *
     * @see MNcdNodeMetadata
     */
    ENcdNodeMetadataUid = 1024,

    /**
     * Interface id of MNcdPurchaseOperation.
     *
     * @see MNcdPurchaseOperation
     */
    ENcdPurchaseOperationUid = 1025,

    /**
     * Interface id of MNcdPurchaseOption.
     *
     * @see MNcdPurchaseOption
     */
    ENcdPurchaseOptionUid = 1026,

    /**
     * Interface id of MNcdPaymentMethod.
     *
     * @see MNcdPaymentMethod
     */
    ENcdPaymentMethodUid = 1027,

    /**
     * Interface id of MNcdNodeScreenshot.
     *
     * @see MNcdNodeScreenshot
     */
    ENcdNodeScreenshotUid = 1028,

    /**
     * Interface id of MNcdNodeSkin.
     *
     * @see MNcdNodeSkin
     */
    ENcdNodeSkinUid = 1029,

    /**
     * Interface id of MNcdFileDownloadOperation.
     *
     * @see MNcdFileDownloadOperation
     */
    ENcdFileDownloadOperationUid = 1030,
    
    /**
     * Interface id of MNcdCreateAccessPointOperation
     *
     * @see MNcdCreateAccessPointOperation
     */
    ENcdCreateAccessPointOperationUid = 1031,

    /**
     * Interface id of MNcdNodeUpgrade.
     *
     * @see MNcdNodeUpgrade
     */
    ENcdNodeUpgradeUid = 1032,

    /**
     * Interface id of MNcdNodeUriContent.
     *
     * @see MNcdNodeUriContent
     */
    ENcdNodeUriContentUid = 1033,

    /**
     * Interface id of MNcdSearchFilter.
     *
     * @see MNcdSearchFilter
     */
    ENcdNodeSearchFilterUid = 1034,

    /**
     * Interface id of MNcdPurchaseHistory.
     *
     * @see MNcdPurchaseHistory
     */
    ENcdPurchaseHistoryUid = 1035,

    /**
     * Interface id of MNcdPurchaseHistoryFilter.
     *
     * @see MNcdPurchaseHistoryFilter
     */
    ENcdPurchaseHistoryFilterUid = 1036,

    /**
     * Interface id of MNcdSubscriptionManager.
     *
     * @see MNcdSubscriptionManager
     */
    ENcdSubscriptionManagerUid = 1037,

    /**
     * Interface id of MNcdSubscriptionManager.
     *
     * @see MNcdSubscriptionManager
     */
    ENcdSubscriptionUid = 1038,

    /**
     * Interface id of MNcdNodeSubscribe.
     *
     * @see MNcdNodeSubscribe
     */
    ENcdNodeSubscribeUid = 1039,

    /**
     * Interface id of MNcdSubscriptionOperation.
     *
     * @see MNcdSubscriptionOperation
     */
    ENcdSubscriptionOperationUid = 1040,

    /**
     * Interface id of MNcdNodeContentFile.
     *
     * @see MNcdNodeContentFile
     */
    ENcdNodeContentFileUid = 1041,

    /**
     * Interface id of MNcdProviderPcClientSupport.
     *
     * @see MNcdProviderPcClientSupport
     */
    ENcdProviderPcClientSupportUid = 1042,

    /**
     * Interface id of MNcdRightsObjectOperation.
     *
     * @see MNcdRightsObjectOperation
     */
    ENcdRightsObjectOperationUid = 1043,

    /**
     * Interface id of MNcdNodeActivate.
     *
     * @see MNcdNodeActivate
     */
    ENcdNodeActivateUid = 1044,
    
    /**
     * Interface id of MNcdBundle.
     *
     * @see MNcdBundle.
     */
    ENcdBundleUid = 1045,
    
    /**
     * Interface id of MNcdDebugInformation.
     *
     * @see MNcdDebugInformation.
     */
    ENcdDebugInformationUid = 1046,
    
    /**
     * Interface id of MNcdConfigurationData.
     *
     * @see MNcdConfigurationData.
     */
    ENcdConfigurationDataUid = 1047,
       

    /**
     * Interface id of MNcdInstalledContent.
     *
     * @see MNcdInstalledContent
     */
    ENcdInstalledContentUid = 1049,
    

    /**
     * Interface id of MNcdInstalledApplication.
     *
     * @see MNcdInstalledApplication
     */
    ENcdInstalledApplicationUid = 1050,


    /**
     * Interface id of MNcdInstalledTheme.
     *
     * @see MNcdInstalledTheme
     */
    ENcdInstalledThemeUid = 1051,


    /**
     * Interface id of MNcdInstalledFile.
     *
     * @see MNcdInstalledFile
     */
    ENcdInstalledFileUid = 1052,
    
    
    /**
     * Interface id of MNcdFavoriteManager.
     *
     * @see MNcdFavoriteManager
     */
    ENcdFavoriteManagerUid = 1053,
    
    
    /**
     * Interface id of MNcdNodeChildOfTransparent.
     *
     * @see MNcdNodeChildOfTransparent
     */
    ENcdNodeChildOfTransparentUid = 1054,
    
    
    /**
     * Interface id of MNcdNodeFavorite.
     *
     * @see MNcdNodeFavorite
     */
    ENcdNodeFavoriteUid = 1055,
    
    
    /**
     * Interface id of MNcdNodeseen.
     *
     * @see MNcdNodeSeen
     */
    ENcdNodeSeenUid = 1056,
    
    
    /**
     * Interface id of MNcdNodeSeenFolder.
     *
     * @see MNcdNodeSeenFolder
     */
    ENcdNodeSeenFolderUid = 1057,
    
    
    /**
     * Interface id of MNcdPoller.
     *
     * @see MNcdPoller
     * @deprecated
     */
    ENcdPollerUid = 1058,
    
    /**
     * Interface id of MNcdSendHttpRequestOperation
     *
     * @see MNcdSendHttpRequestOperation
     */
    ENcdSendHttpRequestOperationUid = 1059,
    
    /**
     * Interface id of MNcdServerReportManager
     *
     * @see MNcdServerReportManager
     */
    ENcdServerReportManagerUid = 1060,
    
    /**
     * Interface id of MNcdServerReportOperation
     *
     * @see MNcdServerReportOperation
     */
    ENcdServerReportOperationUid = 1061
    };

#endif // NCD_INTERFACE_IDS_H
