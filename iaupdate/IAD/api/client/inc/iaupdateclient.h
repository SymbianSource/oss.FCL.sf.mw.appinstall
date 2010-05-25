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
* Description:   This file contains the header file of the RIAUpdateClient class 
*
*/



#ifndef IA_UPDATE_CLIENT_H
#define IA_UPDATE_CLIENT_H

#include <AknServerApp.h> 
#include <e32std.h>
#include <e32cmn.h>

class CIAUpdateParameters;

/**
 * RIAUpdateClient object provides methods to delegate update actions
 * to the server side. Updating will be targeted to the update items that 
 * qualify the requirements of CIAUpdateParameters objects. In asynchronous 
 * actions, observers will be informed about the completion of update action.
 * 
 * Because this object is R-class, it does not provide constructor or destructor.
 * Usage:
 * - First, call Open-function.
 * - Second, use update functions to start update.
 * - Finally, user has to call Close-function to terminate object. 
 *   Otherwise, memory leaks may occur.
 *
 * @see CIAUpdateParameters
 * @see CIAUpdate
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( RIAUpdateClient ) : public RAknAppServiceBase 
    {

public:

    /**
     * Constructor.
     */
     
     RIAUpdateClient();

    /** 
     * This function will open the update action.
     *
     * @note This has to be called before calling operation functions.
     *
     * @return System wide error code.
     *
     * @since S60 v3.2
     */
    TInt Open();
    
    /** 
     * Close releases the resources allocated by this class object.
     * After Close is called, this R-class object may go out of scope.
     *
     * @note If Close is called before asynchronous operation has finished, 
     * the operation will continue but callback functions of the observer
     * will not be called when operation progresses. 
     * 
     * @since S60 v3.2
     */
    TInt OpenToBackroundAsync( TRequestStatus& aStatus );

    TInt ConnectToApp();
   
    void Close();


    /** 
     * @see CIAUpdate::CheckUpdates
     *
     * @note The client server connection has to be opened by calling
     * Open function before this function can be called.
     *
     * @param aAvailableUpdates Number of the updates that were found available.
     * @param aStatus The status will be updated when the operation
     * has been completed.
     * @return Informs if the initialization of server request failed.
     *         System wide error code. 
     *
     * @since S60 v3.2
     */
    TInt CheckUpdates( const CIAUpdateParameters& aUpdateParameters,
                       TInt& aAvailableUpdates,
                       TRequestStatus& aStatus );
    
        
    /** 
     * @see CIAUpdate::ShowUpdates
     *
     * @note The client server connection has to be opened by calling
     * Open function before this function can be called.
     *
     * @param aNumberOfSuccessfullUpdates Number of the successfull updates 
     *                                    that were done during the update.
     * @param aNumberOfFailedUpdates Number of the failed updates 
     *                               that were done during the update.
     * @param aNumberOfCancelledUpdates Number of the cancelled updates 
     *                                  during the update.     
     * @param aStatus The status will be updated when the operation
     * has been completed.
     * @return Informs if the initialization of server request failed.
     *         System wide error code. 
     *
     * @since S60 v3.2
     */
    TInt ShowUpdates( const CIAUpdateParameters& aUpdateParameters,
                      TInt& aNumberOfSuccessfullUpdates,
                      TInt& aNumberOfFailedUpdates,
                      TInt& aNumberOfCancelledUpdates,
                      TRequestStatus& aStatus );
    

        
                 
    /** 
     * @see CIAUpdate::Update
     *
     * @note The client server connection has to be opened by calling
     * Open function before this function can be called.
     *
     * @param aUpdateNow  Value depending on a user's choice
     * @param aStatus The status will be updated when the operation
     * has been completed.
     * @return Informs if the initialization of server request failed.
     *         System wide error code. 
     *
     * @since S60 v3.2
     */
    TInt UpdateQuery( TBool& aUpdateNow, TRequestStatus& aStatus );
                      
    
    void BroughtToForeground();
    
    
    /**
     * Cancels the ongoing asynchronous operation.
     * The active object that has started the update operation
     * will be informed when the requested update operation 
     * has been cancelled.
     *
     * @since S60 v3.2
     */             
    void CancelAsyncRequest();


public: // RAknAppServiceBase

    /**
     * @see RAknAppServiceBase::ServiceUid
     *
     * @since S60 v3.2
     */
    TUid ServiceUid() const;


private:
    
    // These functions send the operation requests to the server.
    
    TInt SendCheckUpdatesRequest( TInt aUpdateFunction,
                                  const CIAUpdateParameters& aUpdateParameters,
                                  TInt& aCount,
                                  TRequestStatus& aStatus );

    TInt SendUpdateRequest( TInt aUpdateFunction,
                            const CIAUpdateParameters& aUpdateParameters,
                            TInt& aNumberOfSuccessfullUpdates,
                            TInt& aNumberOfFailedUdpdates,
                            TInt& aNumberOfCancelledUpdates,
                            TRequestStatus& aStatus );
    
    void ConnectNewAppToBackgroundL( TUid aAppUid );
    
    void StartNewAppToBackgroundL( TUid aAppUid, TRequestStatus& aStatus ); 
    
    void ServerName(TName& aServerName, TUid aAppServerUid, TUint aServerDifferentiator);
    
    TUint GenerateServerDifferentiatorAndName(TName& aServerName, TUid aAppServerUid);
    
                               
private: //data

    HBufC8* iData;
    
    TBool iConnected;
    
    TInt iOwnWgId;
    
    TUint iDifferentiator;
    
    TPtr8 iPtr1;
    TPtr8 iPtr2;
    TPtr8 iPtr3;  
    };

#endif // IA_UPDATE_CLIENT_H
