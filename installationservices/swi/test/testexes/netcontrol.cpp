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
*
*/


#include <e32base.h>
#include <es_sock.h>
#include <es_enum.h>

TInt MainL(void)
	{
	RSocketServ serv;
	User::LeaveIfError(serv.Connect());
	CleanupClosePushL(serv);

	RConnection conn;
	User::LeaveIfError(conn.Open(serv));
	CleanupClosePushL(conn);
	
	TUint connCount(0);
	User::LeaveIfError(conn.EnumerateConnections(connCount));
	
	TConnectionInfo info;
	TPckg<TConnectionInfo> infoPckg(info);
	
	for (TInt i = 1; i <= connCount; ++i)
		{
		User::LeaveIfError(conn.GetConnectionInfo(i, infoPckg));
		User::LeaveIfError(conn.Attach(infoPckg, RConnection::EAttachTypeNormal));
		TInt err = conn.Stop();
		}
		
	CleanupStack::PopAndDestroy(2, &serv); // conn
	return 0;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
	}
