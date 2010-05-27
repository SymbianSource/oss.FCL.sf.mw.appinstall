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
* Description:   Capability definitions
*
*/


#ifndef NCD_CAPABILITIES_H
#define NCD_CAPABILITIES_H

/**
 * Capabilities defined in the Preminet protocol
 *
 * Clients should add the capabilities they support by using 
 * MNcdProvider::AddConfigurationL(). NcdConfigurationKeys::Capability must be
 * used as the key
 *
 * @see Preminet protocol documentation
 */    
namespace NcdCapabilities
    {
    _LIT( KEmbeddedSessions, "embeddedSessions" );
    _LIT( KSearch, "search" );
    _LIT( KActivities, "activities" );
    _LIT( KSubscriptions, "subscriptions" );
    _LIT( KLegacySubscriptions, "legacySubscriptions" );
    _LIT( KUpload, "upload" );
    _LIT( KReDownload, "reDownload" );
    _LIT( KGpsLocation, "gpsLocation" );
    _LIT( KDelayedTransfer, "delayedTransfer" );
    _LIT( KAutoDownload, "autoDownload" );
    _LIT( KXmlResponseFiltering, "xmlResponseFiltering" );
    _LIT( KInstallationReport, "installationReport" );
    _LIT( KBrowseFiltering, "browseFiltering" );
    _LIT( KClientReview, "clientReview" );
    _LIT( KPromotionalEntities, "promotionalEntities" );
    _LIT( KDirectSchemeLinks, "directSchemeLinks" );
    _LIT( KIndirectSchemeLinks, "indirectSchemeLinks" );
    _LIT( KEntityRequest, "entityRequest" ); 
    _LIT( KDiffQuery, "diffQuery" ); 
    _LIT( KUpLevel, "upLevel" ); 
    _LIT( KRemoteContent, "remoteContent" ); 
    _LIT( KPredefinedSearch, "predefinedSearch" ); 
    _LIT( KPredefinedPurchase, "predefinedPurchase" ); 
    _LIT( KCacheExpiration, "cacheExpiration" ); 
    _LIT( KMultiPurchase, "multiPurchase" ); 
    _LIT( KMultiDownload, "multiDownload" ); 
    _LIT( KBasicQueries, "basicQueries" ); 
    _LIT( KSourceScopeCookies, "sourceScopeCookies" ); 
    _LIT( KSimScopeCookies, "simScopeCookies" ); 
    _LIT( KCancelPurchase, "cancelPurchase" ); 
    _LIT( KSmsAction, "smsAction" ); 
    _LIT( KDrmClientDownload, "drmClientDownload" ); 
    _LIT( KGiftPurchase, "giftPurchase" );
    _LIT( KUniversalSubscriptions, "universalSubscriptions" );
    _LIT( KDownloadReport, "downloadReport" );
    }
    
#endif // NCD_CAPABILITIES_H
