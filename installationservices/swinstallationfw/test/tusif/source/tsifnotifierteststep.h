/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSIFNOTIFIERTESTSTEP_H
#define TSIFNOTIFIERTESTSTEP_H

#include <test/testexecutestepbase.h>
#include <usif/sif/sifnotification.h>
#include "tsifsuitestepbase.h"

namespace Usif
	{
	
    class CStopTimer : public CTimer
        {
    public:
        static CStopTimer* NewL(CSifOperationsNotifier* aNotifier, TInt aSeconds);
        CStopTimer(CSifOperationsNotifier* aNotifier);
        ~CStopTimer();
        void ConstructL(TInt aSeconds);
        void Start();
    private:
        void RunL();
        TInt RunError(TInt aError);
        TInt iSeconds;
        CSifOperationsNotifier* iNotifier;
        };  	

	class CSifSubscribeTestStep : public CSifSuiteStepBase, public MSifOperationsHandler
		{
	public:
		virtual ~CSifSubscribeTestStep();
		CSifSubscribeTestStep();
		virtual void ImplTestStepPreambleL();
		virtual void ImplTestStepL();
		virtual void ImplTestStepPostambleL();
		
		//Implementing MSifOperationsHandler
		void StartOperationHandler(TUint aKey, const CSifOperationStartData& aStartData);
		void EndOperationHandler(const CSifOperationEndData& aEndData);
		void ProgressOperationHandler(const CSifOperationProgressData& aProgressData);
		
	private:
	    void GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex);
	private:
	    CActiveScheduler* iSched;
	    CSifOperationsNotifier* iNotifier;
	    TInt iKey;
	    TInt iStartMessageCounter;
	    TInt iProgressMessageCounter;
	    TInt iEndMessageCounter;
	    TInt iSubscribeCounter;
	    TInt iSubscriberNum;
	    CStopTimer * iTimer;
		};
	
	_LIT(KSifSubscribeTestStep,"SifSubscribeStep");
		

	class CSifPublishTestStep : public CSifSuiteStepBase
	    {
	public:
	    virtual ~CSifPublishTestStep();
	    CSifPublishTestStep();
	    virtual void ImplTestStepPreambleL();
	    virtual void ImplTestStepL();
	    virtual void ImplTestStepPostambleL();
	    
	    void GenerateIndexedAttributeNameL(TDes& aInitialAttributeName, TInt aIndex);
	    void PublishDataL(HBufC* aConfigSection);    
	private:
	    CActiveScheduler* iSched;
	    CPublishSifOperationInfo* iPublisher;
	    CStopTimer* iTimer;
	    };

	_LIT(KSifPublishTestStep,"SifPublishStep");
	
	    
	} // namespace Sif

#endif // TSIFNOTIFIERTESTSTEP_H
