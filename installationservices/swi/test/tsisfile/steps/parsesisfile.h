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


#ifndef __PARSESISFILE_H__
#define __PARSESISFILE_H__


#include <test/testexecutestepbase.h>
#include "tsisstep.h"


_LIT(KParseStep,"ParseStep");

namespace Swi
{
  namespace Sis
  { 
    namespace Test
    {

	  class CTestParserServer;

      class CParseStep : public CParserTestStep
      {
	  public:
	   CParseStep(CTestParserServer& aParent);
	  	
      public: // from CTestStep	
		IMPORT_C enum TVerdict	doTestStepL();
		//IMPORT_C enum TVerdict	doTestStepPreambleL();
		//IMPORT_C enum TVerdict	doTestStepPostambleL();

    private:
    	CTestParserServer& iParent;

      };

    } // namespace Test

  } // namespace Sis

} // namespace Swi




#endif // __PARSESISFILE_H__
