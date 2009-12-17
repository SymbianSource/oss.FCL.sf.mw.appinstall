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
* Description:   This file contains the header file of the MIAUpdateObserver class 
*
*/


#ifndef IA_UPDATE_OBSERVER_H
#define IA_UPDATE_OBSERVER_H


class CIAUpdateResult;


/**
 * MIAUpdateObserver provides callback functions that the updater will call
 * when an update operation has been completed.
 *
 * @note If an update is targeted to multiple items during one operation call, 
 * the observer is informed only once about the completion. This is done,
 * when the update operation as a whole has been completed.
 *
 * @note When the application updates itself, installation requires the shutdown 
 * of the application. Then, the updater will continue the update operation 
 * in the background but callbacks to nonexisting observers are skipped. 
 * After installation is finished, the updated application will be restarted 
 * if that information is included in CIAUpdateParameters.
 *
 * @note An application may also update itself without a need for restart. 
 * For example, when plug-ins are retrieved for the application. 
 * Then, the callback functions of the observer can be called from the updater. 
 * And, the observer will know when the necessary updates have been installed.
 *
 * @see CIAUpdate
 * @see CIAUpdateParameters
 * @see CIAUpdateResult
 *
 * @since S60 v3.2
 */
class MIAUpdateObserver
    {

public:

    /** 
     * This callback function is called when the update checking operation has completed.
     *
     * @param aErrorCode The error code of the observed update operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param aAvailableUpdates Number of the updates that were found available.
     *
     * @since S60 v3.2
     */
    virtual void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates ) = 0;


    /** 
     * This callback function is called when an update operation has completed.
     * Even if multiple functions are provided to start different update operations,
     * this callback function is always called after an update operation has completed.
     *
     * @param aErrorCode The error code of the completed update operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param aResult Details about the completed update operation.
     *                Ownership is transferred.
     *
     * @since S60 v3.2
     */
    virtual void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult ) = 0;


    /** 
     * This callback function is called when an update query operation has completed.
     *
     * @param aErrorCode The error code of the observed query operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param aUpdateNow ETrue informs that an update operation should be started.
     *                   EFalse informs that there is no need to start an update
     *                   operation.
     * @since S60 v3.2
     */
    virtual void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow ) = 0;

    };

#endif // IA_UPDATE_OBSERVER_H
