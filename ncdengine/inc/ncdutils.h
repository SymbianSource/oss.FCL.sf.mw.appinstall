/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Public utility classes for NCD provider usage.
*
*/


#ifndef M_NCD_UTILS_H
#define M_NCD_UTILS_H


#include <e32cmn.h>
#include <s32strm.h>
#include <badesca.h>

#include "ncdkeyvaluepair.h"
#include "ncdpurchasedownloadinfo.h"
#include "ncdpurchaseinstallinfo.h"
#include "ncdpurchasehistoryfilter.h"
#include "ncdpurchasedetails.h"
#include "ncdsearchfilter.h"

class MNcdClientLocalizer;
class CNcdString;
class CNcdAttributes;

/**
 *  A default implementation for the MNcdKeyValuePair interface,
 *  providing storage for the key and value in simple heap allocated
 *  buffers.
 *
 *  @lib ncdutils_20019119.dll
 *  
 *  @see MNcdKeyValuePair
 */
class CNcdKeyValuePair : public CBase, public MNcdKeyValuePair
    {
public: // Construction & destruction

    /**
     * Constructor.
     *
     * @param aKey Key. Can not be empty
     * @param aValue Value. Can be an empty descriptor
     * 
     * @return Pointer to the created object.
     * @exception System wide error code.
     * @throw KErrArgument if aKey is empty
     */
    IMPORT_C static CNcdKeyValuePair* NewL( 
        const TDesC& aKey, const TDesC& aValue );
    
    /**
     * Constructor, leaves pointer on clean-up stack.
     *
     * @param aKey Key. Can not be empty
     * @param aValue Value. Can be an empty descriptor
     * 
     * @return Pointer to the created object.
     * @exception System wide error code.
     * @throw KErrArgument if aKey is empty
     */
    IMPORT_C static CNcdKeyValuePair* NewLC(
        const TDesC& aKey, const TDesC& aValue );
    

    /**
    * Creates a copy of an key-value -pair.
    * 
    * @param aOther Pair to copy
    * @return A copy of the given pair
    */
    IMPORT_C static CNcdKeyValuePair* NewL( const MNcdKeyValuePair& aOther );

    
   /**
    * Creates a copy of the key-value -pair, leaves pointer on clean-up stack
    * 
    * @param aOther Pair to copy
    * @return A copy of the given pair
    */
    IMPORT_C static CNcdKeyValuePair* NewLC( const MNcdKeyValuePair& aOther );


    /** 
     * Internalizes the key-value pair from a stream
     *
     * @param aStream
     * @return A new key-value pair
     */     
    IMPORT_C static CNcdKeyValuePair* NewL( RReadStream& aStream );


    /** 
     * Internalizes the key-value pair from a stream
     *
     * @param aStream
     * @return A new key-value pair
     */     
    IMPORT_C static CNcdKeyValuePair* NewLC( RReadStream& aStream );
    
            
    /**
     * Destructor.
     *
     * 
     */
    virtual ~CNcdKeyValuePair();

public: // from MNcdKeyValuePair

    /**
     * Returns the key set for the pair.
     * 
     * 
     * @return Key of the pair.
     */
    const TDesC& Key() const;

    /**
     * Returns the value set for the pair.
     * 
     * 
     * @return Value of the pair.
     */
    const TDesC& Value() const;

public: // new functions

    /**
     * Set the key for the pair.
     * 
     * 
     * @param aKey Key for the pair.     
     * @throw KErrArgument if the key is empty
     */
    IMPORT_C void SetKeyL( const TDesC& aKey );

    /**
     * Set the value for the pair.
     * 
     * 
     * @param aValue value for the pair.
     */
    IMPORT_C void SetValueL( const TDesC& aValue );
    

    /** 
     * Comparison method. Compares the keys of two pairs to 
     * determine equivalence.
     *
     * @return  Boolean, ETrue if pairs match.
     */
    IMPORT_C static TBool MatchByKey( 
        const CNcdKeyValuePair& aFirst, 
        const CNcdKeyValuePair& aSecond );


    /**
     * Externalizes the pair to a stream
     *
     * @param aStream Target stream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;


    /**
     * Externalizes the pair to a descriptor
     *
     * @return Descriptor containing the pair
     */
    IMPORT_C HBufC8* ExternalizeToDesLC() const;
    

    /**
     * Internalizes the pair from a stream
     *
     * @param aStream Source stream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

private: // Construction & assignment

    /**
     * Private 2nd phase constructor.
     */
    void ConstructL( const TDesC& aKey, const TDesC& aValue );
    
    /**
     * Private constructor.
     */
    CNcdKeyValuePair();

private: // Data members

    // Key of the pair
    HBufC* iKey;
    
    // Value of the pair
    HBufC* iValue;
    
    };


/**
 *  A default implementation for the MNcdPurchaseDownloadInfo interface.
 *
 *  @lib ncdutils_20019119.dll
 *  
 *  @see MNcdPurchaseDownloadInfo
 */
