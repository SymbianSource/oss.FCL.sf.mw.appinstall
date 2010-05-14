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


#ifndef M_CATALOGSHTTPSTACKOBSERVER_H
#define M_CATALOGSHTTPSTACKOBSERVER_H

#include <e32base.h>

class TCatalogsConnectionMethod;

/**
 * Observer interface for HTTP stack.
 */
class MCatalogsHttpStackObserver 
    {
public:
    virtual TBool ResponseReceived( TInt aResponseStatusCode, 
                                   const TDesC8& aResponseStatusText ) = 0;

    virtual void ResponseHeaderReceived( const TDesC8& aHeader, 
                                         const TDesC8& aValue ) = 0;
                                         
    
    virtual TBool ResponseBodyReceived( const TDesC8& aData ) = 0;
    
    virtual void RequestSubmitted() = 0;
    virtual void RequestCompleted( TInt aError ) = 0;
    
    virtual void UpdateAccessPoint( 
        const TCatalogsConnectionMethod& aMethod ) = 0;
    };



#endif // M_CATALOGSHTTPSTACKOBSERVER_H

