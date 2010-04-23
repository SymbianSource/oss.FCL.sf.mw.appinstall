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
*
*/


/**
 @file
 @internalTechnology 
*/

#if (!defined TSIFSUITESTEPBASE_H)
#define TSIFSUITESTEPBASE_H
#include <test/testexecutestepbase.h>
#include <usif/usifcommon.h>
#include <scs/oomteststep.h>

namespace Usif
	{

	TComponentId FindComponentInScrL(const TDesC& aName, const TDesC& aVendor);

	class CSifSuiteStepBase : public COomTestStep
		{
	public:
		virtual ~CSifSuiteStepBase();
		CSifSuiteStepBase();
		virtual void ImplTestStepPreambleL(); 
		virtual void ImplTestStepPostambleL();
		};

	} // namespace Sif

#endif