class CNcdPurchaseDownloadInfo : public CBase,
                                 public MNcdPurchaseDownloadInfo
    {
public: // construction and destruction

    /**
     * Constructor.
     * 
     * 
     * @return Download information class.
     */
    IMPORT_C static CNcdPurchaseDownloadInfo* NewL();

    /**
     * Constructor.
     * 
     * 
     * @return Download information class.
     */
    IMPORT_C static CNcdPurchaseDownloadInfo* NewLC();

    /**
     * Copies and returns new download information.
     * 
     * 
     * @param aSource Download information to be copied.
     * @return Download information class.
     */
    IMPORT_C static CNcdPurchaseDownloadInfo* NewL( 
        const MNcdPurchaseDownloadInfo& aSource );

    /**
     * Copies and returns new download information.
     * 
     * 
     * @param aSource Download information to be copied.
     * @return Download information class.
     */
    IMPORT_C static CNcdPurchaseDownloadInfo* NewLC(
        const MNcdPurchaseDownloadInfo& aSource );

    /**
     * Destructor.
     * Because a class object outside of this library may be inherited 
     * from this class, this virtual destructor is also set to 
     * IMPORT_C.
     */
    IMPORT_C virtual ~CNcdPurchaseDownloadInfo();

public: // from MNcdPurchaseDownloadInfo

    /**
     * Gets the content usage type.
     * 
     * 
     * @return Content usage.
     */
    IMPORT_C TContentUsage ContentUsage() const;

    /**
     * Gets the content URI.
     * 
     * 
     * @return Content URI.
     */
    IMPORT_C const TDesC& ContentUri() const;

    /**
     * Gets the content validity delta.
     * 
     * 
     * @return Content validity delta. -1 if not set.
     */
    IMPORT_C TInt ContentValidityDelta() const;

    /**
     * Gets the content mime type.
     * 
     * 
     * @return Content mime type.
     */
    IMPORT_C const TDesC& ContentMimeType() const;

    /**
     * Gets the content size.
     * 
     * 
     * @return Content size.
     */
    IMPORT_C TInt ContentSize() const;

    /**
     * Is content launchable.
     * 
     * 
     * @return Launchable status.
     */
    IMPORT_C TBool IsLaunchable() const;

    /**
     * Gets the descriptor type.
     * 
     * 
     * @return Descriptor type.
     */
    IMPORT_C const TDesC& DescriptorType() const;

    /**
     * Gets the descriptor name.
     * 
     * 
     * @return Descriptor name.
     */
    IMPORT_C const TDesC& DescriptorName() const;

    /**
     * Gets the descriptor URI.
     * 
     * 
     * @return Descriptor URI.
     */
    IMPORT_C const TDesC& DescriptorUri() const;

    /**
     * Gets the descriptor data.
     * 
     * 
     * @return Descriptor data.
     */
    IMPORT_C const TDesC8& DescriptorData() const;

    /**
     * Gets the rights URI.
     * 
     * 
     * @return Rights URI.
     */
    IMPORT_C const TDesC& RightsUri() const;

    /**
     * Gets the rights type.
     * 
     * 
     * @return Rights type.
     */
    IMPORT_C const TDesC& RightsType() const;

    /**
     * Gets the activation key.
     *
     * 
     * @return Activation key.
     */
    IMPORT_C const TDesC& ActivationKey() const;
    
    /**
     * Gets the install notification URI.
     *
     * 
     * @return Install notification URI.
     */
    IMPORT_C const TDesC& InstallNotificationUri() const;
    
    
    /**
     * @see MNcdDownloadInfo::AttributeStringL
     */
    IMPORT_C const TDesC& AttributeStringL( TDownloadAttribute aAttribute ) const;
    

    /**
     * @see MNcdDownloadInfo::AttributeInt32L
     */
    IMPORT_C TInt32 AttributeInt32L( TDownloadAttribute aAttribute ) const;
    
    
public: // new methods

    /**
     * Externalizer.
     * 
     * 
     * @param aStream Write stream.
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalizer.
     * 
     * 
     * @param aStream Read stream.
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * Sets the content usage type.
     * 
     * 
     * @param aUsage Content usage.
     */
    IMPORT_C void SetContentUsage( TContentUsage aUsage );

    /**
     * Sets the content URI.
     * 
     * 
     * @param aUri Content URI.
     */
    IMPORT_C void SetContentUriL( const TDesC& aUri );

    /**
     * Sets the content validity delta.
     * 
     * 
     * @return Content validity delta.
     */
    IMPORT_C void SetContentValidityDelta( TInt aValidityDelta );

    /**
     * Sets the content mime type.
     * 
     * 
     * @param aMimeType Content mime type.
     */
    IMPORT_C void SetContentMimeTypeL( const TDesC& aMimeType );

    /**
     * Sets the content size.
     * 
     * 
     * @param aSize Content size.
     */
    IMPORT_C void SetContentSize( TInt aSize );

    /**
     * Sets content launchable.
     * 
     * 
     * @param aLaunchable Launchable status.
     */
    IMPORT_C void SetLaunchable( TBool aLaunchable );

    /**
     * Sets the descriptor type.
     * 
     * 
     * @param aType Descriptor type.
     */
    IMPORT_C void SetDescriptorTypeL( const TDesC& aType );

    /**
     * Sets the descriptor name.
     * 
     * 
     * @param aName Descriptor name.
     */
    IMPORT_C void SetDescriptorNameL( const TDesC& aName );

    /**
     * Sets the descriptor URI.
     * 
     * 
     * @param aUri Descriptor URI.
     */
    IMPORT_C void SetDescriptorUriL( const TDesC& aUri );

    /**
     * Sets the descriptor data.
     * 
     * 
     * @param aData Descriptor data.
     */
    IMPORT_C void SetDescriptorDataL( const TDesC8& aData );

    /**
     * Sets the rights URI.
     * 
     * 
     * @param aUri Rights URI.
     */
    IMPORT_C void SetRightsUriL( const TDesC& aUri );

    /**
     * Sets the rights type.
     * 
     * 
     * @param aType Rights type.
     */
    IMPORT_C void SetRightsTypeL( const TDesC& aType );

    /**
     * Sets the activation key.
     * 
     * 
     * @param aActivationKey Activation key.
     */
    IMPORT_C void SetActivationKeyL( const TDesC& aActivationKey );

    /**
     * Sets the install notification URI.
     * 
     * 
     * @param aInstallNotificationUri Install notification URI.
     */
    IMPORT_C void SetInstallNotificationUriL( const TDesC& aInstallNotificationUri );
    
    /**
     * Integer attribute setter
     */
    IMPORT_C void SetAttributeL( TDownloadAttribute aAttribute, TInt32 aValue );
    
    /**
     * String attribute setter
     */
    IMPORT_C void SetAttributeL( TDownloadAttribute aAttribute, const TDesC& aValue );
    

protected: // Protected and imported constructor functions enable inheritance
           // outside of this library.

    /**
     * Constructor.
     */
    IMPORT_C CNcdPurchaseDownloadInfo();

    /**
     * 2nd phase constructor.
     */
    IMPORT_C void ConstructL();

    /**
     * Alternative 2nd phase constructor.
     */
    IMPORT_C void ConstructL( const MNcdPurchaseDownloadInfo& aSource );


    /**
     * Creates attributes if they haven't been created yet
     */
    IMPORT_C void CreateAttributesL();
    
        
private:

    // Content usage.
    TContentUsage iContentUsage;

    // Content URI.
    HBufC* iContentUri;
    
    // Content validity delta.
    TInt iValidityDelta;

    // Content mime type.
    HBufC* iContentMimeType;

    // Content size.
    TInt iContentSize;

    // Launchable status.
    TBool iLaunchable;

    // Descriptor type.
    HBufC* iDescriptorType;

    // Descriptor name.
    HBufC* iDescriptorName;

    // Descriptor URI.
    HBufC* iDescriptorUri;

    // Descriptor data.
    HBufC8* iDescriptorData;

    // Rights URI.
    HBufC* iRightsUri;

    // Rights type.
    HBufC* iRightsType;

    // Activation key.
    HBufC* iActivationKey;

    // Install notification URI.
    HBufC* iInstallNotificationUri;

    CNcdAttributes* iAttributes;
    };


