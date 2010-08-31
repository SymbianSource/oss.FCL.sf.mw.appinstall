/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the header file of CIAUpdateUiController 
*                class .
*
*/



//INCLUDES
#ifndef IAUPDATEUICONTROLLER_H
#define IAUPDATEUICONTROLLER_H

#include <e32base.h>
#include "iaupdateuidefines.h"
#include "iaupdatecontroller.h"
#include "iaupdatecontrollerobserver.h"
#include "iaupdatenodeobserver.h"
#include "iaupdatewaitdialogobserver.h"
#include "iaupdatedialogobserver.h"
#include "iaupdatestarterobserver.h"
#include "iaupdateroaminghandlerobserver.h"
#include "iaupdaterefreshobserver.h"
#include "iaupdateresultsinfo.h"
#include "iaupdateversion.h"
#include "iaupdateanynode.h"

// FORWARD DECLARATIONS
class CIAUpdateWaitDialog;
class CIAUpdateProgressDialog;
class CIAUpdateNodeFilter;
class CIAUpdateStarter;
class CIAUpdateControllerFile;
class CIAUpdateUiConfigData;
class CIAUpdateRoamingHandler;
class CIAUpdateRefreshHandler;
class MIAUpdateUiControllerObserver;
class MIAUpdateHistory;
class MIAUpdateFwNode;
class IAUpdateWaitDialog;
class IAUpdateDialogUtil;

class QString;
class CIAUpdateNodeId;
class CEikonEnv;
class HbAction;


// CLASS DECLARATION
/**
 *  CIAUpdateUiController controls update flow in UI  
 *
 */
