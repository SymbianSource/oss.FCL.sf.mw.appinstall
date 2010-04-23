/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Server-side class which are used to keep clients waiting if the SWI
* Observer processor is busy.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SWIOBSERVERWAITER_H
#define SWIOBSERVERWAITER_H

#include "swiobserver.h"

namespace Swi
	{
	
	class CSwiObserverWaiter : public CAsyncRequest
	/**
		An object instansiated from this class is used to keep client requests waiting
		while the Swi Observer processor is busy. When the processor is finished with its
		task, that active object is notified. Then the request made by the client is done
		and the client is completed.
	 */
		{
	public:
		static CSwiObserverWaiter* NewLC(CSwiObserverSession* aSession, const RMessage2& aMessage);
		virtual ~CSwiObserverWaiter();
		void Startup();
	private:
		CSwiObserverWaiter(CSwiObserverSession* aSession, const RMessage2& aMessage);
		
		// implement CActive
		void DoCancel();
		};

	
	} // End of namesapce Swi

#endif