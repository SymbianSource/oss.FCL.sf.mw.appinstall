/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CATALOGSCONNECTIONEVENTSINK_H
#define C_CATALOGSCONNECTIONEVENTSINK_H

#include <e32base.h>

/**
 * This class handles reporting connection activity from server-side 
 * to client-side process.
 */
class CCatalogsConnectionEventSink : public CBase
    {
public:
    /**
     * Param aClientSID must be the client side process SID.
     */
    static CCatalogsConnectionEventSink* NewL( const TUid& aClientSID );

    ~CCatalogsConnectionEventSink();

    /**
     * Call this with ETrue when connection is active. When activity stops,
     * call with EFalse. Calls to this method must be balanced.
     */
    void ReportConnectionStatus( TBool aActive );
    
private:
    CCatalogsConnectionEventSink();

    void ConstructL( const TUid& aClientSID );

private:
    TUint iKey;
    TInt iActiveConnectionCount;
    TUid iCategoryUid;
    };

#endif // C_CATALOGSCONNECTIONEVENTSINK_H

