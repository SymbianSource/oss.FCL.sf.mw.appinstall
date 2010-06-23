/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdInstallOperationProxy declaration
*
*/


#ifndef C_NCDINSTALLOPERATIONPROXY_H	
#define C_NCDINSTALLOPERATIONPROXY_H	

#include <f32file.h>

#include "ncdoperationimpl.h"
#include "ncdbaseoperationproxy.h"
#include "ncdinstalloperation.h"
#include "ncdoperationdatatypes.h"
#include "ncdinstallationserviceobserver.h"
#include "ncditempurpose.h"

class MNcdInstallationService;
class MNcdInstallOperationObserver;
class CNcdNodeProxy;
class CNcdInstallInfo;
class CNcdFileInfo;

class CNcdInstallOperationProxy : 
    public CNcdOperation< MNcdInstallOperation >,
    public MNcdInstallationServiceObserver
    {
public:

    /**
     * Constructor
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aDownloadType Download type
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */    
    static CNcdInstallOperationProxy* NewLC( MCatalogsClientServer& aSession,                        
                        TInt aHandle,
                        MNcdOperationProxyRemoveHandler* aRemoveHandler,
                        CNcdNodeProxy* aNode,
                        CNcdNodeManagerProxy* aNodeManager,
                        MNcdInstallOperationObserver* aObserver,
                        MNcdInstallationService& aInstallationService );

        
public: // From MNcdInstallOperation
    
    /**
     * @see MNcdInstallOperation::FileCount()
     */
    TInt FileCount();
    
    /**
     * @see MNcdInstallOperation::CurrentFile()
     */
    TInt CurrentFile();    


public: // From MNcdOperation

    /**
     * @see MNcdOperation::OperationType();
     */
    void StartOperationL();

   /**
     * @see MNcdOperation::CancelOperation()
     */
    void CancelOperation();

    
    /**
     * @see MNcdOperation::OperationType()
     */
    TNcdInterfaceId OperationType() const;

    
protected: // Constructor and destructor

    /**
     * Constructor
     *
     */
    CNcdInstallOperationProxy( MNcdInstallationService& aInstallationService );
    
    virtual ~CNcdInstallOperationProxy();


    /**
     * ConstructL
     *
     * @param aSession is the session that is used between
     *                 the proxy and the server.
     * @param aHandle is the handle that identifies the serverside
     *                object that this proxy uses.
     * @param aRemoveHandler An observer that is called during destruction.
     * @param aNode The node this operation was started from.
     */
    void ConstructL( MCatalogsClientServer& aSession,
                    TInt aHandle,
                    MNcdOperationProxyRemoveHandler* aRemoveHandler,
                    CNcdNodeProxy* aNode,
                    CNcdNodeManagerProxy* aNodeManager,
                    MNcdInstallOperationObserver* aObserver );
    
    
protected: // From CNcdBaseOperationProxy

    /**
     * @see CNcdBaseOperationProxy::ProgressCallback()
     */
    void ProgressCallback();
    
    
    /**
     * @see CNcdBaseOperationProxy::QueryReceivedCallback()
     */
    void QueryReceivedCallback( CNcdQuery* aQuery );
    
    
    /**
     * @see CNcdBaseOperationProxy::CompleteCallback()
     */
    void CompleteCallback( TInt aError );
    

    /**
     * @see CNcdBaseOperationProxy::CreateInitializationBufferL()
     */    
    HBufC8* CreateInitializationBufferL();
    
    
    /**
     * @see CNcdBaseOperationProxy::InitializationCallback()    
     */
    void InitializationCallback( RReadStream& aReadStream, 
        TInt aDataLength );    


protected: // MNcdInstallationServiceObserver

    /**
     * @see MNcdInstallationServiceObserver::InstallationCompleteL()
     */
    void InstallationCompleteL( const TDesC& aFileName,
                                const TUid& aAppUid,
                                TInt aError );

protected: // CActive

    /**
     * @see CActive::RunL
     */
    void RunL();

    /**
     * @see CActive::DoCancel
     */    
    void DoCancel();
    
    /**
     * @see CActive::RunError
     */
    TInt RunError( TInt aError );

protected:

    /**
     * This function uses the correct installer functions that are class
     * specific. Child classes may have their own implementations that
     * are automatically used when installation is handled.
     * 
     * @param aFile The file that is going to be installed.
     */ 
    virtual void UseInstallServiceL( const CNcdFileInfo& aFile );

    /** 
     * @return MNcdInstallationService& Service that is used to
     * handle the installation of the files.
     */
    MNcdInstallationService& Installer();
    
    /**
     * @return CNcdInstallInfo& File(s) currently being installed.
     */
    CNcdInstallInfo& CurrentInfo();


    /**
     * Updates the file's information to the server side.
     * This should be called only after succesfull installation
     * because iCurrentFile count is increased by one.
     *
     * For parameter descriptions:
     * @see MNcdInstallationServiceObserver::InstallationCompleteL
     */
    void UpdateInfoToServerL( const TDesC& aFileName,
                              const TUid& aAppUid,
                              TInt aError );


    /**
     * @aErrorCode Current error code for the installation operation.
     */
    void SetInstallationErrorCode( const TInt& aErrorCode );
    
    /**
     * @return TInt Current error code for the installation operation.
     */
    TInt InstallationErrorCode() const;


    /**
     * Inform server side install report that the installing has been started.
     */
    void StartInstallReportL();
    
    
    /**
     * Inform server side install report that the installing has been completed
     * with the given error code.
     */
    void CompleteInstallReportL( TInt aErrorCode );


    /**
     * Deletes the current content file that has been set for the installation.
     */ 
    void DeleteCurrentFileL();
    
                              
private: // new methods

    void InstallNextFileL();
    CNcdInstallInfo* InstallInfoL( TInt aIndex );
    
    RFile OpenFileL( TInt aFileIndex );
    
    void DeleteFileL( TInt aFileIndex );

protected: // data
        
    RFile iFileHandle; // file that is currently being installed    
    
private: // data

    /**
     * Operation observer, for callbacks.
     * Observer not own.
     */    
    MNcdInstallOperationObserver* iObserver;        
    
    // Installer, not own
    MNcdInstallationService* iInstaller;
    
    // Total number of files      
    TInt iFileCount;
    
    // Number of the currently downloaded file
    TInt iCurrentFile;                
    
    RPointerArray<HBufC> iMimeTypes;
    RArray<TNcdItemPurpose> iPurposes;
    
    // File(s) currently being installed
    CNcdInstallInfo* iCurrentInfo;
    
    TInt iInstallationError;        
        
    };

#endif // C_NCDINSTALLOPERATIONPROXY_H
