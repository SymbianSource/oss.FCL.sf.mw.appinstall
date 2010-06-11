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


#ifndef C_NCDDESCRIPTORDOWNLOADSUBOPERATION_H
#define C_NCDDESCRIPTORDOWNLOADSUBOPERATION_H

#include <e32base.h>

#include "ncdbaseoperation.h"
#include "ncdoperationdatatypes.h"
#include "ncddownloadoperationstates.h"
#include "catalogshttptypes.h"
#include "catalogshttpobserver.h"
#include "ncdoperationobserver.h"

class MCatalogsHttpSession;
class MCatalogsHttpConfig;
class MCatalogsHttpOperation;
class MCatalogsHttpHeaders;
class CNcdGeneralManager;

/** 
 * Download operation
 *
 */
class CNcdDescriptorDownloadSubOperation : public CNcdBaseOperation,
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
    static CNcdDescriptorDownloadSubOperation* NewL( 
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        const TDesC& aUri,
        MNcdOperationObserver& aObserver,
        MCatalogsSession& aSession );
        
    
    /**
     * Destructor
     */
    virtual ~CNcdDescriptorDownloadSubOperation();
    
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
    
    
    /**
     * Returns the response body
     */
    const TDesC8& Body() const;
    
    
public: // From CNcdBaseOperation

    /**
     * Starts the operation.
     */
    TInt Start();

    
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

protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    
    
protected:

    // Constructor
    CNcdDescriptorDownloadSubOperation(  
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSession& aSession );
        
    void ConstructL( 
        const TDesC& aUri, 
        MNcdOperationObserver& aObserver );

private:

    void NotifyObserversComplete( TInt aError );
    
    void NotifyObserversProgress();    
        
private:

    MCatalogsHttpSession& iHttpSession;
    MCatalogsHttpOperation* iDownload;
    TNcdDownloadState iDownloadState;    
    
    CBufFlat* iBody;
    mutable TPtr8 iBodyPtr;    
    };

#endif // C_NCDDESCRIPTORDOWNLOADSUBOPERATION_H
