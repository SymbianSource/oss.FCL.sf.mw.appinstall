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
	

#ifndef M_NCDOPERATIONREMOVEHANDLER_H
#define M_NCDOPERATIONREMOVEHANDLER_H

class CNcdBaseOperation;

/**
 *  An interface for handling operation removal.
 *
 *  This interface is used to notify operation manager that the
 *  operation needs to be removed from it's op array. Manager needs to be
 *  explicitly notified to remove the operations because operations,
 *  being reference counting objects, handle their own destruction.
 * 
 */
class MNcdOperationRemoveHandler                                    
    {
    
public:


    /**
     * Removes operation.
     *
     * @param aOperation The operation to remove.
     */
    virtual void RemoveOperation(
                    CNcdBaseOperation& aOperation ) = 0;

protected:

    /**
     *
     */
    virtual ~MNcdOperationRemoveHandler() {}

	};
	
	
#endif //  M_NCDOPERATIONREMOVEHANDLER_H
