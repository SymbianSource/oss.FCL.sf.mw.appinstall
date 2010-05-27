/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_NCDSENDHTTPREQUESTOPERATION_H
#define C_NCDSENDHTTPREQUESTOPERATION_H

#include "ncdbaseoperation.h"
#include "ncdoperationobserver.h"
#include "catalogsconnectionmethod.h"


class MCatalogsHttpOperation;
class CCatalogsHttpRequestAdapter;
class TNcdConnectionMethod;
class CNcdGeneralManager;

/** 
 * Creates a HTTP transaction from a standard HTTP request data
 * 
 */
class CNcdSendHttpRequestOperation : 
    public CNcdBaseOperation,
    public MCatalogsHttpObserver    
    {
public:
    
    /**
     * NewL
     *
     * @param aUri Request URI. Ownership is transferred. NULL is not allowed
     * @param aRequest Request data. Ownership is transferred. NULL is not allowed
     */
    static CNcdSendHttpRequestOperation* NewL(
        HBufC8* aUri,
        HBufC8* aRequest,        
        const TNcdConnectionMethod& aMethod,
        CNcdGeneralManager& aGeneralManager,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSession& aSession );

    
    /**
     * Destructor
     */
    virtual ~CNcdSendHttpRequestOperation();
    

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


protected: // From CNcdBaseOperation


    /**
     * @see CNcdBaseOperation::RunOperation()
     */
    TInt RunOperation();        

protected:

    // Constructor
    CNcdSendHttpRequestOperation(    
        CNcdGeneralManager& aGeneralManager,
        MNcdOperationRemoveHandler& aRemoveHandler, 
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSession& aSession );
        
    void ConstructL( 
        const TNcdConnectionMethod& aMethod, 
        HBufC8* aUri, 
        HBufC8* aRequest );
    
    
    void HandleStateL();
    
    TBool HandleError( TInt aError );
    
    void GetResponseL( MCatalogsBaseMessage& aMessage );
        
private:

    MCatalogsHttpSession& iHttpSession;   
    MCatalogsHttpOperation* iTransaction;
    
    HBufC8* iUri;
    HBufC8* iRequest;
    HBufC8* iResponse;
    TCatalogsConnectionMethod iConnectionMethod;
    CCatalogsHttpRequestAdapter* iAdapter;
    CBufFlat* iBody;
    };

#endif // C_NCDSENDHTTPREQUESTOPERATION_H
