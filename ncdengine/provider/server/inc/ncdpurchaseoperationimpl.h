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
* Description:   ?Description
*
*/


#ifndef C_NCDPURCHASEOPERATIONIMPL_H
#define C_NCDPURCHASEOPERATIONIMPL_H

#include <e32base.h>

#include "ncdbaseoperation.h"
#include "ncdparserobserver.h"
#include "catalogshttpobserver.h"
#include "catalogssmsobserver.h"

class MNcdParser;
class MNcdProtocol;
class CNcdNodeManager;
class CNcdPurchaseHistoryDb;
class MCatalogsSmsSession;
class CNcdNode;
class CNcdPreminetProtocolPurchaseInformationImpl;
class CNcdPreminetProtocolPurchaseProcessedImpl;
class MCatalogsAccessPointManager;

class CNcdPurchaseOptionImpl;

class MNcdPreminetProtocolPayment;
class MNcdOperationRemoveHandler;

class CNcdSubscriptionManager;
class CNcdDelay;
class MNcdConfigurationProtocolQuery;
class CNcdNodeIdentifier;
class CNcdDownloadInfo;
class CNcdPurchaseDetails;
class CNcdNodeMetaData;
class CNcdKeyValuePair;


/**
 *  Purchase operation implementation.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdPurchaseOperationImpl : public CNcdBaseOperation,
                                  public MNcdParserObserver,
                                  public MNcdParserPurchaseObserver,
                                  public MCatalogsHttpObserver,
                                  public MCatalogsSmsObserver
    {
    
public:

    /**
     *  Sub states of purchase operation 
     */
    enum TPurchaseOperationState
        {
        EBegin,
        EPurchaseRequestSent,
        EPurchaseInformationReceived,
        EPurchaseRequestResent,
        EDisclaimerQueried,
        EPaymentMethodsQueried,
        EPaymentInformationQueried,
        EPaymentProcessed,
        EPurchaseConfirmationSent,
        EDownloadDetailsReceived,
        EPurchaseComplete,
        EFailed
        };

    enum TPurchaseRequestType
        {
        EPurchaseRequest,
        EPurchaseConfirmation
        };
        

    static CNcdPurchaseOperationImpl* NewL( 
        const TDesC& aNameSpace,
        const TDesC& aNodeId,
        const TUid& aClientUid,
        const TDesC& aPurchaseOptionId,
        CNcdGeneralManager& aGeneralManager,        
        MCatalogsHttpSession& aHttpSession, 
        MCatalogsSmsSession& aSmsSession, 
        CNcdSubscriptionManager& aSubscriptionManager,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession );

    static CNcdPurchaseOperationImpl* NewLC( 
        const TDesC& aNameSpace,
        const TDesC& aNodeId,
        const TUid& aClientUid,
        const TDesC& aPurchaseOptionId,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSmsSession& aSmsSession,
        CNcdSubscriptionManager& aSubscriptionManager,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession );
        
    virtual ~CNcdPurchaseOperationImpl();
    
public: // New functions

    /**
     * Call back function used in resend after case.
     *
     * @param aOperation The operation called in the function.
     */
    static TInt ResendAfterCallBack( TAny* aOperation );

    /**
     * Returns the identifier of the node that this operation relates to.
     *
     * @return Node identifier.
     */
    const CNcdNodeIdentifier& NodeIdentifier() const;
    
    
public: // from base class CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel
     */
    virtual void Cancel();

    /**
     * @see CNcdBaseOperation::QueryCompletedL
     */
    virtual TBool QueryCompletedL( CNcdQuery* aQuery );

public: // from base class MCatalogsHttpObserver

    /**     
     * @see MCatalogsHttpObserver::HandleHttpEventL
     */

    virtual void HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent );

    /**
     * @see MCatalogsHttpObserver::HandleHttpError()
     */
    virtual TBool HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError );
        
public: // from base class MCatalogsSmsObserver

	/**
	 * @see MCatalogsSmsObserver::HandleSmsEvent()
	 */
	virtual TInt HandleSmsEvent( 
	    MCatalogsSmsOperation& aOperation, 
	    TCatalogsSmsEvent aEvent );

public: // from MNcdParserQueryObserver

    /**
     * @see MNcdParserQueryObserver
     */
    virtual void QueryL( MNcdConfigurationProtocolQuery* aData );



public: // from base class MNcdParserObserver

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseError( TInt aErrorCode );

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseCompleteL( TInt aError );

