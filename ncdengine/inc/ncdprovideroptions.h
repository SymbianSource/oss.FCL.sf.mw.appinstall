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
* Description:   Contains provider options
*
*/


#ifndef NCD_PROVIDER_OPTIONS_H
#define NCD_PROVIDER_OPTIONS_H

/**
 * Bitflags that can be used to control provider behaviour when it is 
 * being created.
 * 
 * The default provider behaviour is the opposite of these flags.
 * 
 * @see MCatalogsEngine::CreateProviderL
 *  
 */  
enum TNcdProviderOptions
    {
    /**
     * Disable node cache cleaner 
     */ 
    ENcdProviderDisableNodeCacheCleaner = 1,
    
    /**
     * Enable client cache cleaning when the SIM is changed
     */
    ENcdProviderEnableSimChangeCacheCleaning = 2,
    
    /**
     * Enable IMEI sending in configuration requests
     */
    ENcdProviderSendImei = 4,
    
    /**
     * Disable HEAD request from downloads
     * 
     * @note Names of downloaded files won't be correct 
     * @note If content's MIME type is received as OMA DD, JAD or DRM content in
     * downloadableContent-element then HEAD request is sent.
     */
    ENcdProviderDisableHttpHeadRequest = 8
    
    };

#endif // NCD_PROVIDER_OPTIONS_H
