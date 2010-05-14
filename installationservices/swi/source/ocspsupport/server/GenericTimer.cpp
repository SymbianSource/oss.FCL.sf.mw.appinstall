/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of the CGenericTimer class
*
*/


#include "GenericTimer.h"

CGenericTimer* CGenericTimer::NewL(MGenericTimerClient& aClient)
	{
	CGenericTimer* self = new(ELeave) CGenericTimer(aClient);
	CleanupStack::PushL(self);
	self->ConstructL(); // calls CTimer::Construct
	CleanupStack::Pop(self);
	return self;
	}

CGenericTimer::CGenericTimer(MGenericTimerClient& aClient)
	: CTimer(-1), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

void CGenericTimer::RunL()
	{
	// When the timeout expires, then call the client's handler
	iClient.HandleTimeout();
	}
