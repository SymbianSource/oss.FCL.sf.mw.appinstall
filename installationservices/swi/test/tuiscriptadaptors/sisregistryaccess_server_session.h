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
* SisRegistryAccess - server and session classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/
#ifndef __SISREGISTRYACCESSSERVER_H__
#define __SISREGISTRYACCESSSERVER_H__

#include <e32base.h>
#include "sisregistrywritablesession.h"

class CController;
//////////////////////// Server declarations /////////////////////////

enum TSisRegistryAccessPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction
	};

void PanicClient(const RMessagePtr2& aMessage, TSisRegistryAccessPanic aPanic);

class CSisRegistryAccessServer : public CServer2
	{
public:
	static CServer2* NewLC();
	virtual ~CSisRegistryAccessServer();
	void AddSession();
	void DropSession();
	inline Swi::RSisRegistryWritableSession& RegistrySession();
private:
	CSisRegistryAccessServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	Swi::RSisRegistryWritableSession iRegistry;
	};

inline Swi::RSisRegistryWritableSession& CSisRegistryAccessServer::RegistrySession()
	{
	return iRegistry;
	}

//////////////////////// Session declarations /////////////////////////

class CSisRegistryAccessSession : public CSession2
	{
public:
	CSisRegistryAccessSession();
	void CreateL();
private:
	~CSisRegistryAccessSession();
	inline CSisRegistryAccessServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);
private:
	RMessagePtr2 iReceiveMsg;
	};

inline CSisRegistryAccessServer& CSisRegistryAccessSession::Server()
	{return *static_cast<CSisRegistryAccessServer*>(const_cast<CServer2*>(CSession2::Server()));}

#endif	// __SISREGISTRYACCESSSERVER_H__

