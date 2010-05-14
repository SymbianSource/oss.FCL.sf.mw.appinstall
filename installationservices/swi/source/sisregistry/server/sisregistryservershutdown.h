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
* CSisRegistryShutdown class definition a wrapper around a timer which is set with a delay
* when there are no more sessions (clients connected to the server)   
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISREGISTRYSERVERSHUTDOWN_H__
#define __SISREGISTRYSERVERSHUTDOWN_H__

#include <e32base.h>
const TInt KSisRegistryShutdownDelay= 0x200000;// approx 2s
namespace Swi
{

class CShutdown : public CTimer
	{
public:
	CShutdown();
	void ConstructL();
	void Start();
	
private:
	void RunL();
	};

inline CShutdown::CShutdown()
	:CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}
	
inline void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}
	
inline void CShutdown::Start()
	{
	After(KSisRegistryShutdownDelay);
	}
	
} //namespace

#endif // __SISREGISTRYSERVERSHUTDOWN_H__
