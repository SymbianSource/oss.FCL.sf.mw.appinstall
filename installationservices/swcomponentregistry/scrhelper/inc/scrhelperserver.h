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
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRHELPERSERVER_H
#define SCRHELPERSERVER_H

#include <scs/scsserver.h>
#include <e32cmn.h>
#include <f32file.h> 

namespace Usif
	{
	class CScrHelperServer : public CScsServer
		{
	public:
		static CScrHelperServer* NewLC();
		~CScrHelperServer();
		
		inline RFs& FileServer();
		
		//Implement from CScsServer
		CScsSession* DoNewSessionL(const RMessage2& aMessage);
		
	private:
		CScrHelperServer();
		void ConstructL();
	
	private:
		RFs iFs; ///< File server session handle.
		};	
	
	
	class CScrHelperSession : public CScsSession
		{
	public:
		static CScrHelperSession* NewL(CScrHelperServer &aServer, const RMessage2& aMessage);
		~CScrHelperSession();
		
		// implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);	
		
	private:
		CScrHelperSession(CScrHelperServer &aServer);
		void GetFileHandleL(RFs& aFs, const RMessage2& aMessage, const TDesC& aFileName);
		};
	
	} // namespace Usif

#endif /* SCRHELPERSERVER_H */
