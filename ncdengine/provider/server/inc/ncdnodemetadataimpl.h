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
* Description:   Contains CNcdNodeMetaData class
*
*/


#ifndef NCD_NODE_META_DATA_H
#define NCD_NODE_META_DATA_H


#include <e32cmn.h>
#include <s32mem.h>
#include <badesca.h>

#include "catalogscommunicable.h"
#include "ncdnodemanager.h"
#include "ncdstoragedataitem.h"
#include "ncdnodeclassids.h"


class CNcdNodeIdentifier;
class MNcdPreminetProtocolDataEntity;
class MNcdPreminetProtocolPurchaseOption;
class CNcdNodeIcon;
class CNcdNodeScreenshot;
class CNcdNodeSkin;
class CNcdPurchaseOptionImpl;
class CNcdNodeDisclaimer;
class CNcdNodeUriContent;
class CNcdNodeDownload;
class CNcdNodeInstall;
class CNcdNodeContentInfo;
class CNcdNodePreview;
class CNcdNodeUpgrade;
class CNcdNodeDependency;
class CNcdServerSubscribableContent;
class CNcdPurchaseDetails;
class MNcdPurchaseDetails;
class CNcdNodeUserData;
class CNcdKeyValuePair;

/**
 *  CNcdNodeMetaData ...
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeMetaData : public CCatalogsCommunicable,
                         public MNcdStorageDataItem
    {

public:

    /**
     * Destructor
     */
    virtual ~CNcdNodeMetaData();


    /**
     * @return The nodemanager that has created this node metadata.
     */
    CNcdNodeManager& NodeManager() const;


    /**
     *
     */
    const CNcdNodeIdentifier& Identifier() const;
    

    /**
     * Retrieves the data type that informs what class the data is for.
     * By checking the data type information, an InternalizeL function
     * of a right class can be called when the object data is set
     * from the storage.
     * The data type may be decided and set in a factory that creates object.
     * The factory should know which integer is reserved for which class.
     *
     * @return NcdNodeClassIds::TNcdNodeClassId Describes the data type. 
     */
    NcdNodeClassIds::TNcdNodeClassId ClassId() const;


    /**
     *
     */
    const TDesC& TimeStamp() const;

    /**
     *
     */
    const TDesC& NodeName() const;
    
    void SetNodeNameL( const TDesC& aName );

    /**
     *
     */
    const TDesC& Description() const;
    
    void SetDescriptionL( const TDesC& aDescription);
    
    /**
     * Returns the layout type if defined.
     */
    const TDesC& LayoutType() const;

    /**
     * @exception KErrNotFound if the disclaimer does not exist.
     */
    const CNcdNodeDisclaimer& DisclaimerL() const;
    
    /**
     * Disclaimer setter. Used by bundle load operation for setting
     * the bundle disclaimer
     */
    void SetDisclaimer( CNcdNodeDisclaimer* aDisclaimer );

    /**
     *
     */
    CNcdNodeIcon& IconL() const;
    
    void SetIcon( CNcdNodeIcon* aIcon );

    /**
     *
     */
    const CNcdNodeScreenshot& ScreenshotL() const;

    /**
     *
     */
    const CNcdNodeSkin& SkinL() const;


    /**
     *
     */
    CNcdNodePreview& PreviewL() const;
    
    
    /**
     *
     */
    const CNcdNodeUpgrade& UpgradeL() const;


    /**
     *
     */
    const CNcdNodeDependency& DependencyL() const;

    
    /**
     * Content info getter
     *
     * @return Content info
     */     
    const CNcdNodeContentInfo& ContentInfoL() const;


    const CNcdNodeDisclaimer& MoreInfoL() const;

    /**
     *
     */
    const CNcdServerSubscribableContent* SubscribableContent() const;
    
    /**
     *
     */
    const RPointerArray<CNcdPurchaseOptionImpl>& PurchaseOptions() const;

    CNcdPurchaseOptionImpl& PurchaseOptionByIdL(
        const TDesC& aPurchaseOptionId ) const;
    
    /**
     * Tells whether the node should be always visible in UI.
     *
     * @return True if always visible, otherwise false.
     */
    TBool AlwaysVisible() const;
    
    /**
     * Sets the always visible property.
     *
     * @param aValue The new value.
     */
    void SetAlwaysVisible( TBool aValue );


    /**
     *
     */
    void InternalizeL( MNcdPreminetProtocolDataEntity& aData );
    

    void InternalizeContentInfoL( const MNcdPurchaseDetails& aDetails );

    void InternalizeUriContentL( const MNcdPurchaseDetails& aDetails );

    
    /**
     * Get content download.
     *
     * @return Reference to the node download. 
     * @exception KErrNotFound if node download has not been set.
     */
    CNcdNodeDownload& DownloadL();

    /**
     * Internalizes data to the node download from purchase history. If the
     * node download does not exists it is created.
     */
    void InternalizeDownloadL( const MNcdPurchaseDetails& aDetails );


    /**
     * Get content install.
     *
     * @return Reference to the node install. 
     * @exception KErrNotFound if node install has not been set.
     */    
    CNcdNodeInstall& InstallL();

    /**
     * Internalizes data to the node install from purchase history. If the
     * node install does not exists it is created.
     */    
    void InternalizeInstallL( const MNcdPurchaseDetails& aDetails );

    /**
     * Internalizes install from content info. Creates node install if necessary
     */
    void InternalizeInstallFromContentInfoL();
    
    /**
     * Internalizes dependency information from purchase history
     *
     * @param aDetails Purchase details
     */
    void InternalizeDependencyL( const MNcdPurchaseDetails& aDetails );

    /**
     * Internalizes icon information from purchase history
     *
     * @param aDetails Purchase details
     */
    void InternalizeIconL( const MNcdPurchaseDetails& aDetails );
    
    /**
     * Get purchase details from the purchase history
     */
    CNcdPurchaseDetails* PurchaseDetailsLC( TBool aLoadIcon = EFalse ) const;
    
    
    /**
     * Updates CNcdNodeUpgrade from CNcdNodeContentInfo and CNcdNodeInstall
     */
    TBool HandleContentUpgradeL();
            
    
    void SetDeleteSoon( TBool aDeleteSoon );
    
    
    TBool DeleteSoon() const;
    
        
