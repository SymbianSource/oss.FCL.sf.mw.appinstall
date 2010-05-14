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


#ifndef __STSRECOVERY_H__
#define __STSRECOVERY_H__

#include <scs/scsclient.h>
#include <f32file.h>
#include <usif/sts/stsdefs.h>

namespace Usif
{

   /**
	 This class provides provides synchronous interface to the STS server for rolling back all pending transactions.

	 @internalComponent
	 @released
	 */
	class RStsRecoverySession : public RScsClientBase
		{
	public:
		IMPORT_C RStsRecoverySession();

		/**
		 Connects to the STS server and instructs it to roll back all pending transactions. 
		 Does nothing if there are no pending transactions.
		 @leave system wide error codes
		 */
		IMPORT_C void RollbackAllPendingL();

		/**
		 Closes this session to the STS server.
		 */
		IMPORT_C void Close();

	private:
		/**
		 Connects to the STS server and starts it if necessary
		 @leave system wide error codes
		 */
		void ConnectL();

		};//class RStsRecoverySession   
	}////namespace Usif

#endif
