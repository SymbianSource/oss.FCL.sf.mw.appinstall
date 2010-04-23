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


#include "ncd_pp_downloadimpl.h"
#include "ncd_pp_rights.h"
#include "ncd_pp_descriptor.h"
#include "ncd_pp_descriptorimpl.h"
#include "ncd_cp_query.h"
#include "ncdprotocoltypes.h"
#include "ncdprotocolutils.h"


CNcdPreminetProtocolDownloadImpl* 
CNcdPreminetProtocolDownloadImpl::NewL() 
    {
    CNcdPreminetProtocolDownloadImpl* self =
        new (ELeave) CNcdPreminetProtocolDownloadImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CNcdPreminetProtocolDownloadImpl* 
CNcdPreminetProtocolDownloadImpl::NewLC() 
    {
    CNcdPreminetProtocolDownloadImpl* self =
        new (ELeave) CNcdPreminetProtocolDownloadImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
  
void CNcdPreminetProtocolDownloadImpl::ConstructL() 
    {
    NcdProtocolUtils::AssignEmptyDesL( iUri );
    NcdProtocolUtils::AssignEmptyDesL( iMime );
    NcdProtocolUtils::AssignEmptyDesL( iInstallNotificationUri );
    NcdProtocolUtils::AssignEmptyDesL( iFingerprint );
    NcdProtocolUtils::AssignEmptyDesL( iPreviewType );
    }
    
/**
 * Destructor
 */
CNcdPreminetProtocolDownloadImpl::~CNcdPreminetProtocolDownloadImpl() 
    {
    delete iUri;
    delete iMime;
    delete iInstallNotificationUri;
    delete iMessage;
    delete iFingerprint;
    delete iRights;
    
    delete iDescriptor;
    delete iPreviewType;
    }

CNcdPreminetProtocolDownloadImpl::CNcdPreminetProtocolDownloadImpl() 
    : CBase(),
      iDeliveryMethod( EUriDeliveryNotSet ),
      iValidityDelta( -1 ), // -1 as a default means that element not found
      iTarget( EDownloadTargetDownloadable ),
      iLaunchable( ETrue ), // default for launchable is TRUE!
      iFingerprintAlgorithm( EAlgorithmNotSet )
    {
    }

/**
 * Returns the ID of this entity.
 * @return Id
 */
const TDesC& CNcdPreminetProtocolDownloadImpl::Uri() const 
    {
    return *iUri;
    }

/**
 * Returns the ID of this entity.
 * @return Id
 */
TBool CNcdPreminetProtocolDownloadImpl::AutoDownload() const
    {
    return iAutoDownload;
    }

/**
 * Returns the last modified date for this entity.
 * @return Last modified time, or 0 if never modified.
 */
TInt CNcdPreminetProtocolDownloadImpl::DelayDelta() const
    {
    return iDelayDelta;
    }

/**
 * Returns the namespace for this entity
 * @return namespace or KNullDesC 
 */
TNcdUriDeliveryMethod 
CNcdPreminetProtocolDownloadImpl::DeliveryMethod() const
    {
    return iDeliveryMethod;
    }

/**
 * Entity name, localized.
 * @return Name, never KNullDesC
 */
TInt CNcdPreminetProtocolDownloadImpl::ValidityDelta() const
    {
    return iValidityDelta;
    }

/**
 * Retuns the description for this entity, localized.
 * @return Description or KNullDesC
 */
TNcdDownloadTargetType 
CNcdPreminetProtocolDownloadImpl::Target() const
    {
    return iTarget;
    }

/**
 * Returns icon information for the entity.
 * @return Icon info or null if icon not available.
 */
TInt CNcdPreminetProtocolDownloadImpl::ChunkSize() const
    {
    return iChunkSize;
    }

/**
 * Returns the disclaimer for the entity.
 * @return Disclaimer text or KNullDesC if no disclaimer.
 */
TBool CNcdPreminetProtocolDownloadImpl::Launchable() const
    {
    return iLaunchable;
    }

const TDesC& CNcdPreminetProtocolDownloadImpl::Mime() const
    {
    return *iMime;
    }
    
const TDesC& CNcdPreminetProtocolDownloadImpl::InstallNotificationUri() const
    {
    return *iInstallNotificationUri;
    }
    
TInt CNcdPreminetProtocolDownloadImpl::Size() const
    {
    return iSize;
    }

const MNcdConfigurationProtocolQuery* 
CNcdPreminetProtocolDownloadImpl::Message() const
    {
    return iMessage;
    }

const TDesC& CNcdPreminetProtocolDownloadImpl::Fingerprint() const
    {
    return *iFingerprint;
    }

TNcdUriFingerprintAlgorithm
CNcdPreminetProtocolDownloadImpl::FingerprintAlgorithm() const
    {
    return iFingerprintAlgorithm;
    }

const MNcdPreminetProtocolDescriptor* 
CNcdPreminetProtocolDownloadImpl::Descriptor() const
    {
    return iDescriptor;
    }


TBool CNcdPreminetProtocolDownloadImpl::ReDownloadable() const 
    {
    return iReDownloadable;
    }

const MNcdPreminetProtocolRights*
CNcdPreminetProtocolDownloadImpl::Rights() const
    {
    return iRights;
    }

const TDesC& CNcdPreminetProtocolDownloadImpl::PreviewType() const
    {
    return *iPreviewType;
    }
