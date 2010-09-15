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

#ifndef __UT_CSWHANDLERPLUGIN_H__
#define __UT_CSWHANDLERPLUGIN_H__

// INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>

#include "swhandlerplugin.h"

// FORWARD DECLARATIONS


// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CSwHandlerPlugin )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static UT_CSwHandlerPlugin* NewL();
        static UT_CSwHandlerPlugin* NewLC();
        ~UT_CSwHandlerPlugin();

    private: // Constructors

        UT_CSwHandlerPlugin();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_Global_HandleUrlEmbeddedLL();
        
         void T_Global_HandleUrlStandaloneLL();
        
         void T_Global_ObserverL();
        

    private: // Data

        CSwHandlerPlugin* iCSwHandlerPlugin;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CSWHANDLERPLUGIN_H__
