/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef IAUPDATECONTROLLEROBSERVER_H
#define IAUPDATECONTROLLEROBSERVER_H


#include <e32cmn.h>

class MIAUpdateAnyNode;


/**
 *
 */
class MIAUpdateControllerObserver
    {
    
public:

    /**
     * Notifies observer that controller startup is completed.
     *
     * @param aError IAUpdateErrorCodes::KErrCacheCleared if the
     * database cache was cleared. In this case, the refresh is 
     * recommended to update the database. In other cases, 
     * system wide error code. 
     **/
    virtual void StartupComplete( TInt aError ) = 0;


    /**
     * Notifies observer that nodes have been loaded from the server
     * or from the local storage.
     *
     * @note aNodes does not contain nodes that are set hidden.
     * UI side should not normally use hidden nodes. But, in case
     * of dependencies, hidden nodes may be asked for special purposes.
     *
     * @param aNodes Nodes resulting from operation.
     * @param aError Error value
     **/
    virtual void RefreshComplete( 
        const RPointerArray< MIAUpdateAnyNode >& aNodes,
        TInt aError ) = 0;


    /**
     * Notifies observer that the self updater has completed its task. 
     * In normal case the application will be closed before self update completes.
     * If this function is called, then most likely something has gone wrong in the
     * self update.
     *
     * @param aErrorCode Error code of the self update operation.
     */
    virtual void SelfUpdaterComplete( TInt aErrorCode ) = 0;


    /**
     * Called when a server report has been sent.
     *
     * 
     * @param aError Error code for operation completion.
     *               KErrNone for successful completion, otherwise a system
     *               wide error code.
     *
     * @see MIAUpdateController::FinishedUpdatesL
     */
    virtual void ServerReportSent( TInt aError ) = 0;

    
    /**
     * Called when setup configuration is read
     *
     * 
     * @param aClientRole
     */
    virtual void ClientRole( const TDesC& aClientRole ) = 0;


protected:
        
    virtual ~MIAUpdateControllerObserver()
        {
        }
        
    };

#endif  //  IAUPDATECONTROLLEROBSERVER_H
