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
* Description:   MNcdPreminetProtocolDataEntity declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_DATAENTITY_H
#define NCD_PROTOCOL_ELEMENT_DATAENTITY_H

#include <e32base.h>
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolPurchaseOption;
class MNcdConfigurationProtocolQuery;
class MNcdPreminetProtocolIcon;
class MNcdPreminetProtocolSkin;
class MNcdPreminetProtocolDataEntityContent;
class MNcdPreminetProtocolDownload;

class MNcdPreminetProtocolDataEntity
    {
public:

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolDataEntity() {}

    /**
     * Returns the type of this reference entity.
     * @return Type
     */
    virtual TNcdDataEntityType Type() const = 0;

    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;
    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or KNullDesC if never modified.
     */
    virtual const TDesC& Timestamp() const = 0;
    /**
     * Returns the namespace for this entity
     * @return namespace or KNullDesC 
     */
    virtual const TDesC& Namespace() const = 0;

    /**
     * Retrieves the server uri from which this entity was received
     * @return Server URI
     */
    virtual const TDesC& ServerUri() const = 0;

    /**
     * Entity name, localized.
     * @return Name, never KNullDesC
     */
    virtual const TDesC& Name() const = 0;

    /**
     * Retuns the description for this entity, localized.
     * @return Description or KNullDesC
     */
    virtual const TDesC& Description() const = 0;

    /**
     * Returns icon information for the entity.
     * @return Icon info object, NULL if no icon
     */
    virtual const MNcdPreminetProtocolIcon* Icon() const = 0;
    
    /**
     * Returns the disclaimer for the entity. Ownership NOT transferred.
     * @return Disclaimer object or NULL if no disclaimer. 
     */
    virtual const MNcdConfigurationProtocolQuery* Disclaimer() const = 0;

    /**
     * Returns the help information for the entity. Ownership NOT transferred.
     * @return Help information object or NULL if no information. 
     */
    virtual const MNcdPreminetProtocolDownload* HelpInformation() const = 0;

    /** 
     * Returns the downloadable content info if available.
     * Object ownership is NOT transferred.
     * @return Pointer to the downloadable content or NULL.
     */
    virtual const MNcdPreminetProtocolDataEntityContent* 
        DownloadableContent() const = 0;

    /**
     * Returns the subscribable content info if available.
     * Object ownership is NOT transferred.
     * @return Pointer to the subscribable content or NULL.
     */
    virtual const MNcdPreminetProtocolDataEntityContent* 
        SubscribableContent() const = 0;

    /**
     * Returns the amount of purchase options.
     * @return Purchase option count.
     */
    virtual TInt PurchaseOptionCount() const = 0;
    
    /**
     * Returns the purchase option. 
     * @param aIndex Index ( 0 .. PurchaseOptionCount()-1 )
     * @return Purchase option reference. Leaves if aIndex is out of bounds.
     */
    virtual const MNcdPreminetProtocolPurchaseOption& 
        PurchaseOptionL(TInt aIndex) const = 0;

    /**
     * Returns the skin
     * @return Skin object pointer or NULL.
     */
    virtual const MNcdPreminetProtocolSkin* Skin() const = 0;
    
    /**
     * Returns the layout type
     * @return Layout type or KNullDesC
     */
    virtual const TDesC& LayoutType() const = 0;

    /**
     * Returns the amount of screenshots
     * @return Screenshot count
     */
    virtual TInt ScreenshotCount() const = 0;

    /**
     * Returns a screenshot. 
     * @param aIndex Index of a screenshot
     * @return Screenshot reference. Leaves if aIndex is out of bounds.
     */
    virtual const MNcdPreminetProtocolDownload& 
        ScreenshotL(TInt aIndex) const = 0;

    /**
     * Returns the amount of previews
     * @return Preview count
     */
    virtual TInt PreviewCount() const = 0;

    /**
     * Returns a preview.
     * @param aIndex Index of preview.
     * @return Preview reference. Leaves if aIndex is out of bounds.
     */
    virtual const MNcdPreminetProtocolDownload& 
        PreviewL(TInt aIndex) const = 0;
        
    /**
     * Returns the more info data object for the entity. Ownership NOT transferred.
     * @return More info object or NULL if no more info. 
     */
    virtual const MNcdConfigurationProtocolQuery* 
        MoreInfo() const = 0;
        
    /**
     * Returns the amount of details.
     * @return Cookie count
     */
    virtual TInt DetailCount() const = 0;

    /**
     * Get a detail element.
     * @param aIndex Detail index. Leaves if index is out of bounds.
     * @return Detail reference.
     */    
    virtual const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const = 0;
    };


#endif //NCD_PROTOCOL_ELEMENT_DATAENTITY_H
