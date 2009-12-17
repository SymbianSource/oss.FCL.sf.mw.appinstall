/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SISREGISTRYHELPERSERVER_H
#define SISREGISTRYHELPERSERVER_H

#include <scs/scsserver.h>
#include <e32cmn.h>

namespace Swi
	{
	class CSisRegistryHelperServer : public CScsServer
		{
	public:
		static CSisRegistryHelperServer* NewLC();
		~CSisRegistryHelperServer();
		
		//Implement from CScsServer
		CScsSession* DoNewSessionL(const RMessage2& aMessage);
		
	private:
		CSisRegistryHelperServer();
		void ConstructL();
	
		};	
	
	
	class CSisRegistryHelperSession : public CScsSession
		{
	public:
		static CSisRegistryHelperSession* NewL(CSisRegistryHelperServer &aServer, const RMessage2& aMessage);
		~CSisRegistryHelperSession();
		
		// implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);	
		
	private:
		CSisRegistryHelperSession(CSisRegistryHelperServer &aServer);
		void GetEquivalentLanguagesL(const RMessage2& aMessage);
		};
	
	} // namespace SWI

#endif /* SISREGISTRYHELPERSERVER_H */
