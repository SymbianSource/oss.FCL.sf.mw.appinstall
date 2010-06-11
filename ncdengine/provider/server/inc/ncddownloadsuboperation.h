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


#ifndef C_NCDDOWNLOADSUBOPERATION_H
#define C_NCDDOWNLOADSUBOPERATION_H

#include <e32base.h>

#include "ncdbaseoperation.h"
#include "ncdoperationdatatypes.h"
#include "ncddownloadoperationstates.h"
#include "catalogshttptypes.h"
#include "catalogshttpobserver.h"
#include "ncdoperationobserver.h"

class RWriteStream;
class RReadStream;

class MCatalogsHttpSession;
class MCatalogsHttpConfig;
class MCatalogsHttpOperation;
class MCatalogsHttpHeaders;
class CNcdGeneralManager;

/** 
 * Download operation
 *
 * @ Add username & password -handling
 * @ Add observers if this is used by other operations
 */
class CNcdDownloadSubOperation : public CNcdBaseOperation,
    public MCatalogsHttpObserver
    {
public:
    
    /**
     * NewL
     *
     * @param aHttpSession HTTP session used for creating download operations
     * @param aDestination Destination directory if the descriptor ends in '\'
     * otherwise full destination path (path + filename)
     *
     * @note Preferably the HTTP session has suitable defaults but if not then
     * Config() can be used to retrieve and change the settings for the download.
     * @note At least the target directory and access point must be set to the 
     * configuration.
     *    
     */
    static CNcdDownloadSubOperation* NewL( 
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        const TDesC& aUri,
        const TDesC& aDestination,
        MNcdOperationObserver& aObserver,
        MCatalogsSession& aSession );


    /**
     * NewL
     *
     * Internalizes a download operation from a stream
     */
    static CNcdDownloadSubOperation* NewL( 
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationObserver& aObserver,
        RReadStream& aStream,
        MCatalogsSession& aSession );
        
    
    /**
     * Destructor
     */
    virtual ~CNcdDownloadSubOperation();
    
public: // New methods

    /** 
     * HTTP configuration getter. 
     */
    MCatalogsHttpConfig& Config();
    
    /**
     * HTTP request headers getter
     */
    MCatalogsHttpHeaders& RequestHeaders();
    
    const MCatalogsHttpHeaders& ResponseHeadersL();
    
    /**
     * Download operation getter
     */
    MCatalogsHttpOperation& HttpOperation();
    
    
public: // From CNcdBaseOperation

    /**
     * Starts the operation.
     */
    TInt Start();


    /**
     *
     */
    TInt Pause();
    
    
    TInt Resume();
    
    /**
     * @see CNcdBaseOperation::Cancel()
     */
    void Cancel();    
    

public: // From MCatalogsHttpObserver

    /**
     * @see MCatalogsHttpObserver::HandleHttpEvent()
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


public:
       
    /**
     * Externalize
     *
     * @param aStream Target stream
     */ 
    void ExternalizeL( RWriteStream& aStream ) const;
    
    
    /**
     * Internalize
     *
     * @param aStream Source stream
     */
    void InternalizeL( RReadStream& aStream ); 


protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    
    
protected:

    // Constructor
    CNcdDownloadSubOperation(  
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSession& aSession );
        
    void ConstructL( 
        const TDesC& aUri, 
        const TDesC& aDestination,
        MNcdOperationObserver& aObserver );


    void ConstructL( 
        MNcdOperationObserver& aObserver );

private:

    void NotifyObserversComplete( TInt aError );
    
    void NotifyObserversProgress();    
        
private:

    MCatalogsHttpSession& iHttpSession;
    MCatalogsHttpOperation* iDownload;
    TNcdDownloadState iDownloadState;        
    };

#endif // C_NCDDOWNLOADSUBOPERATION_H
