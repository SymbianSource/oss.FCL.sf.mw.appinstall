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
* Description:   Contains MNcdProvider interface
*
*/


#ifndef M_NCD_PROVIDER_PC_CLIENT_SUPPORT_H
#define M_NCD_PROVIDER_PC_CLIENT_SUPPORT_H


#include <e32cmn.h>
#include "catalogsbase.h"
#include "ncdinterfaceids.h"

class MNcdRightsObjectOperation;
class MNcdRightsObjectOperationObserver;
class MNcdCreateAccessPointOperation;
class MNcdCreateAccessPointOperationObserver;
class MNcdSendHttpRequestOperation;
class MNcdSendHttpRequestOperationObserver;
class TNcdConnectionMethod;

/**
 *  MNcdProviderPcClientSupport provides provider functionality specifically needed
 *  for Catalogs PC Client connectivity.
 *
 *  
 */
class MNcdProviderPcClientSupport : public virtual MCatalogsBase

    {
public:


    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdProviderPcClientSupportUid };


    /**
     * Create an operation for downloading and installing a DRM rights object.
     *
     * 
     * @param aObserver Observer interface to receive operation completion etc callbacks.
     * @param aDownloadUri Full URI to the rights object to download.
     * @param aMimeType Mime type of the rights object.
     * @param aConnectionMethod Network connection method
     */
    virtual MNcdRightsObjectOperation* DownloadAndInstallRightsObjectL(
        MNcdRightsObjectOperationObserver& aObserver,
        const TDesC& aDownloadUri, 
        const TDesC& aMimeType, 
        const TNcdConnectionMethod& aConnectionMethod ) = 0;

    /**
     * Create an access point for PC Client use. If the given access point is not yet
     * installed on the phone, it is installed temporarily for the PC client session.
     *
     * @param aAccessPointData Configuration protocol detail XML fragment describing the
     *  access point.
     * @return Operation that creates the accesspoint
     * @leave KErrArgument if aAccessPointData is empty
     */
    virtual MNcdCreateAccessPointOperation* CreateAccessPointL( 
        const TDesC& aAccessPointData,
        MNcdCreateAccessPointOperationObserver& aObserver ) = 0;


    /**
     * Send a standard HTTP request
     *
     * The request must be a valid HTTP/1.1 request. Only POST, GET and HEAD methods
     * are supported.
     *
     * Note that Request-URI and Host-header that are in the request are ignored and
     * aUri is used as the destination URI instead.
     *
     * @param aUri Request URI.
     * @param aRequest Request data
     * @param aConnectionMethod Network connection method          
     */
    virtual MNcdSendHttpRequestOperation* SendHttpRequestL(
        const TDesC8& aUri,
        const TDesC8& aRequest, 
        const TNcdConnectionMethod& aConnectionMethod, 
        MNcdSendHttpRequestOperationObserver& aObserver ) = 0;

    
protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdProviderPcClientSupport() {}

    };


#endif // M_NCD_PROVIDER_PC_CLIENT_SUPPORT_H
