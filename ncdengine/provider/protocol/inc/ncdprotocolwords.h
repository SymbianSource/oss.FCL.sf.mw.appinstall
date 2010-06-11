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
* Description:   Protocol keywords
*
*/


#ifndef NcdPROTOCOLWORDS_H
#define NcdPROTOCOLWORDS_H

#include <e32base.h>

// _LIT8( KTag

// Preminet protocol
_LIT8( KIdPreminetProtocol, "preminetProtocol" );
_LIT8( KIdPreminetProtocolVersion, "2.0" );

_LIT8( KAttrAuthorizedCount, "authorizedCount" );
_LIT8( KTagLabel, "label" );
_LIT8( KTagPreminetResponse, "preminetResponse" );
_LIT8( KTagEntity, "entity" );
_LIT8( KTagValidUntil, "validUntil" );
_LIT8( KTagQueries, "queries" );
_LIT8( KTagQuery, "query" );
_LIT8( KTagInformation, "information" );
_LIT8( KTagError, "error" );
_LIT8( KTagPurchase, "purchase" );
_LIT8( KTagSession, "session" );
_LIT8( KTagNotification, "notification" );
_LIT8( KTagSubscriptionReport, "subscriptionReport" );
_LIT8( KTagEmbeddedData, "embeddedData" );
_LIT8( KTagEntities, "entities" );
_LIT8( KTagActivities, "activities" );
_LIT8( KTagEmbeddedSession, "embeddedSession" );
_LIT8( KTagDataEntity, "dataEntity" );
_LIT8( KTagActionEntity, "actionEntity" );
_LIT8( KTagDownloadableContent, "downloadableContent" );
_LIT8( KTagSubscribableContent, "subscribableContent" );
_LIT8( KAttrMime, "mime" );
_LIT8( KTagDescription, "description" );
_LIT8( KTagIcon, "icon" );
_LIT8( KTagDisclaimer, "disclaimer" );
_LIT8( KTagPurchaseOptions, "purchaseOptions" );
_LIT8( KTagPurchaseOption, "purchaseOption" );
_LIT8( KTagParentSubscription, "parentSubscription" );
_LIT8( KValuePeriodic, "periodic" );
_LIT8( KValueAutomaticContinous, "automaticContinous" );
_LIT8( KTagDependencies, "dependencies" );
_LIT8( KTagDependency, "dependency" );
_LIT8( KTagPreview, "preview" );
_LIT8( KTagReview, "review" );
_LIT8( KTagChildAccessibility, "childAccessibility" );
_LIT8( KAttrViewable, "viewable" );
_LIT8( KAttrSeparatelyPurchasable, "separatelyPurchasable" );
_LIT8( KTagSkin, "skin" );
_LIT8( KTagLayout, "layout" );
_LIT8( KAttrDependencyId, "dependencyId" );
_LIT8( KValueDependency, "dependency" );
_LIT8( KValueUpgrade, "upgrade" );
_LIT8( KAttrContentId, "contentId" );
_LIT8( KAttrContentVersion, "contentVersion" );
_LIT8( KAttrName, "name" );
_LIT8( KAttrPurchase, "purchase" );
_LIT8( KValueContent, "content" );
_LIT8( KValueSubscription, "subscription" );
_LIT8( KValuePartOfSubscription, "partOfSubscription" );
_LIT8( KTagPriceText, "priceText" );
_LIT8( KTagPrice, "price" );
_LIT8( KTagCreditPrice, "creditPrice" );
_LIT8( KAttrCurrency, "currency" );
_LIT8( KAttrDataBlock, "dataBlock" );
_LIT8( KTagDownloadDetails, "downloadDetails" );
_LIT8( KTagValidSubscriptions, "validSubscriptions");
_LIT8( KTagOldSubscriptions, "oldSubscriptions");
_LIT8( KTagSubscriptionDetails, "subscriptionDetails" );
_LIT8( KTagTotalUsageRights, "totalUsageRights" );
_LIT8( KTagRemainingUsageRights, "remainingUsageRights" );
_LIT8( KTagUsageRights, "usageRights" );
_LIT8( KTagAmountOfCredits, "amountOfCredits" );
_LIT8( KTagValidity, "validity" );
_LIT8( KTagNumberOfDownloads, "numberOfDownloads" );
_LIT8( KAttrExpiredOn, "expiredOn" );
_LIT8( KAttrCancelled, "cancelled" );
_LIT8( KAttrUri, "uri" );
_LIT8( KAttrAutoDownload, "autoDownload" );
_LIT8( KTagDescriptor, "descriptor" );
_LIT8( KAttrDelayDelta, "delayDelta" );
_LIT8( KAttrDeliveryMethod, "deliveryMethod" );
_LIT8( KValueWlan, "wlan" );
_LIT8( KValueGprs, "gprs" );
_LIT8( KValueBroadband, "broadband" );
_LIT8( KAttrValidityDelta, "validityDelta" );
_LIT8( KAttrTarget, "target" );
_LIT8( KAttrChunkSize, "chunkSize" );
_LIT8( KAttrLaunchable, "launchable" );
_LIT8( KAttrInstallNotificationUri, "installNotificationUri" );
_LIT8( KAttrSize, "size" );
_LIT8( KValueConsumable, "consumable" );
_LIT8( KValueDownloadable, "downloadable" );
_LIT8( KAttrReDownloadable, "reDownloadable" );
_LIT8( KTagTitle, "title" );
_LIT8( KTagMessage, "message" );
_LIT8( KTagExpiredCachedData, "expiredCachedData");
_LIT8( KTagDataBlocks, "dataBlocks" );
_LIT8( KTagDataBlock, "dataBlock" );
_LIT8( KAttrCancel, "cancel" );
_LIT8( KAttrVersion, "version" );
_LIT8( KAttrNamespace, "namespace" );
_LIT8( KAttrId, "id" );
_LIT8( KAttrSession, "session" );
_LIT8( KAttrDelta, "delta" );
_LIT8( KAttrAutoUpdate, "autoUpdate" );
_LIT8( KAttrTimeStamp, "timeStamp" );
_LIT8( KAttrTimestamp, "timestamp" );
_LIT8( KAttrCount, "count" );
_LIT8( KTagRightsDetails, "rightsDetails" );
_LIT8( KTagRightsObject, "rightsObject" );
_LIT8( KAttrActivationKey, "activationKey" );
_LIT8( KAttrIssuerUri, "issuerUri" );

