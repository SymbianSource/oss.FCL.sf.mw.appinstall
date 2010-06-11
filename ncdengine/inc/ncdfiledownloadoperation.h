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
* Description:   File download operation interface definition
*
*/


#ifndef M_NCD_FILE_DOWNLOAD_OPERATION_H
#define M_NCD_FILE_DOWNLOAD_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"
#include "ncdoperationdatatypes.h"

class MNcdFileDownloadOperationObserver;

/**
 *  File download operation interface.
 *
 *  A file download operation handles the downloading of one file to a
 *  specified location in the local filesystem.
 *
 *  
 *  @see MNcdFileDownloadOperationObserver
 */
class MNcdFileDownloadOperation : public MNcdOperation
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdFileDownloadOperationUid };


    /**
     * Get target file name.
     *
     * 
     * @returns Full path to target file. Empty string if the path has not
     *          been set.
     */
    virtual const TDesC& TargetFileName() const = 0;


    /**
     * Pause the download.
     *
     * @note Does nothing if the download was already paused, or if the
     *  operation is not yet started.
     *
     * 
     * @exception Leave System wide error code.
     */
    virtual void PauseL() = 0;

    /**
     * Resume a paused download.
     *
     * @note Does nothing if the download has not been paused, or if the
     *  operation is not yet started.
     *
     * 
     * @exception Leave System wide error code.
     */
    virtual void ResumeL() = 0;

    /**
     * Return the paused state of the download.
     *
     * 
     * @return ETrue, if the operation has been started and PauseL() has
     *  been called successfully, pausing the download. EFalse otherwise.
     */
    virtual TBool IsPaused() = 0;


    /**
     * Can download be paused
     *
     * @return ETrue if the operation can be paused, EFalse if pausing is not
     * allowed
     * @note Some DRM content is not pausable
     * @note Pausable state may change during the operation
     */
    virtual TBool IsPausableL() = 0;


    /**
     * Download data type getter
     *
     * @return Type of the data the operation is downloading
     * @see ncdoperationdatatypes.h
     */
    virtual TNcdDownloadDataType DownloadDataType() const = 0;
    
    
protected:

    /**
     * Destructor.
     *
     * @see MCatalogsBase::~MCatalogsBase
     */
    virtual ~MNcdFileDownloadOperation() {}

    };

	
#endif //  M_NCD_FILE_DOWNLOAD_OPERATION_H
