/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef C_CATALOGSHTTPSESSIONMANAGER_H
#define C_CATALOGSHTTPSESSIONMANAGER_H

#include "catalogshttpsessionmanager.h"
#include "catalogshttptypes.h"
#include "catalogsconnectionmethod.h"
#include "catalogsconnection.h"

class CCatalogsNetworkManager;

class CCatalogsHttpSessionManager : public CActive,
    public MCatalogsHttpSessionManager,
    public MCatalogsConnectionObserver,
    public MCatalogsConnectionStateObserver
    
    {
    public:
    
        /**
         * Maximum number of concurrently running operations.
         *
         */
        static const TInt KMaxConcurrentOperations = 8;
        
        /**
         * Minimum number of running downloads. In other words, there can
         * be only (KMaxConcurrentOperations - KMinDownloads) number of
         * running transactions
         */
        static const TInt KMinDownloads = 4;

        /** 
         * Minimum number of running transactions. In other words, there can
         * be only (KMaxConcurrentOperations - KMinTransactions) number of
         * running downloads
         */
        static const TInt KMinTransactions = 4;
        
    public:
    
        /**
        * NewL
        */
        static CCatalogsHttpSessionManager* NewL( );
        
        /**  
        * Destructor
        */
        ~CCatalogsHttpSessionManager();
        
    public: // MCatalogsHttpSessionManager


        void AddRef();
        TInt Release();


        /**
        * @see MCatalogsHttpSessionManager::StartOperation()
        */        
        TInt StartOperation( MCatalogsHttpOperation* aOperation, TBool aConnect );


        /**
        * @see MCatalogsHttpSessionManager::PauseOperation()
        */        
        TInt PauseOperation( MCatalogsHttpOperation* aOperation );
        
                
        /**
        * @see MCatalogsHttpSessionManager::CompleteOperation()
        */                
        TInt CompleteOperation( MCatalogsHttpOperation* aOperation );
    
    
        /**
        * @see MCatalogsHttpSessionManager::OperationPriorityChanged()
        */
        TInt OperationPriorityChanged( MCatalogsHttpOperation* aOperation );


        /**
         * Returns a connection creator
         */
        CCatalogsHttpConnectionCreator& ConnectionCreatorL();
        
        
        /**
         */
        void SetResumeMode( TBool aResumeQueued );
    

        static CCatalogsNetworkManager& NetworkManagerL();
        
        TBool DisconnectL();
        
    public:
    
        TInt ResumeOperationAction();

    public: // MCatalogsConnectionObserver
    
        void ConnectionCreatedL( 
            const TCatalogsConnectionMethod& aMethod );


        void ConnectionError( TInt aError );
        
    public: // MCatalogsConnectionStateObserver
    
        void ConnectionStateChangedL( TInt aStage );
        void ConnectionStateError( TInt aError );
    
        
    protected:        
    
        void SetConnectionToQueue( 
            const TCatalogsConnectionMethod& aMethod,  
            RCatalogsHttpOperationArray& aArray );
    
        

        void ReportConnectionError( 
            const TCatalogsConnectionMethod& aMethod,  
            RCatalogsHttpOperationArray& aArray,
            TInt aError );


        void ReportConnectionError( 
            RCatalogsHttpOperationArray& aArray,
            TInt aError );
    
    protected: // CActive
        
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );
        
    protected:
    
        /**
        * Constructor
        */
        CCatalogsHttpSessionManager();
        void ConstructL();
        
    private:
    
        void ResumeOperation();
    
        TInt AddToQueue( MCatalogsHttpOperation* aOperation );
        
        TInt RemoveFromQueue( MCatalogsHttpOperation* aOperation );        
        
        static TInt PrioritizeOperations( 
            const MCatalogsHttpOperation& aFirst, 
            const MCatalogsHttpOperation& aSecond );
            
        RCatalogsHttpOperationArray& ChooseArray( 
            const MCatalogsHttpOperation& aOperation );
        
        void UpdateRunningOperations();


        TBool ConnectL( MCatalogsHttpOperation& aOperation );       
        
        /**
         * @param aStartSwitchTimer If true, timer is started which is used
         * to raise a new AP even if the old one is still up
         */
        TBool StartMonitoringL( TBool aStartSwitchTimer );
        
    private:
    
        enum TCatalogsConnectionState 
            {
            ECatalogsConnectionDisconnected,
            ECatalogsConnectionConnecting,
            ECatalogsConnectionConnected,
            ECatalogsConnectionDisconnecting
            };
        
    private:
    
        RCatalogsHttpOperationArray iDownloadQueue;
        RCatalogsHttpOperationArray iTransactionQueue;
        RCatalogsHttpOperationArray iGeneralQueue; // Simple queue
        RCatalogsHttpOperationArray iRunningQueue; // All currently executing ops
        
        // currently executing operations, doesn't include the operation running
        // from general queue
        TInt iRunningOperations;    
        
        TInt iRunningDownloads;
        // Currently executing transactions        
        TInt iRunningTransactions;        
        TInt iRunningFromGeneral;  
        TInt iRefCount;
        TBool iResumeQueued;
        
        CCatalogsHttpConnectionCreator* iConnectionCreator; // owned
        CAsyncCallBack* iCallback; // owned
        TInt iResumeCount;
        
        static CCatalogsNetworkManager* iNetworkManager; // owned
        CCatalogsConnection* iConnection; // owned
        TCatalogsConnectionState iConnectionState;
        MCatalogsHttpOperation* iConnectedOperation; // not owned
        CCatalogsConnectionMonitor* iMonitor; // owned
        RTimer iConnectionTimer; // owned
        // ETrue if iConnectionTimer is being used for switching to another
        // APN
        TBool iSwitchApTimer;
        
        TCatalogsConnectionMethod iLatestConnectionMethod;
    };
    
#endif // C_CATALOGSHTTPSESSIONMANAGER_H    
