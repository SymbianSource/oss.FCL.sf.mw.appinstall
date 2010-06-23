/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Device information server session
* @internalComponent
* @released
*
*/


#include "devinfosupportsession.h"
#include "devinfosupportserver.h"
#include "devinfofetcher.h"
#include "deviceidlist.h"

namespace Swi
{

CDeviceInfoSession* CDeviceInfoSession::NewL(CDeviceInfoServer& aServer)
	{
	CDeviceInfoSession* self=new (ELeave) CDeviceInfoSession(aServer);
	return self;
	}

CDeviceInfoSession* CDeviceInfoSession::NewLC(CDeviceInfoServer& aServer)
	{
	CDeviceInfoSession* self=new (ELeave) CDeviceInfoSession(aServer);
	CleanupStack::PushL(self);
	return self;
	}

CDeviceInfoSession::CDeviceInfoSession(CDeviceInfoServer& aServer)
	: iServer(aServer)
	{
	iServer.AddSession();
	}
	
CDeviceInfoSession::~CDeviceInfoSession()
	{
	iServer.DropSession();
	}

void CDeviceInfoSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case KMessageGetDeviceIds:
		// device ids requests are stateless (the lsit is stored in
		// the server at startup, so just forward the request to the
		// server.
		iServer.HandleGetDeviceIdsL(aMessage);
		break;
		}
	}

} // namespace Swi
