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
* Description:   Definition of MNcdPurchaseDownloadInfo interface and
*                implementation class.
*
*/


#ifndef M_NCDPURCHASEDOWNLOADINFO_H
#define M_NCDPURCHASEDOWNLOADINFO_H


#include <e32base.h>

class RWriteStream;
class RReadStream;


/**
 *  Interface for purchase download information.
 *
 *  
 */
class MNcdPurchaseDownloadInfo
    {
public:

    /**
     * Type of the content.
     *
     * 
     */
    enum TContentUsage
        {
        /** Content is downloadable. */
        EDownloadable = 0,

        /** Content is consumable. */
        EConsumable,
 
        /** Content is dependency. */
        EDependency,
 
        /** Content is upgrade. */
        EUpgrade,
        
        /** Content is launcher application */         
        ELauncher,
        
        /** Content is launcher application that opens a content file */
        ELauncherOpen
 
        };


    /**
     * Additional attributes
     *
     * These are retrieved with AttributeStringL and AttributeInt32L
     *
     * Adding new attributes does not cause a binary break but removing
     * does
     *
     * 
     */
    enum TDownloadAttribute
        {
        /**
         * Dependency name may be set for dependency and launcher content
         * Type: String
         */
        EDownloadAttributeDependencyName = 0,
        
        /**
         * If non-zero or not set, this download is required, 
         * otherwise it can be skipped
         *
         * Type: TInt32
         */
        EDownloadAttributeRequiredDownload,
        
        /**
         * Internal attribute value, DO NOT USE
         *
         * @note Add new attributes before this
         */
        EDownloadAttributeInternal
        };
        
    /**
     * Gets the content usage type.
     * 
     * 
     * @return Content usage.
     */
    virtual TContentUsage ContentUsage() const = 0;

    /**
     * Gets the content URI.
     * 
     * 
     * @return Content URI.
     */
    virtual const TDesC& ContentUri() const = 0;

    /**
     * Gets the content validity delta. This is in
     * minutes.
     * 
     * 
     * @return Content validity delta. -1 if not set.
     */
    virtual TInt ContentValidityDelta() const = 0;

    /**
     * Gets the content mime type.
     * 
     * 
     * @return Content mime type.
     */
    virtual const TDesC& ContentMimeType() const = 0;

    /**
     * Gets the content size.
     * 
     * 
     * @return Content size.
     */
    virtual TInt ContentSize() const = 0;

    /**
     * Is content launchable.
     * 
     * 
     * @return Launchable status.
     */
    virtual TBool IsLaunchable() const = 0;

    /**
     * Gets the descriptor type.
     * 
     * 
     * @return Descriptor type.
     */
    virtual const TDesC& DescriptorType() const = 0;

    /**
     * Gets the descriptor name.
     * 
     * 
     * @return Descriptor name.
     */
    virtual const TDesC& DescriptorName() const = 0;

    /**
     * Gets the descriptor URI.
     * 
     * 
     * @return Descriptor URI.
     */
    virtual const TDesC& DescriptorUri() const = 0;

    /**
     * Gets the descriptor data.
     * 
     * 
     * @return Descriptor data.
     */
    virtual const TDesC8& DescriptorData() const = 0;

    /**
     * Gets the rights URI.
     * 
     * 
     * @return Rights URI.
     */
    virtual const TDesC& RightsUri() const = 0;

    /**
     * Gets the rights type.
     * 
     * 
     * @return Rights type.
     */
    virtual const TDesC& RightsType() const = 0;
    
    /**
     * Gets the activation key.
     *
     * 
     * @return Activation key.
     */
    virtual const TDesC& ActivationKey() const = 0;
    
    /**
     * Gets the install notification URI.
     *
     * 
     * @return Install notification URI.
     */
    virtual const TDesC& InstallNotificationUri() const = 0;
    
    
    /**
     * String attribute getter
     *
     * @param aAttribute Attribute
     * @return Parameter string
     * @leave KErrNotFound if attribute is not set
     * @panic ENcdPanicIndexOutOfRange if aAttribute is invalid
     * @panic ENcdPanicInvalidArgument if the attribute type is not string
     */
    virtual const TDesC& AttributeStringL( 
        TDownloadAttribute aAttribute ) const = 0;
    

    /**
     * Integer attribute getter
     *
     * @param aAttribute Attribute
     * @return Parameter integer
     * @leave KErrNotFound if attribute is not set
     * @panic ENcdPanicIndexOutOfRange if aAttribute is invalid
     * @panic ENcdPanicInvalidArgument if the attribute type is not TInt32
     */
    virtual TInt32 AttributeInt32L( 
        TDownloadAttribute aAttribute ) const = 0;
    
protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdPurchaseDownloadInfo() {}

    };


#endif // M_NCDPURCHASEDOWNLOADINFO_H
