/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_NCDDOWNLOADOPERATION_H
#define C_NCDDOWNLOADOPERATION_H

#include "ncdbaseoperation.h"
#include "ncdoperationdatatypes.h"
#include "ncddownloadoperationstates.h"
#include "catalogshttptypes.h"
#include "catalogshttpobserver.h"

class MCatalogsHttpSession;
class MCatalogsHttpConfig;
class MCatalogsHttpOperation;
class CNcdNodeManager;
class MNcdDownloadHandler;
class MNcdFileHandler;
class MNcdStorageClient;
class CNcdNodeIdentifier;
class MNcdSessionHandler;
class MNcdConfigurationManager;
class MCatalogsContext;
class MCatalogsAccessPointManager;
class MNcdOperationRemoveHandler;
class CNcdGeneralManager;

/** 
 * Download operation
 *
 * @ Add username & password -handling
 * @ Add observers if this is used by other operations
 */
class CNcdDownloadOperation : public CNcdBaseOperation,
    public MCatalogsHttpObserver
    {
public:
    
    /**
     * NewL
     *
     * @param aHttpSession HTTP session used for creating download operations
     *
     * @note Preferably the HTTP session has suitable defaults but if not then
     * Config() can be used to retrieve and change the settings for the download.
     * @note At least the target directory and access point must be set to the 
     * configuration.
     */
    static CNcdDownloadOperation* NewL( 
        MNcdOperationRemoveHandler& aRemoveHandler,
        TNcdDownloadDataType aType, 
        const CNcdNodeIdentifier& aNodeId,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdSessionHandler* aSessionHandler,
        MNcdStorageClient* aStorageClient,
        const TUid& aClientUid,
        TInt aDownloadIndex,
        MCatalogsSession& aSession );

    
    /**
     * Destructor
     */
    ~CNcdDownloadOperation();
    
public: // New methods

    /** 
     * HTTP configuration getter
     */
    MCatalogsHttpConfig& Config();
    
    /**
     * Returns the identifier of the node that this operation relates to.
     *
     * @return Node identifier.
     */
    const CNcdNodeIdentifier& NodeIdentifier() const;
    
    
public: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel()
     */
    void Cancel();    
    
    
    /**
     * @see CNcdBaseOperation::ReceiveMessage()
     */
    void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );    


public: // New methods

    TBool MatchDownload( const CNcdNodeIdentifier& aId, 
        TNcdDownloadDataType& aType, TInt aIndex ) const;


public: // From MCatalogsHttpObserver

    /**
     * @see MCatalogsHttpObserver::HandleHttpEventL()
     */
    void HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent );
        
    /**
     * @see MCatalogsHttpObserver::HandleHttpError()
     */
    TBool HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError );

protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    

    /**
     * @see CNcdBaseOperation::Initialize()
     */    
    TInt Initialize();
    
protected:

    // Constructor
    CNcdDownloadOperation( 
        MNcdOperationRemoveHandler& aRemoveHandler,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession, 
        MNcdSessionHandler* aSessionHandler,
        TNcdDownloadDataType aType,
        const TUid& aClientUid,
        MCatalogsSession& aSession );
        
    void ConstructL( const CNcdNodeIdentifier& aNodeId, 
        TInt aDownloadIndex,
        MNcdStorageClient* aStorageClient = NULL );
    
protected:

    // Update the download request headers
    void UpdateHeadersL( const MCatalogsContext& aContext );
    
    void GetPausableStateL( MCatalogsBaseMessage& aMessage );
        
private:

    MCatalogsHttpSession& iHttpSession;
    MNcdSessionHandler* iSessionHandler;
    MNcdConfigurationManager& iConfigurationManager;
    MCatalogsAccessPointManager& iAccessPointManager;
    TNcdDownloadDataType iType;
    MCatalogsHttpOperation* iDownload;
    MNcdDownloadHandler* iDownloadHandler;
    MNcdFileHandler* iFileHandler;
    TNcdDownloadState iDownloadState;    
    HBufC* iSessionId;
    TUid iClientUid;  
    TBool iUnhandledEvent;  
    TInt iTotalFileCount;
    TInt iCurrentFile;
    };

#endif // C_NCDDOWNLOADOPERATION_H
