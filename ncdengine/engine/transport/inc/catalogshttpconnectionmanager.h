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


#ifndef C_CATALOGSHTTPCONNECTIONMANAGER_H
#define C_CATALOGSHTTPCONNECTIONMANAGER_H

#include <e32base.h>

#include "catalogsconnectionmethod.h"

class MCatalogsHttpSession;

enum TCatalogsHttpConnectionConfirmationState
    {
    ECatalogsHttpConnectionDenied = -1,
    ECatalogsHttpConnectionConfirmationRequired = 0,
    ECatalogsHttpConnectionConfirmed
    };


class MCatalogsHttpConnectionConfirmationObserver
    {
public:

    /** 
     * @note If this function leaves, the connection is not confirmed
     */
    virtual TCatalogsHttpConnectionConfirmationState 
        HandleConnectionConfirmationRequestL( 
            MCatalogsHttpSession& aSession, 
            const TCatalogsConnectionMethod& aMethod ) = 0;

protected:
    virtual ~MCatalogsHttpConnectionConfirmationObserver()
        {
        }
    };


class MCatalogsHttpConnectionErrorObserver
    {
public:

    /** 
     * 
     */
    virtual void HandleConnectionErrorL( MCatalogsHttpSession& aSession,
        const TCatalogsConnectionMethod& aMethod,
        TInt aError ) = 0;

protected:
    virtual ~MCatalogsHttpConnectionErrorObserver()
        {
        }
    };



class CCatalogsHttpConnectionManager : public CBase
    {
public:

    static CCatalogsHttpConnectionManager* NewL( MCatalogsHttpSession& aSession );
    
    virtual ~CCatalogsHttpConnectionManager();
    
    void SetConnectionConfirmationObserver( 
        MCatalogsHttpConnectionConfirmationObserver* aObserver );


    void SetConnectionErrorObserver( 
        MCatalogsHttpConnectionErrorObserver* aObserver );

    void SetConnectionConfirmationState( 
        const TCatalogsHttpConnectionConfirmationState& aState );

    TCatalogsHttpConnectionConfirmationState ConnectionConfirmationState();

public:

    const TCatalogsConnectionMethod& DefaultConnectionMethod() const;
    
    void SetDefaultConnectionMethod( const TCatalogsConnectionMethod& aMethod );
    void SetAccessPointForDefaultConnectionMethod( const TCatalogsConnectionMethod& aMethod );
    
    /**
     */    
    TUint32 CurrentAccessPoint() const;
    
    TBool AskConnectionConfirmation( const TCatalogsConnectionMethod& aMethod );   
    
    
    TInt ReportConnectionError( 
        const TCatalogsConnectionMethod& aMethod, 
        TInt aError );

private:

    CCatalogsHttpConnectionManager( MCatalogsHttpSession& aSession );

    CCatalogsHttpConnectionManager( const CCatalogsHttpConnectionManager& );
    CCatalogsHttpConnectionManager& operator=( 
        const CCatalogsHttpConnectionManager& );

private:    

    MCatalogsHttpSession& iSession;
    MCatalogsHttpConnectionConfirmationObserver* iConfirmationObserver;
    MCatalogsHttpConnectionErrorObserver* iErrorObserver;
    
    TCatalogsHttpConnectionConfirmationState iConfirmationState;

    TCatalogsConnectionMethod iDefaultAccessPoint;
    TUint32 iCurrentAccessPoint;
    };


#endif // C_CATALOGSHTTPCONNECTIONMANAGER_H