public: // MNcdStorageDataItem

    /**
     * @see MNcdStorageDataItem::ExternalizeL
     */
    virtual void ExternalizeL( RWriteStream& aStream );


    /**
     * @see MNcdStorageDataItem::InternalizeL
     */
    virtual void InternalizeL( RReadStream& aStream );


public: // CCatalogsCommunicable

    /**
     * @see CCatalogsCommunicable::ReceiveMessage
     */
    virtual void ReceiveMessage( MCatalogsBaseMessage* aMessage,
                                 TInt aFunctionNumber );

    /**
     * @see CCatalogsCommunicable::CounterPartLost
     */
    virtual void CounterPartLost( const MCatalogsSession& aSession );


protected:

    /**
     * Constructor
     */
    CNcdNodeMetaData(
        NcdNodeClassIds::TNcdNodeClassId aClassId,
        CNcdNodeManager& aNodeManager );

    /**
     * ConstructL
     */
    virtual void ConstructL( const CNcdNodeIdentifier& aIdentifier );


    /**
     * This function is called when the proxy wants to get the
     * data from the serverside. This function calls the
     * InternalizeDataForRequestL which may be overloaded in the
     * child classes
     */
    void InternalizeRequestL( MCatalogsBaseMessage& aMessage ) const;
    /**
     * This function writes the object data to the stream. Child
     * classes should add theri own data after the parent data.
     */
    virtual void ExternalizeDataForRequestL( RWriteStream& aStream ) const;


    /**
     *
     */
    void InternalizePurchaseMeansRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     * This function writes the purchase option amount and handles
     * to the stream. 
     */        
    void ExternalizePurchaseMeansForRequestL(
        const CDesCArray& aPurchaseOptionIds,
        RWriteStream& aStream,
        MCatalogsSession& aSession ) const;

    /**
     *
     */
    void InternalizePurchaseHistoryRequestL( MCatalogsBaseMessage& aMessage ) const;

    /**
     *
     */
    void ExternalizePurchaseHistoryForRequestL( RWriteStream& aStream ) const;
    
        
    /**
     * This function is called when the proxy wants to get purchase
     * option ids from the server side.
     */
    void PurchaseOptionIdsRequestL( MCatalogsBaseMessage& aMessage ) const;
        

    void ReleaseRequest( MCatalogsBaseMessage& aMessage ) const;

    void IconIdRequestL( MCatalogsBaseMessage& aMessage ) const;
    void IconDataRequestL( MCatalogsBaseMessage& aMessage ) const;
    
    void UserDataHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void DisclaimerHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void IconHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void ScreenshotHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void SkinHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void UriContentHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void ContentInfoHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void PreviewHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void UpgradeHandleRequestL( MCatalogsBaseMessage& aMessage );
    void DependencyHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void DownloadHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void InstallHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void MoreInfoHandleRequestL( MCatalogsBaseMessage& aMessage ) const;
    void IsPurchaseSupportedRequestL( MCatalogsBaseMessage& aMessage ) const;

