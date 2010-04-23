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
* Description:   ?Description
*
*/
	

#ifndef M_NCD_OPERATION_OBSERVER_H
#define M_NCD_OPERATION_OBSERVER_H

#include <e32cmn.h>

class CNcdBaseOperation;
class CNcdQuery;
class CNcdExpiredNode;

/**
 *  Server side observer interface for operations.
 *
 *  This interface offers child operations a way to inform their parent
 *  operations of progress and completion events. It is also used 
 *  to pass querys and expiration info to proxy side as sub-ops don't
 *  usually have proxys.
 *
 *  @since S60 v3.2
 */
class MNcdOperationObserver
    {
    
public:

    /**
     * Called when progress is made in the sub-op. 
     * Should be used only if the parent really needs the progress information
     * for e.g. forwarding it to the proxy.
     *
     * @param aOperation The calling sub-op.
     */
    virtual void Progress( CNcdBaseOperation& aOperation ) = 0;
    
    /**
     * Called when sub-op has received a query.
     *
     * @param aOperation The calling sub-op.
     * @param aQuery The query to forward.
     */
    virtual void QueryReceived( CNcdBaseOperation& aOperation,
                                CNcdQuery* aQuery ) = 0;

    /**
     * Called when a sub-operation has been completed.
     *
     * @param aOperation The calling sub-op.
     * @param aOperation aError KErrNone if completed correctly otherwise
     *        system-wide error code.
     */
    virtual void OperationComplete( CNcdBaseOperation* aOperation,
                                    TInt aError ) = 0;
                                    
    /**
     * Called when a sub-op has expired nodes.
     *
     * @param aOperation The calling sub-op.
     * @param aOperation Expired nodes.
     */
    virtual void ExpirationInfoReceived( CNcdBaseOperation* aOperation,
        RPointerArray<CNcdExpiredNode>& aExpiredNodes ) = 0;
    };
    
#endif //  M_NCD_OPERATION_OBSERVER_H
