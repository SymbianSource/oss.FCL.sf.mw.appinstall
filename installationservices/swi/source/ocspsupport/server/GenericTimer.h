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
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef GENERICTIMER_H
#define GENERICTIMER_H

#include <e32base.h>

/**
* This mixin is passed into a CGenericTimer on construction. When the
* timer expires, it will call the client's HandleTimeout method
* 
* @internalComponent
* @released
*/
class MGenericTimerClient
	{
public:
	/** Client's implementation gets called on timer expiry */
	virtual void HandleTimeout() = 0;
	};

/** Simple timer class which calls a callback on timer expiry */
class CGenericTimer : public CTimer
	{
public:
	static CGenericTimer* NewL(MGenericTimerClient& aClient);

private:
	CGenericTimer(MGenericTimerClient& aClient);
	virtual void RunL();

private:
	MGenericTimerClient& iClient;
	};

#endif
