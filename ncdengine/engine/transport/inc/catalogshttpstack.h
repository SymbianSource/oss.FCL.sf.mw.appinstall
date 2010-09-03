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


#ifndef C_CATALOGSHTTPSTACK_H
#define C_CATALOGSHTTPSTACK_H

#include <http/mhttpdatasupplier.h>
#include <http/mhttptransactioncallback.h>
#include <http/mhttpauthenticationcallback.h>

#include "catalogshttpstackobserver.h"
#include "catalogskeyvaluepair.h"
#include "catalogsconnectionmethod.h"


// Constants
const TInt KRequestTimeoutMicroseconds( 60000000 );

// Forward declarations
class RHTTPSession;
class RHTTPTransaction;
class CCatalogsHttpConnectionCreator;
class CCatalogsHttpConnectionManager;
class CCatalogsConnection;

/**
 * Symbian implementation of a HTTP stack.
 */
class CCatalogsHttpStack :   
    public CActive,     
    public MHTTPTransactionCallback,
    public MHTTPDataSupplier,
    public MHTTPAuthenticationCallback
    {
public:
    
    static CCatalogsHttpStack* NewL( 
        MCatalogsHttpStackObserver* aObserver,
        CCatalogsConnection& aConnection,
        CCatalogsHttpConnectionCreator& aConnectionCreator );
        
    static CCatalogsHttpStack* NewLC( 
        MCatalogsHttpStackObserver* aObserver,
        CCatalogsConnection& aConnection,
        CCatalogsHttpConnectionCreator& aConnectionCreator );
        
    virtual ~CCatalogsHttpStack();

private:
    
    void ConstructL();
    CCatalogsHttpStack( 
        MCatalogsHttpStackObserver* iObserver,
        CCatalogsConnection& aConnection,
        CCatalogsHttpConnectionCreator& aConnectionCreator );

    void SetHeaderL( RHTTPHeaders aHeaders, 
                     TInt aHdrField, 
                     const TDesC8& aHdrValue);


public:     

    void ClearHeaders();
    
    void AddHeaderL( const CCatalogsKeyValuePair* aPair );

    void IssueHttpRequestL( const TDesC8& aMethod,
                            const TDesC8& aURI,
                            const TDesC8& aContentType,
                            const TDesC8& aBody );

    void CancelTransaction();
    
    void ReleaseBody();    
    

    void SetConnectionMethodL( const TCatalogsConnectionMethod& aMethod );
    
    /** 
     * Real accesspoint id if connected     
     */
    TUint32 AccessPointID() const;
    
    void SetObserver( MCatalogsHttpStackObserver* aObserver );

    
    /**
     * @param aManager Connection manager. Ownership is NOT transferred
     */
    void SetConnectionManager( CCatalogsHttpConnectionManager* aManager );

private:

    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );
    TInt MHFRunError( TInt aError, 
                      RHTTPTransaction aTransaction, 
                      const THTTPEvent& aEvent );

private:
    
    void ReleaseData();
    TBool GetNextDataPart( TPtrC8& aDataPart );
    TInt Reset();
    TInt OverallDataSize();    

    void SetTimeoutTimer();
    void CancelTimeoutTimer();

private:
    
    TBool GetCredentialsL( const TUriC8& aURI, 
                           RString aRealm, 
                           RStringF aAuthenticationType, 
                           RString& aUsername, 
                           RString& aPassword );

private:

    void RunL();
    void DoCancel();

private:

    enum
        {
        ENone,
        ETimeout
        } iState;

    RTimer                  iTimer;        
    
    RHTTPSession            iSession;
    RHTTPTransaction        iTransaction;
    
    RPointerArray<CCatalogsKeyValuePair> iUserHeaders;

    TBool                   iConnected;
    TCatalogsConnectionMethod iConnectionMethod;    

    
    TInt                    iLastError;

    RStringPool             iPool;
    TStringTable const&     iTable;

    MHTTPDataSupplier*      iBody;

    MCatalogsHttpStackObserver*  iObserver;  // Used for passing body data and
                                        // events to the client.
    HBufC8*                 iPostData;  // Data for HTTP requests
    TBool                   iRunning;   // ETrue, if transaction running

    CCatalogsConnection&    iConnection; 
    CCatalogsHttpConnectionCreator& iConnectionCreator;
    TBool                   iConnectionOpened; // ETrue if iConnection is open
    CCatalogsHttpConnectionManager* iConnectionManager; // not owned
    
    };

#endif // C_CATALOGSHTTPSTACK_H
