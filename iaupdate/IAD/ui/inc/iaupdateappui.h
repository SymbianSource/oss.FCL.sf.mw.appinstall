/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the CIAUpdateAppUi class 
*
*/



#ifndef __IAUPDATE_APPUI_H__
#define __IAUPDATE_APPUI_H__

//INCLUDES
#include <e32base.h>
#include <aknViewAppUi.h> 
#include <cmmanagerext.h>

#include "iaupdateuidefines.h"
#include "iaupdateuicontrollerobserver.h"
#include "iaupdatedialogutil.h"
#include "iaupdateuitimerobserver.h"

// FORWARD DECLARATIONS
class CIAUpdateMainView;
class CIAUpdateHistoryView;
class CIAUpdateUiController;
class CIAUpdateParameters;
class MIAUpdateRequestObserver;
class CIAUpdateGlobalLockHandler;
class CIAUpdateUITimer;

/*! 
@class CIAUpdateAppUi
  
An instance of class CIAUpdateAppUi is the UserInterface part of the AVKON
application framework for the IAUpdate example application
*/
class CIAUpdateAppUi : 
    public CAknViewAppUi, 
    public MIAUpdateUiControllerObserver,
    public MIAUpdateTextLinkObserver,
    public MIAUpdateUITimerObserver
    {
public:

    /*
     * Perform the second phase construction of a CIAUpdateAppUi object
     * this needs to be public due to the way the framework constructs the AppUi 
     */
    void ConstructL();
    
    /*!
     * Perform the first phase of two phase construction.
     * This needs to be public due to the way the framework constructs the AppUi 
     */
    CIAUpdateAppUi();


    /*
     * Destroy the object and release all memory objects
     */
    virtual ~CIAUpdateAppUi();
    
    void StartedByLauncherL( MIAUpdateRequestObserver& aObserver, 
                             TBool aRefreshFromNetworkDenied );
    
    /**
     * When the update check operation is started through
     * the API, the session directs the requests for the UI. UI will
     * handle the operation and inform the observer when the operation is
     * completed.
     * @param aObserver This observer is informed when the operation has
     * been completed.
     * @param aFilterParams These parameters are used when update items
     * are filtered for the UI list.
     */
    void CheckUpdatesRequestL( MIAUpdateRequestObserver& aObserver,
                               CIAUpdateParameters* aFilterParams );

    /**
     * When the show update operation is started through
     * the API, the session directs the requests for the UI. UI will
     * hanle the operation and inform the observer when the operation is
     * completed.
     * @param aObserver This observer is informed when the operation has
     * been completed.
     * @param aFilterParams These parameters are used when update items
     * are filtered for the UI list.
     */
    void ShowUpdatesRequestL( MIAUpdateRequestObserver& aObserver,
                              CIAUpdateParameters* aFilterParams );
    
             
    
    /** 
     * When the update operation is started through
     * the API, the session directs the requests for the UI. UI will
     * hanle the operation and inform the observer when the operation is
     * completed.
     * @param aObserver This observer is informed when the operation has
     * been completed.
     * @param aUid  Uid of the caller of the request
     */
    void ShowUpdateQueryRequestL( MIAUpdateRequestObserver& aObserver, TUint aUid );
    
    /**
     *  Handle removal of observer
     */    
    void HandleObserverRemoval();
    
    /**
    * Set this application visible/unvisible
    * @param aVisible If EFalse application is put background and is hidden in FSW 
    */
    void SetVisibleL( TBool aVisible );
    
    /**
    * Show status dialog of update
    */ 
    void ShowStatusDialogL();
    
    /**
    * Starts CIdle. Status dialog is shown in callback function. 
    */ 
    void ShowStatusDialogDeferredL();
    
    /**
    * Get CIAUpdateUiController instance
    * @return Returns pointer of iController
    */    
    CIAUpdateUiController* Controller() const;
    
    /**
    * Performs cancel depending on ongoing request and state
    */   
    void PotentialCancel();
    
    /**
    * Set ordinal position to foreground and 
    * set iClientMovedToBackground to EFalse
    */  
    void HandleClientToForegroundL();
    
    /**
    * Handle situation when iaupdate.exe leaves running although 
    * client session has been closed
    */
    void HandleAllClientsClosedL();
    
    /**
    * Indicates whether Status dialog to be shown or not when history view is deactivated
    *
    * @return ETrue when status dialog to be re-shown
    */ 
    TBool ShowStatusDialogAgain() const;
        
    /**
    * Starts monitoring of window group list changes 
    */
    void StartWGListChangeMonitoring();
    
    /**
    * Stops monitoring of window group list changes 
    */
    void StopWGListChangeMonitoring();
    
    /**
    * Set window group id of client application 
    *
    * @param aWgId Window group id
    */
    void SetClientWgId( TInt aWgId );
    
    /**
    * Is client application in backround
    *
    * @param True value if client application is in background
    */
    TInt ClientInBackgroundL() const;
    
    
    /**
    * Set DM support
    * 
    * @param True value if DM is supported
    */
    void SetDMSupport( TBool aDMSupport );
    
    /**
    * Check DM support
    * 
    * @return True value if DM is supported
    */
    TBool IsDMSupport();
    
    /**
     * Check if DM support is set
     * 
     * @return True value if DM support is set
     */
    TBool IsDMSupportSet();
    
    /**
    * Check if UI refresh allowed
    * 
    * @return True value if UI refresh is allowed 
    */
    TBool UiRefreshAllowed();
    
        
private: // from CAknViewAppUi
    
    /*!
     * @function HandleCommandL  
     * @discussion Handle user menu selections
     * @param aCommand the enumerated code for the option selected
     */
    void HandleCommandL( TInt aCommand );
    
    /**
    * Handle window server event
    *
    * @param aEvent The window server event that occurred
    * @param aDestination The controller associated with the event 
    */
    void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );
    
             
