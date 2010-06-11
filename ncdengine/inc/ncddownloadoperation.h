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
* Description:   Download operation interface definition
*
*/
	

#ifndef M_NCD_DOWNLOAD_OPERATION_H
#define M_NCD_DOWNLOAD_OPERATION_H

#include <e32cmn.h>

#include "ncdoperation.h"
#include "ncdinterfaceids.h"
#include "ncdoperationdatatypes.h"

class MNcdDownloadOperationObserver;

/**
 *  Download operation interface.
 *
 *  A download operation handles the downloading of one or more files.
 *
 *  The files are downloaded consecutively and in a predefined order.
 *  If parallel downloading is desired, multiple operations can be started.
 *
 *  Common uses for download operatios are downloading a downloadable node's 
 *  content and downloading graphics for nodes.
 *
 *  Download operations can be queried for the number of the download that is 
 *  currently being downloaded. The progress information is always relative
 *  to the current download.
 *  
 *  
 *  @see MNcdDownloadOperationObserver
 *  @see MNcdNodeDownload
 */
class MNcdDownloadOperation : public MNcdOperation
    {
    
public:

    /**
     * Unique identifier for the interface, required for all MCatalogsBase interfaces.
     *
     * 
     */
    enum { KInterfaceUid = ENcdDownloadOperationUid };    
    

    /**
     * Getter for file count. This is the total number of files this operation
     * is downloading.
     *
     * 
     * @return Total number of files to be downloaded.
     */
    virtual TInt FileCount() = 0;
    

    /**
     * Getter for current file number. This is the number of the
     * file that is currently being downloaded.
     *
     * 
     * @return Number of the file currently being downloaded.
     */
    virtual TInt CurrentFile() = 0;
    

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
     * @leave Symbian error code
     * @note Some DRM content is not pausable
     * @note Pausable state may change during the operation
     */
    virtual TBool IsPausableL() = 0;

    /**
     * Add a new observer for this download operation.
     *
     * If the observer has already been added, this function doesn't do anything.
     *
     * 
     * @exception Leave System wide error code.
     */
    virtual void AddObserverL( MNcdDownloadOperationObserver& aObserver ) = 0;

    /**
     * Remove an observer previously added with AddObserverL().
     *
     * @return ETrue if an observer was removed
     * 
     */
    virtual TBool RemoveObserver( MNcdDownloadOperationObserver& aObserver ) = 0;


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
    virtual ~MNcdDownloadOperation() {}

    };
	
	
#endif //  M_NCD_DOWNLOAD_OPERATION_H
