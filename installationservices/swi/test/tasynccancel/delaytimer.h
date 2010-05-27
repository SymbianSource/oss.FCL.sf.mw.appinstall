/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/
#include "common.h"

// This class might be used in the program in order to provide delays
// between some stages of its execution
class CDelayTimer : public CTimer
	{
	public:
		CDelayTimer();
		virtual ~CDelayTimer();
		void ConstructL()
			{
			CTimer::ConstructL();
			}
		void RunL()
			{
			CActiveScheduler::Stop();
			}
	};
	
// End of file
