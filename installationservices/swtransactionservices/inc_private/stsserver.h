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
* Defines server-side classes which are used to implement the STS server.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 

#ifndef __STSSERVER_H__
#define __STSSERVER_H__

#include <f32file.h>
#include <scs/scsserver.h>
#include <usif/sts/stsdefs.h>
#include "stscommon.h"
#include "integrityservices.h"
#include "transactionwrapper.h"

namespace Usif
	{
	class CStsServerSession;
	
	class CStsServer : public CScsServer
	/**
		The CScsServer-derived object which is used to generate STS sessions for the STS clients.
	 */
		{
		friend class CStsServerSession;
		
		public:
			static CStsServer* NewLC();
					
			~CStsServer();
			
			//Implement from CScsServer
			CScsSession* DoNewSessionL(const RMessage2& aMessage);
		
			//Transaction management functions
			CIntegrityServices* CreateTransactionL();
			CIntegrityServices* AttachTransactionL(TStsTransactionId aTransactionID);
			void ReleaseTransactionL(CIntegrityServices* &aTransactionPtr, TBool aMarkAsCompleted=EFalse);
			void FinaliseTransactionL(CIntegrityServices* &aTransactionPtr);
			
		protected:
			CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);		

		private:
			CStsServer();
			void ConstructL();
			//Transaction management helper functions
			TInt FindActiveTransaction(TStsTransactionId aTransactionID);
			TStsTransactionId CreateTransactionID();
			TBool  IsExistingTransaction(TStsTransactionId aTransactionID);
			
			//custom security check
			TBool CheckIfFileModificationAllowedL(const RMessage2& aMsg);
			
			//rolls back all pending transactions
			void RollBackAllPendingL();
			
		private:
//			RFs iFs;						
			RPointerArray<CReferenceCountedTransactionWrapper> iTransactionWrapperContainer;
			}; //End of class CStsServer
	
	} // End of namespace USIF				
#endif
