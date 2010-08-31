/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines the unit test steps for the SCR Data Layer.
*
*/


/**
 @file 
 @internalComponent
 @test
*/

#ifndef TSCRDATALAYER_H
#define TSCRDATALAYER_H

#include "scrdatabase.h"
#include "tscrdatalayerserver.h"

#include <scs/oomteststep.h>
using namespace Usif;

// Constant used to name this test case
_LIT(KScrDataLayerStep,"SCRDataLayer");

class CScrTestDataLayer : public COomTestStep
	{
public:
	CScrTestDataLayer(CScrDataLayerTestServer& aParent);
	~CScrTestDataLayer();
	void ImplTestStepPreambleL();
	void ImplTestStepL();
	void ImplTestStepPostambleL();
	
private:
    void InsertRecordL(CStatement& aStmt, TInt aCol1, const TDesC& aCol2, const TDesC& aCol3);
	void VerifyRecordL(CStatement& aStmt, TInt aCol1, const TDesC& aCol2, const TDesC& aCol3);
	void ExecuteBadStatementL(CDatabase &aDb, TInt aTestNum, const TDesC& aStatement);
	void PrintErrorL(const TDesC& aMsg, TInt aErrNum,...);
	};

#endif /* TSCRDATALAYER_H */
