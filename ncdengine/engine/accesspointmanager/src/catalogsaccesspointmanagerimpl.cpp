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
* Description:   Class CCatalogsAccessPointManager implementation
*
*/


#include "catalogsaccesspointmanagerimpl.h"

#ifdef _0
#include <ApUtils.h>
#include <ApSelect.h> 
#include <ApDataHandler.h>
#endif

#include <badesca.h>

#include "catalogsaccesspoint.h"
#include "catalogsaccesspointsettings.h"
#include "catalogsaccesspointmap.h"
#include "catalogsclientaccesspointdata.h"
#include "ncdstorage.h"
#include "ncdstoragemanager.h"
#include "ncdproviderdefines.h"
#include "ncdstorageitem.h"
#include "ncddatabasestorage.h"
#include "ncd_cp_detail.h"
#include "ncd_cp_clientconfiguration.h"
#include "ncdprotocolutils.h"
#include "ncdnodeidentifier.h"
#include "ncdnodeimpl.h"
#include "ncdnodelink.h"
#include "ncdproviderdefines.h"
#include "ncdnodeidentifiereditor.h"
#include "ncdsearchnodefolder.h"
#include "ncdutils.h"
#include "ncdgeneralmanager.h"
#include "catalogshttpsessionmanagerimpl.h"
#include "catalogsnetworkmanager.h"
#include "catalogsremoveaccesspointshutdownoperation.h"
#include "catalogsclientserverserver.h"

#include "catalogsdebug.h"

// literals used in client configuration protocol
_LIT(KAccessPoints, "accessPoints");
_LIT(KContentSources, "contentSources");
_LIT(KNamespace, "namespace");
_LIT(KId, "id");
_LIT(KCatalogBundle, "catalogBundle");
_LIT(KSubCatalogs, "subCatalogs");
_LIT(KDownloadAccessPoint, "downloadingAccessPoint");
_LIT(KBrowseAccessPoint, "browsingAccessPoint");
_LIT(KPurchaseAccessPoint, "purchasingAccessPoint");

#ifdef _0
// literals used for access point settings in client configuration protocol (in alphabetical order)
_LIT(KApName, "name");


_LIT(KApCdmaDisablePlainTextAuth, "EApCdmaDisablePlainTextAuth");
_LIT(KApCdmaIfAuthName, "EApCdmaIfAuthName");
_LIT(KApCdmaIfAuthPassword, "EApCdmaIfAuthPassword");
_LIT(KApCdmaIfName, "EApCdmaIfName");
_LIT(KApCdmaIfNetworks, "EApCdmaIfNetworks");
_LIT(KApCdmaIpAddr, "EApCdmaIpAddr");
_LIT(KApCdmaIpAddrFromServer, "EApCdmaIpAddrFromServer");
_LIT(KApCdmaIpDnsAddrFromServer, "EApCdmaIpDnsAddrFromServer");
_LIT(KApCdmaIpGateway, "EApCdmaIpGateway");
_LIT(KApCdmaIPNameServer1, "EApCdmaIPNameServer1");
_LIT(KApCdmaIPNameServer2, "EApCdmaIPNameServer2");
_LIT(KApCdmaIpNetMask, "EApCdmaIpNetMask");
_LIT(KApGprsAccessPointName, "EApGprsAccessPointName");
_LIT(KApGprsDisablePlainTextAuth, "EApGprsDisablePlainTextAuth");
_LIT(KApGprsIfAuthName, "EApGprsIfAuthName");
_LIT(KApGprsIfAuthPassword, "EApGprsIfAuthPassword");
_LIT(KApGprsIfName, "EApGprsIfName");
_LIT(KApGprsIfNetworks, "EApGprsIfNetworks");
_LIT(KApGprsIfParams, "EApGprsIfParams");
_LIT(KApGprsIfPromptForAuth, "EApGprsIfPromptForAuth");
_LIT(KApGprsIpAddr, "EApGprsIpAddr");
_LIT(KApGprsIpAddrFromServer, "EApGprsIpAddrFromServer");
_LIT(KApGprsIpDnsAddrFromServer, "EApGprsIpDnsAddrFromServer");
_LIT(KApGprsIpGateway, "EApGprsIpGateway");
_LIT(KApGprsIPNameServer1, "EApGprsIPNameServer1");
_LIT(KApGprsIPNameServer2, "EApGprsIPNameServer2");
_LIT(KApGprsIpNetMask, "EApGprsIpNetMask");
_LIT(KApGprsPdpAddress, "EApGprsPdpAddress");
_LIT(KApHasProxySettings, "EApHasProxySettings");
_LIT(KApIapChargecard, "EApIapChargecard");
_LIT(KApIapName, "EApIapName");
_LIT(KApIapServiceId, "EApIapServiceId");
_LIT(KApIapServiceType, "EApIapServiceType");
_LIT(KApIP6NameServer1, "EApIP6NameServer1");
_LIT(KApIP6NameServer2, "EApIP6NameServer2");
_LIT(KApIspDefaultTelNumber, "EApIspDefaultTelNumber");
_LIT(KApIspDescription, "EApIspDescription");
_LIT(KApIspDisablePlainTextAuth, "EApIspDisablePlainTextAuth");
_LIT(KApIspIfAuthName, "EApIspIfAuthName");
_LIT(KApIspIfAuthPass, "EApIspIfAuthPass");
_LIT(KApIspIfCallbackInfo, "EApIspIfCallbackInfo");
_LIT(KApIspIfName, "EApIspIfName");
_LIT(KApIspIfNetworks, "EApIspIfNetworks");
_LIT(KApIspIfParams, "EApIspIfParams");
_LIT(KApIspInitString, "EApIspInitString");
_LIT(KApIspIPAddr, "EApIspIPAddr");
_LIT(KApIspIPAddrFromServer, "EApIspIPAddrFromServer");
_LIT(KApIspIPDnsAddrFromServer, "EApIspIPDnsAddrFromServer");
_LIT(KApIspIPGateway, "EApIspIPGateway");
_LIT(KApIspIPNameServer1, "EApIspIPNameServer1");
_LIT(KApIspIPNameServer2, "EApIspIPNameServer2");
_LIT(KApIspIPNetMask, "EApIspIPNetMask");
_LIT(KApIspIspType, "EApIspIspType");
_LIT(KApIspLoginName, "EApIspLoginName");
_LIT(KApIspLoginPass, "EApIspLoginPass");
_LIT(KApIspLoginScript, "EApIspLoginScript");
_LIT(KApIspName, "EApIspName");
_LIT(KApLanBearerName, "EApLanBearerName");
_LIT(KApNetworkName, " EApNetworkName");
_LIT(KApProxyExceptions, "EApProxyExceptions");
_LIT(KApProxyLoginName, "EApProxyLoginName");
_LIT(KApProxyLoginPass, "EApProxyLoginPass");
_LIT(KApProxyPortNumber, "EApProxyPortNumber");
_LIT(KApProxyProtocolName, "EApProxyProtocolName");
_LIT(KApProxyServerAddress, "EApProxyServerAddress");
_LIT(KApProxyUseProxy, "EApProxyUseProxy");
_LIT(KApWapAccessPointName, "EApWapAccessPointName");
_LIT(KApWapCurrentBearer, "EApWapCurrentBearer");
_LIT(KApWapGatewayAddress, "EApWapGatewayAddress");
_LIT(KApWapIap, "EApWapIap");
_LIT(KApWapProxyPort, "EApWapProxyPort");
_LIT(KApWapSecurity, "EApWapSecurity");
_LIT(KApWapStartPage, "EApWapStartPage");
_LIT(KApWapWspOption, "EApWapWspOption");
_LIT(KApWlanIfAuthName, "EApWlanIfAuthName");
_LIT(KApWlanIfAuthPassword, "EApWlanIfAuthPassword");
_LIT(KApWlanIfNetworks, "EApWlanIfNetworks");
_LIT(KApWlanIpAddr, "EApWlanIpAddr");
_LIT(KApWlanIpAddrFromServer, "EApWlanIpAddrFromServer");
_LIT(KApWlanIpDnsAddrFromServer, "EApWlanIpDnsAddrFromServer");
_LIT(KApWlanIpGateway, "EApWlanIpGateway");
_LIT(KApWlanIPNameServer1, "EApWlanIPNameServer1");
_LIT(KApWlanIPNameServer2, "EApWlanIPNameServer2");
_LIT(KApWlanIpNetMask, "EApWlanIpNetMask");

