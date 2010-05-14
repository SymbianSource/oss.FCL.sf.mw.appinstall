/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef M_NCDSENDHTTPREQUESTOPERATIONOBSERVER_H
#define M_NCDSENDHTTPREQUESTOPERATIONOBSERVER_H

#include <e32cmn.h>

class TNcdProgress;
class MNcdSendHttpRequestOperation;
class MNcdQuery;

/**
 *  Observer interface for HTTP request send operation
 *
 * 
 */

class MNcdSendHttpRequestOperationObserver
    {
public:

    /**
     * Called when the operation has progressed.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aProgress Progress of the operation.
     */
    virtual void Progress( 
        MNcdSendHttpRequestOperation& aOperation,
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
    virtual void QueryReceived( 
        MNcdSendHttpRequestOperation& aOperation,
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
    virtual void OperationComplete( 
        MNcdSendHttpRequestOperation& aOperation,
        TInt aError ) = 0;
    

protected:

    virtual ~MNcdSendHttpRequestOperationObserver()
        {
        }
    };

#endif // M_NCDSENDHTTPREQUESTOPERATIONOBSERVER_H
