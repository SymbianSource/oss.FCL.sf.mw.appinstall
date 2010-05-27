/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateEngine
*                class 
*
*/

#ifndef IAUPDATEENGINE_H_
#define IAUPDATEENGINE_H_

#include <QObject>

#include "iaupdateuidefines.h"
#include "iaupdateuicontrollerobserver.h"
#include "iaupdatenode.h"

class IAUpdateServiceProvider;
class CIAUpdateUiController;
class CIAUpdateFWUpdateHandler;
class CIAUpdateParameters;
class CIAUpdateGlobalLockHandler;
class MIAUpdateNode;
class MIAUpdateFwNode;
class CEikonEnv;
class RCmManagerExt;
class CIdle;

class IAUpdateEngine : public QObject,
                       public MIAUpdateUiControllerObserver
    {
    Q_OBJECT

public:
    IAUpdateEngine(QObject *parent = 0);
    ~IAUpdateEngine();
    
    void StartedByLauncherL( TBool aRefreshFromNetworkDenied );
     
     /**
      * When the update check operation is started through
      * the API, the session directs the requests for the UI. UI will
      * handle the operation and inform the observer when the operation is
      * completed.
      * @param aFilterParams These parameters are used when update items
      * are filtered for the UI list.
      */
     void CheckUpdatesRequestL( int wgid, 
                                CIAUpdateParameters* aFilterParams, 
                                TBool aForcedRefresh );

     /**
      * When the show update operation is started through
      * the API, the session directs the requests for the UI. UI will
      * hanle the operation and inform the observer when the operation is
      * completed.
      * @param aFilterParams These parameters are used when update items
      * are filtered for the UI list.
      */
     void ShowUpdatesRequestL( int wgid, CIAUpdateParameters* aFilterParams );
     
              
     
     /** 
      * When the update operation is started through
      * the API, the session directs the requests for the UI. UI will
      * hanle the operation and inform the observer when the operation is
      * completed.
      * @param aUid  Uid of the caller of the request
      */
     void ShowUpdateQueryRequestL( int wgid, TUint aUid );
     
     
     void StartUpdate( TBool aFirmwareUpdate );
     /**
      * Set this application visible/unvisible
      * @param aVisible If EFalse application is put background and is hidden in FSW 
      */
      void SetVisibleL( TBool aVisible );
      
      /**
      * Set window group id of client application 
      *
      * @param aWgId Window group id
      */
      void SetClientWgId( TInt aWgId );
          
      /**
      * Is client application in background
      *
      * @param True value if client application is in background
      */
      TInt ClientInBackgroundL() const;
    
      //void refresh(int error);
signals:
    void toMainView();
    
    /**
    * Refreshes main view 
    * @param aError  Error code
    */              
    void refresh(const RPointerArray<MIAUpdateNode>& nodes,
                 const RPointerArray<MIAUpdateFwNode>& fwNodes,
                 int error);
    
    void toHistoryView();

public slots:    
    void handleAllClientsClosed();    
  

private: // From MIAUpdateUiControllerObserver   
    /**
    * Called when async engine start-up is completed
    *
    * @param aError Error code
    */
    void StartupComplete( TInt aError ); 
    
    /**
    * Leaving method, called as trapped in StartupComplete
    */
    void StartupCompleteL(); 
 
    
    /**
    * Handles leave error. 
    *
    * @param aError Error code
    */      
    void HandleLeaveErrorL( TInt aError );
    
    /**
    * Handles leave error withot leaving
    *
    * @param aError Error code
    */      
    void HandleLeaveErrorWithoutLeave( TInt aError );
            
    /**
    * Called when async update list refresh is completed
    *
    * @param aError Error code
    */
    void RefreshCompleteL( TBool aWithViewActivation, TInt aError );
    
    /**
    * Called when async updating is completed
    *
    * @param aError Error code
    */     
    void UpdateCompleteL( TInt aError );
    
private:  //new methods

    /**
    * Show results dialog of update
    */ 
    void ShowResultsDialogL();
        
    /**
    * Starts CIdle. Results dialog is shown in callback function. 
    */ 
    void ShowResultsDialogDeferredL();
    
    
    /**
    * Informs an observer that its async request is completed 
    *
    * @param aError  Error code 
    */ 
    void InformRequestObserver( TInt aError );
    
    /**
     * Sets the default connection method used for network access.
     * Connection can be set as an accesspoint or as a snap for example.
     *
     * @param aTotalSilent Set to ETrue if want silent connection
     *
     * @note Not stored persistently. A client needs to set this 
     * again every time it creates the controller.
     *
     * @note Leaves with KErrNotFound if connection method was not
     * set in the repocitory.
     */
    void SetDefaultConnectionMethodL( TBool aTotalSilent );   
    
     /**
     * Choose a usable IAP from all SNAPs
     *
     * @param aCmManagerExt handle of connection manager
     */  
    TUint32 GetBestIAPInAllSNAPsL( RCmManagerExt& aCmManagerExt  );
        
     /**
     * Choose a usable IAP from Internet SNAP
     *
     * @param aCmManagerExt handle of connection manager
     */
    TUint32 GetBestIAPInInternetSNAPL( RCmManagerExt& aCmManagerExt  );

     /**
     * Choose the best IAP under the given SNAP
     *
     * @param aCmManagerExt handle of connection manager
     * @param aSNAP SNAP ID
     */  
     TUint32 GetBestIAPInThisSNAPL( RCmManagerExt& aCmManagerExt, TUint32 aSNAP = 0);
     
     /**
     * Shows update query dialog (now/later) 
     */ 
     void ShowUpdateQueryL();

     void HideApplicationInFSWL( TBool aHide ) const;
     
     /**
     * CIdle callback function, that shows status dialog
     * To be used to guarantee that possible old status dialog is totally 
     * removed by AVKON before showing new one  
     *
     * @param aPtr  Pointer to this instance
     */ 
     static TInt ShowResultsDialogCallbackL( TAny* aPtr );
     
     
     /**
     * CIdle callback function, that shows update query dialog
     * To be used because a client to be informed immediately that its 
     * async request is issued. Waiting dialog cannot be shown before responding to a client  
     *
     * @param aPtr  Pointer to this instance
     */
     static TInt UpdateQueryCallbackL( TAny* aPtr );
     
     /**
     * CIdle callback function, that performs automatic update check and query
     *
     * @param aPtr  Pointer to this instance
     */
     static TInt AutomaticCheckCallbackL( TAny* aPtr );
         
     
    
public: //temp
//private:
    IAUpdateServiceProvider *mServiceProvider;
    
private:
    CIAUpdateUiController* iController;
    CIAUpdateFWUpdateHandler* iFwUpdateHandler;
    CIAUpdateGlobalLockHandler* iGlobalLockHandler;
    CEikonEnv* iEikEnv; //not owned
    CIdle* iIdle;
    CIdle* iIdleAutCheck;
    
    IAUpdateUiDefines::TIAUpdateUiRequestType iRequestType;
    TBool iUpdateNow;
    RArray<TUint32> iDestIdArray;
    TBool iRequestIssued;
    TBool iStartedFromApplication;
    TBool iUiRefreshAllowed;
    TUint iUpdatequeryUid;
    TInt iWgId;
  
    
    };

#endif /* IAUPDATEENGINE_H_ */
