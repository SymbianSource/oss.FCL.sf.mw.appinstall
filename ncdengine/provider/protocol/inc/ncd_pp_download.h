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
* Description:   MNcdPreminetProtocolDownload declaration
*
*/


#ifndef NCD_PROTOCOL_ELEMENT_DOWNLOAD_H
#define NCD_PROTOCOL_ELEMENT_DOWNLOAD_H

#include <e32base.h>
#include "ncdprotocoltypes.h"

class MNcdConfigurationProtocolQuery;
class MNcdPreminetProtocolRights;
class MNcdPreminetProtocolDescriptor;


class MNcdPreminetProtocolDownload
    {
public:    
    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolDownload() {}


    /**
     * Download URI.
     * @return URI or KNullDesC
     */
    virtual const TDesC& Uri() const = 0;

    /**
     * Returns whether the content is automatically downloadable.
     * @return True or false
     */
    virtual TBool AutoDownload() const = 0;

    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or 0 if never modified.
     */
    virtual TInt DelayDelta() const = 0;

    /**
     * Delivery method of this download
     * @return Delivery type 
     */
    virtual TNcdUriDeliveryMethod DeliveryMethod() const = 0;

    /**
     * Validity time in minutes.
     * @return Delta value or -1 if element not found.
     */
    virtual TInt ValidityDelta() const = 0;

    /**
     * Download mime type
     * @return Descriptor
     */
    virtual const TDesC& Mime() const = 0;

    /**
     * ServerMessage information object.
     * @return Pointer or NULL. Ownership NOT transferred.
     */
    virtual const MNcdConfigurationProtocolQuery*
        Message() const = 0;

    /**
     * Possible fingerprint for the target binary
     * @return Fingerprint.
     */
    virtual const TDesC& Fingerprint() const = 0;

    /**
     * Fingerprint algorithm
     * @return MD5 or SHA-1.
     */
    virtual TNcdUriFingerprintAlgorithm FingerprintAlgorithm() const = 0;

    /**
     * Download descriptor. Ownership is NOT transferred.
     * @return Desriptor pointer or NULL.
     */
    virtual const MNcdPreminetProtocolDescriptor* Descriptor() const = 0;


public:
    /**
     * Indicates whether the download is consumable or downloadable.
     * @return EConsumable or EDownloadable
     */
    virtual TNcdDownloadTargetType Target() const = 0;

    /**
     * Maximum chuck size for download requests.
     * @return Chunk size.
     */
    virtual TInt ChunkSize() const = 0;
    
    /**
     * Is content launchable
     * @return true or false.
     */
    virtual TBool Launchable() const = 0;

    /**
     * HTTP URI for install notification
     * @return Notification URI
     */
    virtual const TDesC& InstallNotificationUri() const = 0;

    /**
     * Total size of the download target in bytes
     * @return Download size
     */
    virtual TInt Size() const = 0;
    
    /**
     * Can the content be re-downloaded.
     * @return True or false
     */
    virtual TBool ReDownloadable() const = 0;

    /**
     * Rights object. Ownership is NOT transferred.
     * @return Rights object pointer or NULL.
     */
    virtual const MNcdPreminetProtocolRights* Rights() const = 0;

    /**
     * Preview type.
     * @return Type string or KNullDesC
     */
    virtual const TDesC& PreviewType() const = 0;
    
    };


#endif //NCD_PROTOCOL_ELEMENT_DOWNLOAD_H
