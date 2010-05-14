/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CATALOGSHTTPTRANSACTION_H
#define C_CATALOGSHTTPTRANSACTION_H

#include "catalogshttpstackobserver.h"
#include "catalogshttpoperation.h"
#include "catalogstransportoperationid.h"
#include "catalogshttpconfigobserver.h"

class MCatalogsHttpTransactionManager;
class MCatalogsHttpConfig;
class MCatalogsHttpObserver;
class CCatalogsHttpConfig;
class CCatalogsHttpHeaders;
class CCatalogsKeyValuePair;
class CCatalogsHttpStack;
class CCatalogsConnection;

/**
* HTTP transaction implementation
*/
class CCatalogsHttpTransaction : public CBase, 
    public MCatalogsHttpOperation, 
    public MCatalogsHttpStackObserver,
    public MCatalogsHttpConfigObserver
    {
public:
    /**
    * Creator
    * 
    * @param aDownload Platform download
    * @param aConfig Configuration
    * @return A new HTTP download
    */
    static CCatalogsHttpTransaction* NewL( 
        MCatalogsHttpTransactionManager& aOwner, 
        const CCatalogsHttpConfig& aConfig,
        const TCatalogsTransportOperationId& aId,
        TCatalogsHttpTransactionType aType = 
            ECatalogsHttpTransactionNormal );

    static CCatalogsHttpTransaction* NewLC( 
        MCatalogsHttpTransactionManager& aOwner, 
        const CCatalogsHttpConfig& aConfig,
        const TCatalogsTransportOperationId& aId,
        TCatalogsHttpTransactionType aType =
            ECatalogsHttpTransactionNormal );

    /**
    * Destructor
    */
    ~CCatalogsHttpTransaction();


public: // MCatalogsTransportOperation

   
    /**
    * @see MCatalogsTransportOperation::AddRef()
    */        
    TInt AddRef();		
    
    /**
    * @see MCatalogsTransportOperation::Release()
    */        
    TInt Release();

    /**
    * @see MCatalogsTransportOperation::RefCount()
    */                
    TInt RefCount() const;    

    /**
    * @see MCatalogsTransportOperation::Cancel()
    */        
    TInt Cancel();

    /**
    * @see MCatalogsTransportOperation::Progress()
    */    
    TCatalogsTransportProgress Progress() const;		


public: // MCatalogsHttpOperation

    /**
    * @see MCatalogsHttpOperation::Start()
    */
    TInt Start( TResumeStartInformation aResumeOrigin = EResumeFromUser );
    
    
    /**
    * @see MCatalogsHttpOperation::Pause()
    */
    TInt Pause();


    /**
    * @see MCatalogsHttpOperation::NotifyCancel()
    */
    virtual void NotifyCancel();
    
    // Body and headers
    
    /**
    * @see MCatalogsHttpOperation::SetBodyL( const TDesC8& )
    */
    void SetBodyL( const TDesC8& aBody );


    /**
    * @see MCatalogsHttpOperation::SetBodyL( const TDesC16&	)    
    */
    void SetBodyL( const TDesC16& aBody );
    
    
    /**	    
    * @see MCatalogsHttpOperation::Body()        
    */
    const TDesC8& Body() const;


    /**
    * @see MCatalogsHttpOperation::SetUriL()
    */
    void SetUriL( const TDesC8& aUri );
    
    
    /**
    * @see MCatalogsHttpOperation::SetUriL()
    */
    void SetUriL( const TDesC16& aUri );

    /**
    * @see MCatalogsHttpOperation::Uri()
    */
    const TDesC8& Uri() const;


    /**
    * @see MCatalogsHttpOperation::Config()
    */
    MCatalogsHttpConfig& Config() const;

    
    /**
    * @see MCatalogsHttpOperation::RequestHeadersL()
    */
    MCatalogsHttpHeaders& RequestHeadersL() const;
    
    /**
    * @see MCatalogsHttpOperation::ResponseHeadersL()
    */
    const MCatalogsHttpHeaders& ResponseHeadersL() const;
    	    	    
    	    
    /**
    * @see MCatalogsHttpOperation::OperationType()
    */
    TCatalogsHttpOperationType OperationType() const;

    
    /**
    * @see MCatalogsHttpOperation::OperationId()
    */
    const TCatalogsTransportOperationId& OperationId() const;


    /**
    * @see MCatalogsHttpOperation::ContentType()
    */
    const TDesC8& ContentType() const;


    /**
    * @see MCatalogsHttpOperation::ContentSize()
    */
    TInt32 ContentSize() const;
    
    
    /**
    * @see MCatalogsHttpOperation::TransferredSize()
    */
    TInt32 TransferredSize() const;
    
    
    /**        
    * @see MCatalogsHttpOperation::IsPausable()
    * @note Transactions can not be paused
    */
    TBool IsPausable() const;

    /**
    * @see MCatalogsHttpOperation::SetHeaderMode()
    * @note This setting has no effect on transactions.
    */
    void SetHeaderMode( TCatalogsHttpHeaderMode aMode );   

    /**
    * @see MCatalogsHttpOperation::SetContentType( const TDesC8&)
    */
    void SetContentTypeL( const TDesC8& aContentType );        

    /**
    * @see MCatalogsHttpOperation::SetContentType( const TDesC16&)
    */
    void SetContentTypeL( const TDesC16& aContentType );


    /**
     * @see MCatalogsHttpOperation::StatusCode()
     */
    TInt StatusCode() const;        
    
    
    /**
     * @see MCatalogsHttpOperation::StatusText()
     */
    const TDesC8& StatusText() const;
    

    /** 
     * @see MCatalogsHttpOperation::SetConnection()
     */
    void SetConnection( CCatalogsConnection& aConnection );
    
    
    /** 
     * @see MCatalogsHttpOperation::ReportConnectionError()
     */
    void ReportConnectionError( TInt aError );
    
    
    CCatalogsHttpConnectionManager& ConnectionManager();

    /**
     * Not supported
     */
    void ExternalizeL( RWriteStream& aStream ) const;
    
    
    /**
     * Not supported
     */
    void InternalizeL( RReadStream& aStream );
    
    /**
     * @see MCatalogsHttpOperation::State()
     */
    TCatalogsHttpOperationState State() const;
    
    
public: // MCatalogsHttpConfigObserver

    /**
    * @see MCatalogsHttpConfigObserver::HandleHttpConfigEvent
    */
    TInt HandleHttpConfigEvent( MCatalogsHttpConfig* aConfig,
        const TCatalogsHttpConfigEvent& aEvent );
            
            
public: // MCatalogsHttpStackObserver

    /**
    *
    */
    TBool ResponseReceived( TInt aResponseStatusCode, 
        const TDesC8& aResponseStatusText );


    /**
    *
    */
    void ResponseHeaderReceived( const TDesC8& aHeader, 
        const TDesC8& aValue );

    /**
    *
    */
    TBool ResponseBodyReceived( const TDesC8& aData );
    
    /**
    *
    */
    void RequestSubmitted();
    
    /**
    *
    */
    void RequestCompleted( TInt aError );
    

    void UpdateAccessPoint( const TCatalogsConnectionMethod& aMethod );
    
public:
    
    static TInt StartCallBack( TAny* aPtr );

protected:  
    
    /**
    * Constructor
    */
    CCatalogsHttpTransaction( MCatalogsHttpTransactionManager& aOwner,
        const TCatalogsTransportOperationId& aId,
        TCatalogsHttpTransactionType aType );
    
    /**
    * 2nd phase constructor
    */
    void ConstructL( const CCatalogsHttpConfig* aConfig );


private: // New methods

    
    /** 
    * Updates the http stack to match the current configuration
    */
    void UpdateConfiguration();    

    
    /**
    * Updates the request headers to the http stack
    */
    void UpdateRequestHeadersL();
    
    /**
    * Informs the observer of HTTP errors
    * @param aErr Symbian error code
    */
    TBool HandleHttpError( TCatalogsHttpErrorType aType, TInt aErr );
    
    
    /**
     * Actual start
     */
    void DoStartL();
    
    
    void DoRequestCompletedL( TInt aError );
    
    void NotifyObserver();
    
    void SetTransferring( TBool aTransferring );

    void AsyncStartL();
    
    void EncodeUriL();
    
    const TDesC8& EncodedUri() const;
    
private:

    MCatalogsHttpTransactionManager& iOwner;
    CCatalogsHttpStack* iHttp;
    TCatalogsTransportOperationId iId;  // Operation id
    
    CCatalogsHttpConfig* iConfig;       // Configuration
    MCatalogsHttpObserver* iObserver;   // Observer        
    TCatalogsHttpEvent iState;
    HBufC8* iRequestBody;
    TPtrC8 iResponseBody;
    HBufC8* iContentType;
    HBufC8* iUri;
    HBufC8* iEncodedUri;
    CCatalogsHttpHeaders* iResponseHeaders;
    TInt iRefCount;    
    TCatalogsHttpTransactionType iType;
    TInt iTransferredLength;
    TInt iContentLength;
    TInt iRetryCount;
    
    TBool iTransferring;        
    TInt iResponseStatusCode;
    HBufC8* iResponseStatusText;
    CCatalogsConnection* iConnection; // not owned
    
    TCallBack iStartCallBack;
    CAsyncCallBack* iCallBack; // owned
    };


#endif // C_CATALOGSHTTPTRANSACTION_H