class CIAUpdateUiController : 
    public CBase, 
    public MIAUpdateControllerObserver,
    public MIAUpdateNodeObserver,
    public MIAUpdateWaitDialogObserver,
    public MIAUpdateStarterObserver,
    public MIAUpdateRoamingHandlerObserver,
    public MIAUpdateRefreshObserver,
    public IAUpdateDialogObserver
    {
           
public: //new functions

    /**
     * Symbian two phased constructor.
     * @param aObserver Observer interface of controller
     * @return Instance of the created server object.
     */
    static CIAUpdateUiController* NewLC( 
        MIAUpdateUiControllerObserver& aObserver );    

    /**
     * Symbian two phased constructor.
     * @param aObserver Observer interface of controller
     * @return Instance of the created server object.
     */
    static CIAUpdateUiController* NewL( 
        MIAUpdateUiControllerObserver& aObserver );    

    /**
     * Destructor
     */
    virtual ~CIAUpdateUiController();

    /**
     * @return const CIAUpdateUiConfigData& Configuration data
     * from the configuration file.
     */
    const CIAUpdateUiConfigData& ConfigData() const;

    /**
     * @see MIAController::SetDefaultConnectionMethodL
     */
    void SetDefaultConnectionMethodL( 
        const TIAUpdateConnectionMethod& aMethod );

    /**
     * Check updates
     */
    void CheckUpdatesL();

    /**
     * Start refresh
     */
    void StartRefreshL();

    /**
     * Starts installation
     * @param aNodes An array of nodes to be installed
     */
    void StartInstallL( MIAUpdateNode& aNode );

    /**
     * Starts software updating.
     */
    void StartUpdateL();

    /**
     * Continue software updating, next item is handled.
     * @param aSelfUpdateFinished ETrue if the update is continued right after
     * the self update.
     */
    void ContinueUpdateL( TBool aSelfUpdateFinished );
        
    /**
     * Get list of nodes
     * @return An array of nodes 
     */    
    const RPointerArray< MIAUpdateNode >& Nodes() const;

    /**
     * Get list of firmware nodes
     * @return An array of nodes 
     */    
    const RPointerArray< MIAUpdateFwNode >& FwNodes() const;
    
    /**
     * @see MIAUpdateController::HistoryL
     */ 
    MIAUpdateHistory& HistoryL();    

    /**
     * Sets the request type
     * @param aRequestType Request type
     */
    void SetRequestType( 
        IAUpdateUiDefines::TIAUpdateUiRequestType aRequestType );

    /**
     * Cancel ongoing async operation
     **/
    void CancelOperation();

    /**
     * Info about success of updates
     * @return TIAUpdateResultsInfo Describes success
     * of update operations
     **/
    TIAUpdateResultsInfo ResultsInfo() const;

    /**
     * Count of available updates
     * @return Count of available updates
     **/
    TInt CountOfAvailableUpdates() const; 

    /**
     * Gives access to starter object
     * @return Pointer to CIAUpdateStarter instance
     **/
    const CIAUpdateStarter* Starter() const;
    
    /**
     * Gives access to filter object
     * @return Pointer to CIAUpdateNodeFilte instance
     **/
    const CIAUpdateNodeFilter* Filter() const;

    /**
     * Do cancel depending on request type and state
     * @return True value if possible cancel has been performed 
     *         and/or application can be closed by client
     **/
    TBool DoCancelIfAllowed(); 

    /**
     * Depending on request type and state closing Application update is
     * allowed or not by a client application
     *
     * @return True if closing is allowed by a client
     **/
    TBool ClosingAllowedByClient();

    /**
     * Starts CIdle before update check. That's because waiting dialogs 
     * may be shown before update check, still client's request to be responsed immediately  
     *
     * @param aParams Update parameters passed from a client
     **/
    void CheckUpdatesDeferredL( CIAUpdateParameters* aParams, TBool aRefreshFromNetworkDenied );
    
    
    void PrepareRoamingHandlerL();

    /**
     * This function handles situation when the iaupdate has been restarted after
     * self update. If restart has occurred, then the update operation will be continued
     * to the end as in normal update case.
     *
     * @param aShutdownRequest True value means that update was completed by end key, 
     *                         only purchase history to be updated in this function 
     *
     * @return TBool ETrue if the restart has occurred. EFalse if the application was
     * not started from self updater.
     */
    TBool HandlePossibleSelfUpdateRestartL( TBool aShutdownRequest );
    
    /**
    * This is used to get information after self update when
    * IAD is restarted.
    *
    * @return CIAUpdateParameters* Parameter object that is read
    * from the file. NULL if file was not found. 
    * Ownership is not transferred.
    */
    CIAUpdateParameters* ParamsReadAndRemoveFileL();
    
    TBool ForcedRefresh() const;
      
    void SetForcedRefresh( TBool aForcedRefresh );
    
    /**
     *  Set node list to normal state after update. 
     */
    void RefreshNodeList();
    
    
    /**
     * Is client role "testing"
     *
     * @return ETrue when client's role is "testing"
     **/
    TBool TestRole() const; 
    
            
public: // MIAUpdateControllerObserver     

    /**
     * @see MIAUpdateControllerObserver::StartupComplete 
     **/
    void StartupComplete( TInt aError );
 
    /**
     * @see MIAUpdateControllerObserver::RefreshComplete
     **/
    void RefreshComplete( const RPointerArray< MIAUpdateAnyNode >& aNodes,
                          TInt aError );

    /**
     * @see MIAUpdateControllerObserver::SelfUpdaterComplete
     **/
    void SelfUpdaterComplete( TInt aErrorCode );

    /**
     * @see MIAUpdateControllerObserver::ServerReportSent
     **/
    void ServerReportSent( TInt aError );
    
    /**
     * @see MIAUpdateControllerObserver::ClientRole
     **/
    void ClientRole( const TDesC& aClientRole );


public: // MIAUpdateNodeObserver
            
    /**
     * @see MIAUpdateNodeObserver::DownloadProgress
     **/
    void DownloadProgress( MIAUpdateNode& aNode, 
                           TUint aProgress,
                           TUint aMaxProgress );
                            
    /**
     * @see MIAUpdateNodeObserver::DownloadComplete
     **/
    void DownloadComplete( MIAUpdateNode& aNode,
                           TInt aError );

    /**
     * @see MIAUpdateNodeObserver::InstallProgress
     **/
    void InstallProgress( MIAUpdateNode& aNode, 
                          TUint aProgress,
                          TUint aMaxProgress );
                            
    /**
     * @see MIAUpdateNodeObserver::InstallComplete
     **/
    void InstallComplete( MIAUpdateNode& aNode,
                          TInt aError );

    
private: // MIAUpdateStarterObserver    

    /**
     * Called when an outstanding request is completed
     *
     * @param aError Error code that informs the result 
     * of the start operation. KErrNone if operation was
     * succesfull.
     */
     void StartExecutableCompletedL( TInt aError );
 
     
private:  // MIAUpdateRoamingHandlerObserver

     /**
      * Called when roaming handler is prepared
      */
     void RoamingHandlerPrepared();
  
        
private: // From MIAUpdateWaitDialogObserver    
    
    /**
     * This is called when the dialog is  closed.
     */
     void HandleWaitDialogCancel();   
     
private: // From IAUpdateDialogObserver     
         
     void dialogFinished(HbAction *action);   
    

private: // From MIAUpdateRefreshObserver   

     void HandleUiRefreshL();


private: // construction

    /**
     * Perform the second phase construction of a CIAUpdateUiController object
     * this needs to be public due to the way the framework constructs the AppUi 
     */
    void ConstructL();
    
    /**
     * Perform the first phase of two phase construction.
     * This needs to be public due to the way the framework constructs the AppUi 
     * @param aObserver Observer interface of controller
     */
    CIAUpdateUiController( MIAUpdateUiControllerObserver& aObserver );


private: // new functions
    
    void AgreementHandledL();

    /**
     * Called when refreshing updates list is completed.
     *
     * @param aNodes An array of nodes
     * @param aError Error code
     */
    void RefreshCompleteL( const RPointerArray< MIAUpdateAnyNode >& aNodes,
                           TInt aError );
    
    /**
     * Called when refreshing updates list is completed. 
     *
     * @param aNodes An array of nodes
     * @param aError Error code
     */
    void RefreshUiCompleteL( const RPointerArray< MIAUpdateAnyNode >& aNodes,
                             TInt aError );
    
    
    /**
     * Called when downloading of an update is completed
     *
     * @param aNode Downloaded node
     * @param aError Error code
     */
    void DownloadCompleteL( MIAUpdateNode& aNode, TInt aError );
    
    /**
     * Called when installation of an update is completed
     *
     * @param aNode Installed node
     * @param aError Error code
     */
    void InstallCompleteL( MIAUpdateNode& aNode, TInt aError );
    
    /**
     * Called when updating is completed
     *
     * @param aError Error code
     */
    void UpdateCompleteL( TInt aError );

    /**
     * Finishes the update flow after server reports are sent
     * or when cancel is called while sending.
     *
     * @param aError Error code
     */
    void EndUpdateFlowL( TInt aError );
    
    /**
     * Callback of CIdle, calls CheckUpdates()
     *
     * @param aPtr Pointer to this instance
     */
    static TInt CheckUpdatesDeferredCallbackL( TAny* aPtr );
                       
    /**
     * Shows waiting/progress dialog during update process.
     *
     * @param aTextResourceId Resource id of text (Downloading or Installing)
     * @param aName           Name of update
     * @param aNumber         Counter of current update
     * @param aTotalCount     Total count of updates to be updated
     */                   
    void ShowUpdatingDialogL( TInt aTextResourceId,
                              const TDesC& aName,
                              TInt aNumber,
                              TInt aTotalCount );  
     
    /**
     * Shows waiting dialog during update process.
     *
     * @param aDisplayString       A string to be displayed
     * @param aVisibilityDelayOff  ETrue if visibility delay is off
     */                  
    void ShowWaitDialogL( const QString& aDisplayString, 
                          TBool aVisibilityDelayOff ); 
    
    /**
     * Shows progress dialog during update process.
     *
     * @param aDisplayString       A string to be displayed
     * @param aVisibilityDelayOff  ETrue if visibility delay is off
     */  
    void ShowProgressDialogL( const TDesC& aDisplayString, 
                              TBool aVisibilityDelayOff ); 
    
    /**
     * Removes all possible existing update flow dialogs.
     * This method is a general way to handle removal of 
     * wait or progress dialogs.
     */
    void RemoveUpdatingDialogsL();
    
    /**
     * Removes waiting dialog.
     */ 
    void RemoveWaitDialogL();
    
    /**
     * Removes progress dialog.
     */ 
    void RemoveProgressDialogL();
    
    /**
     * Checks if refreshing of update list allowed from a network
     * @return ETrue if network refresh allowed
     */
    TBool AllowNetworkRefreshL();
    
    /**
     * Checks if local update list cache is expired
     * @return ETrue if local cache is expired
     */
    TBool LocalNodesExpiredL();
    
    /**
     * Updates local update list cache controller file
     */
    void UpdateControllerFileL();
    
    /**
     * Checks if a phone is connected in a roaming network
     * @return ETrue if roaming network 
     */ 
    TBool IsRoaming();
    
    /**
     * Checks how iaupdate is started
     * @return ETrue if started by iaupdatelauncher, otherwise started by a client application 
     */
    TBool IsStartedByLauncher();

    /**
     * Starts the self updater process if there exists updates that require separate
     * updater.
     * @return TBool ETrue if the updater is started. 
     *               EFalse if updater was not started.
     */
    TBool StartPossibleSelfUpdateL();
    
    /**
     * Updates self update item error codes into the purchase history
     * after self updater could not do its job.
     * @param aErrorCode Error code that self updater has failed with.
     */
    void UpdateFailedSelfUpdaterInfoToPurchaseHistoryL( TInt aErrorCode );
    
   /**
     * Handles user cancel
     */    
    void HandleUserCancelL();

    /**
     * @return TBool ETrue if IAD has been started right after
     * the self update. In other words, self update related data
     * has not been handled yet. Else EFalse.
     */
    TBool CIAUpdateUiController::RestartedFromSelfUpdate();
    
    /**
     * Writes the possible parameter information into the temporary
     * file. This is used to save paremeter data during self update.
     */    
    void ParamsWriteFileL();
    
    /**
     * Removes the temporary parameters file if it exists.
     */
    void ParamsRemoveFileL();
            
    void CreateSelectedNodesArrayL();
    
    TBool IAUpdateEnabledL() const;
    
    TBool AutomaticConnectionWhenRoamingL() const;

private: // data

    enum TState
        {
        EIdle,
        ERefreshing,
        EDownloading,
        EInstalling,
        ESendingReport,
        EUiRefreshing,
        ESelfUpdating
        };

    enum TDialogState
        {
        ENoDialog,
        EAgreement,
        EInsufficientMemory  
        };
    
    
    MIAUpdateUiControllerObserver& iObserver;

    MIAUpdateController* iController;
    
    RPointerArray< MIAUpdateNode > iNodes;   
    
    RPointerArray< MIAUpdateNode > iSelectedNodesArray; 

    RPointerArray< MIAUpdateFwNode > iFwNodes;   
   
    RPointerArray<CIAUpdateNodeId> iPreviousSelections;
    
    RPointerArray<MIAUpdateNode> iServicePackNodes;
    
    IAUpdateDialogUtil *mDialogUtil;  
 
    IAUpdateWaitDialog *mWaitDialog;
    
    CIAUpdateProgressDialog* iProgressDialog;
    
    CIAUpdateStarter* iStarter;
    
    CIAUpdateRoamingHandler* iRoamingHandler;
    
    CIAUpdateRefreshHandler* iRefreshHandler;
    
    TInt iCountOfAvailableUpdates;

    TInt iNodeIndex;                               

    CIAUpdateNodeFilter* iFilter;

    CIAUpdateControllerFile* iControllerFile;
    
    CIdle* iIdle;
            
    CEikonEnv* iEikEnv; //not owned
    
    IAUpdateUiDefines::TIAUpdateUiRequestType iRequestType;
    
    TState iState;
    
    TDialogState iDialogState;
    
    TBool iClosingAllowedByClient;
    
    CIAUpdateUiConfigData* iConfigData;
    
    CIAUpdateParameters* iParams;
    
    TBool iCancelling;
    
    TBool iFileInUseError;
    
    TBool iRefreshed;
    
    TBool iRefreshFromNetworkDenied; 
    
    TBool iOffConfigurated;
    
    TBool iForcedRefresh;
        
    TBool iTestRole;
    
    HbAction *mPrimaryAction;

    };

#endif // IAUPDATEUICONTROLLER_H