public: // from base class MNcdParserPurchaseObserver

    /**
     * @see MNcdParserPurchaseObserver::InformationL
     */
    virtual void InformationL( MNcdPreminetProtocolPurchaseInformation* aData );

    /**
     * @see MNcdParserPurchaseObserver::ProcessedL
     */
    virtual void ProcessedL( MNcdPreminetProtocolPurchaseProcessed* aData );
    
public: // from base class MNcdParserInformationObserver

    virtual void InformationL( 
        MNcdPreminetProtocolInformation* aData );
        
public: // from base class MNcdParserErrorObserver

    virtual void ErrorL( MNcdPreminetProtocolError* aData );
        

        
protected:

    CNcdPurchaseOperationImpl(
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MCatalogsSmsSession& aSmsSession,
        CNcdSubscriptionManager& aSubscriptionManager,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession );
        
    
    void ConstructL( 
        const TDesC& aNameSpace, 
        const TDesC& aNodeId,
        const TUid& aClientUid,
        const TDesC& aPurchaseOptionId );
        

public: // from base class CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual TInt RunOperation();
    
    void DoRunOperationL();
    
    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual void ChangeToPreviousStateL();


private:

    HBufC8* CreatePurchaseRequestLC( TPurchaseRequestType aType, 
                                    const TDesC& aServerUri );

    void SendRequestL( const TDesC& aServerUri, const TDesC8& aData );

    void ProcessSmsPaymentL();

    void UpdatePurchaseHistoryL();
    
    // Copies relevant info from CNcdNodeContentInfo to given purchase details
    void SetContentInfoToPurchaseDetailsL( 
        CNcdPurchaseDetails& aDetails, 
        const CNcdNodeMetaData& aMetadata ) const;

    void HandleDependenciesL( CNcdPurchaseDetails& aDetails );   
    
    void AddDownloadInfosToDetailsL( CNcdPurchaseDetails& aDetails,
        const RPointerArray<CNcdDownloadInfo>& aInfos );
        
    TInt RemovePendingSmsOp( const TCatalogsTransportOperationId& aId );

    TInt StartResendAfterTimer();
    
private: // data
    
    /**
     * A sub-state of this operation's execution.
     */
    TPurchaseOperationState iPurchaseOperationState;
    
    /**
     * Reference to the access point manager.
     * Not owned by the purchase operation.
     */
    MCatalogsAccessPointManager& iAccessPointManager;
    
    /**
     * Not own.
     */
    CNcdPurchaseHistoryDb* iPurchaseHistory;

    /**
     * Http session for sending requests and receiving responses.
     * Not owned by the purchase operation.
     */
    MCatalogsHttpSession& iHttpSession;
    
    /**
     * Sms session for sending requests and receiving responses.
     * Not owned by the purchase operation.
     */
    MCatalogsSmsSession* iSmsSession;
    
    /**
     * Protocol.
     * Not owned by the purchase operation.
     */
    MNcdProtocol& iProtocol;    
    
    /**
     * Http operation for current transaction.
     * Not owned by the purchase operation.
     */
    MCatalogsHttpOperation* iHttpTransaction;

    /**
     * Subscription manager.
     * Not owned by the purchase operation.
     */
    CNcdSubscriptionManager& iSubscriptionManager;


    CNcdPurchaseOptionImpl* iSelectedPurchaseOption;
    const MNcdPreminetProtocolPayment* iSelectedPaymentMethod;
    
   
    /**
     * 
     * Not own.
     */
    CNcdNode* iNode;
    
    /**
     * Node identifier of the node to purchase.
     */
    CNcdNodeIdentifier* iNodeIdentifier;
    
    /**
     * Purchase option id. Used in subscription renew.
     */
    HBufC* iPurchaseOptionId;


    MNcdPreminetProtocolPurchaseInformation* iPurchaseInformationData;
    MNcdPreminetProtocolPurchaseProcessed* iPurchaseProcessedData;

	RPointerArray<MCatalogsSmsOperation> iPendingSmsOperations;

    /**
     * Purchase query.
     */
    CNcdQuery* iPurchaseQuery;
    
    TBool iLocked;
        
    CNcdDelay* iDelay;

    // Key() is address and Value() is message
    CNcdKeyValuePair* iSmsRegistrationRequest;
    
    HBufC* iRedirectUri;
    
    // Time after which the purchase request is resent
    TInt iResendAfter;
    };

#endif // C_NCDPURCHASEOPERATIONIMPL_H
