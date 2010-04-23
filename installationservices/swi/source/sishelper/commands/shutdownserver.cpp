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
* Definition of the shutdown server command handler.
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#include <e32base.h>

#include "uisssession.h"
#include "sisuihandler.h"

namespace Swi
{
// CShutdownServer

EXPORT_C CShutdownServer* CShutdownServer::NewL()
	{
	return new(ELeave)CShutdownServer;
	}

EXPORT_C CShutdownServer* CShutdownServer::NewLC()
	{
	CShutdownServer* self=new(ELeave)CShutdownServer;
	CleanupStack::PushL(self);
	return self;
	}

CShutdownServer::CShutdownServer()
	: CUissCmd(-1)  
	{
	}

void CShutdownServer::MarshallArgumentsL(TIpcArgs& /*aArguments*/)
	{
	}

} // namespace Swi
