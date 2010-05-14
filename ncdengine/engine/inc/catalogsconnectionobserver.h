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


#ifndef C_CATALOGSCONNECTIONOBSERVER_H
#define C_CATALOGSCONNECTIONOBSERVER_H

#include <e32base.h>
#include <e32property.h>

class MCatalogsEngineObserver;

class CCatalogsConnectionObserver : public CActive
    {
public:
    static CCatalogsConnectionObserver* NewL( MCatalogsEngineObserver& aObserver );

    ~CCatalogsConnectionObserver();

protected: // from CActive
    void RunL();
    
    void DoCancel();
    
private:
    CCatalogsConnectionObserver( MCatalogsEngineObserver& aObserver );

    void ConstructL();

private:
    RProperty iConnectionEventProperty;
    MCatalogsEngineObserver& iObserver;
    };

#endif // C_CATALOGSCONNECTIONOBSERVER_H

