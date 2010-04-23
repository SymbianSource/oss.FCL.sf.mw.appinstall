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
* Description:  
*
*/


#ifndef NCD_CONFIGURATION_KEYS_H
#define NCD_CONFIGURATION_KEYS_H

/**
 * Configurations supported by the provider
 *
 * Clients can set values for these with
 * MNcdProvider::AddConfigurationL()
 *
 * Keys mentioned here are mostly used for filling specific
 * elements/attributes in configuration requests.
 *
 * Keys that are not listed here, are always added 
 * to software details in configuration requests.
 */
namespace NcdConfigurationKeys
    {
    /**
     * List at least all of the keys that are used for
     * filling specific attributes/elements in configuration 
     * requests. Preferably list all so that clients don't have to know
     * the exact configuration attribute/element names.
     */
    
    /** 
     * The following keys MUST be set by the client before
     * any requests are sent
     */
     
     /**
      * Maximum disk space that the node cache is allowed to use
      *
      * @note This is not sent in any requests
      */
    _LIT( KMaxStorageSize, "max-storage-size" );
    
    /**
     * Client's version number
     */
    _LIT( KSoftwareVersion, "version" );
    
    /**
     * Type of the client software.
     * 
     * This can used to denote a client variant or the general type of the client
     */
    _LIT( KSoftwareType, "type" );


    /**
     * Master server URI, also known as CDB URI
     */
    _LIT( KMasterServer, "master-server" );




    /**
     * These keys SHOULD always be set
     */
     
     
    /**
     * Client provisioning
     *
     * @note Sent as a software detail in the client's client-element
     */
    _LIT( KProvisioning, "provisioning" );
    
    /**
     * Client language
     */
    _LIT( KSoftwareLanguage, "language" );


    /**
     * Key for setting display properties
     *
     * Format for the value: 
     * "'display number' 'width' 'height' 'number of colors'"
     *
     * eg. value="1 240 320 256"
     *
     * @note Display number is for future use. It should be 1 for now.
     */
    _LIT( KDisplay, "display" );
    
    
    /**
     * These keys MAY BE set
     */
    
     /**
      * Client-specific ID of the client skin.
      */
    _LIT( KSkinId, "skinId" );  
    
    /**
     * Drive letter where the client is installed on, eg. C:
     */  
    _LIT( KInstallDrive, "installDrive" );        
    
   /**
    * If this application-specific Online Info url is not set, the corresponding 
    * menu option is not visible
    */
    _LIT( KOnlineInfoUrl, "online-info-url");
    
    /**
     * Capability key can be added multiple times with different values
     * @see NcdCapabilities     
     */
    _LIT( KCapability, "capability" );


    /**
     * These keys MAY BE set for hardware information
     */

    /**
     * Device product code can be set for the hardware information.
     * This information is sent in requests to the server.
     */
    _LIT( KDeviceProductCode, "productCode" );

    }
    
#endif // NCD_CONFIGURATION_KEYS_H
