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


#ifndef C_NCDINSTALLOPERATION_H
#define C_NCDINSTALLOPERATION_H

#include <e32cmn.h>
#include "ncdbaseoperation.h"
#include "ncdoperationdatatypes.h"
#include "ncdreportdefines.h"

class CNcdNodeManager;
class MNcdStorageClient;
class CNcdNodeIdentifier;
class MCatalogsContext;
class CNcdFileInfo;
class MNcdOperationRemoveHandler;
class MNcdPurchaseDetails;
class MNcdPurchaseInstallInfo;
class MCatalogsHttpSession;
class MCatalogsAccessPointManager;
class MNcdInstallReportObserver;
class MNcdInstallationService;

/** 
 * Install operation
 *
 */
class CNcdInstallOperation : public CNcdBaseOperation
    {
public:
    
    /**
     * NewL
     *
     */
    static CNcdInstallOperation* NewL(
        MNcdOperationRemoveHandler& aRemoveHandler, 
        const CNcdNodeIdentifier& aNodeId,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdInstallReportObserver& aReportObserver,
        MCatalogsSession& aSession );

    
    /**
     * Destructor
     */
    ~CNcdInstallOperation();
    
public: // New functions

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

    /**
     * @see CNcdBaseOperation::CounterPartLost
     */
    void CounterPartLost( const MCatalogsSession& aSession );


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
    CNcdInstallOperation( 
        MNcdOperationRemoveHandler& aRemoveHandler,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,        
        MNcdInstallReportObserver& aReportObserver,
        MCatalogsSession& aSession );
        
    void ConstructL( const CNcdNodeIdentifier& aNodeId );

protected:

    
    void GetFileInfoL( MCatalogsBaseMessage& aMessage );
    
    void HandleAllFilesInstalledL( 
        TInt aFinalIndex, 
        MCatalogsBaseMessage& aMessage );
    
    void UpdateInstalledFileInfoL( 
        MCatalogsBaseMessage& aMessage );

    void CreateReportL();
    void StartReportL( MCatalogsBaseMessage& aMessage );
    void CompleteReportL( MCatalogsBaseMessage& aMessage );
    void UpdateReportAccessPointL( const TUid& aClientUid );


    // Updates the file info to purchase history
    void UpdatePurchaseHistoryL( const CNcdFileInfo& aInfo,
        const RArray<TUid>& aAppUids, const TUid& aClientUid );


    TBool ReplaceInstallInfoL( 
        MNcdPurchaseDetails& aDetails, 
        const MNcdPurchaseInstallInfo& aInfo );


    TBool IsJava( 
        const TDesC& aMimeType, 
        TBool aAcceptJad = EFalse ) const;

    /**
     * Gets the path of the file that matches the index in the message
     */
    HBufC* FilePathLC( 
        MCatalogsBaseMessage& aMessage );

    void OpenFileL( 
        MCatalogsBaseMessage& aMessage );
    
    void DoReceiveMessageL( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );
    
    
    void DeleteFileL( MCatalogsBaseMessage& aMessage );
    
    TInt ReadFileIndexL( MCatalogsBaseMessage& aMessage );
    
    // Deletes the JAD from disk, iJadFile is also deleted
    void DeleteJad();
private:

    MCatalogsHttpSession& iHttpSession;
    MCatalogsAccessPointManager& iAccessPointManager;
    MNcdInstallReportObserver& iReportObserver;
    TNcdReportId iReportId;

    CNcdNodeIdentifier* iNodeId;   

    // The message is set when ReceiveMessage is called. The message
    // is used in the CounterPartLost-function that informs the message
    // if the session has been lost.
    MCatalogsBaseMessage* iMessage;
    
    TBool iIsDependency;
        
    HBufC* iJadFile; // JAD path if installing JAD+JAR
    MNcdInstallationService* iInstallService; // not owned 
    };

#endif // C_NCDINSTALLOPERATION_H
