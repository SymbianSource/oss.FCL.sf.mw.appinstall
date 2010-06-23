/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines server-side classes which are used to implement the SCR server.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 

#ifndef SCRSERVER_H
#define SCRSERVER_H

#include <scs/scsserver.h>
#include "scrcommon.h"
#include "usifcommon.h"

_LIT_SECURE_ID(KSisRegistryServerSid, 0x10202DCA); 
_LIT_SECURE_ID(KApparcServerSid, 0x10003A3F);

namespace Usif
	{
	// forward declarations
	class CScrServer;
	class CScrRequestImpl;
												
	class CScrSession : public CScsSession
	/**
		The instance of this class is used to perform SCR client requests and 
		is created for each client. 
	 */
		{
	public:
		static CScrSession* NewL(CScrServer &aServer, const RMessage2& aMessage);
		~CScrSession();
				
		// implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
		// override CScsSession
		CScsSubsession* DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage);
		
	private:
		CScrSession(CScrServer& aServer);
		
		void ApplySubsessionConstraintL(CScrServer& aServer);
		void ApplyTransactionConstraintL(CScrServer& aServer, TBool& aCreateImplicitTransaction);
		void MutatingOperationsPreambleL(CScrServer& aServer, TScrSessionFunction aFunction, TBool& aIsTransactionImplicit);
		void MutatingOperationsPostambleL(CScrServer& aServer);
		
	private:
		TBool iTransactionOwner; 
		};
	
	class CScrServer : public CScsServer
	/**
		The CScsServer-derived object which is used to generate SCR sessions for the SCR clients.
	 */
		{
	public:
		static CScrServer* NewLC();
				
		~CScrServer();
		
		//Implement from CScsServer
		CScsSession* DoNewSessionL(const RMessage2& aMessage);
	
		inline CScrRequestImpl* RequestImpL();
		void ReconnectL();
		
		void AddSubsessionOwnerL(CScrSession* aSession);
		void RemoveSubsessionOwner(CScrSession* aSession);
		TBool IsTheOnlySubsessionOwner(CScrSession* aSession);
		TInt SubsessionCount();
		
		void SetTransactionOwner(CScrSession* aSession);
		TBool IsTransactionInProgress();
		TBool IsTransactionOwner(CScrSession* aSession);
		virtual void DoPreHeapMarkOrCheckL();
		virtual void DoPostHeapMarkOrCheckL();
	protected:
		CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);		
		
	private:
		CScrServer();
		void ConstructL();
		void SetupL();
		CPolicyServer::TCustomResult CheckDeleteComponentAllowedL(const RMessage2& aMsg);
		CPolicyServer::TCustomResult CheckComponentIdMatchingEnvironmentL(const RMessage2& aMsg, TBool aCheckForSingleApp=EFalse);				
		CPolicyServer::TCustomResult CheckSoftwareNameMatchingEnvironmentL(const RMessage2& aMsg);
		CPolicyServer::TCustomResult CheckAllowedFilePathL(const RMessage2& aMsg);
		CPolicyServer::TCustomResult CheckClientIsInstallerL(const RMessage2& aMsg);
		CPolicyServer::TCustomResult CheckCommonComponentPropertySettableL(const RMessage2& aMsg, TCapability aRequiredCapability);
		
	private:
		RFs iFs;										///< File server session handle.
		CScrRequestImpl* iRequestImpl;					///< Pointer to the request implementation object.
		CScrSession* iTransactionOwningSession; 		///< Pointer to the session which is owner of the current transaction. Owned by another object. NULL means there is no transaction in progress.
		RPointerArray<CScrSession> iSubsessionOwners;   ///< The list of sessions which own active subsessions
		RFile iDatabaseFile;							///< The database file handle
		RFile iJournalFile;								///< The journal file handle
		};
	
	inline CScrRequestImpl* CScrServer::RequestImpL()
		{
		if(!iRequestImpl)
			{
			User::Leave(KErrArgument);
			}
		return iRequestImpl;
		}
	
	} // End of namespace SCRSERVER_H	
#endif