_LIT(KWapWspOptionConnectionless, "EWapWspOptionConnectionless");
_LIT(KWapWspOptionConnectionOriented, "EWapWspOptionConnectionOriented");
#endif

CCatalogsAccessPointManager::CCatalogsAccessPointManager(
    CNcdGeneralManager& aGeneralManager ) :
    iGeneralManager( aGeneralManager ),
    iStorageManager( aGeneralManager.StorageManager() ), 
    iNodeManager( aGeneralManager.NodeManager() ),
    iCheckAp( ETrue )
    {
    }
    
CCatalogsAccessPointManager::~CCatalogsAccessPointManager()
    {
    DLTRACEIN((""));    

    TRAP_IGNORE( DestructL() );

    delete iClientAccessPointData;
    delete iFixedAp;
    }
    
CCatalogsAccessPointManager* CCatalogsAccessPointManager::NewL(
    CNcdGeneralManager& aGeneralManager )
    {
    CCatalogsAccessPointManager* apMan = 
        new (ELeave) CCatalogsAccessPointManager( aGeneralManager );
    CleanupStack::PushL( apMan );
    apMan->ConstructL();
    CleanupStack::Pop( apMan );
    return apMan;
    }
    
void CCatalogsAccessPointManager::ConstructL()
    {
    DLTRACEIN((""));
    CCatalogsHttpSessionManager::NetworkManagerL().AddObserverL( *this );
    }

void CCatalogsAccessPointManager::SetFixedApL(
    const RPointerArray<CNcdKeyValuePair>& aApDetails ) 
    {
    DLTRACEIN((""));
    DASSERT( !iFixedAp );
    
    // Create CCatalogsAccessPoint object and set the details to it.
    CCatalogsAccessPoint* ap = CCatalogsAccessPoint::NewLC( KNullDesC, KNullDesC );
    TInt detailCount = aApDetails.Count();
    for ( TInt i = 0; i < detailCount; i++ ) 
        {
        CNcdKeyValuePair* detail = aApDetails[ i ];
        SetApDetailL( *ap, detail->Key(), detail->Value() );
        }


    if ( !ap->Name().Length() ) 
        {
        DLERROR(("No accesspoint name given"));
        User::Leave( KErrArgument );
        }
        
    CreateAccessPointL( *ap );
    iFixedAp = ap;
    CleanupStack::Pop( ap );
    }


void CCatalogsAccessPointManager::GetFixedApL( TUint32& aAccessPointId )
    {
    DLTRACEIN((""));
    if ( iFixedAp ) 
        {
        // We need to check that the AP in Commsdb matches wanted settings
        if ( iCheckAp ) 
            {
            DLTRACE(("Checking AP"));
            // iCheckAp is set to EFalse in HandleAccessPointEventL 
            // when an AP has been raised

            // Only parameter that matters here is aAccessPointId
            AccessPointIdL( 
                KNullDesC,
                EBrowse,
                TUid(),
                aAccessPointId );
            }
        else
            {            
            aAccessPointId = iFixedAp->AccessPointId();
            DLTRACE(("AP ID: %u", aAccessPointId ));
            }
        }
    }
    

