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
* Description:   This file contains the header file of the CIAUpdateManager class 
*
*/



#ifndef IA_UPDATE_MANAGER_H
#define IA_UPDATE_MANAGER_H


//INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <coemain.h>

#include "iaupdateclient.h"

//FORWARD DECLARATIONS
class CIAUpdateParameters;
class MIAUpdateObserver;
class CEikonEnv;




/**
 * CIAUpdateManager object provides methods for update actions.
 * Updating will be targeted to the update items that qualify the requirements
 * of CIAUpdateParameters objects. In asynchronous actions, the callback 
 * functions of MIAUpdateObserver objects will be informed about the progress 
 * of update actions.
 *
 * @see MIAUpdateObserver
 * @see CIAUpdateParameters
 * @see CIAUpdate
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CIAUpdateManager ) : public CActive, public MCoeForegroundObserver
    {

public:

    /**
     * These values inform what operation has been requested.
     *
     * @since S60 v3.2
     */
    enum TIAUpdateType
        {
        /**
         * No operation is going on.
         */ 
        EIAUpdateIdle,    
        /**
         * SWUpdate app start requested.
         */
        
        EIAUpdateStartServer,
        
        /**
         * Update check has been requested.
         */
        EIAUpdateCheck,

        /**
         * Update operation has been requested.
         */
        EIAUpdateUpdate,     
        
        /**
         * Update query has been requested.
         */
        EIAUpdateQuery   
        };


    /**
     * @param aObserver Callback functions of the observer are called
     * when operations progress.    
     * @return CIAUpdateManager* Pointer to the created CIAUpdateManager 
     * object that can be used for update actions.
     *
     * @since S60 v3.2
     */
    static CIAUpdateManager* NewL( MIAUpdateObserver& aObserver );
    
    /**
     * @see NewL
     *
     * @since S60 v3.2
     */
    static CIAUpdateManager* NewLC( MIAUpdateObserver& aObserver );
    

    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateManager();
    

    /** 
     * @see CIAUpdate::CheckUpdates
     *
     * @since S60 v3.2
     */
    void CheckUpdates( const CIAUpdateParameters& aUpdateParameters );
    
        
    /** 
     * @see CIAUpdate::ShowUpdates
     *
     * @since S60 v3.2
     */
    void ShowUpdates( const CIAUpdateParameters& aUpdateParameters );
    

    /** 
     * @see CIAUpdate::Update
     *
     * @since S60 v3.2
     */
    void Update( const CIAUpdateParameters& aUpdateParameters );
    
     /** 
     * @see CIAUpdate::UpdateQuery
     *
     * @since S60 v3.2
     */   
    void UpdateQuery();


protected: // CActive

    /**
     * @see CActive::DoCancel
     *
     * @since S60 v3.2
     */
    virtual void DoCancel();

    /**
     * When the server side has finished operation, the CActive object will
     * be informed about it, and as a result RunL will be called. This function
     * well inform the observer that the operation has been completed.
     * @see CActive::RunL
     *
     * @since S60 v3.2
     */
    virtual void RunL();


protected:

    /**
     * @see NewL
     *
     * @since S60 v3.2
     */
    CIAUpdateManager( MIAUpdateObserver& aObserver );
    
    /**
     * @see NewL
     *
     * @since S60 v3.2
     */
    virtual void ConstructL();


    /**
     * Resets all the result variables to their default values.
     *
     * @since S60 v3.2
     */
    virtual void ResetResults();


    /**
     * @return ETrue if an operation is going on. Else EFalse.
     *
     * @since S60 v3.2
     */
    TBool BusyCheck() const;


    /**
     * @return RIAUpdateClient& Handles the client server communication.
     *
     * @since S60 v3.2
     */
    RIAUpdateClient& UpdateClient();


    /**
     * @return MUpdateObserver& Observer who will be informed about the
     * completion of the operations.
     *
     * @since S60 v3.2
     */
    MIAUpdateObserver& Observer() const;


    /**
     * @return TIAUpdateType Informs what kind of operation is going on.
     *
     * @since S60 v3.2
     */
    TIAUpdateType UpdateType() const;
    
    /**
     * @param aUpdateType Informs what kind of operation is going on.
     *
     * @since S60 v3.2
     */
    void SetUpdateType( TIAUpdateType aUpdateType );


private:

    // Prevent these if not implemented
    CIAUpdateManager( const CIAUpdateManager& aObject );
    CIAUpdateManager& operator =( const CIAUpdateManager& aObject );
    
private: // Functions from MCoeForegroundObserver

	/**
	* Handles the application coming to the foreground.
	*/
	void HandleGainingForeground();

	/**
	* Handles the application going into the background.
	*/
	void HandleLosingForeground();
	
private:// new functions
	
	void CheckUpdatesContinue();
	
	void CopyUpdateParamsL( const CIAUpdateParameters& aUpdateParameters );

private: // data

	CIAUpdateParameters* iUpdateParameters;
	
    // Observer who will be informed about the completion of the operations. 
    MIAUpdateObserver& iObserver;    
    
    // Handles the client server communication.
    RIAUpdateClient iUpdateClient;

    // Informs what kind of operation is going on.
    TIAUpdateType iUpdateType;

    // This is used to storage the success count values for update operations.
    TInt iSuccessCount;
    
    // This is used to storage the fail count values for update operations.
    TInt iFailCount;
    
    // This is used to storage the cancel count values for update operations.
    TInt iCancelCount;

    TBool iUpdateNow;
    
    CEikonEnv* iEikEnv; //not owned

    };

#endif // IA_UPDATE_IMPL_H
