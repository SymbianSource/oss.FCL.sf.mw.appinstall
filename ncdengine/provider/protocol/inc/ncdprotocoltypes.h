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


#ifndef NCD_REQUEST_TYPES_HH
#define NCD_REQUEST_TYPES_HH


#include "ncdxmlheaders.h"
#include "ncd_cp_detail.h"
#include "catalogsutils.h"


#ifndef RD_XML_ENGINE_API_CHANGE
typedef TString TXmlEngString;
typedef TElement TXmlEngElement;
typedef RDocument RXmlEngDocument;
typedef TNamespace TXmlEngNamespace;
#endif


enum TNcdBool
    {
    EValueNotSet = 0,
    EValueFalse = 1,
    EValueTrue = 2
    };

struct TNcdRequestFilter
    {
    TNcdBool exclude;
    TXmlEngString text;
    
    void Close() 
        {
        text.Close();
        }
    };

struct TNcdRequestEntityFilter 
    {
    TNcdBool subscribableContent;
    TNcdBool freeContent;
    RArray<TNcdRequestFilter> keywords;
    RArray<TNcdRequestFilter> contentPurposes;       

    void Close() 
        {
        ResetAndCloseArray( keywords );
        ResetAndCloseArray( contentPurposes );
        }
     
    };

struct TNcdRequestResponseFilter 
    {
    TUint pageSize;
    TUint pageStart;
    TUint structureDepth;
    TUint metadataDepth;
    TUint metadataPerLevel;
    RArray<TXmlEngString> includedElements;
    RArray<TXmlEngString> excludedElements;
    
    void Close() 
        {
        ResetAndCloseArray( includedElements );
        ResetAndCloseArray( excludedElements );
        }
    };

struct TNcdRequestRemoteEntity 
    {
    TXmlEngString id;
    TXmlEngString timeStamp;
    TNcdBool includeMetaData;
    
    void Close() 
        {
        id.Close();
        timeStamp.Close();
        }
    };

class CNcdConfigurationDisplay : public CBase
    {
public:
    TInt iColors;
    TInt iHeight;
    TInt iWidth;
    };

struct TNcdConfigurationNetwork
    {
    TXmlEngString mcc;
    TXmlEngString mnc;
    TXmlEngString currentMcc;
    TXmlEngString currentMnc;
    TXmlEngString gid1;
    TXmlEngString gid2;
    TXmlEngString smsc;
    TXmlEngString imsi;
    TXmlEngString msisdn;
    TXmlEngString cellId;
    TXmlEngString serviceProviderName;
    TXmlEngString operatorName;
    RPointerArray<MNcdConfigurationProtocolDetail> details;
    };

struct TNcdConfigurationSoftware
    {
    TXmlEngString type;
    TXmlEngString version;
    TXmlEngString uid;
    TXmlEngString id;
    TXmlEngString ssid;
    TXmlEngString language;    
    RArray<TXmlEngString> capabilities;
    RPointerArray<MNcdConfigurationProtocolDetail> details;
    };
    
struct TNcdConfigurationHardware 
    {
    TXmlEngString uaProfileUri;
    TXmlEngString identification;
    TXmlEngString manufacturer;
    TXmlEngString model;
    TXmlEngString language;
    TXmlEngString platform;
    TXmlEngString firmwareVersion;
    RPointerArray<CNcdConfigurationDisplay> displays;
    RPointerArray<MNcdConfigurationProtocolDetail> details;
    };

struct TNcdConfigurationQuerySemantics 
    {
    TXmlEngString id;
    TXmlEngString semantics;
    RArray<TXmlEngString> values;
    
    void Close() 
        {
        id.Free();
        semantics.Free();
        ResetAndCloseArray( values );
        }
    };
    
struct TNcdConfigurationQueryResponse 
    {
    TXmlEngString id;
    TXmlEngString semantics;
    TNcdBool cancel;
    RArray<TNcdConfigurationQuerySemantics> response;
    
    void Close() 
        {
        id.Free();
        semantics.Free();
        ResetAndCloseArray( response );
        }
    };
    
struct TNcdConfigurationGpsLocation
    {
    TXmlEngString latitude;
    TXmlEngString longitude;
    TXmlEngString elevation;
    TXmlEngString time; // xs:dateTime
    TXmlEngString velocity;
    };

enum TNcdDataEntityType
    {
    EItemEntity,
    EFolderEntity
    };

enum TNcdDeliveryMethod
    {
    EDeliveryNotSet,
    EDeliverySms,
    EDeliveryWapPush,
    EDeliveryDirect,
    EDeliveryNone
    };

enum TNcdPurchaseType 
    {
    ETypeNotSet,
    EContent,
    ESubscription,
    EPartOfSubscription,
    EUpgrade
    };
    
enum TNcdSubscriptionType 
    {
    ENotSubscribable,
    EPeriodic,
    EAutomaticContinous    
    };

enum TNcdDependencyType 
    {
    EDependency,
    EUpgradeDependency
    };

enum TNcdSubscriptionOperation 
    {
    EStatus,
    EUnsubscribe
    };
    
enum TNcdUriDeliveryMethod 
    {
    EUriDeliveryNotSet = -1,
    EUriDeliveryWlan = 0,
    EUriDeliveryGprs,
    EUriDeliveryBroadband
    };

enum TNcdDownloadTargetType 
    {
    EDownloadTargetDownloadable = 0,
    EDownloadTargetConsumable
    };

enum TNcdUriFingerprintAlgorithm 
    {
    EAlgorithmNotSet = -1,
    EAlgorithmMd5 = 0,
    EAlgorithmSha1
    };

    
#endif //NCD_REQUEST_TYPES_HH