void CCatalogsAccessPointManager::ParseAccessPointDataFromClientConfL(
    const MNcdConfigurationProtocolClientConfiguration& aConfiguration,
    const TUid& /*aClientUid*/,
    TBool aIgnoreFixedAp, 
    RArray<TUint32>* aCreatedAps ) 
    {
    DLTRACEIN(("Starting to parse access point data from client configuration"));    
    
    if ( iFixedAp && !aIgnoreFixedAp ) 
        {
        DLINFO(("Fixed AP used, no need to parse AP data"));
        return;
        }
    
    for( TInt i = 0 ; i < aConfiguration.DetailCount() ; i++ )
        {
        const MNcdConfigurationProtocolDetail& detail = aConfiguration.DetailL( i );
        if (detail.Id() == KAccessPoints) 
            {
            // parse access point details
            CreateAccessPointsFromClientConfL( detail, aCreatedAps );
            }
        else if (detail.Id() == KContentSources) 
            {
            const RPointerArray<MNcdConfigurationProtocolDetail>& csDetails = detail.Details();
            for (TInt i = 0; i < csDetails.Count(); i++) 
                {
                MNcdConfigurationProtocolDetail* csDetail = csDetails[i];
                
                if (csDetail->GroupId() == KNullDesC && csDetail->Id() == KNamespace)
                    {
                    const TDesC& nameSpace = csDetail->Value();
                    const TDesC* id = NULL;
                    const RPointerArray<MNcdConfigurationProtocolContent>& nsContents = 
                        csDetail->Contents();
                    for (TInt i = 0; i < nsContents.Count(); i++) 
                        {
                        MNcdConfigurationProtocolContent* content = nsContents[i];
                        if (content->Key() == KId) 
                            {
                            id = &content->Value();
                            break;
                            }
                        }
                                        
                    const RPointerArray<MNcdConfigurationProtocolDetail>& nsDetails = csDetail->Details();                

                    if (nsDetails.Count() > 0) 
                        {
                        MNcdConfigurationProtocolDetail* uriDetail = nsDetails[0];
                        const RPointerArray<MNcdConfigurationProtocolDetail>& apDetails = uriDetail->Details();
                        
                        for (TInt i = 0; i < apDetails.Count(); i++) 
                            {
                            MNcdConfigurationProtocolDetail* apDetail = apDetails[i];
                            const TDesC& accessPointId = apDetail->Value();
                            MCatalogsAccessPointManager::TAction action;
                            TBool unknown = EFalse;
                            if (apDetail->Id() == KDownloadAccessPoint)
                                {
                                action = MCatalogsAccessPointManager::EDownload;
                                }
                            else if (apDetail->Id() == KBrowseAccessPoint) 
                                {
                                action = MCatalogsAccessPointManager::EBrowse;
                                }
                            else if (apDetail->Id() == KPurchaseAccessPoint) 
                                {
                                action = MCatalogsAccessPointManager::EPurchase;
                                }
                            else 
                                {
                                DLWARNING((_L("Unknown detail id: %S"),&apDetail->Id()));
                                unknown = ETrue;
                                }
                                
                            if (!unknown && id) 
                                {
                                MapAccessPointL(
                                    nameSpace, *id, action, accessPointId );
                                }
                            else if (!unknown) 
                                {
                                MapAccessPointL(
                                    nameSpace, action, accessPointId );
                                }
                            }
                        }
                    }
                    
                else if (csDetail->GroupId() == KCatalogBundle) 
                    {
                    const RPointerArray<MNcdConfigurationProtocolDetail>& bundleDetails =
                        csDetail->Details();
                    for (TInt i = 0; i < bundleDetails.Count(); i++) 
                        {
                        MNcdConfigurationProtocolDetail* bundleDetail = bundleDetails[i];
                        if (bundleDetail->GroupId() != KSubCatalogs) 
                            {
                            continue;
                            }
                            
                        // find the subcatalog ID
                        const TDesC* id = NULL;
                        const RPointerArray<MNcdConfigurationProtocolContent>& subCatalogContents =
                            bundleDetail->Contents();
                        for (TInt i = 0; i < subCatalogContents.Count(); i++) 
                            {
                            if (subCatalogContents[i]->Key() == KId) 
                                {
                                id = &subCatalogContents[i]->Value();
                                break;
                                }
                            }
                                
                        const TDesC& nameSpace = bundleDetail->Value();
                        const RPointerArray<MNcdConfigurationProtocolDetail>& subCatalogDetails =
                            bundleDetail->Details();
                        if (subCatalogDetails.Count() > 0) 
                            {
                            MNcdConfigurationProtocolDetail* scDetail = subCatalogDetails[0];
                            const RPointerArray<MNcdConfigurationProtocolDetail>& apDetails =
                                scDetail->Details();
                            for (TInt i = 0; i < apDetails.Count(); i++) 
                                {
                                MNcdConfigurationProtocolDetail* apDetail = apDetails[i];
                                const TDesC& accessPointId = apDetail->Value();
                                MCatalogsAccessPointManager::TAction action;
                                TBool unknown = EFalse;
                                if (apDetail->Id() == KDownloadAccessPoint) 
                                    {
                                    action = EDownload;
                                    }
                                else if (apDetail->Id() == KBrowseAccessPoint) 
                                    {
                                    action = EBrowse;
                                    }
                                else if (apDetail->Id() == KPurchaseAccessPoint) 
                                    {
                                    action = EPurchase;
                                    }
                                else 
                                    {
                                    DLWARNING((_L("Unknown detail id: %S"), &apDetail->Id()));
                                    unknown = ETrue;
                                    }
                                
                                if (!unknown && id) 
                                    {
                                    MapAccessPointL(
                                        nameSpace, *id, action, accessPointId );
                                    }
                                else if (!unknown) 
                                    {
                                    MapAccessPointL(
                                        nameSpace, action, accessPointId );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    SaveToStorageL();
    DLTRACEOUT(("Access point data from client configuration parsed"));
    }            

    
void CCatalogsAccessPointManager::CreateAccessPointsFromClientConfL(
    const MNcdConfigurationProtocolDetail& aDetail,     
    RArray<TUint32>* aCreatedAps ) 
    {
    DLTRACEIN(("Creating access points from client configuration"));
    const RPointerArray<MNcdConfigurationProtocolDetail>& apDetails = aDetail.Details();
    TInt apCount = apDetails.Count();
    
    for (TInt i = 0; i < apCount; i++) 
        {
        MNcdConfigurationProtocolDetail* apDetail = apDetails[i];
        CCatalogsAccessPoint* ap = CCatalogsAccessPoint::NewLC(apDetail->Id(), KNullDesC);
            
        const RPointerArray<MNcdConfigurationProtocolContent>& apContents = 
            apDetail->Contents();
        TInt contentCount = apContents.Count();
        for (TInt i = 0; i < contentCount; i++) 
            {
            MNcdConfigurationProtocolContent* content = apContents[i];
            SetApDetailL( *ap, content->Key(), content->Value() );
            }
        CreateAccessPointL( ap );
        CleanupStack::Pop( ap );
        
        if ( aCreatedAps ) 
            {
            DLTRACE(("Adding accesspoint id to aCreatedAps"));
            aCreatedAps->AppendL( ap->AccessPointId() );
            }
            
        }
    DLTRACEOUT(("Access points created from client configuration"));
    }


void CCatalogsAccessPointManager::CreateAccessPointL(
    CCatalogsAccessPoint* aSettings  )
    {
    CreateAccessPointL( *aSettings );        
    
    if ( !iClientAccessPointData ) 
        {
        iClientAccessPointData = CCatalogsClientAccessPointData::NewL();        
        }        

    iClientAccessPointData->AddAccessPointL( aSettings );
    }
    
void CCatalogsAccessPointManager::CreateAccessPointL( CCatalogsAccessPoint& aSettings )
    {
    DLTRACEIN(("Creating access point"));

    // Check if comms database already have access point with suitable settings.
    TUint32 iapId = 0;        
    if( MatchInCommsDbL( aSettings, iapId ) )
        {
        DLTRACE(("Matching AP found from comms DB, access point id: %d", iapId));
        // There is no need to create own access point settings to the comms database.
        aSettings.SetAccessPointId( iapId );
        }
        
    // Create new access point to the comms database
    else
        {
        DLTRACE(_L("Mathing AP not found from comms DB, creating new one"));
        CreateApToCommsDBL( aSettings, iapId );
        aSettings.SetAccessPointId( iapId );     
        // Mark this access point for deletition at the manager's destructor.
        // Fix for [#PRECLI-1596].
        aSettings.SetCreatedByManager();
        DLTRACE(("Created access point: %d", iapId));
        }
    DLTRACEOUT(("Access point created"));
    }    
    
    
void CCatalogsAccessPointManager::MapAccessPointL(
    const TDesC& aNameSpace,
    const CCatalogsAccessPointManager::TAction& aType,
    const TDesC& aApId ) 
    {
    DLTRACEIN((_L("Namespace: %S, Action: %d, access point id: %S "),
               &aNameSpace, aType, &aApId ));

    if ( !iClientAccessPointData ) 
        {
        iClientAccessPointData = CCatalogsClientAccessPointData::NewL();
        }
        
    iClientAccessPointData->MapAccessPointL( aNameSpace, aType, aApId );
    
    DLTRACEOUT((""));
    }

    
void CCatalogsAccessPointManager::MapAccessPointL(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const CCatalogsAccessPointManager::TAction& aAction,
    const TDesC& aApId ) 
    {
    DLTRACEIN((_L("Namespace: %S, CatalogId: %S, Action: %d, access point id: %S"),
               &aNameSpace, &aCatalogId, aAction, &aApId ));
    
    if ( !iClientAccessPointData ) 
        {
        iClientAccessPointData = CCatalogsClientAccessPointData::NewL();
        }
        
    iClientAccessPointData->MapAccessPointL(
        aNameSpace, aCatalogId, aAction, aApId );
    DLTRACEOUT((""));
    }

    
TInt CCatalogsAccessPointManager::AccessPointIdL(
    const TDesC& aNameSpace,
    const MCatalogsAccessPointManager::TAction& aAction,
    const TUid& aClientUid,
    TUint32& aAccessPointId ) 
    {
    DLTRACEIN((_L("Namespace: %S, Action: %d, clientUid: %d"), &aNameSpace, aAction, aClientUid.iUid));
    
    CCatalogsAccessPoint* accessPoint( NULL );
    CCatalogsClientAccessPointData* apData( NULL );
    TInt err = GetAccessPoint( accessPoint, apData );
    if ( err != KErrNone ) 
        {
        DLERROR(("Error: %d", err));
        return err;
        }

    if ( !accessPoint )
        {
        DASSERT( apData );
        accessPoint = apData->AccessPoint( aNameSpace, aAction );
        }

    return ValidateOrCreateAccessPointL( accessPoint, aAccessPointId );
    }
    

TInt CCatalogsAccessPointManager::AccessPointIdL(
    const TDesC& aNameSpace,
    const TDesC& aCatalogId,
    const MCatalogsAccessPointManager::TAction& aAction,
    const TUid& aClientUid,
    TUint32& aAccessPointId) 
    {
    DLTRACEIN((_L("Namespace: %S, CatalogId: %S, Action: %d, clientUid: %d"), &aNameSpace, &aCatalogId, aAction, aClientUid.iUid));
    
    CCatalogsAccessPoint* accessPoint( NULL );
    CCatalogsClientAccessPointData* apData( NULL );
    TInt err = GetAccessPoint( accessPoint, apData );
    if ( err != KErrNone ) 
        {
        DLERROR(("Error: %d", err));
        return err;
        }

    if ( !accessPoint )
        {
        DASSERT( apData );
        accessPoint = apData->AccessPointL( aNameSpace, aCatalogId, aAction );
        }

    return ValidateOrCreateAccessPointL( accessPoint, aAccessPointId );
    }


TInt CCatalogsAccessPointManager::AccessPointIdL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const MCatalogsAccessPointManager::TAction& aAction,
    const TUid& aClientUid,
    TUint32& aAccessPointId) 
    {
    DLTRACEIN((_L("Namespace: %S, NodeId: %S, Action: %d, clientUid: %d"),
               &aNodeIdentifier.NodeNameSpace(), &aNodeIdentifier.NodeId(), aAction,
               aClientUid.iUid));
    
    CCatalogsAccessPoint* accessPoint( NULL );
    CCatalogsClientAccessPointData* apData( NULL );
    TInt err = GetAccessPoint( accessPoint, apData );
    if ( err != KErrNone ) 
        {
        DLERROR(("Error: %d", err));
        return err;
        }

    if ( !accessPoint )
        {
        DASSERT( apData );
        accessPoint = FindAccessPointL(
            aNodeIdentifier, aAction, *apData );
        }

    return ValidateOrCreateAccessPointL( accessPoint, aAccessPointId );
    }


void CCatalogsAccessPointManager::HandleAccessPointEventL( 
    const TCatalogsConnectionMethod& /* aAp */,
    const TCatalogsAccessPointEvent& aEvent )
    {
    DLTRACEIN((""));

    if ( aEvent == ECatalogsAccessPointClosed ||
         aEvent == ECatalogsAccessPointReallyClosed ) 
        {
        DLTRACE(("Ap closed, need to check it when connecting"));
        // Connection has been closed, we want to ensure
        // that we use the correct AP settings even if
        // someone tampers with the existing AP
        iCheckAp = ETrue;
        }
    else
        {
        iCheckAp = EFalse;
        }
    }

    
CCatalogsAccessPoint* CCatalogsAccessPointManager::FindAccessPointL(
    const CNcdNodeIdentifier& aNodeIdentifier,
    const MCatalogsAccessPointManager::TAction& aAction,
    const CCatalogsClientAccessPointData& aAccessPointData ) 
    {
    DLTRACEIN((""));
    CCatalogsAccessPoint* accessPoint = NULL;
    const CNcdNodeIdentifier* nodeIdentifier = &aNodeIdentifier;
    
    if( NcdNodeIdentifierEditor::IdentifiesSearchNodeL( *nodeIdentifier ) )
        {
        // Search node, retrieve origin identifier.
        // Requesting ap for a search node so node should always exist and
        // it's safe to ask it from nodemanager.
        CNcdNode& node = iNodeManager.NodeL( *nodeIdentifier );
        //const CNcdNodeIdentifier* originIdentifier = NULL;
        if( node.ClassId() == NcdNodeClassIds::ENcdSearchItemNodeClassId )
            {
            // get the parent folder's origin identifier
            nodeIdentifier = 
                &iNodeManager.SearchFolderL( node.NodeLinkL().ParentIdentifier() )
                    .OriginIdentifierL();
            }
        else if( node.ClassId() == NcdNodeClassIds::ENcdSearchFolderNodeClassId )
            {
            nodeIdentifier =
                &(static_cast<CNcdSearchNodeFolder&>(node).OriginIdentifierL());
            }
        else
            {
            // search root or search budle, no origin identifier available
            return NULL;
            }
        }
    
    if ( nodeIdentifier->NodeId() == KNullDesC ) 
        {
        accessPoint = aAccessPointData.AccessPoint(
            nodeIdentifier->NodeNameSpace(), aAction );
        }
    else 
        {
        accessPoint = aAccessPointData.AccessPointL(
            nodeIdentifier->NodeNameSpace(), nodeIdentifier->NodeId(), aAction );
        }
        
    if ( !accessPoint &&
         nodeIdentifier->NodeNameSpace() != NcdProviderDefines::KRootNodeNameSpace &&
         !NcdNodeIdentifierEditor::IdentifiesTemporaryNodeL( *nodeIdentifier ) )
        {
        
        // Get parent identifier from child identifier.
        // NOTE: Parent is not required to exist in cache/db for this to work.
        CNcdNodeIdentifier* parentIdentifier = NcdNodeIdentifierEditor::ParentOfLC( *nodeIdentifier );
        
        if ( parentIdentifier->NodeNameSpace() == NcdProviderDefines::KRootNodeNameSpace )
            {
            // already at root node
            CleanupStack::PopAndDestroy( parentIdentifier );
            return NULL;
            }        
        // Partial fix for PRECLI-1321
        // This prevents missing parents from causing unwanted leaves
        else if ( !parentIdentifier->ContainsEmptyFields() )
            {
            accessPoint = FindAccessPointL( *parentIdentifier, aAction, aAccessPointData );
            }
        CleanupStack::PopAndDestroy( parentIdentifier );
        return accessPoint;
        }

    return accessPoint;
    }
            
    
TBool CCatalogsAccessPointManager::ValidateAccessPointL( CCatalogsAccessPoint* /* aAccessPoint */ )
    {
    DLTRACEIN((""));
    
#ifdef __WINS__
    // Comms database is not accessible in emulator.
    return EFalse;
#endif

#ifdef _0
    CCommsDatabase *commdb = CCommsDatabase::NewL( /* EDatabaseTypeIAP */);
    CleanupStack::PushL( commdb );
    CApSelect*  select;
    select = CApSelect::NewLC(
        *commdb,
        KEApIspTypeAll,
        EApBearerTypeAll,
        KEApSortUidAscending
        );
    
    CApDataHandler*     handler = CApDataHandler::NewLC( *commdb );        
    CApUtils* util = CApUtils::NewLC( *commdb );
    
    // Loop through array and try to find the right access point. 
    // After access point is found check that its parameters are still right.
    // If they aren't we have to create a new access point.
    TBool success = select->MoveToFirst();
    while (success)
        {
        TUint32 wapId = 0;
        TRAPD( err, wapId = util->WapIdFromIapIdL( aAccessPoint->AccessPointId() ) )
        if( err != KErrNone )
            {
            // AccessPoint has propably been removed from the commsdatabase.
            CleanupStack::PopAndDestroy( 4 );
            return EFalse;            
            }
        
        if( select->Uid() != wapId )
            {
            // This is not the right access point, proceed to the next one.
            success = select->MoveNext();                
            continue;
            }
        
        CApAccessPointItem* item = CApAccessPointItem::NewLC();
        handler->AccessPointDataL( select->Uid(), *item );
     
        // Check all the setting are similar in commsDb and in CCatalogsAccessPoint.
        TBool matching = MatchingSettingsL(*aAccessPoint, *item);
                   
        CleanupStack::PopAndDestroy(5);
        return matching;        
        }        
            
    // Someone has altered the access point settings.
    CleanupStack::PopAndDestroy(4);
#endif
    
    return EFalse;
    }

TBool CCatalogsAccessPointManager::MatchInCommsDbL( const CCatalogsAccessPointSettings& /* aSettings */, TUint32& /* aId */)
    {
    DLTRACEIN(_L("Checking comms DB for matching access point"));
    
#ifdef __WINS__
    // Comms database is not accessible in emulator.
    return EFalse;
#endif
    
#ifdef _0
    CCommsDatabase *commdb = CCommsDatabase::NewL( /* EDatabaseTypeIAP */ );
    CleanupStack::PushL( commdb );
    CApSelect*  select;
    select = CApSelect::NewLC(
        *commdb,
        KEApIspTypeAll,
        EApBearerTypeAll,
        KEApSortUidAscending
        );
    
    CApDataHandler*     handler = CApDataHandler::NewLC( *commdb );        
    CApUtils* util = CApUtils::NewLC( *commdb );
    
    // Loop through array and check if there already exists an access poinst which settings
    // are suitable.
    TBool success = select->MoveToFirst();
    while ( success )
        {
        CApAccessPointItem* item = CApAccessPointItem::NewLC();
        DLTRACE(("Loading access point data from comms DB"));
        handler->AccessPointDataL( select->Uid(), *item );

        DLTRACE(("Access point data loaded from comms DB"));        
        TBool matching = MatchingSettingsL(aSettings, *item);
        if ( !matching ) 
            {
            success = select->MoveNext();
            CleanupStack::PopAndDestroy(); // item
            continue;
            }       
            
        // We have a match in all fields.
        aId = util->IapIdFromWapIdL( select->Uid() );            
        CleanupStack::PopAndDestroy( 5, commdb ); // item, util, handler, select, commdb
        DLTRACEOUT(("Matching AP found"));
        return ETrue;
        }        
        
    CleanupStack::PopAndDestroy( 4, commdb ); // util, handler, select, commdb
    DLTRACEOUT(("Matching AP not found"));
#endif
    
    return EFalse;
    }

void CCatalogsAccessPointManager::CreateApToCommsDBL( const CCatalogsAccessPoint& /* aSettings */, TUint32& /* aId */ )
    {
    DLTRACEIN(_L("Creating AP to comms DB"));
    
#ifdef __WINS__
    // Comms database is not accessible in emulator.
    return;
#endif

#ifdef _0
    CCommsDatabase *commdb = CCommsDatabase::NewL( /*EDatabaseTypeIAP */);
    CleanupStack::PushL( commdb );
    
    CApDataHandler*     handler = CApDataHandler::NewLC( *commdb );        
    CApAccessPointItem* newAp = CApAccessPointItem::NewLC();

    newAp->SetNamesL( aSettings.Name() );
    
    // write the settings to newAp
    TInt longTextCount = aSettings.LongTextParameterCount();
    for (TInt i = 0; i < longTextCount; i++) 
        {
        TApMember key;
        TPtrC value;
        aSettings.LongTextParameter(i, key, value);
        newAp->WriteLongTextL(key, value);
        }
        
    TInt text16Count = aSettings.Text16ParameterCount();
    for (TInt i = 0; i < text16Count; i++) 
        {
        TApMember key;
        TPtrC16 value;
        aSettings.Text16Parameter(i, key, value);
        newAp->WriteTextL(key, value);
        }
        
    TInt text8Count = aSettings.Text8ParameterCount();
    for (TInt i = 0; i < text8Count; i++) 
        {
        TApMember key;
        TPtrC8 value;
        aSettings.Text8Parameter(i, key, value);
        newAp->WriteTextL(key, value);
        }
         
    TInt uintCount = aSettings.UintParameterCount();
    for (TInt i = 0; i < uintCount; i++) 
        {
        TApMember key;
        TUint32 value;
        aSettings.UintParameter(i, key, value);
        newAp->WriteUint(key, value);
        }
        
    TInt boolCount = aSettings.BoolParameterCount();
    for (TInt i = 0; i < boolCount; i++) 
        {
        TApMember key;
        TBool value;
        aSettings.BoolParameter(i, key, value);
        newAp->WriteBool(key, value);
        }            
    
    if( !newAp->SanityCheckOk() )
        {
        User::Leave( KErrArgument );
        }
        
    TUint32 wapId = 0;
    TRAPD( err, wapId = handler->CreateFromDataL( *newAp ) );
    if (err != KErrNone) 
        {
        DLERROR(("Unable to create access point to comms DB"));
        User::Leave(err);
        }

    CApUtils* util = CApUtils::NewLC( *commdb );
    aId = util->IapIdFromWapIdL( wapId );
    CleanupStack::PopAndDestroy( 4, commdb ); // commDb, handler, newAp, util
    
#endif
    DLTRACEOUT(("Access point created to comms DB"));
    }

#ifdef _0
HBufC16* CCatalogsAccessPointManager::ReadText16L( CApAccessPointItem* aItem, const TApMember& aApMember )
    {
    TInt length = aItem->ReadTextLengthL( aApMember );
    if( length == 0 )
        {
        return KNullDesC16().AllocL();
        }
    HBufC16* buffer = HBufC16::NewLC( length );
    TPtr16 ptr( buffer->Des());
    aItem->ReadTextL( aApMember, ptr );
    CleanupStack::Pop(); // buffer
    return buffer;
    }

HBufC8* CCatalogsAccessPointManager::ReadText8L( CApAccessPointItem* aItem, const TApMember& aApMember )
    {
    TInt length = aItem->ReadTextLengthL( aApMember );
    if( length == 0 )
        {
        return KNullDesC8().AllocL();
        }
    HBufC8* buffer = HBufC8::NewLC( length );
    TPtr8 ptr( buffer->Des());
    aItem->ReadTextL( aApMember, ptr );
    CleanupStack::Pop(); // buffer
    return buffer;
    }
#endif

void CCatalogsAccessPointManager::RemoveAccessPointsFromCommsDbL(
    TBool aAddShutdownOperation )
    {
    
    if ( !iClientAccessPointData ) 
        {
        return;
        }
    TInt apCount = iClientAccessPointData->AccessPointCount();
    
    // remove the access points from comms db
    for ( TInt i = 0; i < apCount; i++ ) 
        {      
        // remove access point only if it has been created by the manager
        // Fix for [#PRECLI-1596]
        if ( iClientAccessPointData->AccessPoint( i ).CreatedByManager() )
        	{
        	TUint32 idInCommsDb = 
        	    iClientAccessPointData->AccessPoint( i ).AccessPointId();
        	TRAPD( err, RemoveApFromCommsDatabaseL( idInCommsDb ) );
        	        	
        	if ( err != KErrNone &&
        	     err != KErrNotFound &&
        	     err != KErrNoMemory &&
        	     aAddShutdownOperation )
        	    {
        	    CCatalogsRemoveAccesspointShutdownOperation* op =
        	        CCatalogsRemoveAccesspointShutdownOperation::NewL( 
        	            iGeneralManager.FamilyId(),
        	            idInCommsDb );
        	    // takes ownership, deletes the op if adding fails
        	    CCatalogsClientServerServer::AddShutdownOperation( op );
        	    }
        	// we can ignore other errors
        	else if ( err == KErrNoMemory ) 
        	    {
        	    User::Leave( err );
        	    }
        	}     
        }
    }


void CCatalogsAccessPointManager::RemoveAccessPointsL() 
    {
    // remove from comms db
    RemoveAccessPointsFromCommsDbL( EFalse );
    
    delete iClientAccessPointData;
    iClientAccessPointData = NULL;
    }

void CCatalogsAccessPointManager::RemoveApFromCommsDatabaseL( const TUint32& /* aId */ )
    {
    
#ifdef _0
    CCommsDatabase *commdb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commdb );
    CApDataHandler* dataHandler = CApDataHandler::NewLC( *commdb );
    CApUtils* util = CApUtils::NewLC( *commdb );
    TUint32 wapId = util->WapIdFromIapIdL( aId );
    dataHandler->RemoveAPL( wapId );
    
    CleanupStack::PopAndDestroy( 3, commdb ); // util, dataHandler, commdb
#endif
    }

    
void CCatalogsAccessPointManager::LoadFromStorageL() 
    {
    // delete clients access point data
    RemoveAccessPointsL();
    
    MNcdStorage& storage = iStorageManager.ProviderStorageL( 
        iGeneralManager.FamilyName() );
    MNcdDatabaseStorage& database = storage.DatabaseStorageL(
        NcdProviderDefines::KDefaultDatabaseUid);  // not sure if this the correct db

    if( !database.ItemExistsInStorageL( iGeneralManager.FamilyName(), 
        NcdProviderDefines::ENcdAccessPointManager ) )
        {
        DLTRACE( ( _L("Data: %S was not in provider storage"), 
                    &iGeneralManager.FamilyName() ) );
        User::Leave( KErrNotFound );            
        }
        
    // Get the storage item from which the access point data is loaded
    // Note: database has the ownership of the item
    MNcdStorageItem* item = database.StorageItemL(
        iGeneralManager.FamilyName(), 
        NcdProviderDefines::ENcdAccessPointManager );    
        
    // Get data from database by using CCatalogsClientAccessPointData as the target so that 
    // internalize will be called for it
    CCatalogsClientAccessPointData* data = CCatalogsClientAccessPointData::NewLC();
    item->SetDataItem(data);
    
    // Read data -> calls CCatalogsClientAccessPointData::InternalizeL
    item->ReadDataL();
    
    iClientAccessPointData = data; 
    CleanupStack::Pop(); //data
    }
    
void CCatalogsAccessPointManager::SaveToStorageL() 
    {    
    if ( !iClientAccessPointData ) 
        {
        return;
        }
    
    MNcdStorage& storage = iStorageManager.ProviderStorageL( 
        iGeneralManager.FamilyName() );
    MNcdDatabaseStorage& database = storage.DatabaseStorageL(
        NcdProviderDefines::KDefaultDatabaseUid);
    
    // Get the storage item to which the access point manager is stored
    // Note: database has the ownership of the item
    MNcdStorageItem* item = database.StorageItemL(
        iGeneralManager.FamilyName(), 
        NcdProviderDefines::ENcdAccessPointManager);
    
    item->SetDataItem( iClientAccessPointData );
    item->OpenL();
    
    // Calls ExternalizeL for this
    item->WriteDataL();
    item->SaveL();    
    }

#ifdef _0   
TBool CCatalogsAccessPointManager::MatchingSettingsL(
    const CCatalogsAccessPointSettings& aSettings, CApAccessPointItem& aItem) 
    {
    DLTRACEIN(("Checking for matching AP settings"));

    // Check long text settings.   
    DLTRACE(("Checking long text parameters")); 
    TInt longTextCount = aSettings.LongTextParameterCount();
    for (TInt i = 0; i < longTextCount; i++) 
        {
        TApMember key;
        TPtrC value;
        DLTRACE(("Getting long text param data, i=%d", i));
        aSettings.LongTextParameter(i, key, value);
        DLTRACE(("Reading const long text from CApAccessPointItem, member=%d", key));

        const HBufC* buf = aItem.ReadConstLongTextL(key);
        DLTRACE((""));
        if (*buf != value) 
            {
            // Settings aren't correct.
            DLTRACEOUT(("Not matching settings"));
            return EFalse;
            }
        }
        
    // Check 16-bit text settings.
    DLTRACE(("Checking 16-bit text parameters")); 
    TInt text16Count = aSettings.Text16ParameterCount();
    for (TInt i = 0; i < text16Count; i++) 
        {
        TApMember key;
        TPtrC16 value;
        aSettings.Text16Parameter(i, key, value);
        DLTRACE(( _L("Key: %d, value: %S"), key, &value ));
        
        // CApDataHandler changes all of these if it needs to create a new
        // unique name for an accesspoint when it's created so we ignore
        // them when trying to find a matching AP so that we don't end
        // up creating new APs everytime we check if the correct AP
        // exists
        // See DLEB-179
        // CApAccessPointItem::SetNamesL sets these
        if ( key == EApWapAccessPointName ||
             key == EApIapName ||
             key == EApIspName || 
             key == EApNetworkName )
            {
            DLTRACE(( _L("Skipping name comparison, key: %d, value: %S"), 
                key, &value ));
            continue;
            }
        
        const HBufC16* buf = ReadText16L(&aItem, key);
        if (*buf != value) 
            {
            // Settings aren't correct.
            delete buf;
            DLTRACEOUT(("Not matching settings"));
            return EFalse;
            }
        delete buf;
        }

    // Check 8-bit text settings.
    DLTRACE(("Checking 8-bit text parameters")); 
    TInt text8Count = aSettings.Text8ParameterCount();
    for (TInt i = 0; i < text8Count; i++) 
        {
        TApMember key;
        TPtrC8 value;
        aSettings.Text8Parameter(i, key, value);
        const HBufC8* buf = ReadText8L(&aItem, key);
        if (*buf != value) 
            {
            // Settings aren't correct.
            delete buf;
            DLTRACEOUT(("Not matching settings"));
            return EFalse;
            }
        delete buf;
        }
        
    // Check uint settings.
    DLTRACE(("Checking uint parameters"));    
    TInt uintCount = aSettings.UintParameterCount();
    for (TInt i = 0; i < uintCount; i++) 
        {
        TApMember key;
        TUint32 value;
        aSettings.UintParameter(i, key, value);
        TUint32 dbValue;
        aItem.ReadUint(key, dbValue);
        if (dbValue != value) 
            {
            // Settings aren't correct.
            DLTRACEOUT(("Not matching settings"));
            return EFalse;
            }
        }
    
    // Check bool settings.
    DLTRACE(_L("Checking bool parameters"));     
    TInt boolCount = aSettings.BoolParameterCount();
    for (TInt i = 0; i < boolCount; i++) 
        {
        TApMember key;
        TBool value;
        aSettings.BoolParameter(i, key, value);
        TBool dbValue;
        aItem.ReadBool(key, dbValue);
        
        // TBools should not be compared to each other because there's no
        // guarantee that TBool contains only 0 or 1 because any non-zero
        // value is considered as true. So, we have to use ! operator
        // to limit the values to 0 and 1
        // 
        // Option B !( a && b || !a && !b )
        // Option C ( a && !b || !a && b )
        if ( !dbValue != !value ) 
            {
            // Settings aren't correct.
            DLTRACEOUT(("Not matching settings"));
            return EFalse;
            }
        }
    
    // Setting are correct.
    DLTRACEOUT(("Matching settings"));
    return ETrue;
    }
#endif   
    
void CCatalogsAccessPointManager::SetApDetailL(
    CCatalogsAccessPoint& /* aAp */, const TDesC& /* aKey */, const TDesC& /* aValue */ ) 
    {
    
#ifdef _0
    DLTRACEIN((""));
    TBool value = EFalse;
    
    if (aKey == KApName) 
        {
        aAp.SetNameL(aValue);
        }                
    else if (aKey == KApCdmaDisablePlainTextAuth) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApCdmaDisablePlainTextAuth, value);
        }
    else if (aKey == KApCdmaIfAuthName) 
        {
        aAp.SetText16ParameterL(EApCdmaIfAuthName, aValue);
        }
    else if (aKey == KApCdmaIfAuthPassword) 
        {
        aAp.SetText16ParameterL(EApCdmaIfAuthPassword, aValue);
        }       
    else if (aKey == KApCdmaIfName) 
        {
        aAp.SetText16ParameterL(EApCdmaIfName, aValue);
        }
    else if (aKey == KApCdmaIfNetworks) 
        {
        aAp.SetText16ParameterL(EApCdmaIfNetworks, aValue);
        }
    else if (aKey == KApCdmaIpAddr) 
        {
        aAp.SetText16ParameterL(EApCdmaIpAddr, aValue);
        }
    else if (aKey == KApCdmaIpAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApCdmaIpAddrFromServer, value);
        }
    else if (aKey == KApCdmaIpDnsAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApCdmaIpDnsAddrFromServer, value);
        }
    else if (aKey == KApCdmaIpGateway) 
        {
        aAp.SetText16ParameterL(EApCdmaIpGateway, aValue);
        }
    else if (aKey == KApCdmaIPNameServer1) 
        {
        aAp.SetText16ParameterL(EApCdmaIPNameServer1, aValue);
        }
    else if (aKey == KApCdmaIPNameServer2) 
        {
        aAp.SetText16ParameterL(EApCdmaIPNameServer2, aValue);
        }            
    else if (aKey == KApCdmaIpNetMask) 
        {
        aAp.SetText16ParameterL(EApCdmaIpNetMask, aValue);
        }
    else if (aKey == KApGprsAccessPointName) 
        {
        aAp.SetLongTextParameterL(EApGprsAccessPointName, aValue);
        }
    else if (aKey == KApGprsDisablePlainTextAuth)
        {        
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApGprsDisablePlainTextAuth, value);
        }
    else if (aKey == KApGprsIfAuthName) 
        {
        aAp.SetText16ParameterL(EApGprsIfAuthName, aValue);
        }
    else if (aKey == KApGprsIfAuthPassword) 
        {
        aAp.SetText16ParameterL(EApGprsIfAuthPassword, aValue);
        }
    else if (aKey == KApGprsIfName) 
        {
        aAp.SetText16ParameterL(EApGprsIfName, aValue);
        }
    else if (aKey == KApGprsIfNetworks) 
        {
        aAp.SetText16ParameterL(EApGprsIfNetworks, aValue);
        }
    else if (aKey == KApGprsIfParams) 
        {
        aAp.SetText16ParameterL(EApGprsIfParams, aValue);
        }
    else if (aKey == KApGprsIfPromptForAuth) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApGprsIfPromptForAuth, value);
        }
    else if (aKey == KApGprsIpAddr) 
        {
        aAp.SetText16ParameterL(EApGprsIpAddr, aValue);
        }
    else if (aKey == KApGprsIpAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApGprsIpAddrFromServer, value);
        }
    else if (aKey == KApGprsIpDnsAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApGprsIpDnsAddrFromServer, value);
        }
    else if (aKey == KApGprsIpGateway) 
        {
        aAp.SetText16ParameterL(EApGprsIpGateway, aValue);
        }
    else if (aKey == KApGprsIPNameServer1) 
        {
        aAp.SetText16ParameterL(EApGprsIPNameServer1, aValue);
        }
    else if (aKey == KApGprsIPNameServer2) 
        {
        aAp.SetText16ParameterL(EApGprsIPNameServer2, aValue);
        }
    else if (aKey == KApGprsIpNetMask) 
        {
        aAp.SetText16ParameterL(EApGprsIpNetMask, aValue);
        }
    else if (aKey == KApGprsPdpAddress) 
        {
        aAp.SetText16ParameterL(EApGprsPdpAddress, aValue);
        }
    else if (aKey == KApHasProxySettings) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApHasProxySettings, value);
        }
    else if (aKey == KApIapChargecard) 
        {
        TInt value = NcdProtocolUtils::DesDecToIntL(aValue);
        aAp.SetUintParameterL(EApIapChargecard, value);
        }
    else if (aKey == KApIapName) 
        {
        aAp.SetText16ParameterL(EApIapName, aValue);
        }
    else if (aKey == KApIapServiceId) 
        {
        TInt value = NcdProtocolUtils::DesDecToIntL(aValue);
        aAp.SetUintParameterL(EApIapServiceId, value);
        }
    else if (aKey == KApIapServiceType) 
        {
        aAp.SetText16ParameterL(EApIapServiceType, aValue);
        }
    else if (aKey == KApIP6NameServer1) 
        {
        aAp.SetText16ParameterL(EApIP6NameServer1, aValue);
        }
    else if (aKey == KApIP6NameServer2) 
        {
        aAp.SetText16ParameterL(EApIP6NameServer2, aValue);
        }
    else if (aKey == KApIspDefaultTelNumber) 
        {
        aAp.SetText16ParameterL(EApIspDefaultTelNumber, aValue);
        }
    else if (aKey == KApIspDescription) 
        {
        aAp.SetText16ParameterL(EApIspDescription, aValue);
        }
    else if (aKey == KApIspDisablePlainTextAuth) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApIspDisablePlainTextAuth, value);
        }
    else if (aKey == KApIspIfAuthName) 
        {
        aAp.SetText16ParameterL(EApIspIfAuthName, aValue);
        }
    else if (aKey == KApIspIfAuthPass) 
        {
        aAp.SetText16ParameterL(EApIspIfAuthPass, aValue);
        }
    else if (aKey == KApIspIfCallbackInfo) 
        {
        const TDesC& value = aValue;
        
        // convert the descriptor to 8-bit
        HBufC8* buf8 = HBufC8::NewLC(value.Length());
        buf8->Des().Copy(value);
        aAp.SetText8ParameterL(EApIspIfCallbackInfo, *buf8);
        CleanupStack::PopAndDestroy();
        }
    else if (aKey == KApIspIfName) 
        {
        aAp.SetText16ParameterL(EApIspIfName, aValue);
        }
    else if (aKey == KApIspIfNetworks) 
        {
        aAp.SetText16ParameterL(EApIspIfNetworks, aValue);
        }
    else if (aKey == KApIspIfParams) 
        {
        aAp.SetText16ParameterL(EApIspIfParams, aValue);
        }
    else if (aKey == KApIspInitString) 
        {
        const TDesC& value = aValue;

        // convert the descriptor to 8-bit
        HBufC8* buf8 = HBufC8::NewLC(value.Length());
        buf8->Des().Copy(value);
        aAp.SetText8ParameterL(EApIspIfCallbackInfo, *buf8);
        CleanupStack::PopAndDestroy();
        }
    else if (aKey == KApIspIPAddr) 
        {
        aAp.SetText16ParameterL(EApIspIPAddr, aValue);
        }
    else if (aKey == KApIspIPAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApIspIPAddrFromServer, value);
        }
    else if (aKey == KApIspIPDnsAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApIspIPAddrFromServer, value);
        }
    else if (aKey == KApIspIPGateway) 
        {
        aAp.SetText16ParameterL(EApIspIPGateway, aValue);
        }
    else if (aKey == KApIspIPNameServer1) 
        {
        aAp.SetText16ParameterL(EApIspIPNameServer1, aValue);
        }
    else if (aKey == KApIspIPNameServer2) 
        {
        aAp.SetText16ParameterL(EApIspIPNameServer2, aValue);
        }
    else if (aKey == KApIspIPNetMask) 
        {
        aAp.SetText16ParameterL(EApIspIPNetMask, aValue);
        }
    else if (aKey == KApIspIspType) 
        {
        // 
        }
    else if (aKey == KApIspLoginName) 
        {
        aAp.SetText16ParameterL(EApIspLoginName, aValue);
        }
    else if (aKey == KApIspLoginPass) 
        {
        aAp.SetText16ParameterL(EApIspLoginPass, aValue);
        }
    else if (aKey == KApIspLoginScript) 
        {
        aAp.SetText16ParameterL(EApIspLoginScript, aValue);
        }
    else if (aKey == KApIspName) 
        {
        aAp.SetText16ParameterL(EApIspName, aValue);
        }
    else if (aKey == KApLanBearerName) 
        {
        aAp.SetText16ParameterL(EApLanBearerName, aValue);
        }
    else if (aKey == KApNetworkName) 
        {
        aAp.SetText16ParameterL(EApNetworkName, aValue);
        }
    else if (aKey == KApProxyExceptions) 
        {
        aAp.SetLongTextParameterL(EApProxyExceptions, aValue);
        }
    else if (aKey == KApProxyLoginName) 
        {
        aAp.SetText16ParameterL(EApProxyLoginName, aValue);
        }
    else if (aKey == KApProxyLoginPass) 
        {
        aAp.SetText16ParameterL(EApProxyLoginPass, aValue);
        }
    else if (aKey == KApProxyPortNumber) 
        {
        TInt value = NcdProtocolUtils::DesDecToIntL(aValue);
        aAp.SetUintParameterL(EApProxyPortNumber, value);
        }
    else if (aKey == KApProxyProtocolName) 
        {
        aAp.SetText16ParameterL(EApProxyProtocolName, aValue);
        }
    else if (aKey == KApProxyServerAddress) 
        {
        aAp.SetLongTextParameterL(EApProxyServerAddress, aValue);
        }
    else if (aKey == KApProxyUseProxy) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApProxyUseProxy, value);
        }
    else if (aKey == KApWapAccessPointName) 
        {
        aAp.SetText16ParameterL(EApWapAccessPointName, aValue);
        }
    else if (aKey == KApWapCurrentBearer) 
        {
        aAp.SetText16ParameterL(EApWapCurrentBearer, aValue);
        }
    else if (aKey == KApWapGatewayAddress) 
        {
        aAp.SetText16ParameterL(EApWapGatewayAddress, aValue);
        }
    else if (aKey == KApWapIap) 
        {
        TInt value = NcdProtocolUtils::DesDecToIntL(aValue);
        aAp.SetUintParameterL(EApWapIap, value);
        }
    else if (aKey == KApWapProxyPort) 
        {
        TInt value = NcdProtocolUtils::DesDecToIntL(aValue);
        aAp.SetUintParameterL(EApWapProxyPort, value);
        }
    else if (aKey == KApWapSecurity) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApWapSecurity, value);
        }
    else if (aKey == KApWapStartPage) 
        {
        aAp.SetLongTextParameterL(EApWapStartPage, aValue);
        }
    else if (aKey == KApWapWspOption) 
        {
        if (aValue == KWapWspOptionConnectionless) 
            {
            aAp.SetUintParameterL(EApWapWspOption, EWapWspOptionConnectionless);
            }
        else if (aValue == KWapWspOptionConnectionOriented) 
            {
            aAp.SetUintParameterL(EApWapWspOption, EWapWspOptionConnectionOriented);
            }
        else 
            {
            DLWARNING((_L("Unknown EApWapWspOption value: %S"), &aValue));
            }
        }
    else if (aKey == KApWlanIfAuthName) 
        {
        aAp.SetText16ParameterL(EApWlanIfAuthName, aValue);
        }
    else if (aKey == KApWlanIfAuthPassword) 
        {
        aAp.SetText16ParameterL(EApWlanIfAuthPassword, aValue);
        }
    else if (aKey == KApWlanIfNetworks) 
        {
        aAp.SetText16ParameterL(EApWlanIfNetworks, aValue);
        }
    else if (aKey == KApWlanIpAddr) 
        {
        aAp.SetText16ParameterL(EApWlanIpAddr, aValue);
        }
    else if (aKey == KApWlanIpAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApWlanIpAddrFromServer, value);
        }
    else if (aKey == KApWlanIpDnsAddrFromServer) 
        {
        NcdProtocolUtils::DesToBool(value, aValue);
        aAp.SetBoolParameterL(EApWlanIpDnsAddrFromServer, value);
        }
    else if (aKey == KApWlanIpGateway) 
        {
        aAp.SetText16ParameterL(EApWlanIpGateway, aValue);
        }
    else if (aKey == KApWlanIPNameServer1) 
        {
        aAp.SetText16ParameterL(EApWlanIPNameServer1, aValue);
        }
    else if (aKey == KApWlanIPNameServer2) 
        {
        aAp.SetText16ParameterL(EApWlanIPNameServer2, aValue);
        }
    else if (aKey == KApWlanIpNetMask) 
        {
        aAp.SetText16ParameterL(EApWlanIpNetMask, aValue);
        }
    else 
        {
        DLWARNING(( _L("Unknown access point detail: %S"), &aKey));
        }
