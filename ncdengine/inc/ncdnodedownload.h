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
* Description:   Contains MNcdNodeDownload interface
*
*/


#ifndef M_NCD_NODE_DOWNLOAD_H
#define M_NCD_NODE_DOWNLOAD_H


#include "catalogsbase.h"
#include "ncdinterfaceids.h"


class MNcdDownloadOperation;
class MNcdDownloadOperationObserver;

/**
 *  Provides functions for the node that may contains content that can be downloaded.
 *  Downloading of the content may be done directly. But often,
 *  purchase interface functions should be used first to finnish
 *  purcahse operations before downloading the node content.
 *  After downloading, the installation interface functions may be
 *  called to install the files that have been downloaded.
 *
 *  @see MNcdNodePurchase
 *  @see MNcdNodeInstall
 *
 *  
 */
class MNcdNodeDownload : public virtual MCatalogsBase
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdNodeDownloadUid };


    /**
     * Determines if the node content has already been downloaded.
     *
     * 
     * @return ETrue if the node content has been downloade, EFalse otherwise.
     * @exception Leave System wide error code.
     */
    virtual TBool IsDownloadedL() const = 0;


    /**
     * Starts the download operation.
     *
     * @note The reference count of the operation object is increased by one. 
     * So, Release function of the operation should be called when operation 
     * is not needed anymore.
     *
     * 
     * @param aObserver Observer for the operation.
     * @return Pointer to the download operation. Can be used to check the
     *         progress of the downloading. Counted, Release() must be called
     *         after use.
     * @exception Leave System wide error code.
     *  KNcdErrorParallelOperationNotAllowed if a parallel client is running
     *  an operation for the same metadata. See MNcdOperation for full explanation.
     */
    virtual MNcdDownloadOperation* DownloadContentL( 
        MNcdDownloadOperationObserver& aObserver ) = 0;


protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase()
     */
    virtual ~MNcdNodeDownload() {}
    
    };


#endif // M_NCD_NODE_DOWNLOAD_H