/**
 *  A default implementation for the MNcdPurchaseInstallInfo interface.
 *
 *  @lib ncdutils_20019119.dll
 *  
 *  @see MNcdPurchaseInstallInfo
 */
class CNcdPurchaseInstallInfo : public CBase,
                                public MNcdPurchaseInstallInfo
    {
public: // construction and destruction

    /**
     * Constructor.
     * 
     * 
     * @return File installation information class.
     */
    IMPORT_C static CNcdPurchaseInstallInfo* NewL();

    /**
     * Constructor.
     * 
     * 
     * @return File installation information class.
     */
    IMPORT_C static CNcdPurchaseInstallInfo* NewLC();

    /**
     * Copies and returns new file installation information.
     * 
     * 
     * @param aSource File installation information to be copied.
     * @return File installation information class.
     */
    IMPORT_C static CNcdPurchaseInstallInfo* NewL( 
        const MNcdPurchaseInstallInfo& aSource );

    /**
     * Copies and returns new file installation information.
     * 
     * 
     * @param aSource File installation information to be copied.
     * @return File installation information class.
     */
    IMPORT_C static CNcdPurchaseInstallInfo* NewLC(
        const MNcdPurchaseInstallInfo& aSource );

    /**
     * Destructor
     */
    IMPORT_C virtual ~CNcdPurchaseInstallInfo();

public: // from MNcdPurchaseInstallInfo

    /**
     * Get name and path of installed file.
     * 
     * 
     * @return File name.
     */
    IMPORT_C const TDesC& Filename() const;
    
    /**
     * Get application UID of installed application.
     * 
     * 
     * @return TUid Application UID.
     */
    IMPORT_C TUid ApplicationUid() const;
    
    /**
     * Get application version of installed application.
     * 
     * 
     * @return Application version.
     */
    IMPORT_C const TDesC& ApplicationVersion() const;
    
    /**
     * Get name of installed theme.
     * 
     * 
     * @return Theme name.
     */
    IMPORT_C const TDesC& ThemeName() const;
    

public: // new methods


    /**
     * Externalizer
     * 
     * 
     * @param aStream Write stream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizer
     * 
     * 
     * @param aStream Read stream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * Set name and path of installed file.
     * 
     * 
     * @param aFilename File name.
     */
    IMPORT_C void SetFilenameL( const TDesC& aFilename );
    
    /**
     * Set application UID of installed application.
     * 
     * 
     * @param aApplicationUid Application UID.
     */
    IMPORT_C void SetApplicationUid( const TUid aApplicationUid );

    /**
     * Set application version of installed application.
     * 
     * 
     * @param aApplicationVersion Application version.
     */
    IMPORT_C void SetApplicationVersionL( const TDesC& aApplicationVersion );
    
    /**
     * Set name of installed theme.
     * 
     * 
     * @param aThemeName Theme name.
     */
    IMPORT_C void SetThemeNameL( const TDesC& aThemeName );

protected:

    /**
     * Private 2nd phase constructor.
     */
    IMPORT_C void BaseConstructL();


    /**
     * Private 2nd phase constructor.
     */
    IMPORT_C void BaseConstructL( const MNcdPurchaseInstallInfo& aSource );

    /**
     * Private constructor.
     */
    IMPORT_C CNcdPurchaseInstallInfo();

private:

    // Name and path of installed file.
    HBufC* iFilename;

    // Application UID of installed application.
    TUid iApplicationUid;

    // Application version of installed application.
    HBufC* iApplicationVersion;

    // Name of installed theme.
    HBufC* iThemeName;

    };


