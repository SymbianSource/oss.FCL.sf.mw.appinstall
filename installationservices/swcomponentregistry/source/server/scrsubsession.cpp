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
* Implements CScrSubsession. See class and function definitions for more information.
*
*/


/**
 @file
*/

#include "scrsubsession.h"
#include "scrrequestimpl.h"
#include "scrdatabase.h"
#include <scs/ipcstream.h>
#include <scs/cleanuputils.h>

using namespace Usif;

/////////////////
// CScrSubsession
/////////////////

CScrSubsession::CScrSubsession(CScrSession &aSession)
/**
	This constructor is protected and prevents direct instantiation.
 */
	: CScsSubsession(aSession)
	{
	iSession = static_cast<CScrSession *>(&aSession);
	iServer = static_cast<CScrServer *>(&iSession->iServer);
	}

void CScrSubsession::ConstructL()
/**
	Second-phase constructor.
 */
	{
	iServer->AddSubsessionOwnerL(iSession);
	}

CScrSubsession::~CScrSubsession()
/**
 	Virtual destructor. Closes the database session handle.
 */
	{
	iServer->RemoveSubsessionOwner(iSession);
	delete iStatement;
	}
	
///////////////////////////
// CCompViewSubsessionContext
///////////////////////////	
	
CCompViewSubsessionContext::~CCompViewSubsessionContext()
	{
	delete iLastSoftwareTypeName;
	iComponentFilterSuperset.Close();
	}
	
///////////////////////////
// CComponentViewSubsession
///////////////////////////

CComponentViewSubsession* CComponentViewSubsession::NewL(CScrSession &aSession)
/**
	Factory function allocates a new instance of CComponentViewSubsession.
	
	Note that CComponentViewSubsession derives from CObject and therefore it MUST NOT be deleted directly,
	instead it should be closed.
	 
	@return					The newly created instance of CComponentViewSubsession.
							Ownership is transferred to the caller.
 */
	{
	CComponentViewSubsession* self = new(ELeave) CComponentViewSubsession(aSession);
	CleanupClosePushL(*self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CComponentViewSubsession::CComponentViewSubsession(CScrSession &aSession)
/**
	This constructor is private and prevents direct instantiation. It provides
	a single point of definition from which to call the superclass c'tor.
 */
	:	CScrSubsession(aSession)
	{	
	}

void CComponentViewSubsession::ConstructL()
/**
	Second-phase constructor.
 */
	{
	iSubsessionContext = new (ELeave) CCompViewSubsessionContext;		
	CScrSubsession::ConstructL();
	}

CComponentViewSubsession::~CComponentViewSubsession()
/**
	Destructor for the subsession object.
 */
	{
	delete iFilter;
	delete iComponentEntry;
	iComponentEntryList.ResetAndDestroy();
	delete iSubsessionContext;
	}

TBool CComponentViewSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CComponentViewSubsession by handling the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.
	@return ETrue means complete client request now. EFalse for asynchronous tasks.
 */
	{
	TScrSubSessionFunction f = static_cast<TScrSubSessionFunction>(aFunction);
	
	switch (f)
		{
		case EOpenComponentsView:
			{
			DeleteObjectZ(iFilter);
			iFilter = iServer->RequestImpL()->ReadComponentFilterL(aMessage);
			DeleteObjectZ(iStatement);
			iStatement = iServer->RequestImpL()->OpenComponentViewL(*iFilter, iSubsessionContext->iComponentFilterSuperset, iSubsessionContext->iFilterSupersetInUse);
			break;
			}
		case EGetNextComponentSize:
			iServer->RequestImpL()->NextComponentSizeL(aMessage, iStatement, iFilter, iComponentEntry, this->iSubsessionContext);
			break;
		case EGetNextComponentData:
			iServer->RequestImpL()->NextComponentDataL(aMessage, iComponentEntry);
			break;
		case EGetNextComponentSetSize:
			iServer->RequestImpL()->NextComponentSetSizeL(aMessage, iStatement, iFilter, iComponentEntryList, this->iSubsessionContext);
			break;
		case EGetNextComponentSetData:
			iServer->RequestImpL()->NextComponentSetDataL(aMessage, iComponentEntryList);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	return ETrue;
	}


///////////////////////
// CFileListSubsession
///////////////////////

CFileListSubsession* CFileListSubsession::NewL(CScrSession &aSession)
/**
	Factory function allocates a new instance of CFileListSubsession.
	
	Note that CFileListSubsession derives from CObject and therefore it MUST NOT be deleted directly,
	instead it should be closed.
	 
	@return					The newly created instance of CFileListSubsession.
							Ownership is transferred to the caller.
 */
	{
	CFileListSubsession* self = new(ELeave) CFileListSubsession(aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFileListSubsession::CFileListSubsession(CScrSession &aSession)
/**
	This constructor is private and prevents direct instantiation. It provides
	a single point of definition from which to call the superclass c'tor.
 */
	:	CScrSubsession(aSession)
	{
	// empty.
	}

CFileListSubsession::~CFileListSubsession()
/**
	Destructor for the subsession object.
 */
	{		
	delete iFilePath;
	iFileList.Close();
	}

TBool CFileListSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CFileListSubsession by handling the supplied message.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.
	@return ETrue means complete client request now. EFalse for asynchronous tasks.
 */
	{
	TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
	
	switch (f)
		{
		case EOpenFileList:
			{
			DeleteObjectZ(iStatement);
			iStatement = iServer->RequestImpL()->OpenFileListL(aMessage);
			break;
			}
		case EGetNextFileSize:
			iServer->RequestImpL()->NextFileSizeL(aMessage, iStatement, iFilePath);
			break;
		case EGetNextFileData:
			iServer->RequestImpL()->NextFileDataL(aMessage, iFilePath);
			break;
		case EGetNextFileSetSize:
			iServer->RequestImpL()->NextFileSetSizeL(aMessage, iStatement, iFileList);
			break;
		case EGetNextFileSetData:
			iServer->RequestImpL()->NextFileSetDataL(aMessage, iFileList);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	return ETrue;
	}
