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
* Description:   CNcdPreminetProtocolDownloadImpl declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_DOWNLOAD_IMPL_H
#define NCD_PROTOCOL_ELEMENT_DOWNLOAD_IMPL_H

#include <e32base.h>
#include "ncd_pp_download.h"
#include "ncdprotocoltypes.h"

class MNcdPreminetProtocolRights;
class MNcdPreminetProtocolDescriptor;
class CNcdPreminetProtocolDescriptorImpl;
class MNcdConfigurationProtocolQuery;

class CNcdPreminetProtocolDownloadImpl 
: public CBase, public MNcdPreminetProtocolDownload
    {
public:

    static CNcdPreminetProtocolDownloadImpl* NewL();
    static CNcdPreminetProtocolDownloadImpl* NewLC();

    /**
     * Destructor
     */
    virtual ~CNcdPreminetProtocolDownloadImpl();

    void ConstructL();

    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& Uri() const;

    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual TBool AutoDownload() const;

    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or 0 if never modified.
     */
    virtual TInt DelayDelta() const;

    /**
     * Returns the namespace for this entity
     * @return namespace or KNullDesC 
     */
    virtual TNcdUriDeliveryMethod DeliveryMethod() const;

    /**
     * Validity time in minutes.
     * @return Delta value or -1 if element not found.
     */
    virtual TInt ValidityDelta() const;

    /**
     * Download mime type
     * @return Descriptor
     */
    virtual const TDesC& Mime() const;

    /**
     * ServerMessage information object.
     * @return Pointer or NULL. Ownership NOT transferred.
     */
    virtual const MNcdConfigurationProtocolQuery*
        Message() const;

    /**
     * Possible fingerprint for the target binary
     * @return Fingerprint.
     */
    virtual const TDesC& Fingerprint() const;

    /**
     * Fingerprint algorithm
     * @return MD5 or SHA-1.
     */
    virtual TNcdUriFingerprintAlgorithm FingerprintAlgorithm() const;

    virtual const MNcdPreminetProtocolDescriptor* Descriptor() const;



    /**
     * @see MNcdPreminetProtocolDownload
     */
    virtual TNcdDownloadTargetType Target() const;

    /**
     * @see MNcdPreminetProtocolDownload
     */
    virtual TInt ChunkSize() const;
    
    /**
     * @see MNcdPreminetProtocolDownload
     */
    virtual TBool Launchable() const;

    /**
     * @see MNcdPreminetProtocolDownload
     */
    virtual const TDesC& InstallNotificationUri() const;

    /**
     * @see MNcdPreminetProtocolDownload
     */
    virtual TInt Size() const;
    
    /**
     * @see MNcdPreminetProtocolDownload
     */
    virtual TBool ReDownloadable() const;

    virtual const MNcdPreminetProtocolRights* Rights() const;

    virtual const TDesC& PreviewType() const;

private:
    CNcdPreminetProtocolDownloadImpl();
    
public:
    
    
    HBufC* iUri;
    TBool iAutoDownload;
    TInt iDelayDelta;
    TNcdUriDeliveryMethod iDeliveryMethod;
    TInt iValidityDelta;
    HBufC* iMime;
    TNcdDownloadTargetType iTarget;
    TInt iChunkSize;
    TBool iLaunchable;
    HBufC* iInstallNotificationUri;
    TInt iSize;
    
    TBool iReDownloadable;
    
    HBufC* iFingerprint;
    TNcdUriFingerprintAlgorithm iFingerprintAlgorithm;

    HBufC* iPreviewType;

    MNcdConfigurationProtocolQuery* iMessage;
    CNcdPreminetProtocolDescriptorImpl* iDescriptor;
    
    MNcdPreminetProtocolRights* iRights;    
    };


#endif //NCD_PROTOCOL_ELEMENT_DOWNLOAD_IMPL_H