#endif
    }
    

TInt CCatalogsAccessPointManager::GetAccessPoint( 
    CCatalogsAccessPoint*& aAccessPoint,
    CCatalogsClientAccessPointData*& aApData )
    {
    DLTRACEIN((""));
#ifdef __WINS__
    // Comms database is not accessible in emulator
    return KErrNotFound;
#endif

    if ( iFixedAp ) 
        {
        // Use fixed AP if it is set.
        aAccessPoint = iFixedAp;
        }    
    else  
        {        
        aApData = iClientAccessPointData;
        if ( !aApData ) 
            {
            TRAPD( err, LoadFromStorageL() );
        
            if ( err != KErrNone ) 
                {
                return err;
                }
            
            aApData = iClientAccessPointData;
            }
                
        if ( !aApData ) 
            {
            return KErrNotFound;
            }
        } 
    return KErrNone;
    }


TInt CCatalogsAccessPointManager::ValidateOrCreateAccessPointL(
    CCatalogsAccessPoint* aAccessPoint,
    TUint32& aAccessPointId )
    {
    DLTRACEIN((""));
    if ( !aAccessPoint ) 
        {
        return KErrNotFound;
        }

    // If we are using fixed AP and we don't need to check
    // the existance of the AP, we can skip validation etc.
    if ( aAccessPoint == iFixedAp && !iCheckAp ) 
        {
        aAccessPointId = aAccessPoint->AccessPointId();
        return KErrNone;
        }
    
    if ( ValidateAccessPointL( aAccessPoint ) ) 
        {
        aAccessPointId = aAccessPoint->AccessPointId();        
        }
    else 
        {
        // Somehow accesspoint settings has been altered
        // in the commsdatabase. Recreate access point.
        TUint32 id = 0;
        CreateApToCommsDBL( *aAccessPoint, id );
        aAccessPoint->SetAccessPointId( id );            
        if ( aAccessPoint != iFixedAp ) 
            {
            // If not using fixed AP, client's AP settings were altered, save them.
            SaveToStorageL();
            }
        aAccessPointId = id;        
        }
    return KErrNone;
    }

    
void CCatalogsAccessPointManager::DestructL()
    {
    DLTRACEIN((""));
    CCatalogsHttpSessionManager::NetworkManagerL().RemoveObserver( *this );
    // remove access points from comms db

    RemoveAccessPointsFromCommsDbL( ETrue );
        
    // Remove fixed ap too but only if it was created by us
    if ( iFixedAp && iFixedAp->CreatedByManager() ) 
        { 
        CCatalogsRemoveAccesspointShutdownOperation* op =
            CCatalogsRemoveAccesspointShutdownOperation::NewL( 
                iGeneralManager.FamilyId(),
                iFixedAp->AccessPointId() );
        // takes ownership, deletes the op if adding fails
        CCatalogsClientServerServer::AddShutdownOperation( op );
        }
    }