/**
 *  A default implementation for the MNcdPurchaseHistoryFilter interface.
 *
 *  @lib ncdutils_20019119.dll
 *  
 *  @see MNcdPurchaseHistoryFilter
 */
 class CNcdPurchaseHistoryFilter : public CBase,
                                   public MNcdPurchaseHistoryFilter
    {
public: // Construction & destruction

    /**
     * Constructor.
     * @return CNcdPurchaseHistoryFilter* Filter for purchase event queries.
     */
    IMPORT_C static CNcdPurchaseHistoryFilter* NewL();
    
    /**
     * Constructor.
     * @return CNcdPurchaseHistoryFilter* Filter for purchase event queries.
     */
    IMPORT_C static CNcdPurchaseHistoryFilter* NewLC();
    
    /**
     * Constructor.
     * @param aClientUids Client UIDs.
     * @param aNamespace Namespace. Can be KNullDesC.
     * @param aEntityId Entity ID. Can be KNullDesC.
     * @param aEventId Event ID.
     * @return CNcdPurchaseHistoryFilter* Filter for purchase event queries.
     */
    IMPORT_C static CNcdPurchaseHistoryFilter* NewL(
        TArray< TUid > aClientUids,
        const TDesC& aNamespace,
        const TDesC& aEntityId,
        const TUint aEventId );
    
    /**
     * Constructor.
     * @param aClientUids Client UIDs.
     * @param aNamespace Namespace. Can be KNullDesC.
     * @param aEntityId Entity ID. Can be KNullDesC.
     * @param aEventId Event ID.
     * @return CNcdPurchaseHistoryFilter* Filter for purchase event queries.
     */
    IMPORT_C static CNcdPurchaseHistoryFilter* NewLC(
        TArray< TUid > aClientUids,
        const TDesC& aNamespace,
        const TDesC& aEntityId,
        const TUint aEventId );

    /**
     * Destructor.
     */
    virtual ~CNcdPurchaseHistoryFilter();

private: // Construction

    /**
     * Private constructor.
     * @param aClientUids Client UIDs.
     * @param aNamespace Namespace. Can be KNullDesC.
     * @param aEntityId Entity ID. Can be KNullDesC.
     * @param aEventId Event ID.
     */
    void ConstructL(
        TArray< TUid > aClientUids,
        const TDesC& aNamespace,
        const TDesC& aEntityId,
        const TUint aEventId );
    
    /**
     * Private constructor.
     */
    CNcdPurchaseHistoryFilter();

public: // from MNcdPurchaseHistoryFilter

    /**
     * @see MNcdPurchaseHistoryFilter::ClientUids
     */
    const TArray< TUid > ClientUids() const;

    /**
     * @see MNcdPurchaseHistoryFilter::Namespace
     */
    const TDesC& Namespace() const;

    /**
     * @see MNcdPurchaseHistoryFilter::EntityId
     */
    const TDesC& EntityId() const;

    /**
     * @see MNcdPurchaseHistoryFilter::EventId
     */
    TUint EventId() const;


public: // New functions

    /**
     * Externalizer
     * @param aStream Write stream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizer
     * @param aStream Read stream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
     * Set client UIDs.
     * @param aClientUids Client UIDs.
     */
    IMPORT_C void SetClientUids( const TArray< TUid > aClientUids );

    /**
     * Set namespace.
     * @param aNamespace Namespace. Can be KNullDesC.
     */
    IMPORT_C void SetNamespaceL( const TDesC& aNamespace );

    /**
     * Set entity ID.
     * @param aEntityId Entity ID. Can be KNullDesC.
     */
    IMPORT_C void SetEntityIdL( const TDesC& aEntityId );

    /**
     * Set event ID.
     * @param aEventId Event ID.
     */
    IMPORT_C void SetEventId( const TUint aEventId );

private: // Data members

    RArray< TUid > iClientUids;
    HBufC* iNamespace;
    HBufC* iEntityId;
    TUint iEventId;

    };


/**
 *  A default implementation for the MNcdPurchaseDetails interface.
 *
 *  @lib ncdutils_20019119.dll
 *  
 *  @see MNcdPurchaseDetails
 * 
 *  The number of DownloadedFiles must always match the number of
 *  DownloadInfos. DownloadFiles are added/removed automatically if necessary
 *  when the details are saved to purchase history
 * 
 *  The number of InstallInfos must be equal to or lesser than the number
 *  of DownloadInfos. Excess InstallInfos are removed when the details
 *  are saved to purchase history 
 */
