/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file contains the class CDataProviderStep.
*
*/


/**
 @file tdataproviderstep.h
 @internalTechnology
*/

#if (!defined __T_DATA_PROVIDER_STEP_H__)
#define __T_DATA_PROVIDER_STEP_H__

#include <test/testexecutestepbase.h>
#include "tdataprovider.h"
#include <e32def.h> 
#include <e32std.h>
#include <e32base.h>


#include "dessisdataprovider.h"
#include "filesisdataprovider.h"
#include "cafsisdataprovider.h"

#define KMAXBUF 100

using namespace Swi;

class CDataProviderStep : public CTestStep
	{
public:

	enum TDataProviderType
	/**
	 Identifies the Type of the input 
	 */
		{
		/** Read data from the descriptor */
		EDesSisDataProvider = 1,
		/** Read data from the file */
		EFileSisDataProvider,
		/** Read data from DRM file */
		ECafSisDataProvider
		};  

	enum TOperation
	/**
	 Identifies the operation mode
	 */
		{
		/** Read and Seek */
		EReadAndSeek = 1,
		/** Read only */
		EReadOnly,
		/** Read to a specific length*/
		EReadSpecificLength
		};  

	enum TFileType 
		{
		/** Use filename */
		EFilename = 1,
		/** Use file handle*/
		EFileHandle
		};
	
	enum TStepType 
		{
		/** Test DataL() */
		EReadStep = 1,
		/** Test OpenDrmStepL()*/
		EOpenStep,
		/** Seek Step*/
		ESeekStep
		};	

	CDataProviderStep();
	~CDataProviderStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
		
private:
	void DoReadL(MSisDataProvider *aSisDataProvider,const TInt& aOpType); 
	void  VerifyResult (const TPtr8& aDataConst ,  const TPtrC16& aExpectedSisData); 
	};


_LIT(KDataProviderStep,"tdataproviderstep");
_LIT(KMemoryCheckStep,"MemoryCheckStep");

#endif
