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
* Description:   Observer interface definition for install operations
*
*/
	

#ifndef M_NCD_INSTALL_OPERATION_OBSERVER_H
#define M_NCD_INSTALL_OPERATION_OBSERVER_H

#include <e32cmn.h>

class MNcdInstallOperation;
class TNcdProgress;
class MNcdQuery;

/**
 *  Observer interface for install operations
 *
 *  Install operation users should implement this interface to be able to
 *  receive callbacks.
 *
 *  
 *  @see MNcdInstallOperation
 */
class MNcdInstallOperationObserver
    {
    
public:
     
     
    /**
     * Called when install has progressed. 
     *
     * @note This will be called only once per an installed file. 
     * The reason for this is that the install process is handled outside of
     * Catalogs Engine in the platform installer.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aProgress Progress of the operation.
     */
    virtual void InstallProgress( MNcdInstallOperation& aOperation,
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
     * @param aOperation Uncounted reference to the operation that sent the event.
     * @param aQuery Pointer to the query that must be completed. Counted, Release()
     *        must be called after use.
     * @see MNcdOperation::CompleteQueryL()
     */
    virtual void QueryReceived( MNcdInstallOperation& aOperation,
        MNcdQuery* aQuery ) = 0;

    /**
     * Called when an operation has been completed.
     *
     * 
     * @param aOperation The operation that sent the event.
     * @param aError Error code for operation completion. 
     *               KErrNone for successful completion, KErrAbort if the user
     *               cancelled the operation, otherwise a system
     *               wide error code.
     */
    virtual void OperationComplete( MNcdInstallOperation& aOperation,
        TInt aError ) = 0;


protected:

    /**
     * Destructor
     *
     * An empty virtual destructor is implemented to prevent deletion 
     * of a class object through this interface.
     * Observer should not be deleted by the one that is observed.
     */
    virtual ~MNcdInstallOperationObserver() { }
    
    };
    
#endif //  M_NCD_INSTALL_OPERATION_OBSERVER_H