class CNcdPurchaseDetails : public CBase,
                            public MNcdPurchaseDetails
    {
public: // Construction & destruction

    /**
     * Constructor.
     *
     * 
     * @return const CNcdPurchaseDetails* Purchase details.
     * @exception Leave System wide error code.
     */
    IMPORT_C static CNcdPurchaseDetails* NewL();
    
    /**
     * Constructor. Leaves pointer on cleanup stack.
     *
     * 
     * @return const CNcdPurchaseDetails* Purchase details.
     * @exception Leave System wide error code.
     */
    IMPORT_C static CNcdPurchaseDetails* NewLC();
    
    /**
     * Copy constructor.
     *
     * 
     * @param aDetails Details object to copy information from.
     * @return const CNcdPurchaseDetails* Purchase details.
     * @exception Leave System wide error code.
     */
    IMPORT_C static CNcdPurchaseDetails* NewL( const MNcdPurchaseDetails& aDetails );
    
    /**
     * Copy constructor. Leaves pointer on cleanup stack.
     *
     * 
     * @param aDetails Details object to copy information from.
     * @return const CNcdPurchaseDetails* Purchase details.
     * @exception Leave System wide error code.
     */
    IMPORT_C static CNcdPurchaseDetails* NewLC( const MNcdPurchaseDetails& aDetails );
    
    /**
     * Destructor.
     */
    virtual ~CNcdPurchaseDetails();
    
public: // From MNcdPurchaseDetails

    /**
     * @see MNcdPurchaseDetails::State
     */
    TState State() const;

    /**
     * @see MNcdPurchaseDetails::ClientUid
     */
    TUid ClientUid() const;
    
    /**
     * @see MNcdPurchaseDetails::Namespace
     */
    const TDesC& Namespace() const;
    
    /**
     * @see MNcdPurchaseDetails::EntityId
     */
    const TDesC& EntityId() const;
    
    /**
     * @see MNcdPurchaseDetails::ItemName
     */
    const TDesC& ItemName() const;

    /**
     * @see MNcdPurchaseDetails::ItemPurpose
     */
    TUint ItemPurpose() const;
    
    /**
     * @see MNcdPurchaseDetails::CatalogSourceName
     */
    const TDesC& CatalogSourceName() const;
    
    /**
     * @see MNcdPurchaseDetails::DownloadInfoL
     */
    TArray< MNcdPurchaseDownloadInfo* > DownloadInfoL() const;
    
    /**
     * @see MNcdPurchaseDetails::PurchaseOptionId
     */
    const TDesC& PurchaseOptionId() const;
    
    /**
     * @see MNcdPurchaseDetails::PurchaseOptionName
     */
    const TDesC& PurchaseOptionName() const;
    
    /**
     * @see MNcdPurchaseDetails::PurchaseOptionPrice
     */
    const TDesC& PurchaseOptionPrice() const;
    
    /**
     * @see MNcdPurchaseDetails::FinalPrice
     */
    const TDesC& FinalPrice() const;
    
    /**
     * @see MNcdPurchaseDetails::PaymentMethodName
     */
    const TDesC& PaymentMethodName() const;
    
    /**
     * @see MNcdPurchaseDetails::PurchaseTime
     */
    TTime PurchaseTime() const;
    
    /**
     * @see MNcdPurchaseDetails::DownloadedFiles
     */
    const MDesCArray& DownloadedFiles() const;
    
    /**
     * @see MNcdPurchaseDetails::InstallInfoL
     */
    TArray< MNcdPurchaseInstallInfo* > InstallInfoL() const;

    /**
     * @see MNcdPurchaseDetails::Icon
     */
    const TDesC8& Icon() const;

    /**
     * @see MNcdPurchaseDetails::HasIcon
     */
    TBool HasIcon() const;

    /**
     * @see MNcdPurchaseDetails::DownloadAccessPoint
     */
    const TDesC& DownloadAccessPoint() const;

    /**
     * @see MNcdPurchaseDetails::Description
     */
    const TDesC& Description() const;

    /**
     * @see MNcdPurchaseDetails::Version
     */
    const TDesC& Version() const;
    
    /**
     * @see MNcdPurchaseDetails::ServerUri
     */
    const TDesC& ServerUri() const;

    /**
     * @see MNcdPurchaseDetails::ItemType
     */
    TItemType ItemType() const;
    
    /**
     * @see MNcdPurchaseDetails::TotalContentSize
     */
    TInt TotalContentSize() const;
    
    /**
     * @see MNcdPurchaseDetails::OriginNodeId
     */
    const TDesC& OriginNodeId() const;

    /**
     * @see MNcdPurchaseDetails::LastOperationTime
     */
     TTime LastOperationTime() const;
    
    /**
     * @see MNcdPurchaseDetails::LastOperationErrorCode
     */
    TInt LastOperationErrorCode() const;    

    
    /**
     * @see MNcdPurchaseDetails::AttributeString
     */
    const TDesC& AttributeString( TPurchaseAttribute aAttribute ) const;
    
        
    /**
     * @see MNcdPurchaseDetails::AttributeInt32
     */
    TInt32 AttributeInt32( TPurchaseAttribute aAttribute ) const;
    
public: // New functions

    /**
     * Externalizer
     * @param aStream Write stream.
     * @exception Leave System wide error code.
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalizer
     * @param aStream Read stream.
     * @exception Leave System wide error code.
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * Set client UID.
     *
     * 
     * @param aClientUid Client UID.
     */
    IMPORT_C void SetClientUid( TUid aClientUid );
    
    /**
     * Set namespace.
     *
     * 
     * @param aNamespace Namespace.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetNamespaceL( const TDesC& aNamespace );
    
    /**
     * Set namespace.
     *
     * 
     * @param aNamespace Namespace. Ownership is transferred.
     */
    IMPORT_C void SetNamespace( HBufC* aNamespace );
    
    /**
     * Set entity ID.
     *
     * 
     * @param aEntityId Entity ID.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetEntityIdL( const TDesC& aEntityId );
    
    /**
     * Set entity ID.
     *
     * 
     * @param aEntityId Entity ID. Ownership is transferred.
     */
    IMPORT_C void SetEntityId( HBufC* aEntityId );
    
    /**
     * Set item name.
     *
     * 
     * @param aItemName Item name.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetItemNameL( const TDesC& aItemName );

    /**
     * Set item name.
     *
     * 
     * @param aItemName Item name. Ownership is transferred.
     */
    IMPORT_C void SetItemName( HBufC* aItemName );

    /**
     * Set item purpose.
     *
     * 
     * @param aItemPurpose Item purpose. Bit field of TNcdItemPurpose flags.
     * @see TNcdItemPurpose
     */
    IMPORT_C void SetItemPurpose( TUint aItemPurpose );
    
    /**
     * Set catalog source name.
     *
     * 
     * @param aCatalogSourceName Catalog source name.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetCatalogSourceNameL( const TDesC& aCatalogSourceName );
    
    /**
     * Set catalog source name.
     *
     * 
     * @param aCatalogSourceName Catalog source name. Ownership is transferred.
     */
    IMPORT_C void SetCatalogSourceName( HBufC* aCatalogSourceName );
    
    /**
     * Set download infos.
     *
     * 
     * @param aDownloadInfo Download infos. Ownership is not transferred, copies
     *  will be taken.
     */
    IMPORT_C void SetDownloadInfoL(
        const TArray< MNcdPurchaseDownloadInfo* >& aDownloadInfo );


    /**
     * Add download info
     *
     * 
     * @param aDownloadInfo A download info. Ownership is transferred if the 
     * operation is successful.
     */
    IMPORT_C void AddDownloadInfoL( MNcdPurchaseDownloadInfo* aDownloadInfo );


    /**
     * Insert download info
     *
     * 
     * @param aDownloadInfo A download info. Ownership is transferred if the      
     * operation is successful.
     * @param aIndex
     */
    IMPORT_C void InsertDownloadInfoL( 
        MNcdPurchaseDownloadInfo* aDownloadInfo, 
        TInt aIndex );

    
    /**
     * Remove download info
     *
     * 
     * @param aIndex Index
     */
    IMPORT_C void RemoveDownloadInfo( TInt aIndex );
    
    /**
     * Download info count getter
     *
     * 
     */
    IMPORT_C TInt DownloadInfoCount() const;
    
    /**
     * Download info getter
     *
     * 
     * @return Download info
     */
    IMPORT_C CNcdPurchaseDownloadInfo& DownloadInfo( TInt aIndex );

    
    /**
     * Set purchase option ID.
     *
     * 
     * @param aPurchaseOptionId Purchase option ID.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetPurchaseOptionIdL( const TDesC& aPurchaseOptionId );
    
    /**
     * Set purchase option ID.
     *
     * 
     * @param aPurchaseOptionId Purchase option ID. Ownership is transferred.
     */
    IMPORT_C void SetPurchaseOptionId( HBufC* aPurchaseOptionId );
    
    /**
     * Set purchase option name.
     *
     * 
     * @param aPurchaseOptionName Purchase option name.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetPurchaseOptionNameL( const TDesC& aPurchaseOptionName );
    
    /**
     * Set purchase option name.
     *
     * 
     * @param aPurchaseOptionName Purchase option name. Ownership is transferred.
     */
    IMPORT_C void SetPurchaseOptionName( HBufC* aPurchaseOptionName );
    
    /**
     * Set purchase option price.
     *
     * 
     * @param aPurchaseOptionPrice Purchase option price.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetPurchaseOptionPriceL( const TDesC& aPurchaseOptionPrice );
    
    /**
     * Set purchase option price.
     *
     * 
     * @param aPurchaseOptionPrice Purchase option price. Ownership is transferred.
     */
    IMPORT_C void SetPurchaseOptionPrice( HBufC* aPurchaseOptionPrice );
    
    /**
     * Set final price.
     *
     * 
     * @param aFinalPrice Final price.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetFinalPriceL( const TDesC& aFinalPrice );
    
    /**
     * Set final price.
     *
     * 
     * @param aFinalPrice Final price. Ownership is transferred.
     */
    IMPORT_C void SetFinalPrice( HBufC* aFinalPrice );
    
    /**
     * Set payment method name.
     *
     * 
     * @param aPaymentMethodName Payment method name.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetPaymentMethodNameL( const TDesC& aPaymentMethodName );
    
    /**
     * Set payment method name.
     *
     * 
     * @param aPaymentMethodName Payment method name. Ownership is transferred.
     */
    IMPORT_C void SetPaymentMethodName( HBufC* aPaymentMethodName );
    
    /**
     * Set time of purchase.
     *
     * 
     * @param aPurchaseTime Purchase time.
     */
    IMPORT_C void SetPurchaseTime( TTime aPurchaseTime );
    
    /**
     * Set file name(s) of the downloaded file(s).
     *
     * 
     * @param aDownloadedFiles Array of file names. Ownership is transferred.
     */
    IMPORT_C void SetDownloadedFiles( CDesCArray* aDownloadedFiles );
    
    /**
     * Set file name(s) of the downloaded file(s).
     *
     * 
     * @param aDownloadedFiles Array of file names. Ownership is not transferred,
     *  copies will be taken.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetDownloadedFilesL( const MDesCArray& aDownloadedFiles );


    
    /**
     * Replaces a file name of the downloaded file.
     *
     * 
     * @param aIndex Index
     * @param aDownloadFile New file name
     * @exception Leave System wide error code.
     */
    IMPORT_C void ReplaceDownloadedFileL( TInt aIndex, 
        const TDesC& aDownloadedFile );
    
    
    /**
     * Add file name for a downloaded file.
     *
     * 
     * @param aDownloadedFile Filename
     * @exception Leave System wide error code.
     */
    IMPORT_C void AddDownloadedFileL( const TDesC& aDownloadedFile );


    /**
     * Insert file name for a downloaded file.
     *
     * 
     * @param aDownloadedFile Filename
     * @param aIndex
     * @exception Leave System wide error code.
     */
    IMPORT_C void InsertDownloadedFileL( 
        const TDesC& aDownloadedFile, 
        TInt aIndex );

    /**
     * Remove a file name
     *
     * 
     * @param aIndex
     */
    IMPORT_C void RemoveDownloadedFile( TInt aIndex );
    
    
    /**
     * Set file installation infos.
     *
     * 
     * @param aInstallInfo File installation info array. Ownership is not
     *  transferred, copies will be taken.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetInstallInfoL(
        const TArray< MNcdPurchaseInstallInfo* >& aInstallInfo );


    /**
     * Add file installation info.
     *
     * 
     * @param aInstallInfo File installation info. Ownership is transferred 
     * if the operation is successful.     
     * @exception Leave System wide error code.
     */
    IMPORT_C void AddInstallInfoL( MNcdPurchaseInstallInfo* aInstallInfo );


    /**
     * Insert file installation info.
     *
     * 
     * @param aInstallInfo File installation info. Ownership is transferred 
     * if the operation is successful.     
     * @param aIndex
     * @exception Leave System wide error code.
     */
    IMPORT_C void InsertInstallInfoL( 
        MNcdPurchaseInstallInfo* aInstallInfo,
        TInt aIndex );

    /**
     * Remove install info
     *
     * 
     * @param aIndex Index
     */
    IMPORT_C void RemoveInstallInfo( TInt aIndex );

    /**
     * Install info count getter
     *
     * 
     * @return Install info count
     */
    IMPORT_C TInt InstallInfoCount() const;
    
    
    /**
     * Install info getter
     *
     * 
     * @param aIndex Index
     * @return Install info
     */
    IMPORT_C CNcdPurchaseInstallInfo& InstallInfo( TInt aIndex );
    
    /**
     * Set icon.
     *
     * 
     * @param aIcon Icon data.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetIconL( const TDesC8& aIcon );

    /**
     * Set icon.
     *
     * 
     * @param aIcon Icon data. Ownership is transferred.
     */
    IMPORT_C void SetIcon( HBufC8* aIcon );
    
    /**
     * Get icon data.
     *
     * @note Ownership is transferred.
     * @return Icon data.
     */
    IMPORT_C HBufC8* GetIconData();
    
    /**
     * Setter for HasIcon flag.
     *
     * @param aHasIcon flag value.
     */
    IMPORT_C void SetHasIcon( TBool aHasIcon );
    
    /**
     * Set access point used in download process.
     *
     * 
     * @param aAccessPoint Access point.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetDownloadAccessPointL( const TDesC& aAccessPoint );

    /**
     * Set access point used in download process.
     *
     * 
     * @param aAccessPoint Access point. Ownership is transferred.
     */
    IMPORT_C void SetDownloadAccessPoint( HBufC* aAccessPoint );

    /**
     * Set description.
     *
     * 
     * @param aDescription Description.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetDescriptionL( const TDesC& aDescription );

    /**
     * Set description.
     *
     * 
     * @param aDescription Description. Ownership is transferred.
     */
    IMPORT_C void SetDescription( HBufC* aDescription );

    /**
     * Set version.
     *
     * 
     * @param aVersion Version.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetVersionL( const TDesC& aVersion );

    /**
     * Set version.
     *
     * 
     * @param aVersion Version. Ownership is transferred.
     */
    IMPORT_C void SetVersion( HBufC* aVersion );

    /**
     * Set server URI.
     *
     * 
     * @param aServerUri Server URI.
     * @exception Leave System wide error code.
     */
    IMPORT_C void SetServerUriL( const TDesC& aServerUri );

    /**
     * Set server URI.
     *
     * 
     * @param aServerUri Server URI. Ownership is transferred.
     */
    IMPORT_C void SetServerUri( HBufC* aServerUri );

    /**
     * Set item type.
     *
     * 
     * @param aItemType Item type.
     */
    IMPORT_C void SetItemType( TItemType aItemType );
    
    /**
     * Set the total content size.
     *
     * 
     * @param aSize The size.
     */
    IMPORT_C void SetTotalContentSize( TInt aSize );
    
    /**
     * Set the origin node id.
     *
     * 
     * @param aOriginNodeId The id.
     */
    IMPORT_C void SetOriginNodeIdL( const TDesC& aOriginNodeId );
    
    /**
     * Set the origin node id.
     *
     * 
     * @param aOriginNodeId The id.
     */
    IMPORT_C void SetOriginNodeId( HBufC* aOriginNodeId );

    /**
     * Set the last operation time to the current universal time.
     *
     * 
     */
    IMPORT_C void SetLastUniversalOperationTime();
      
    /**
     * Set the last operation time. This should be universal time.
     *
     * 
     * @param aTime The universal time.
     */
    IMPORT_C void SetLastOperationTime( const TTime& aTime );

    /**
     * Set the last operation error code.
     *
     * 
     * @param aError The error code.
     */
    IMPORT_C void SetLastOperationErrorCode( TInt aError );

    /**
     * Integer attribute setter
     */
    IMPORT_C void SetAttributeL( TPurchaseAttribute aAttribute, TInt32 aValue );
    
    /**
     * String attribute setter
     */
    IMPORT_C void SetAttributeL( 
        TPurchaseAttribute aAttribute, const TDesC& aValue );
    
    /**
     * Externalizes attributes to a stream
     * 
     * @param aStream Write stream
     */
    IMPORT_C void ExternalizeAttributesL( RWriteStream& aStream ) const;
    
    /**
     * Internalizes attributes from a stream
     * 
     * @param aStream Read stream
     */    
    IMPORT_C void InternalizeAttributesL( RReadStream& aStream );
    
    
private: // Construction

    /**
     * Private 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Private constructor.
     */
    CNcdPurchaseDetails();

    /**
     * Creates attributes if they haven't been created yet
     */
    void CreateAttributesL();
    
private: // Data members

    // Client UID.
    TUid iClientUid;
    
    // Namespace.
    HBufC* iNamespace;
    
    // Entity ID.
    HBufC* iEntityId;
    
    // Item name.
    HBufC* iItemName;

    // Item purpose.
    TUint iItemPurpose;
    
    // Catalog source name.
    HBufC* iCatalogSourceName;
    
    // Download info array.
    RPointerArray< MNcdPurchaseDownloadInfo > iDownloadInfo;
    
    // Purchase option ID.
    HBufC* iPurchaseOptionId;
    
    // Purchase option name.
    HBufC* iPurchaseOptionName;
    
    // Purchase option price.
    HBufC* iPurchaseOptionPrice;
    
    // Final price.
    HBufC* iFinalPrice;
    
    // Payment method name.
    HBufC* iPaymentMethodName;
    
    // Purchase time.
    TTime iPurchaseTime;
    
    // Downloaded files array.
    CDesCArray* iDownloadedFiles;
    
    // Install info array.
    RPointerArray< MNcdPurchaseInstallInfo > iInstallInfo;

    // Icon data.
    HBufC8* iIcon;
    
    // Determines whether this object has an icon.
    TBool iHasIcon;

    // Access point used in download process.
    HBufC* iDownloadAccessPoint;

    // Description.
    HBufC* iDescription;

    // Version.
    HBufC* iVersion;

    // Server URI.
    HBufC* iServerUri;

    // Item type.
    TItemType iItemType;
    
    // Total downloadable content size in bytes.
    TInt iTotalContentSize;
    
    // The id of the structure node that this entity was purchased from,
    HBufC* iOriginNodeId;

    // The universal time that informs when the last operation 
    // was directed to the purchase item.
    TTime iLastOperationTime;
    
    // The error code of the last operation that was directed
    // to the purchase item.
    TInt iLastOperationErrorCode;

    // Additional attributes, owned
    CNcdAttributes* iAttributes; 
    };

