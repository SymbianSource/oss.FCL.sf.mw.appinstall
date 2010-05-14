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
* Description:   Observer interface definition for download operations
*
*/
	

#ifndef M_NCD_DOWNLOAD_OPERATION_OBSERVER_H
#define M_NCD_DOWNLOAD_OPERATION_OBSERVER_H

#include <e32cmn.h>

class TNcdProgress;
class MNcdDownloadOperation;
class MNcdQuery;

/**
 *  Observer interface for download operations
 *
 *  Download operation users should implement this interface to be able to
 *  receive callbacks.
 *
 * 
 */
class MNcdDownloadOperationObserver
    {
    
public:

    /**
     * Called when a download has progressed. The progress value
     * is relative to the current file being downloaded.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aProgress Progress of the operation.
     */
    virtual void DownloadProgress( MNcdDownloadOperation& aOperation,
        TNcdProgress aProgress ) = 0;
        
    /**
     * Called when a query has been received for an operation.
     *
     * @note Querys need to be completed with MNcdOperation::CompleteQueryL()
     *       to continue the operation! Alternatively, the operation must be
     *       cancelled.
     * 
     * 
     * @see MNcdOperation
     * @see MNcdQuery
     * @param aOperation The operation that sent the event.
     * @param aQuery Pointer to the query object that needs to be
     *        completed. Counted, Release() must be called after use.
     * @see MNcdOperation::CompleteQueryL()
     */
    virtual void QueryReceived( MNcdDownloadOperation& aOperation,
        MNcdQuery* aQuery ) = 0;

    /**
     * Called when an operation has been completed.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aError Error code for operation completion.
     *               KErrNone for successful completion, otherwise a system
     *               wide error code.
     */
    virtual void OperationComplete( MNcdDownloadOperation& aOperation,
        TInt aError ) = 0;
    
    };
    
#endif //  M_NCD_DOWNLOAD_OPERATION_OBSERVER_H
