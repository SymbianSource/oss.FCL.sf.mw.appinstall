/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains NcdProviderDefines namespace
*
*/


#ifndef NCD_PROVIDER_DEFINES_H
#define NCD_PROVIDER_DEFINES_H


#include <e32def.h>
#include <e32cmn.h>
#include <f32file.h>

/**
 *  NcdProviderDefines
 *
 *  @lib
 *  @since S60 v3.2 
 */
namespace NcdProviderDefines
    {
    // Node definitions
    
    // This is used when the ram caches sizes are checked
    // for example to check if the cache cleanup is required.
    // If the elements in the cache exceed the max count, then
    // elements that are not in any other use are removed from
    // the cache until the cache count has reached 
    // KNodeRamCacheDelimiterCount or until no free elements exist
    // in the cache. 
    // So, KNodeRamCacheDelimiterCount should be set here to be
    // less than KNodeRamCacheMaxCount.
    const TInt KNodeRamCacheMaxCount = 50;
    const TInt KNodeRamCacheDelimiterCount = 25;

    // Maximum length of client ID.
    const TInt KClientIdMaxLength = 24;


    // Maximum number of previews per client
    const TInt KMaxClientPreviews = 3;


    // Database definitions
    
    _LIT( KPurchaseHistoryDirectory, "ph\\" );    

    _LIT( KDatabaseExtension, ".db" );
    
    // Default UID for client databases
    _LIT( KDefaultDatabaseUid, "Db0" );

    // UID for download report databases
    _LIT( KReportDatabaseUid, "Reports" );

    // Client definitions
    
    // Default UID for file storages in a namespace
    // There's no need to change this unless more than one filestorage
    // is needed for a namespace
    _LIT( KDefaultFileStorageUid, "Fs0" );

    // Namespaces that are also used as db directory names
    _LIT( KRootNodeNameSpace, "r_" );
    
    _LIT( KSearchRootNameSpace, "s_" );
    
    
    // Namespace used for serialized content downloads
    _LIT( KDownloadNamespace, "Dl0" );
    
    _LIT( KSubscriptionNamespace, "sub_" );
 
    _LIT( KProviderStorageNamespace, "p_" );
    
    _LIT( KPreviewStorageNamespace, "pr_" );
    
    _LIT( KDataNamespace, "data" );
    
    _LIT( KTempNamespace, "temp" );
    
    // This default byte size that may be used for the db cache cleaner.
    // If db size exeeds this value, then db cleaning should be
    // started. Notice, this is bytes not kilobytes.
    const TInt KDbDefaultMaxByteSize = 1000000;   


    // Protocol defines
    const TInt KNeverExpires = -1;
    
    
    /**
     * Hardcoded Master Server URI
     */
    //_LIT( KMasterServerUri, "http://lynx.ionific.com:9084/" );
    _LIT( KMasterServerUri, "" );
    
    /**
     * Types used for database dataitems in provider's storage
     */
    enum TNcdDatabaseDataType 
        {
        ENcdReservedTypes = 20000,
        ENcdBinaryData = ENcdReservedTypes,
        ENcdMasterServerAddress,
        ENcdContextData,
        ENcdAccessPointManager,
        ENcdPreviewManager,
        ENcdDownloadData,
        ENcdFavoriteManager,
        ENcdNodeSeenInfo
        };

    // Namespace used to differentiate between CDB and CGW server details
    // (cookies, capabilities)    
    _LIT( KConfigNamespace, "cfg" );
        
    _LIT( KNcdTempDir, "temp" );    

    // File used to check if the startup sequence was successful or not
    _LIT( KNcdProviderStartupFile, "ncdstartup" );
    
    _LIT( KNcdProviderShutdownFile, "ncdshutdown" );
    
    _LIT( KNcdDatabaseVersionFile, "ncddbversions.cfg" );
    
    /**
     * Maximum number of failed shutdowns before the dbs are cleaned
     */
    const TInt KNcdMaxFailedShutdowns = 3;
    
    /**
     * String types used to index CCatalogsStringManager
     */
    enum TNcdStringTypes 
        {
        ENcdStringNodeId = 0,
        ENcdStringNamespace,
        // Server URI and Namespace are often the same so this
        // can save some space
        ENcdStringServerUri = ENcdStringNamespace        
        };
        
    const TInt KNcdBufferExpandSize = 4096;
    
    const TUint KNcdDefaultSearchRecursionDepth = 2;
    const TUint KNcdMaxSearchRecursionDepth = 5;
    
    const TUint KNcdSharableFileOpenFlags = EFileShareReadersOrWriters;
    
    /**
     * These are used for separating objects and end of data
     * in a data stream
     */
    enum TNcdStreamDataState
        {
        ENcdStreamDataObject,
        ENcdStreamDataEnd
        };
    }

#endif // NCD_PROVIDER_DEFINES_H

