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
* uisupportserver.h
* Definition of the UI Support Server main class
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __UISUPPORTSERVER_H__
#define __UISUPPORTSERVER_H__

#include <e32base.h>
#include "installclientserver.h"

namespace Swi
{
class MUiHandler;
class CUissSession;
class MCancelHandler;
class CServerInstallWatcher;
/**
 * The main UI Support server class. This handles the creation of sessions.
 *
 * @internalComponent 
 * @released 
 */
class CUissServer : public CPolicyServer
	{
public:
	IMPORT_C static CUissServer* NewL(CActive::TPriority aPriority = 
		CActive::EPriorityStandard);
	IMPORT_C static CUissServer* NewLC(CActive::TPriority aPriority = 
		CActive::EPriorityStandard);

	~CUissServer();

	const RMessage2* InstallRequest() const;
	void SetInstallRequestL(const RMessage2* aMessage, const CUissSession* aSession);
	void ClearInstallRequest();

	const RMessage2* DialogRequest() const;
	void SetDialogRequestL(const RMessage2* aMessage, const CUissSession* aSession);
	void ClearDialogRequest();

	TBool DelayedCancel() const;
	void SetDelayedCancel(TBool aDelayedCancel=ETrue);

	void HandleDisconnect(const CUissSession* aSession);

	CServerInstallWatcher* InstallWatcher();
	void CreateInstallWatcherL();
	
	/**
	 * Signals that the main installation request to SWIS has been completed
	 *
	 * @param aResult The result fo the installation request.
	 */ 
	void SignalInstallComplete(TInt aResult);
		
private:
	CUissServer(TPriority aPriority);

	void ConstructL();
	
	/**
	 * Creates a new session.
	 */
	CSession2* NewSessionL(const TVersion& aVersion, 
		const RMessage2 &aMessage) const;
		
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	public:	
		HBufC8* iCompInfoBuffer;
		TPtr8   iCompInfoBufPtr;
		void SendBackComponentInfoL();	
	#endif

private:
	/** 
	 * Current outstanding install request from the launcher (a copy, 
	 * because otherwise the pointer gets messed up on the second session 
	 * creation
	 */
	RMessage2* iInstallRequest;
	
	/**
	 * Current outstanding dialog request from SWIS (a copy, see 
	 * iInstallRequest)
	 */
	RMessage2* iDialogRequest;

	TBool iDelayedCancel;

	CServerInstallWatcher* iInstallWatcher;

	/**
	 * Source session for current/last install request.  Used for handling
	 * disconnects.
	 */
	const CUissSession* iInstallRequestSession;

	/**
	 * Source session for current/last dialog request.  Used for handling
	 * disconnects.
	 */
	const CUissSession* iDialogRequestSession;
	
private:
	// Server Policies
  	static const TUint iRangeCount=2;
  	static const TInt iRanges[iRangeCount];
  	static const TUint8 iElementsIndex[iRangeCount];
  	static const CPolicyServer::TPolicyElement iPolicyElements[2];
  	static const CPolicyServer::TPolicy iPolicy;
	};


// inline functions from CUissServer

inline const RMessage2* CUissServer::InstallRequest() const
	{
	return iInstallRequest;
	}

inline void CUissServer::SetInstallRequestL(const RMessage2* aMessage, const CUissSession* aSession)
	{
	ClearInstallRequest();
	iInstallRequest=new(ELeave) RMessage2(*aMessage);
	iInstallRequestSession = aSession;
	}

inline void CUissServer::ClearInstallRequest()
	{
	delete iInstallRequest;
	iInstallRequest=0;
	}

inline const RMessage2* CUissServer::DialogRequest() const
	{
	return iDialogRequest;
	}

inline void CUissServer::SetDialogRequestL(const RMessage2* aMessage, const CUissSession* aSession)
	{
	ClearDialogRequest();
	iDialogRequest=new(ELeave) RMessage2(*aMessage);
	iDialogRequestSession = aSession;
	}

inline void CUissServer::ClearDialogRequest()
	{
	delete iDialogRequest;
	iDialogRequest=0;
	}

inline TBool CUissServer::DelayedCancel() const
	{
	return iDelayedCancel;
	}
inline void CUissServer::SetDelayedCancel(TBool aDelayedCancel)
	{
	iDelayedCancel=aDelayedCancel;
	}

inline CServerInstallWatcher* CUissServer::InstallWatcher()
	{
	return iInstallWatcher;
	}

} // namespace Swi

#endif // #ifndef __UISUPPORTSERVER_H__