_LIT8( KAttrTransactionId, "transactionId" );
_LIT8( KAttrQueryId, "queryId" );
_LIT8( KTagProcessed, "processed" );
_LIT8( KAttrResultCode, "resultCode" );
_LIT8( KAttrInitiateSession, "initiateSession" );
_LIT8( KTagPayments, "payments" );
_LIT8( KTagTotalPrice, "totalPrice" );
_LIT8( KAttrTicket, "ticket" );
_LIT8( KAttrPriceText, "priceText" );
_LIT8( KTagDelivery, "delivery" );
_LIT8( KAttrMethod, "method" );
_LIT8( KValueDirect, "direct" );
_LIT8( KValueSms, "sms" );
_LIT8( KValueWapPush, "wap-push" );
_LIT8( KValueNone, "none" );
_LIT8( KValueCreditCard, "creditcard" );
_LIT8( KValueCustom, "custom" );
_LIT8( KTagPayment, "payment" );
_LIT8( KTagSmsDetails, "smsDetails" );
_LIT8( KTagSmsDetail, "smsDetail" );
_LIT8( KAttrAddress, "address" );
_LIT8( KAttrMessage, "message" );
_LIT8( KAttrFree, "free" );
_LIT8( KAttrCanBeGift, "canBeGift" );
_LIT8( KAttrRequirePurchaseProcess, "requirePurchaseProcess" );
_LIT8( KTagHelpInformation, "helpInformation" );
_LIT8( KTagMessages, "messages" );
_LIT8( KTagResendAfter, "resendAfter" );
_LIT8( KAttrSeconds, "seconds" );
_LIT8( KTagExpiredEntity, "expiredEntity" );
_LIT8( KAttrEntityId, "entityId" );
_LIT8( KAttrRecursive, "recursive" );
_LIT8( KAttrForceUpdate, "forceUpdate" );
_LIT8( KTagClearOldEntities, "clearOldEntities" );
_LIT8( KAttrStartTimestamp, "startTimestamp" );
_LIT8( KAttrEndTimestamp, "endTimestamp" );
_LIT8( KTagCapability, "capability" );
_LIT8( KTagScreenshot, "screenshot" );
_LIT8( KTagMoreInfo, "moreInfo" );

