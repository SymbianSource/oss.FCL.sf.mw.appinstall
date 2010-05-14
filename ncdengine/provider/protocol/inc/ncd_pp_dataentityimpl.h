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
* Description:   CNcdPreminetProtocolDataEntityImpl declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_DATAENTITY_IMPL_H
#define NCD_PROTOCOL_ELEMENT_DATAENTITY_IMPL_H

#include <e32base.h>
#include "ncd_pp_dataentity.h"
#include "ncd_pp_iconimpl.h"
#include "ncd_pp_skinimpl.h"
#include "ncd_pp_download.h"
#include "ncdprotocoltypes.h"

class CNcdPreminetProtocolDataEntityImpl 
: public CBase, public MNcdPreminetProtocolDataEntity
    {
    
public:

    static CNcdPreminetProtocolDataEntityImpl* NewL();
    static CNcdPreminetProtocolDataEntityImpl* NewLC();

    virtual ~CNcdPreminetProtocolDataEntityImpl();
    
    void ConstructL();

// From base class MNcdPreminetProtocolDataEntity

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual TNcdDataEntityType Type() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& Id() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& Timestamp() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& Namespace() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& ServerUri() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& Name() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& Description() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolIcon* Icon() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdConfigurationProtocolQuery* Disclaimer() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolDownload* HelpInformation() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolDataEntityContent* 
        DownloadableContent() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolDataEntityContent* 
        SubscribableContent() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual TInt PurchaseOptionCount() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolPurchaseOption& 
        PurchaseOptionL(TInt aIndex) const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolSkin* Skin() const;
    
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const TDesC& LayoutType() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual TInt ScreenshotCount() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolDownload& 
        ScreenshotL(TInt aIndex) const;
    
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual TInt PreviewCount() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdPreminetProtocolDownload& 
        PreviewL(TInt aIndex) const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual const MNcdConfigurationProtocolQuery* 
        MoreInfo() const;
        
    /**
     * @see MNcdPreminetProtocolDataEntity
     */
    virtual TInt DetailCount() const;

    /**
     * @see MNcdPreminetProtocolDataEntity
     */    
    virtual const MNcdConfigurationProtocolDetail& DetailL( TInt aIndex ) const;


public:

    HBufC* iId;
    HBufC* iTimestamp;
    HBufC* iNamespace;
    HBufC* iServerUri;
    HBufC* iName;
    HBufC* iDescription;
    MNcdConfigurationProtocolQuery* iDisclaimer;
    //MNcdConfigurationProtocolQuery* iHelpInformation;
    CNcdPreminetProtocolIcon* iIcon;
    TNcdDataEntityType iType;
    RPointerArray<MNcdPreminetProtocolPurchaseOption> iPurchaseOptions;
    RPointerArray<MNcdPreminetProtocolDownload> iScreenshots;
    RPointerArray<MNcdPreminetProtocolDownload> iPreviews;
    MNcdPreminetProtocolDataEntityContent* iDownloadableContent;
    MNcdPreminetProtocolDataEntityContent* iSubscribableContent;
    CArrayPtr<MNcdConfigurationProtocolDetail>* iDetails;
        
    
    // FolderData stuff
    CNcdPreminetProtocolSkin* iSkin;
    HBufC* iLayoutType;
    MNcdConfigurationProtocolQuery* iMoreInfo;
    };

#endif // NCD_PROTOCOL_ELEMENT_DATAENTITY_IMPL_H
