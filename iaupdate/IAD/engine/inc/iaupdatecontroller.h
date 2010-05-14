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




#ifndef IAUPDATECONTROLLER_H
#define IAUPDATECONTROLLER_H


#include <e32cmn.h>

#include "iaupdatecontrollerobserver.h"
#include "iaupdateconnectionmethod.h"

class MIAUpdateNode;
class MIAUpdateHistory;
class CIAUpdateRestartInfo;
class CIAUpdaterIdentifier;


/**
 * 
 *
 * @since S60 v3.2
 */
class MIAUpdateController
    {
    
public:

    /**
     * Virtual destructor of an interface can be used for deletion.
     *
     * @since S60 v3.2
     */        
    virtual ~MIAUpdateController()
        {
        }


    /**
     * Issue an asynchronous operation for controller to start up and ready for operations
     *
     * @note MIAUpdateControllerObserver::StartupComplete function is called when
     * this operation has finished.
     *
     * @note If returned error code is not KErrNone, then operation is not started 
     * and no callback is called.
     *
     * @return TInt System wide error code.
     * KErrInUse if the startup operation is already going on.
     * KErrAlreadyExists if the startup operation has already been successfully done.
     *
     * @since S60 v3.2
     **/
    virtual TInt Startup() = 0;
   
    
    /**
     * Issues a refresh operation. 
     * Only one operation can be in progress at a time; user has to always wait
     * until previous operation has completed.
     *
     * @param aAllowNetConnection ETrue means that information can be refreshed
     * from the network if required. EFalse means that only local information
     * is allowed to be used.
     *
     * @note MIAUpdateControllerObserver::RefreshComplete function is called when
     * this operation has finished.
     *
     * @since S60 v3.2
     **/
    virtual void StartRefreshL( TBool aAllowNetConnection ) = 0;
  
        
    /**
     * Cancel an ongoing refresh operation
     *
     * @since S60 v3.2
     **/
    virtual void CancelRefresh() = 0;


    /**
     * Cancel a possible ongoing reporting operation. 
     *
     * Reporting may have been started if FinishedUpdatesL function is called 
     * and operations are allowed there. Then, reporting operation handles its 
     * job in the background.
     *
     * @since S60 v3.2
     **/
    virtual void CancelReporting() = 0;
   
    
    /**
     * @return MIAUpdateHistory& The history of the update items.
     *
     * @since S60 v3.2
     */
    virtual MIAUpdateHistory& HistoryL() = 0;


    /**
     * Sets the default connection method used for network access.
     *
     * @note Not stored persistently. A client needs to set this again every time it
     * creates the controller.
     * 
     * @param aMethod Identifier of the connection method to use by default.
     *
     * @since S60 v3.2
     */    
    virtual void SetDefaultConnectionMethodL( const TIAUpdateConnectionMethod& aMethod ) = 0;


    /**
     * This function can be used to check if any items have been set for self update.
     * If this function returns EFalse, then StartPossibleSelfUpdateL function will 
     * not start self updater because there is nothing to update.
     *
     * @return TBool ETrue if at least one self update item has been set.
     * EFalse if no self update item has been set.
     */
    virtual TBool SelfUpdateDataExists() const = 0;    


    /**
     * If there exists self updater items that are waiting permission to 
     * be updated, this function starts the self updater which handles the
     * updating of those items. If no updater items are waiting, then this
     * function does not do anything.
     *
     * @note Self updater most likely shuts down this UI.
     * 
     * @param aIndex Index information that informs how many other
     * items have already been installed.
     * @param aTotalCount The total number of items that will be installed.
     * @param aPendingNodes The UIDs of these nodes will be saved to a temporary file
     * and this information can be gotten by calling SelfUpdateRestartInfoL, for example,
     * when restarting the iaupdate after self update.
     * @param aSilent If ETrue, the operation should be silent and no dialogs should be shown.
     * If EFalse, then operation can show dialogs. 
     * @return TBool ETrue if self updater was started. Else EFalse.
     * @exception Leaves with system wide error code.
     *
     * @since S60 v3.2
     */
    virtual TBool StartPossibleSelfUpdateL( TInt aIndex, 
                                            TInt aTotalCount,
                                            const RPointerArray< MIAUpdateNode >& aPendingNodes,
                                            TBool aSilent ) = 0;
    
            
    /**
     * Resets the self updater infos.
     * This only resets the object data.
     *
     * @since S60 v3.2     
     */         
    virtual void ResetSelfUpdate() = 0;


    /**
     * Function provides objects that can be used to check if the self updater was started. 
     * This function can be used, for example, when UI is started. Then, the results values 
     * can be used to check if the UI should continue updates after successfull self installation
     * or if the UI should not continue updates.
     *
     * @notice It is up to the user to decide when to delete the files that contain
     * CIAUpdateRestartInfo information. Files can be deleted by calling
     * CIAUpdateRestartInfo::DeleteFiles function. This way the UI may check
     * the data and release memory when it suits UIs purposes.
     *
     * @return CIAUpdateRestartInfo* NULL if required files were
     * not available. Else pointer to CIAUpdateRestartInfo object that can be
     * used to check the results of the self update and to get the information about
     * the pending node updates. Ownership is transferred.
     */
    virtual CIAUpdateRestartInfo* SelfUpdateRestartInfo() = 0;    


    /**
     * This function will return the latest version of the node that corresponds to the given 
     * identifier.
     * 
     * @note This function can return the nodes that are not given for the controller observer when
     * refresh is called. These hidden nodes have already been installed. So, this function is mainly
     * meant for the self update purposes, when already installed nodes should be gotten for some
     * extra handling in the UI side. The required identifier can be gotten from SelfUpdateRestartInfo
     * function.
     *
     * @param aIdentifier Identifies the node uniquely.
     * @return MIAUpdateNode& Node.
     */
    virtual MIAUpdateNode& NodeL( const CIAUpdaterIdentifier& aIdentifier ) = 0;


    /**
     * This function should be called when updates are started.
     * The purpose of this function is to set engine ready for the whole update process
     * that may contain multiple update items. For example, this way the engine knows
     * that data for the server reports can be start collecting until the whole update
     * process is finished.
     *
     * @see MIAUpdateController::FinishedUpdatesL
     */
    virtual void StartingUpdatesL() = 0;
    
    
    /**
     * This function should be called when updates have been completed.
     * The purpose of this function is to inform engine that the whole update process 
     * that may contain multiple update items has finished. For example, this way the engine
     * knows that the collected server reports can be sent because the whole update process
     * is finsihed.
     * 
     * @note If parameter aOperationsAllowed is given as ETrue, 
     * this function starts an asynchronous server report sending and 
     * MIAUpdateServerReportObserver::ServerReportOperationComplete is 
     * called when the operation is finished.
     
     * @note Even if aOperationsAllowed is given as EFalse and a negative value is
     * given for aMaxWaitTime, one asynchronous loop is done before 
     * MIAUpdateServerReportObserver::ServerReportOperationComplete is called.
     *
     * @param aOperationsAllowed ETrue if new operations can be created and started.
     * Else EFalse. If ETrue is given, then for example, server report operation is started.
     * @param aMaxWaitTime The maximum time in microseconds that completion of server reports
     * is waited. Negative value means that timer should not be used if operation is created.
     * If maximum time is reached, then timer will call 
     * MIAUpdateServerReportObserver::ServerReportOperationComplete.
     * Notice, that the server reporting still continues in the background until operation
     * is cancelled, for example, when the application is closed.
     * 
     * @see MIAUpdateServerReportObserver::ServerReportOperationComplete
     * @see MIAUpdateController::StartingUpdatesL
     */
    virtual void FinishedUpdatesL( TBool aOperationsAllowed, TInt aMaxWaitTime ) = 0;

    };


#endif  //  IAUPDATECONTROLLER_H