// Preminet protocol, request words
_LIT8( KAttrPreminetNamespaceUri, "http://nokia.com/preminet/protocol/v/2/0" );
_LIT8( KAttrCdpNamespacePrefix, "cp" );
_LIT8( KAttrCdpNamespaceUri, "http://nokia.com/preminet/protocol/configuration/v/1/0" );
_LIT8( KAttrXsNamespacePrefix, "xs" );
_LIT8( KAttrXsNamespaceUri, "http://www.w3.org/2001/XMLSchema" );
_LIT8( KAttrXsiNamespacePrefix, "xsi" );
_LIT8( KAttrXsiNamespaceUri, "http://www.w3.org/2001/XMLSchema-instance" );

_LIT8( KTagPreminetRequest, "preminetRequest" );
_LIT8( KAttrPreminetVersion, "2.0" );

_LIT8( KTagConfiguration, "configuration" );
_LIT8( KTagBrowse, "browse" );
_LIT8( KTagSearch, "search" );
_LIT8( KAttrIncludeMetaData, "includeMetaData" );
_LIT8( KTagEntityFilter, "entityFilter" );
_LIT8( KAttrSubscribableContent, "subscribableContent" );
_LIT8( KAttrFreeContent, "freeContent" );
_LIT8( KTagKeywords, "keywords" );
_LIT8( KTagKeyword, "keyword" );
_LIT8( KTagMimes, "mimes" );
_LIT8( KTagContentPurposes, "contentPurposes" );
_LIT8( KTagContentPurpose, "contentPurpose" );
_LIT8( KTagReviewScore, "reviewScore" );
_LIT8( KTagResponseFilter, "responseFilter" );
_LIT8( KTagIncludeElement, "includeElement" );
_LIT8( KTagExcludeElement, "excludeElement" );
_LIT8( KAttrPageSize, "pageSize" );
_LIT8( KAttrPageStart, "pageStart" );
_LIT8( KAttrStructureDepth, "structureDepth" );
_LIT8( KAttrMetadataDepth, "metadataDepth" );
_LIT8( KAttrMetadataPerLevel, "metadataPerLevel" );
_LIT8( KAttrIdentifier, "identifier" );
_LIT8( KAttrOperator, "operator" );
_LIT8( KAttrExclude, "exclude" );
_LIT8( KTagQueryResponse, "queryResponse" );
_LIT8( KTagResponse, "response" );
_LIT8( KTagName, "name" );
_LIT8( KTagEntityDetails, "entityDetails" );
_LIT8( KAttrPurchaseOptionId, "purchaseOptionId" );
_LIT8( KAttrQueryResponseId, "queryResponseId" );
_LIT8( KAttrGift, "gift" );
_LIT8( KTagManageSubscriptions, "manageSubscriptions" );
_LIT8( KTagSubscription, "subscription" );
_LIT8( KAttrOperation, "operation" );
_LIT8( KValueStatus, "status" );
_LIT8( KValueUnsubscribe, "unsubscribe" );
_LIT8( KTagConfirmation, "confirmation" );
_LIT8( KTagGetDownloadDetails, "getDownloadDetails" );

