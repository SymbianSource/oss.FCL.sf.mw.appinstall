/*
* Copyright (c) 2010- Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#include "ut_swhandlerplugin.h"
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>

#include "swhandlerplugin.h"
#include <AknServerApp.h>
#include <e32def.h>

NONSHARABLE_CLASS( UT_CSwHandlerPluginTestObserver ) : public MAknServerAppExitObserver
    {
public:
    inline UT_CSwHandlerPluginTestObserver() { iObserverCalled = EFalse; iReason = 0; }
    virtual void HandleServerAppExit(TInt aReason);
    
    inline TBool WasObserverCalled() const { return iObserverCalled; }
    inline TInt Reason() const { return iReason; }
private:
    TBool iObserverCalled;
    TInt iReason;
    };

void UT_CSwHandlerPluginTestObserver::HandleServerAppExit(TInt aReason)
    {
    iObserverCalled = ETrue;
    iReason = aReason;
    }

// - Construction -----------------------------------------------------------

UT_CSwHandlerPlugin* UT_CSwHandlerPlugin::NewL()
    {
    UT_CSwHandlerPlugin* self = UT_CSwHandlerPlugin::NewLC();
    CleanupStack::Pop();
    return self;
    }

UT_CSwHandlerPlugin* UT_CSwHandlerPlugin::NewLC()
    {
    UT_CSwHandlerPlugin* self = new( ELeave ) UT_CSwHandlerPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_CSwHandlerPlugin::~UT_CSwHandlerPlugin()
    {
    }

UT_CSwHandlerPlugin::UT_CSwHandlerPlugin()
    {
    }

void UT_CSwHandlerPlugin::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------



void UT_CSwHandlerPlugin::SetupL(  )
    {
    iCSwHandlerPlugin = CSwHandlerPlugin::NewL();
    }
    

void UT_CSwHandlerPlugin::Teardown(  )
    {
    delete iCSwHandlerPlugin; 
     iCSwHandlerPlugin = NULL; 
    }
    

void UT_CSwHandlerPlugin::T_Global_HandleUrlEmbeddedLL(  )
    {
    EUNIT_ASSERT_NO_LEAVE(iCSwHandlerPlugin->HandleUrlEmbeddedL( ));
    }
    
void UT_CSwHandlerPlugin::T_Global_HandleUrlStandaloneLL(  )
    {
    EUNIT_ASSERT_NO_LEAVE(iCSwHandlerPlugin->HandleUrlStandaloneL( ));
    }
    
void UT_CSwHandlerPlugin::T_Global_ObserverL(  )
    {
    UT_CSwHandlerPluginTestObserver testObserver;
    
    iCSwHandlerPlugin->Observer(&testObserver);
    
    EUNIT_ASSERT_NO_LEAVE(iCSwHandlerPlugin->HandleUrlEmbeddedL());
    
    EUNIT_ASSERT( testObserver.WasObserverCalled());

    EUNIT_ASSERT_EQUALS( KErrNone, testObserver.Reason() );
    }
    

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CSwHandlerPlugin,
    "Tests the Sw Handler Plugin which launches IAD client based on a URL scheme.",
    "UNIT" )

EUNIT_TEST(
    "HandleUrlEmbeddedL - test0",
    "CSwHandlerPlugin",
    "HandleUrlEmbeddedL - test0",
    "FUNCTIONALITY",
    SetupL, T_Global_HandleUrlEmbeddedLL, Teardown)
    
EUNIT_TEST(
    "HandleUrlStandaloneL - test1",
    "CSwHandlerPlugin",
    "HandleUrlStandaloneL - test1",
    "FUNCTIONALITY",
    SetupL, T_Global_HandleUrlStandaloneLL, Teardown)
    
EUNIT_TEST(
    "Observer - test2",
    "CSwHandlerPlugin",
    "Observer - test2",
    "FUNCTIONALITY",
    SetupL, T_Global_ObserverL, Teardown)
    

EUNIT_END_TEST_TABLE
