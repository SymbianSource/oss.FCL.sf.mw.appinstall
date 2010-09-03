/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Constants and definitions.
*
*/


#ifndef CATALOGS_CONSTANTS_H
#define CATALOGS_CONSTANTS_H

#include <e32base.h>
#include <e32msgqueue.h>

#include "catalogsuids.h"

// Replaced with USE_BUILD_SCRIPT when using build script
#define DUMMY_DEFINE


/**
 * Catalogs engine mutex name. All engine object instances have a handle to the mutex.
 * This can be used to determine if there are engine objects in use.
 */
#ifdef USE_BUILD_SCRIPT
_LIT( KCatalogsEngineMutex, "ncd-engine-mutex_APP_NAME_POSTFIX" );
#else
_LIT( KCatalogsEngineMutex, "ncd-engine-mutex_20019119" ); 
#endif // USE_BUILD_SCRIPT

/**
 * Note on RProperty usage.
 * RProperty is defined by category and key.
 * 
 * The engine server side process defines RProperty that is defined
 * to have category value equal to SID of the process.
 * Two kinds of info are published using RProperty. 
 * - general settings (currently only maintenance lock status)
 * - client specific connection activity indicator
 * 
 * Connection activity uses RProperty with key that is same as client SID.
 * Because both information are published within the same category, we must ensure
 * that there are no conflicts in key values. To ensure this, following assumptions
 * about keys are used:
 * - all settings values (such as maintenance lock) must be from protected UID
 * range class 0 (0x00000000 - 0x0FFFFFFF)
 * - all engine clients must use UIDs from class 2 (0x20000000 - 0x2FFFFFFF)
 */

/** Catalogs engine property category */
const TUid KCatalogsEnginePropertyCategory = { KCatalogsServerUid };

/** 
 * Catalogs engine maintenance lock property key. Written when locking/releasing
 * (0 = released).
 */
const TUint KCatalogsEnginePropertyKeyMaintenanceLock = 1;

/**
 * Catalogs update message queue name format string. Global message queue with this
 * name is created 
 */
_LIT( KCatalogsUpdateQueueNameFormat, "CatalogsUpdate-%08x" );

/** Catalogs OTA update message queue message size */
const TInt KCatalogsUpdateQueueMessageSize = RMsgQueueBase::KMaxLength;
const TInt KCatalogsUpdateQueueSlotCount = 32;

/** Catalogs engine ECom implementation uid */
const TUid KCCatalogsEngineImplUid = { KCatalogsEngineImplementationUid };

/** Catalogs OTA update information maximum field sizes */
const TInt KCatalogsUpdateInformationMaxSize = 3*1024;
const TInt KCatalogsUpdateTargetMaxSize = 256;
const TInt KCatalogsUpdateIdMaxSize = 1024;
const TInt KCatalogsUpdateVersionMaxSize = 256;
const TInt KCatalogsUpdateUriMaxSize = 1024;


/**
 * Mime type match strings. 
 */

_LIT( KMimeTypeMatchSymbianInstall,   "application/vnd.symbian.install" );
_LIT( KMimeTypeMatch1JavaApplication, "*java?archive" );
_LIT( KMimeTypeMatch2JavaApplication, "application/java-archive" );
_LIT( KMimeTypeMatchRealMedia,        "application/vnd.rn-realmedia" );
_LIT( KMimeTypeMatchApplicationStream,"application/octet-stream" );
_LIT( KMimeTypeMatchApplication,      "application/*" );

_LIT( KMimeTypeMatchSis,  "x-epoc/x-app268436505" );
_LIT( KMimeTypeMatchSisx, "x-epoc/x-sisx-app" );


_LIT( KMimeTypeMatchHtml, "text/html" );
_LIT( KMimeTypeMatchTxt,  "text/plain" );
_LIT( KMimeTypeMatchCod,  "text/x-co-desc" );
_LIT8( KMimeTypeXml, "text/xml" );

_LIT( KMimeTypeMatchDrmMessage,     "application/vnd.oma.drm.message" );
_LIT( KMimeTypeMatchDrmRightsXml,   "application/vnd.oma.drm.rights+xml" );
_LIT( KMimeTypeMatchDrmRightsWbxml, "application/vnd.oma.drm.rights+wbxml" );
_LIT8( KMimeTypeMatchDrmRightsXml8,   "application/vnd.oma.drm.rights+xml" );
_LIT8( KMimeTypeMatchDrmRightsWbxml8, "application/vnd.oma.drm.rights+wbxml" );
_LIT( KMimeTypeMatchDrmContent,     "application/vnd.oma.drm.content" );
_LIT8( KMimeTypeMatchDrm8,            "application/vnd.oma.drm.*" );

_LIT( KMimeTypeMatchOdd, "application/vnd.oma.dd*" );
_LIT8( KMimeTypeMatchOdd8, "application/vnd.oma.dd*" );

_LIT( KMimeTypeMatchWidget, "application/x-nokia-widget" );

_LIT( KHttpMatchString, "http://*" );

// Descriptor types
_LIT( KDescriptorTypeOdd, "dd" );
_LIT( KDescriptorTypeJad, "jad" );

_LIT( KMimeTypeMatchJad, "text/vnd.sun.j2me.app-descriptor" );
_LIT8( KMimeTypeMatchJad8, "text/vnd.sun.j2me.app-descriptor" );

_LIT( KDirectorySeparator, "\\" );

// List granularity used in initializing various lists and arrays.
const TInt KListGranularity( 8 );

// CBufFlat expansion size, should not be very small for good 
// performance.
const TInt KBufExpandSize( 256 );

// Granularity for CCatalogsStringManager's string arrays
const TInt KStringArrayGranularity( 256 );


// Query id for payment method query.
_LIT( KQueryIdPaymentMethod, "deadbeef" );


// Engine configuration file
_LIT( KCatalogsConfigFile, "config.xml" );

// Download manager root dir
_LIT( KCatalogsDownloadMgrPath, "C:\\system\\dmgr\\%S\\downloads\\*.*" );

// Number of attempts before we give up on connecting to download manager
const TInt KCatalogsDlMgrConnectRetryAttempts = 5;

// tenth of a second, the time between download manager connect attemps
const TInt KCatalogsDlMgrConnectRetryInterval = 100000; 

// File extension for widget
_LIT( KWidgetExtension, ".wgz" );

// File extension for native packages
_LIT( KNativeExtension, ".sis" );

// Default widget version number
_LIT( KDefVersion, "000" );

#endif // CATALOGS_CONSTANTS_H
