/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Common header for the server and the client.
*
*/


#ifndef __TINTEGRITYSERVICESSERVER_H__
#define __TINTEGRITYSERVICESSERVER_H__

#include <e32base.h>
#include "integrityservices.h"
#include "tintegrityservicescommon.h"

namespace Usif
{

enum TIntegrityServicesServerPanic
	{
	EPanicIntegrityServicesServerIllegalFunction = 10,
	};


/**
 * @test
 * @internalTechnology
 */
class CShutdownTimer : public CTimer
	{
private:
	/** Server timeout shutdown delay (approximately 2 seconds) */
	enum {KShutdownDelay=0x200000};
public:
	static CShutdownTimer* NewL();
	/** Constructs shutdown timer */
	inline CShutdownTimer();
	/** Second-phase construction */
	inline void ConstructL();
	/** Starts server shutdown when the timer goes off */
	inline void Start();
private:
	void RunL();
	};

/**
 * @test
 * @internalTechnology
 */
class CIntegrityServicesServer : public CServer2
	{
public:
	/**
	 * This function creates a new CServer2 object and leaves it on 
	 * the cleanup stack.
	 */
        static CIntegrityServicesServer* NewLC();
	
	/** Increments session count */
	void AddSession();
	
	/** 
	 * Decrements session count and starts shutdown timer if session count is 0
	 */
	void DropSession();

	virtual ~CIntegrityServicesServer();

private:
	CIntegrityServicesServer();

	/**
	 * The second-phase constructor.
	 */
	void ConstructL();

private:
	/**
	 * This function creates a new session object.
	 */
	virtual CSession2* NewSessionL(const TVersion& aVersion, 
				       const RMessage2& aMessage) const;

private:
	// The number of sessions that are connected to the server. When this 
	// number reaches 0, the server will initialize its shutdown.
	TInt iSessionCount;
	CShutdownTimer* iShutdown;
	};



/**
 * @test
 * @internalTechnology
 */
class CIntegrityServicesSession : public CSession2
	{
public:
 	static CIntegrityServicesSession* NewL();
	static CIntegrityServicesSession* NewLC();
	void CreateL();

private:
	CIntegrityServicesSession();

	virtual ~CIntegrityServicesSession();

	/**
	 * The second-phase constructor.
	 */
	void ConstructL();
	
	/** Returns server reference */
	CIntegrityServicesServer& Server();
	
	virtual void ServiceL(const RMessage2& aMessage);

	virtual void ServiceError(const RMessage2& aMessage, TInt aError);

	CIntegrityServices* iIntegrityServices;
	};

} // end namespace Usif
#endif // #ifndef __TINTEGRITYSERVICESSERVER_H__
