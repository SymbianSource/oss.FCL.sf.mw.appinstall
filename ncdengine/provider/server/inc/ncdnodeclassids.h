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
* Description:   Contains enumeration values for the node classes
*
*/
	

#ifndef NCD_NODE_CLASS_IDS_H
#define NCD_NODE_CLASS_IDS_H

namespace NcdNodeClassIds
    {
    
    /**
     *
     */
    enum TNcdNodeClassType
        {
        /**
         *
         */
        ENcdNode,

        /**
         *
         */
        ENcdMetaData,       
        
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
        ENcdNodeUserData,

        /**
         * 
         */
        ENcdSubscriptionsData
        
        };
        
        
    /**
     * 
     */
    enum TNcdNodeClassId
        {
        
        /**
         * This value can be used to inform that the
         * object in turn was NULL when it was supposed to be
         * internalized. So, when internalizing the pointer value
         * of the object may set NULL and internalization may continue
         * to read data for the next object. This value may become handy
         * if the stream contains data for multiple different objects
         * that are identified with these class ids. 
         */
        ENcdNullObjectClassId,
        
        // Node class ids
        
        /**
         *
         */
        ENcdRootNodeClassId,
        
        /**
         *
         */
        ENcdSearchRootNodeClassId,
        
        /**
         * 
         */
        ENcdFolderNodeClassId,

        /**
         * 
         */
        ENcdTransparentFolderNodeClassId,
        
        /**
         *
         */
        ENcdBundleFolderNodeClassId,
        
        /**
         * 
         */
        ENcdSearchFolderNodeClassId,
         
        /**
         * 
         */
        ENcdItemNodeClassId,
        
        /**
         * 
         */
        ENcdSearchItemNodeClassId,
        
        /**
         *
         */
        ENcdSupplierNodeClassId,
        
        /**
         * 
         */
        ENcdSearchBundleNodeClassId,


        // Node link class ids

        /**
         * 
         */
        ENcdFolderNodeLinkClassId,
         
        /**
         * 
         */
        ENcdItemNodeLinkClassId,
        
        /**
         *
         */
        ENcdSupplierNodeLinkClassId,

         
         // Node metadata class ids

        /**
         * 
         */
        ENcdFolderNodeMetaDataClassId,
         
        /**
         * 
         */
        ENcdItemNodeMetaDataClassId,
        
        /**
         *
         */
        ENcdSupplierNodeMetaDataClassId,


        // Misc class ids
         
        /**
         * 
         */
        ENcdNodeIconClassId,

        /**
         * 
         */
        ENcdNodeScreenshotClassId,

        /**
         * 
         */
        ENcdNodeSkinClassId,

        /**
         * 
         */
        ENcdNodeDisclaimerClassId,

        /**
         * 
         */
        ENcdNodeMoreInfoClassId,
               
        
        // Other


        /**
         * 
         */
        ENcdNodeIdentifierClassId,
        
        /**
         * 
         */
        ENcdChildEntityClassId,

        /**
         * 
         */
        ENcdNodeDownloadClassId,

        /**
         * 
         */
        ENcdNodeUserDataClassId,
                
        /**
         * 
         */
        ENcdNodeSearchClassId,

        /**
         * 
         */
        ENcdNodeUpgradeClassId,

        /**
         * 
         */
        ENcdNodeDependencyClassId,

        /**
         * 
         */
        ENcdNodeDependecyClassId,

        /**
         * 
         */
        ENcdNodePurchaseOptionClassId,

        /**
         * 
         */
        ENcdNodeInstallClassId,

        /**
         * 
         */
        ENcdNodeContentInfoClassId,

        /**
         * 
         */
        ENcdNodeUriContentClassId,

        /**
         * 
         */
        ENcdNodePreviewClassId,
        
        
        /**
         * 
         */
        ENcdCacheCleanerClassId
        };
        
    }	
	
#endif //  NCD_NODE_CLASS_IDS_H
