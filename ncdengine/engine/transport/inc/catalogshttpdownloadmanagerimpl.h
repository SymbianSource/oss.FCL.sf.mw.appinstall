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


#ifndef C_CATALOGSHTTPDOWNLOADMANAGER_H
#define C_CATALOGSHTTPDOWNLOADMANAGER_H

// #include <DownloadMgrClient.h>

#include "catalogstransportoperationid.h"
#include "catalogshttpsession.h"    // RCatalogsHttpOperationArray
#include "catalogshttpsessionmanager.h"

#include "catalogsconnectionmethod.h"
#include "catalogsaccesspointobserver.h"
#include <download.h>
#include <downloadmanager.h>
#include <QObject>

#include <f32file.h> //HLa

class MCatalogsHttpConfig;
class CCatalogsHttpConfig;
class CCatalogsHttpDownload;
class MCatalogsHttpOperation;
class MCatalogsHttpObserver;
class MCatalogsHttpSessionManager;
class MCatalogsHttpTransactionManager;
class CCatalogsHttpConnectionManager;
class CCatalogsHttpSession;
class CCatalogsConnection;
class CCatalogsNetworkManager;
class CCatalogsHttpQTDownloadManager;
using namespace WRT;

const TInt KNCDEngineAppID = 0X2002E685;


/**
 * HTTP file download manager
 *
 */
