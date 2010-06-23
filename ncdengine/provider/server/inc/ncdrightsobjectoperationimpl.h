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
* Description:   ?Description
*
*/


#ifndef C_NCD_RIGHTS_OBJECT_OPERATION_H
#define C_NCD_RIGHTS_OBJECT_OPERATION_H

#include "ncdbaseoperation.h"
#include "ncdoperationobserver.h"
#include "catalogsconnectionmethod.h"

class MNcdInstallationService;
class CNcdDescriptorDownloadSubOperation;
class TNcdConnectionMethod;

/** 
 * DRM rights object download & install operation
 *
 */
class CNcdRightsObjectOperation : public CNcdBaseOperation
    {
public:
    
    /**
     * NewL
     *
     * @param aHttpSession HTTP session used for creating download operations
     *
     * @note Preferably the HTTP session has suitable defaults but if not then
     * Config() can be used to retrieve and change the settings for the download.
     * @note At least the access point must be set to the configuration. Target path
     *  is ignored (rights object is downloaded to memory only).
     */
    static CNcdRightsObjectOperation* NewL(
        CNcdGeneralManager& aGeneralManager,
        const TDesC& aDownloadUri,
        const TDesC& aMimeType,
        const TNcdConnectionMethod& aMethod,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSession& aSession );

    
    /**
     * Destructor
     */
    ~CNcdRightsObjectOperation();
    
public: // New methods

    /** 
     * HTTP configuration getter
     */
    MCatalogsHttpConfig& Config();
    
    
public: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel()
     */
    void Cancel();
        

public:  // from MNcdOperationObserver

    void Progress( CNcdBaseOperation& aOperation );
    
    void QueryReceived( CNcdBaseOperation& aOperation,
                        CNcdQuery* aQuery );

    void OperationComplete( CNcdBaseOperation* aOperation,
                            TInt aError );

protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    
    
protected:

    // Constructor
    CNcdRightsObjectOperation(
        CNcdGeneralManager& aGeneralManager,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSession& aSession );
        
    void ConstructL(
        const TNcdConnectionMethod& aMethod,
        const TDesC& aDownloadUri,
        const TDesC& aMimeType );
    
        
private:

    MCatalogsHttpSession& iHttpSession;    

    MNcdInstallationService* iInstallationService;
    CNcdDescriptorDownloadSubOperation* iDownloadOp;

    HBufC8* iMimeType;
    TCatalogsConnectionMethod iConnectionMethod;
    };

#endif // C_NCD_RIGHTS_OBJECT_OPERATION_H