/**
 *  A default implementation for the MNcdSearchFilter interface.
 *
 *  @lib ncdutils_20019119.dll
 *  
 *  @see MNcdSearchFilter
 */
class CNcdSearchFilter : public CBase,
                         public MNcdSearchFilter
    {

public:

    /**
     * Default factory method for CNcdSearchFilter objects. Leaves contents uninitialized.
     *
     * @return CNcdSearchFilter object pointer.
     */
    IMPORT_C static CNcdSearchFilter *NewL();

    /**
     * Default factory method for CNcdSearchFilter objects. Leaves contents uninitialized.
     * Leaves object pointer on cleanup stack.
     *
     * @return CNcdSearchFilter object pointer.
     */
    IMPORT_C static CNcdSearchFilter *NewLC();

    /**
     * Factory method for CNcdSearchFilter objects. Initializes as a copy of another filter.
     *
     * @param aFilter Initialization copy source.
     * @return CNcdSearchFilter object pointer.
     */
    IMPORT_C static CNcdSearchFilter *NewL( const CNcdSearchFilter& aFilter );

    /**
     * Factory method for CNcdSearchFilter objects. Initializes as a copy of another filter.
     * Leaves object pointer on cleanup stack.
     *
     * @param aFilter Initialization copy source.
     * @return CNcdSearchFilter object pointer.
     */
    IMPORT_C static CNcdSearchFilter *NewLC( const CNcdSearchFilter& aFilter );

    /**
     * Factory method for CNcdSearchFilter objects. Initializes from a stream externalized with
     * CNcdSearchFilter::ExternalizeL().
     *
     * @param aStream Initialization source stream.
     * @return CNcdSearchFilter object pointer.
     */
    IMPORT_C static CNcdSearchFilter *NewL( RReadStream& aStream );

    /**
     * Factory method for CNcdSearchFilter objects. Initializes from a stream externalized with
     * CNcdSearchFilter::ExternalizeL(). Leaves object pointer on cleanup stack.
     *
     * @param aStream Initialization source stream.
     * @return CNcdSearchFilter object pointer.
     */
    IMPORT_C static CNcdSearchFilter *NewLC( RReadStream& aStream );
    
    /**
     * Destructor.
     */
    virtual ~CNcdSearchFilter();
     
public: //  From MNcdSearchFilter

    /**
     * @see MNcdSearchFilter::Keywords
     */
    virtual const MDesCArray& Keywords() const;

    /**
     * @see MNcdSearchFilter::Keywords
     */
    virtual TUint ContentPurposes() const;


    /**
     * @see MNcdSearchFilter::SearchMode
     */
    virtual MNcdSearchFilter::TSearchMode SearchMode() const;
    
    
    /**
     * @see MNcdSearchFilter::RecursionDepth
     */
    virtual TUint RecursionDepth() const;
    
    
public:

    /**
     * Adds a search keyword.
     * By default, no keywords are set
     *
     * @param aKeyword Search keyword to add.
     */
    IMPORT_C void AddKeywordL( const TDesC& aKeyword );
    
    /**
     * Set flags for filtering by content purpose.
     * By default, no content flags are set
     *
     * @param aFlags Content flags for filtering. A combination of TNcdItemPurpose flags.
     * @see TNcdItemPurpose
     */
    IMPORT_C void SetContentPurposes( TUint aFlags );
    
    
    /**
     * Set search mode
     *
     * By default, mode is ENormal
     *
     * @param aMode Search mode
     */
    IMPORT_C void SetSearchMode( MNcdSearchFilter::TSearchMode aMode );
    
    
    /**
     * Set recursion depth
     *
     * By default, recursion depth is 2. Depth of 0 disabled recursion
     *
     * @param aRecursionDepth Depth of recursion
     * @leave KErrArgument if aRecursionDepth is greater than 5
     */
    IMPORT_C void SetRecursionDepthL( TUint aRecursionDepth );
    
    
    /**
     * Internalize member data.
     *
     * @param aStream Source stream.
     */
    virtual void InternalizeL( RReadStream& aStream );
    
    
    /**
     * Externalize member data.
     *
     * @param aStream Destination stream.
     */
    virtual void ExternalizeL( RWriteStream& aStream );


protected:

    /**
     * First phase constructor.
     */
    CNcdSearchFilter();    
    
    /**
     * Second phase constructor.
     */
    void ConstructL();
    
    
private:

    /**
     * Keywords to filter the search based on
     */
    CDesCArray *iKeywords;
    
    /**
     * Purposes to filter the search based on
     */
    TUint iPurposes;
    
    /**
     * Search mode
     */
    MNcdSearchFilter::TSearchMode iSearchMode;
    
    /**
     * Recursion depth
     */
    TUint iRecursionDepth;    
    };
    
#endif // M_NCD_UTILS_H
