/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Install Server
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __INSTALLSERVER_H__
#define __INSTALLSERVER_H__

#include "securitypolicy.h"

namespace Swi
{

/**
 * Timer that shuts down SWIS if no sessions are open
 * @internalTechnology
 * @released
 */
class CInstallServerShutdown : public CTimer
	{
private:
	/** Server timeout shutdown delay (approximately 2 seconds) */
	enum {KShutdownDelay=0x200000};
public:
	virtual ~CInstallServerShutdown();
	/** Constructs shutdown timer */
	inline CInstallServerShutdown();
	/** Second-phase construction */
	inline void ConstructL();
	/** Starts server shutdown when the timer goes off */
	inline void Start();
private:
	void RunL();
	void DeleteTempRscFilesL();
	};

/**
 * The main Install Server (SWIS) class.
 * @internalTechnology
 * @released
 */
class CInstallServer : public CPolicyServer
	{
public:
	/**
	 * This function creates a new CInstallServer object and leaves it on 
	 * the cleanup stack.
	 */
	static CInstallServer* NewLC();
	/** Increments session count */
	void AddSession();
	/** 
	 * Decrements session count and starts shutdown timer if session count is 0
	 */
	void DropSession();
	virtual ~CInstallServer();

	TBool IsListRemoveInProgress() const;
	void ListRemoveInProgress(TBool aFlag);
private:
	CInstallServer();
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
		
	virtual TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, 
		TInt& aAction, TSecurityInfo& aMissing);

private:
	// The number of sessions that are connected to the server. When this 
	// number reaches 0, the server will initialize its shutdown.
	TInt iSessionCount;
	CInstallServerShutdown* iShutdown;

	// Server Policies
  	static const TUint iRangeCount=1;
  	static const TInt iRanges[iRangeCount];
  	static const TUint8 iElementsIndex[iRangeCount];
   	static const CPolicyServer::TPolicy iPolicy;

	// Flag to indicate a list/remove operation is in progress
	TBool iListRemoveInProgress;
	};

inline TBool CInstallServer::IsListRemoveInProgress() const
	{
	return iListRemoveInProgress;
	}

inline void CInstallServer::ListRemoveInProgress(TBool aFlag)
	{
	iListRemoveInProgress = aFlag;
	}

class CSwisStateMachine;
class CRestoreMachine;
class CPackageRemover;
/**
 * Server-side session that handles SWIS tasks. For client-side class see 
 * RInstallSession.
 * @released
 * @internalTechnology
 */
class CInstallSession : public CSession2
	{
public:
	CInstallSession();
	void CreateL();

private:
	RMessagePtr2 iReceiveMsg;
	TInt iReceiveLen;
	CSwisStateMachine* iMachine;
	CPackageRemover* iRemover;
	CRestoreMachine* iRestoreMachine;
	RSecPolHandle iSecPolHandle;
	
	virtual ~CInstallSession();
	/** Returns server reference */
	CInstallServer& Server();
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);
	TBool ReceivePending() const;
	};

inline CInstallSession::CInstallSession() : CSession2() {}

inline TBool CInstallSession::ReceivePending() const
	{
	return !iReceiveMsg.IsNull();
	}

} // namespace Swi

#endif // __INSTALLSERVER_H__