_LIT8( KTagConfigurationResponse, "configurationResponse" );
_LIT8( KAttrXsiType, "xsi:type" );
_LIT8( KPrefixXsi, "xsi" );
_LIT8( KValueFolderRef, "FolderRef" );
_LIT8( KValueActionRef, "ActionRef" );
_LIT8( KValueItemRef, "ItemRef" );
_LIT8( KValueItemData, "ItemData" );
_LIT8( KValueFolderData, "FolderData" );
_LIT8( KValueActionData, "ActionData" );

// Preminet protocol, installation report
_LIT8( KTagInstallationReport, "installationReport" );
_LIT8( KTagInstallation, "installation" );
_LIT8( KTagContentUri, "contentUri" );
_LIT8( KAttrStatusCode, "statusCode" );
_LIT8( KAttrElapsedTime, "elapsedTime" );

// Configuration protocol, request words
_LIT8( KTagConfigurationRequest, "configurationRequest" );
_LIT8( KAttrConfigurationVersion, "1.0" );
_LIT8( KTagNetwork, "network" );
_LIT8( KTagClient, "client" );
_LIT8( KTagSoftware, "software" );
_LIT8( KTagHardware, "hardware" );
_LIT8( KAttrType, "type" );
_LIT8( KAttrMcc, "mcc" );
_LIT8( KAttrMnc, "mnc" );
_LIT8( KAttrCurrentMcc, "currentMcc" );
_LIT8( KAttrCurrentMnc, "currentMnc" );
_LIT8( KAttrGid1, "gid1" );
_LIT8( KAttrGid2, "gid2" );
_LIT8( KAttrSmsc, "smsc" );
_LIT8( KAttrImsi, "imsi" );
_LIT8( KAttrMsisdn, "msisdn" );
_LIT8( KTagServiceProviderName, "serviceProviderName" );
_LIT8( KTagOperatorName, "operatorName" );
_LIT8( KAttrUid, "uid" );
_LIT8( KAttrSsid, "ssid" );
_LIT8( KTagLanguage, "language" );
_LIT8( KTagTestModes, "testModes" );
_LIT8( KTagTestMode, "testMode" );
_LIT8( KAttrPreviewKey, "previewKey" );
_LIT8( KTagCapabilities, "capabilities" );
_LIT8( KTagDetails, "details" );
_LIT8( KTagDetail, "detail" );
_LIT8( KTagUaProfileUri, "uaProfileUri" );
_LIT8( KAttrLabel, "label" );
_LIT8( KAttrGroupId, "groupId" );
_LIT8( KTagContent, "content" );
_LIT8( KTagXmlFragment, "xmlFragment" );

_LIT8( KTagIdentification, "identification" );
_LIT8( KTagManufacturer, "manufacturer" );
_LIT8( KTagModel, "model" );
_LIT8( KTagPlatform, "platform" );
_LIT8( KTagFirmwareVersion, "firmwareVersion" );

_LIT8( KTagDisplay, "display" );
_LIT8( KAttrWidth, "width" );
_LIT8( KAttrHeight, "height" );
_LIT8( KAttrColors, "colors" );

_LIT8( KTagGpsLocation, "gpsLocation" );
_LIT8( KAttrLatitude, "latitude" );
_LIT8( KAttrLongitude, "longitude" );
_LIT8( KAttrElevation, "elevation" );
_LIT8( KAttrTime, "time" );
_LIT8( KAttrVelocity, "velocity" );

_LIT8( KTagClientConfiguration, "clientConfiguration" );

_LIT8( KTagCookies, "cookies" );
_LIT8( KTagCookie, "cookie" );
_LIT8( KAttrKey, "key" );
_LIT8( KAttrValue, "value" );
_LIT8( KTagValue, "value" );
_LIT8( KAttrExpirationDelta, "expirationDelta" );
_LIT8( KAttrScope, "scope" );

