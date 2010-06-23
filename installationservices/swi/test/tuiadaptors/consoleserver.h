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
* Console Server
*
*/


#ifndef __CONSOLESERVER_H__
#define __CONSOLESERVER_H__

#include <e32base.h>
#include <e32cons.h>

namespace Swi
{

namespace Test
 {

/**
 * @internalTechnology
 * @test
 */
class CConsoleServerShutdown : public CTimer
	{
private:
	/** Server timeout shutdown delay (approximately 2 seconds) */
	enum {KShutdownDelay=0x200000};
public:
	static CConsoleServerShutdown* NewL();
	/** Constructs shutdown timer */
	inline CConsoleServerShutdown();
	/** Second-phase construction */
	inline void ConstructL();
	/** Starts server shutdown when the timer goes off */
	inline void Start();
private:
	void RunL();
	};

/**
 * @internalTechnology
 * @test
 */
class CConsoleServer : public CServer2
	{
public:
	/**
	 * This function creates a new CInstallServer object and leaves it on 
	 * the cleanup stack.
	 */
        static CConsoleServer* NewLC();
	
	/** Increments session count */
	void AddSession();
	
	/** 
	 * Decrements session count and starts shutdown timer if session count is 0
	 */
	void DropSession();

	virtual ~CConsoleServer();

private:
	CConsoleServer();

	/**
	 * The second-phase constructor.
	 */
	void ConstructL();

private:
	/**
	 * This function creates a new CInstallSession object.
	 */
	virtual CSession2* NewSessionL(const TVersion& aVersion, 
				       const RMessage2& aMessage) const;

private:
	// The number of sessions that are connected to the server. When this 
	// number reaches 0, the server will initialize its shutdown.
	TInt iSessionCount;
	CConsoleServerShutdown* iShutdown;
	CConsoleBase* iConsole;
	};


/**
 * @test
 * @internalTechnology
 */
class CConsoleSession : public CSession2
	{
public:
	CConsoleSession(CConsoleBase& aConsole);
	void CreateL();

private:
	RMessagePtr2 iReceiveMsg;
	TInt iReceiveLen;

	virtual ~CConsoleSession();

	/** Returns server reference */
	CConsoleServer& Server();
	
	virtual void ServiceL(const RMessage2& aMessage);

	virtual void ServiceError(const RMessage2& aMessage, TInt aError);

	TBool ReceivePending() const;

	CConsoleBase& iConsole;
	};


inline TBool CConsoleSession::ReceivePending() const
	{
	return !iReceiveMsg.IsNull();
	}

 } // namespace Test

} // namespace Swi

#endif // __CONSOLESERVER_H__
