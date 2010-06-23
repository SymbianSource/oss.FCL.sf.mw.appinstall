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
* Description:  
*
*/


#ifndef C_NCDCREATEACCESSPOINTOPERATIONIMPL_H
#define C_NCDCREATEACCESSPOINTOPERATIONIMPL_H

#include "ncdbaseoperation.h"
#include "ncdoperationobserver.h"
#include "ncdparserobserver.h"


class MNcdParser;
class CNcdConfigurationProtocolClientConfigurationParser;

/** 
 * Operation for creating a new accesspoint
 *
 *
 * @note A new accesspoint is not created if a matching accesspoint
 * already exists
 */
class CNcdCreateAccessPointOperation : 
    public CNcdBaseOperation,
    public MNcdParserObserver
    {
public:
    
    /**
     * NewL
     *     
     */
    static CNcdCreateAccessPointOperation* NewL(
        HBufC* aAccessPointData,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        CNcdGeneralManager& aGeneralManager,
        MCatalogsSession& aSession );

    
    /**
     * Destructor
     */
    ~CNcdCreateAccessPointOperation();
    
public: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel()
     */
    void Cancel();
        

public: //from MNcdParserObserver

    /**
     * @see MNcdParserObserver
     */
    void ParseError( TInt aErrorCode );

    /**
     * @see MNcdParserObserver
     */
    void ParseCompleteL( TInt aError );
    

protected: // From CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        
    
protected:

    void HandleStateL();
    
    /**
     * Completes pending message with error code if aError != KErrNone
     * @return ETrue if aError != KErrNone
     */
    TBool HandleError( TInt aError );
    
    void StartParsingL();
        
    void ParseAccessPointDataL();    
    
protected:

    // Constructor
    CNcdCreateAccessPointOperation(        
        MNcdOperationRemoveHandler& aRemoveHandler, 
        CNcdGeneralManager& aGeneralManager,
        MCatalogsSession& aSession );

    void ConstructL( HBufC* aAccessPointData );
        
private:
    
    MNcdParser* iParser;
    CNcdConfigurationProtocolClientConfigurationParser* iSubParser; // Not owned
    HBufC* iAccessPointData;
    TUint32 iAccessPoint;
    };

#endif // C_NCDCREATEACCESSPOINTOPERATIONIMPL_H