private:

    // Prevent these two if they are not implemented
    CNcdNodeMetaData( const CNcdNodeMetaData& aObject );
    CNcdNodeMetaData& operator =( const CNcdNodeMetaData& aObject );


    // Functions related to internalization of purchase options

    
    /**
     * Function that creates needed purchaseoption and internalizes
     * it from the given data or reinternalizes already existing
     * purchase option if purchase option with the same id is found
     */
    void InternalizePurchaseOptionL(
        const MNcdPreminetProtocolPurchaseOption& aData );
    
    /**
     * Gets existing purchase option with given id or if
     * one is not found creates one and inserts it into array
     * containing existing purchase options. Option can be
     * removed from the array later if necessary with
     * RemovePurchaseOptionL() function.
     */
    CNcdPurchaseOptionImpl& CreateOrGetPurchaseOptionL(
        const TDesC& aPurchaseOptionId );
    
    /**
     * Finds purchase option with given id and removes it
     * from the array of the current purchase options. Also
     * closes the option once.
     */
    void RemovePurchaseOptionL( const TDesC& aPurchaseOptionId );
    
    /**
     * Removes all purchase options that have not been recently
     * updated. Used to remove purchase options that are still in
     * memory but are not received from the server anymore.
     * Also sets all options as not recently updated.
     */
    void RemoveNotUpdatedPurchaseOptions();

    /**
     * Function that creates needed purchaseoption and internalizes
     * it from the given data or reinternalizes already existing
     * purchase option if purchase option with the same id is found
     */
    void InternalizePurchaseOptionL( RReadStream& aStream );


    /**
     * Updates free content URIs to purchase history if they have been
     * 'bought'. Also internalizes CNcdNodeContentUri
     */
    void HandleContentUriUpdateL( const CNcdPurchaseOptionImpl& aOption );
    
    
private: // data
    
    // The class id is the id for this class that a factory uses
    // when it creates an object of this class.
    NcdNodeClassIds::TNcdNodeClassId  iClassId;
    
    // Node manager
    CNcdNodeManager& iNodeManager;
   
    // This info is set already during creation of the object
    CNcdNodeIdentifier* iIdentifier;
    
    // This information is set when internalizing
    HBufC* iTimeStamp;
    HBufC* iName;
    HBufC* iDescription;
    HBufC* iLayoutType;
    
    // This class owns user data if it has been created.
    CNcdNodeUserData* iUserData;    
    
    CNcdNodeDisclaimer* iDisclaimer;
    
    CNcdNodeIcon* iIcon;
    
    CNcdNodeScreenshot* iScreenshot;

    CNcdNodeSkin* iSkin;

    CNcdNodePreview* iPreview;
    
    CNcdServerSubscribableContent* iSubscribableContent;
    RPointerArray<CNcdPurchaseOptionImpl> iPurchaseOptions;

    CNcdNodeUriContent* iUriContent;
    
    CNcdNodeContentInfo* iContentInfo;

    // This class owns node download if it has been set.
    CNcdNodeDownload* iDownload;
    
    // This class owns node install if it has been set
    CNcdNodeInstall* iInstall;
    
    CNcdNodeDisclaimer* iMoreInfo;
    
    RPointerArray<CNcdKeyValuePair> iDetails;

    CNcdNodeUpgrade* iUpgrade;

    CNcdNodeDependency* iDependency;
    
    TBool iAlwaysVisible;

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
        
    // ETrue if the metadata object should be deleted as soon as possible
    // from memory. It's not necessary save this on disk
    TBool iDeleteSoon;
    
    // Version of the bought content if any. This is read from purchase history 
    // This is here because NodeInstall may not be available when 
    // this is needed and making it available in those cases would make
    // things much more complicated    
    TCatalogsVersion iBoughtContentVersion;
    };
    
#endif // NCD_NODE_META_DATA_H    