private: // From MIAUpdateTextLinkObserver    
    void TextLinkL( TInt aLinkId );

    
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

private: // from MIAUpdateUITimerObserver

     virtual void BackgroundDelayComplete( TInt aError );   
     
     virtual void ForegroundDelayComplete( TInt aError );
     
     virtual void ProcessStartDelayComplete( TInt aError );

private: // new methods
           
    /**
    * Refreshes main view 
    * @param aError  Error code
    */              
    void RefreshL( TInt aError );
    
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
    * Shows update query dialog (now/later) 
    */ 
    void ShowUpdateQueryL();
    
    /**
    * Hides this application from fast swap window 
    *
    * @param aAlwaysAsk
    */ 
    void HideApplicationInFSWL( TBool aHide);
    
    /**
    * Shows global error note 
    *
    * @param aError Symbian error code
    */ 
    void ShowGlobalErrorNoteL( TInt aError ) const;
       
    /**
    * CIdle callback function, that shows status dialog
    * To be used to guarantee that possible old status dialog is totally 
    * removed by AVKON before showing new one  
    *
    * @param aPtr  Pointer to this instance
    */ 
    static TInt ShowStatusDialogCallbackL( TAny* aPtr );
    
    /**
    * CIdle callback function, that shows update query dialog
    * To be used because a client to be informed immediately that its 
    * async request is issued. Waiting dialog cannot be shown before responding to a client  
    *
    * @param aPtr  Pointer to this instance
    */
    static TInt UpdateQueryCallbackL( TAny* aPtr );
    
    /**
    * CIdle callback function, that performs automatic update check ann query
    *
    * @param aPtr  Pointer to this instance
    */
    static TInt AutomaticCheckCallbackL( TAny* aPtr );
    
    /**
    * History view is activated from a link in message query
    */
    void HandleTextLinkL();
            
private:  //data

    CIAUpdateMainView* iMainView;

    CIAUpdateHistoryView* iHistoryView;
        
    CIAUpdateUiController* iController;
    
    CIAUpdateGlobalLockHandler* iGlobalLockHandler;
    
    CIAUpdateUITimer* iForegroundTimer; 
    
    CIAUpdateUITimer* iBackgroundTimer;
    
    MIAUpdateRequestObserver* iRequestObserver;
    
    TIAUpdateDialogParam* iDialogParam;
    
    IAUpdateUiDefines::TIAUpdateUiRequestType iRequestType;
    
    CIdle* iIdle;
    
    CIdle* iIdleAutCheck;
           
    TBool iShowStatusDialogAgain;
    
    TUint iUpdatequeryUid;
    
    TBool iUpdateNow;
    
    TBool iInstUiOnTop;
    
    TBool iStartedFromApplication;
    
    TBool iClientMovedToBackground;
    
    RArray<TUint32> iDestIdArray;
    
    TInt iWgId;
    
    TBool iIsDMSupported;
    
    TBool iIsDMSet;
    
    TBool iUiRefreshAllowed;
    
    TBool iRebootAfterInstall;
    };


#endif // __IAUPDATE_APPUI_H__

