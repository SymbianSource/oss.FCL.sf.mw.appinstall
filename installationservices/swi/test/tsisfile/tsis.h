/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TSIS_H__
#define __TSIS_H__

#include <test/testexecuteserverbase.h>

#include "swi/siscontroller.h"
#include "swi/siscontents.h"

namespace Swi
{
namespace Sis
 {
  
   namespace Test
   {

class CTestParserServer : public CTestServer
	{
public:
	static CTestParserServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs() {return iFs;};
	
	~CTestParserServer()
		{
		}
	
private:
	RFs iFs;
	};

   } // namespace Swi::Sis::Test

 } // namespace Swi::Sis

} //namespace Swi

#endif // #ifndef __TUISUPPORT_H__