_LIT8( KAttrConfExpirationDelta, "configurationExpirationDelta" );
_LIT8( KTagDisabledCapabilities, "disabledCapabilities" );
_LIT8( KTagBodyText, "bodyText" );
_LIT8( KAttrForce, "force" );
_LIT8( KAttrOptional, "optional" );
_LIT8( KAttrTrigger, "trigger" );
_LIT8( KAttrSemantics, "semantics" );
_LIT8( KTagQueryElement, "queryElement" );
_LIT8( KTagOption, "option" );
_LIT8( KTagMime, "mime" );
_LIT8( KAttrResponseUri, "responseUri" );
_LIT8( KTagActionRequest, "actionRequest" );
_LIT8( KTagUpdateDetails, "updateDetails" );
_LIT8( KAttrCode, "code" );
_LIT8( KTagServerDetails, "serverDetails" );
_LIT8( KValueCatalogRequest, "catalogRequest" );
  
_LIT8( KNcdQueryElementSemanticsMsisdn, "msisdn" );
_LIT8( KNcdQueryElementSemanticsEmailAddress, "email" );
_LIT8( KNcdQueryElementSemanticsCreditCardNumber, "cardNumber" );
_LIT8( KNcdQueryElementSemanticsCreditCardExpirationYear, "cardExpYear" );
_LIT8( KNcdQueryElementSemanticsCreditCardExpirationMonth, "cardExpMonth" );
_LIT8( KNcdQueryElementSemanticsCreditCardOwner, "cardOwner" );
_LIT8( KNcdQueryElementSemanticsCreditCardVerificationCode, "cardVerificationCode" );
_LIT8( KNcdQueryElementSemanticsCreditCardType, "cardType" );
_LIT8( KNcdQueryElementSemanticsAddressStreet, "addressStreet" );
_LIT8( KNcdQueryElementSemanticsAddressZipCode, "addressZip" );
_LIT8( KNcdQueryElementSemanticsAddressCity, "addressCity" );
_LIT8( KNcdQueryElementSemanticsAddressCountry, "addressCountry" );
_LIT8( KNcdQueryElementSemanticsUserName, "userName" );
_LIT8( KNcdQueryElementSemanticsPassword, "password" );
_LIT8( KNcdQueryElementSemanticsPinCode, "pinCode" );
_LIT8( KNcdQueryElementSemanticsImei, "imei" );

_LIT8( KNcdQueryElementTypeString, "freetext" );
_LIT8( KNcdQueryElementTypeNumeric, "numeric" );
_LIT8( KNcdQueryElementTypeSingleSelect, "singleselect" );
_LIT8( KNcdQueryElementTypeMultiSelect, "multiselect" );
_LIT8( KNcdQueryElementTypeGpsLocation, "gpsLocation" );
_LIT8( KNcdQueryElementTypeConfiguration, "configuration" );
_LIT8( KNcdQueryElementTypeFile, "file" );
_LIT8( KNcdQueryElementTypeSms, "sms" );
_LIT8( KNcdQueryElementTypePurchaseHistory, "purchaseHistory" );

_LIT8( KNcdQuerySemanticsOperator, "operatorQuery" );
_LIT8( KNcdQuerySemanticsLanguage, "languageQuery" );
_LIT8( KNcdQuerySemanticsRegistration, "registrationQuery" );
_LIT8( KNcdQuerySemanticsAuthentication, "authenticationQuery" );
_LIT8( KNcdQuerySemanticsReview, "reviewQuery" );
_LIT8( KNcdQuerySemanticsInfo, "infoMessage" );
_LIT8( KNcdQuerySemanticsError, "errorMessage" );
_LIT8( KNcdQuerySemanticsDisclaimer, "disclaimer" );
_LIT8( KNcdQuerySemanticsAdvertisment, "adMessage" );
_LIT8( KNcdQuerySemanticsConfirmation, "confirmation" );
_LIT8( KNcdQuerySemanticsPayment, "paymentQuery" );


// Launcher keywords
_LIT( KNcdLauncher, "*[launcher]*" );
_LIT( KNcdLauncherOpen, "*[launcher/open]*" );


#endif