class CCatalogsHttpDownloadManager :       

    public CActive, 
    public MCatalogsAccessPointObserver  
    {
  
    public: // Constructors and destructor
    
        /**
        * Creator
        *
        * @param aSessionId Session ID
        * @return A new download manager
        */
        static CCatalogsHttpDownloadManager* NewL( 
            MCatalogsHttpSessionManager& aManager, 
            CCatalogsHttpSession& aSession,
            TInt32 aSessionId,
            MCatalogsHttpTransactionManager& aTransactionManager,
            CCatalogsHttpConnectionManager& aConnectionManager,
            TBool aCleanup );
        
        
        /**
        * Destructor
        */
        virtual ~CCatalogsHttpDownloadManager();
         
    public: // New methods
    
	    /**
	    * Creates a new download operation
	    * 
	    * @param aUrl URL to download
	    * @param aStart If true, download is started automatically. Otherwise
	    * the caller must start it explicitly. Default: true
	    * @param aObserver Observer for the download. Must be given if the 
	    * default observer in the session configuration is NULL. Default: NULL
	    * 
	    * @return A new download operation
	    */
        MCatalogsHttpOperation* CreateDownloadL( const TDesC16& aUrl,
            TBool aStart, MCatalogsHttpObserver* aObserver );

        MCatalogsHttpOperation* CreateDownloadL( const TDesC8& aUrl,
            TBool aStart, MCatalogsHttpObserver* aObserver );
        
        /**
        * Download getter
        *
        * Returns the download that matches the id or NULL
        * 
        * @param aId Download ID
        * @return Download or NULL
        */
        MCatalogsHttpOperation* Download( 
            const TCatalogsTransportOperationId& aId ) const;
        
        
        /**
        * Returns a list of current downloads
        * @return Download list
        */
        const RCatalogsHttpOperationArray& CurrentDownloads() const;
        
        
        /**
        * Returns an array of downloads restored from the previous session. 
        *
        * The client should always call this method and either delete the 
        * downloads or restore them after setting the observer for them.
        *
        * @return Array of restored downloads
        */
        const RCatalogsHttpOperationArray& RestoredDownloads() const;
        
        
        /**
        * Cancels all downloads
        */
        void CancelAll();
                        
        
        /**
         * 
         */                
        void NotifyCancelAll();

        /**
         * Deletes restored downloads
         */
        void DeleteRestoredDownloads();
        
        /**
        * Default download configuration getter
        *
        * @return Default download configuration
        */
        MCatalogsHttpConfig& DefaultConfig() const;
        
        
        /**
         * Moves a download from restored downloads to current downloads
         *
         * @param aDownload Download to move
         * @return KErrNone if successful, otherwise a Symbian error code
         * @note A restored download is moved to current downloads
         * when it is started but this method can be used to move it there
         * earlier.
         */
        TInt MoveRestoredDlToCurrentDls( 
            MCatalogsHttpOperation& aDownload );
        
    public: 

        void AddRef();

		DownloadManager* GetDownloadManager();
        TInt Release();    
        
        void SetConnectionMethodL( 
            const TCatalogsConnectionMethod& aMethod );


        void SetConnectionL(
            CCatalogsConnection& aConnection );
            
        TInt32 SessionId() const;
    
        
        void RemoveDownload( MCatalogsHttpOperation* aDownload );
    
    
        MCatalogsHttpOperation* CreateDlTransactionL( 
            const TDesC8& aUrl, 
            MCatalogsHttpObserver& aObserver,
            const CCatalogsHttpConfig& aConfig );
    
    
        TInt StartOperation( MCatalogsHttpOperation* aOperation );

        TInt PauseOperation( MCatalogsHttpOperation* aOperation );
                
        TInt CompleteOperation( MCatalogsHttpOperation* aOperation );
    
        
        void ReportConnectionStatus( TBool aActive );
        
        TInt OperationPriorityChanged( MCatalogsHttpOperation* aOperation );
        
        
        CCatalogsHttpConnectionCreator& ConnectionCreatorL();
    

        CCatalogsHttpConnectionManager& ConnectionManager();
        
        TInt NewDownloadId();
        
       WRT::Download& CreatePlatformDownloadL( const TDesC8& aUrl );
      void downloadMgrEventRecieved(DownloadManagerEvent* dlmEvent);



    public: // from MCatalogsAccessPointObserver
    
        virtual void HandleAccessPointEventL( 
            const TCatalogsConnectionMethod& aAp,
            const TCatalogsAccessPointEvent& aEvent );
    
    
    protected: // CActive
    
        virtual void RunL();
        virtual void DoCancel();
        virtual TInt RunError( TInt aError );
                    
    private: // Constructors
    
        /**
        * Constructor
        */        
        CCatalogsHttpDownloadManager( 
            MCatalogsHttpSessionManager& aManager, 
            CCatalogsHttpSession& aSession,
            TInt32 aSessionId, 
            MCatalogsHttpTransactionManager& aTransactionManager,
            CCatalogsHttpConnectionManager& aConnectionManager );
        
        /**
        * 2nd phase constructor
        */
        void ConstructL( TBool aCleanup );
        
        
    private:
    
        /**
        * Restores downloads from previous session
        */
        void RestoreDownloadsL();
        
        
        /**
        * Searches for the given download from the given array and returns
        * an index to it or KErrNotFound.
        *
        * @param aArray Array to search from
        * @param aDownload Download to find
        * @return Index to the download in iDownloads
        */
        /* HLa
        TInt FindInDownloads( 
            const RCatalogsHttpOperationArray& aArray,
            RHttpDownload* aDownload ) const;
        */

        /**
        * Searches for the given download from the given array and returns
        * an index to it or KErrNotFound.
        *
        * @param aArray Array to search from
        * @param aDownload Download to find
        * @return Index to the download in iDownloads
        */      
        TInt FindInDownloads( 
            const RCatalogsHttpOperationArray& aArray,
            MCatalogsHttpOperation* aDownload ) const;
 

    private:
    
        /**
         * Download events
         */
        /* HLa
        class TDownloadEvent        
            {
        public:
            RHttpDownload* iDownload;   // not owned
			THttpDownloadEvent iEvent;
			
			TDownloadEvent( RHttpDownload& aDownload, 
			    THttpDownloadEvent& aEvent ) :
			    iDownload( &aDownload ), iEvent( aEvent ) 
			        {
			        }
            };
          */

        /**
         * Sets this object active if event queue has unhandled events
         */
        void ContinueEventHandling();
        
        /**
         * Removes events from event queue that belong to the same 
         * RHttpDownload as aEvent
         */
        /*
        void RemoveUnhandledEvents( 
            const TDownloadEvent& aEvent );

        void RemoveUnhandledProgressEvents( 
            const TDownloadEvent& aEvent );
        */
        
        /**
         * Deletes downloads that have not been paused by the user
         */
        void DeleteHangingDownloads();

    private:
    
        MCatalogsHttpSessionManager& iManager;
        CCatalogsHttpSession& iSession;
        MCatalogsHttpTransactionManager& iTrManager;
        CCatalogsHttpConnectionManager& iConnectionManager;
        CCatalogsNetworkManager* iNetworkManager; // Not owned
        TInt32 iSessionId;
        DownloadManager *iDmgr;
        CCatalogsHttpQTDownloadManager* iQTmgr;
        WRT::Download* iDownload;           // Platform download
        // All downloads except those that are in Restored
        RCatalogsHttpOperationArray iDownloads;
        RCatalogsHttpOperationArray iRestoredDownloads;
                
        CCatalogsHttpConfig* iDefaultConfig;    // owned
        TBool iCancellingAll;
        
        TInt iRefCount;
        
        RFs iFs;
        TCatalogsConnectionMethod iCurrentAp;

        // RArray<TDownloadEvent> iEventQueue;  //HLa
        TInt iCurrentDlId; // id of the last created download
    };



class  CCatalogsHttpQTDownloadManager: public QObject
	{
		 Q_OBJECT
		 	public:
		 		CCatalogsHttpQTDownloadManager(CCatalogsHttpDownloadManager* aDownloadManager,DownloadManager* aDmgr);
	    public slots:
    	void downloadMgrEventRecieved(DownloadManagerEvent*);
	    public:
	    	CCatalogsHttpDownloadManager* iDownloadManager;
	    	DownloadManager* iDmgr ;
	};
	
	#endif // C_CATALOGSHTTPDOWNLOADMANAGER_H