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
* stsserver.h
* Defines server-side class which is used to implement the STS server session.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 

#ifndef __STSSERVERSESSION_H__
#define __STSSERVERSESSION_H__

#include <scs/scsserver.h>
#include "stscommon.h"
#include "integrityservices.h"

namespace Usif
	{
    class CStsServer;
    
	class CStsServerSession : public CScsSession
	/**
		The instance of this class is used to service STS client requests and 
		is created for each client. 
	 */
		{
	public:
		static CStsServerSession* NewL(CStsServer &aServer, const RMessage2& aMessage);
		~CStsServerSession();
				
		// implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);

	private:
		CStsServerSession(CStsServer& aServer);
		void ConstructL();
		
	private:
		CIntegrityServices* iTransactionPtr;
		};
	
	} // End of namespace SCRSERVER_H				
#endif
