/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Scr Acessor - server and session classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef TSCRACCESSOR_SERVER_SESSION_H
#define TSCRACCESSOR_SERVER_SESSION_H

#include <e32base.h>
#include <usif/scr/scr.h>

//////////////////////// Server declarations /////////////////////////

enum TScrAccessPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction
	};

void PanicClient(const RMessagePtr2& aMessage, TScrAccessPanic aPanic);

class CScrAccessServer : public CServer2
	{
public:
	static CServer2* NewLC();
	virtual ~CScrAccessServer();
	void AddSession();
	void DropSession();
	inline Usif::RSoftwareComponentRegistry& ScrSession();
private:
	CScrAccessServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	Usif::RSoftwareComponentRegistry iScrSession;
	};

inline Usif::RSoftwareComponentRegistry& CScrAccessServer::ScrSession()
	{
	return iScrSession;
	}

//////////////////////// Session declarations /////////////////////////

class CScrAccessSession : public CSession2
	{
public:
	CScrAccessSession();
	void CreateL();
private:
	~CScrAccessSession();
	inline CScrAccessServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);
private:
	RMessagePtr2 iReceiveMsg;
	};

inline CScrAccessServer& CScrAccessSession::Server()
	{return *static_cast<CScrAccessServer*>(const_cast<CServer2*>(CSession2::Server()));}

#endif	// TSCRACCESSOR_SERVER_SESSION_H

