/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MIAUpdateRequestObserver interface
*
*/



#ifndef IA_UPDATE_REQUEST_OBSERVER_H
#define IA_UPDATE_REQUEST_OBSERVER_H

class CIAUpdateResult;

/**
 * MIAUpdateRequestObserver interface is used to observe
 * when the requested operation has been completed.
 *
 * @since 3.2
 */
class MIAUpdateRequestObserver
    {
    
public:

    /**
     * Called when an outstanding request is completed
     *
     * @param aResult Contains information about 
     * available updates or about success of performed updates.
     * Ownership is transferred.
     * @param aUpdateNow True value if an user answered yes to update query
     * @param aCountOfAvailableUpdates Count of available updates
     * @param aError Error code that informs the result 
     * of the requested operation. KErrNone if operation was
     * succesfull.
     *
     * @since 3.2
     */
    virtual void RequestCompleted( CIAUpdateResult* aResult, 
                                   TInt aCountOfAvailableUpdates,  
                                   TBool aUpdateNow,
                                   TInt aError ) = 0;


protected:

    /**
     * Protected destructor prevents unwanted deletion of
     * this class object. 
     *
     * @since 3.2
     */
    virtual ~MIAUpdateRequestObserver() { }
    
    };

#endif // IA_UPDATE_REQUEST_OBSERVER_H