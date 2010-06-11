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
* Description:  
*
*/
	

#ifndef M_NCDOPERATIONPROXYREMOVEHANDLER_H
#define M_NCDOPERATIONPROXYREMOVEHANDLER_H

class CNcdBaseOperationProxy;

/**
 *  An interface for handling operation proxy removal.
 *
 *  This interface is used to notify operation proxy manager that the
 *  operation needs to be removed from it's op array. Manager needs to be
 *  explicitly notified to remove the operation proxys because operation
 *  proxys, being reference counting objects, handle their own destruction.
 * 
 */
class MNcdOperationProxyRemoveHandler                                    
    {
    
public:

    /**
     *
     */
    virtual ~MNcdOperationProxyRemoveHandler() {}

    /**
     * Removes operation proxy.
     *
     * @param aOperationProxy The op proxy to remove.
     */
    virtual void RemoveOperationProxy(
                    CNcdBaseOperationProxy& aOperationProxy ) = 0;
	};
	
	
#endif //  M_NCDOPERATIONPROXYREMOVEHANDLER_H
