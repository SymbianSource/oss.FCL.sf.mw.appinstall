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

//////////////////////////////
///CCompViewSubsessionContext
/////////////////////////////

CAppInfoViewSubsessionContext::~CAppInfoViewSubsessionContext()
    {
    iApps.Close();
    }

/////////////////////////
///CAppInfoViewSubsession
////////////////////////

CAppInfoViewSubsession* CAppInfoViewSubsession::NewL(CScrSession& aSession)
    {
    CAppInfoViewSubsession* self = new(ELeave) CAppInfoViewSubsession(aSession);
    CleanupClosePushL(*self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CAppInfoViewSubsession::CAppInfoViewSubsession(CScrSession &aSession) : CScrSubsession(aSession)
    {   
    }

void CAppInfoViewSubsession::ConstructL()
/**
    Second-phase constructor.
 */
    {
    iSubsessionContext = new (ELeave) CAppInfoViewSubsessionContext;       
    CScrSubsession::ConstructL();
    }

CAppInfoViewSubsession::~CAppInfoViewSubsession()
/**
    Destructor for the subsession object.
 */
    {
    delete iAppInfoFilter;   
    if(NULL != iApaAppInfo)
        delete iApaAppInfo;
    delete iSubsessionContext;
    }

TBool CAppInfoViewSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
    Implement CFileListSubsession by handling the supplied message.

    @param  aFunction       Function identifier without SCS code.
    @param  aMessage        Standard server-side handle to message.
    @return ETrue means complete client request now. EFalse for asynchronous tasks.
 */
    {
    TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
    
    switch (f)
        {
        case EOpenAppInfoView:
            {
            DeleteObjectZ(iAppInfoFilter);
            iAppInfoFilter = iServer->RequestImpL()->ReadAppInfoFilterL(aMessage);
            DeleteObjectZ(iStatement);
            iSubsessionContext->iLocale = TLanguage(aMessage.Int1());
    
            if(iSubsessionContext->iLocale == KUnspecifiedLocale)
                {
                iSubsessionContext->iLocale = User::Language();
                }
            iSubsessionContext->iAppInfoIndex = 0;
            iServer->RequestImpL()->OpenAppInfoViewL(*iAppInfoFilter, this->iSubsessionContext);
            break;
            }
        case EGetNextAppInfoSize:
             iServer->RequestImpL()->NextAppInfoSizeL(aMessage, iApaAppInfo, this->iSubsessionContext);
             break;
        case EGetNextAppInfoData:
             iServer->RequestImpL()->NextAppInfoDataL(aMessage, iApaAppInfo);
             break;
   
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    return ETrue;
    }

///////////////////////////////////////////
///CApplicationRegInfoSubsessionContext
//////////////////////////////////////////
CApplicationRegInfoSubsessionContext::~CApplicationRegInfoSubsessionContext()
    {
    delete iAppServiceInfoFilter;   
    iServiceInfoArray.ResetAndDestroy(); 
    }

/////////////////////////////////
///CApplicationRegInfoSubsession
////////////////////////////////
CApplicationRegInfoSubsession* CApplicationRegInfoSubsession::NewL(CScrSession &aSession)
/**
    Factory function allocates a new instance of CAppInfoQuerySubsession.
    
    Note that CAppInfoQuerySubsession derives from CObject and therefore it MUST NOT be deleted directly,
    instead it should be closed.
     
    @return                 The newly created instance of CAppInfoQuerySubsession.
                            Ownership is transferred to the caller.
 */
    {
    CApplicationRegInfoSubsession* self = new(ELeave) CApplicationRegInfoSubsession(aSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CApplicationRegInfoSubsession::CApplicationRegInfoSubsession(CScrSession &aSession)
/**
    This constructor is private and prevents direct instantiation. It provides
    a single point of definition from which to call the superclass c'tor.
 */
    :   CScrSubsession(aSession)
    {
    // empty.
    }

void CApplicationRegInfoSubsession::ConstructL()
/**
    Second-phase constructor.
 */
    {
    iSubsessionContext = new (ELeave) CApplicationRegInfoSubsessionContext;       
    CScrSubsession::ConstructL();
    }

CApplicationRegInfoSubsession::~CApplicationRegInfoSubsession()
/**
    Destructor for the subsession object.
 */
    {       
    delete iSubsessionContext;
    }

TBool CApplicationRegInfoSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
    Implement CAppInfoQuerySubsession by handling the supplied message.

    @param  aFunction       Function identifier without SCS code.
    @param  aMessage        Standard server-side handle to message.
    @return ETrue means complete client request now. EFalse for asynchronous tasks.
 */
    {
    TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
    
    switch (f)
        {        
        case EGetAppForDataTypeAndService:
            iServer->RequestImpL()->GetAppForDataTypeAndServiceL(aMessage);
            break;
        case EGetAppForDataType:
            iServer->RequestImpL()->GetAppForDataTypeL(aMessage);
            break;                
        case EGetAppServiceInfoSize:
            iServer->RequestImpL()->GetAppServiceInfoSizeL(aMessage, this->iSubsessionContext);
            break;
        case EGetAppServiceInfoData:
            iServer->RequestImpL()->GetAppServiceInfoDataL(aMessage, this->iSubsessionContext);
            break;
        case EGetApplicationInfo:
            iServer->RequestImpL()->GetApplicationInfoL(aMessage);
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    return ETrue;
    }

///////////////////////////////////////////
///CRegInfoForApplicationSubsessionContext
//////////////////////////////////////////
CRegInfoForApplicationSubsessionContext::~CRegInfoForApplicationSubsessionContext()
    {
    iViewInfoArray.ResetAndDestroy(); 
    iServiceUidList.Close(); 
    iAppOwnedFiles.ResetAndDestroy();
    }

/////////////////////////////////////
///CRegInfoForApplicationSubsession
////////////////////////////////////
CRegInfoForApplicationSubsession* CRegInfoForApplicationSubsession::NewL(CScrSession &aSession)
/**
    Factory function allocates a new instance of CAppInfoQueryContextSubsession.
    
    Note that CAppInfoQueryContextSubsession derives from CObject and therefore it MUST NOT be deleted directly,
    instead it should be closed.
     
    @return                 The newly created instance of CAppInfoQueryContextSubsession.
                            Ownership is transferred to the caller.
 */
    {
    CRegInfoForApplicationSubsession* self = new(ELeave) CRegInfoForApplicationSubsession(aSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CRegInfoForApplicationSubsession::CRegInfoForApplicationSubsession(CScrSession &aSession)
/**
    This constructor is private and prevents direct instantiation. It provides
    a single point of definition from which to call the superclass c'tor.
 */
    :   CScrSubsession(aSession)
    {
    // empty.
    }

void CRegInfoForApplicationSubsession::ConstructL()
/**
    Second-phase constructor.
 */
    {
    iSubsessionContext = new (ELeave) CRegInfoForApplicationSubsessionContext;       
    CScrSubsession::ConstructL();
    }

CRegInfoForApplicationSubsession::~CRegInfoForApplicationSubsession()
/**
    Destructor for the subsession object.
 */
    {   
    delete iSubsessionContext;
    }

TBool CRegInfoForApplicationSubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
    Implement CAppInfoQueryContextSubsession by handling the supplied message.

    @param  aFunction       Function identifier without SCS code.
    @param  aMessage        Standard server-side handle to message.
    @return ETrue means complete client request now. EFalse for asynchronous tasks.
 */
    {
    TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
    
    switch (f)
        {        
        case ESetAppUid:
            {
            TUid appUid = TUid::Uid(aMessage.Int0());
            if(!(iServer->RequestImpL()->CheckIfAppUidExistsL(appUid)))
                {
                User::Leave(KErrNotFound);
                }
            iAppUid = appUid;
            iServer->RequestImpL()->SetLocaleForRegInfoForApplicationSubsessionContextL(aMessage, this->iSubsessionContext);
            break;
            }
        case EGetServiceUidSize:
            iServer->RequestImpL()->GetServiceUidSizeL(aMessage,iAppUid, this->iSubsessionContext);
            break;
        case EGetServiceUidList:
            iServer->RequestImpL()->GetServiceUidDataL(aMessage, this->iSubsessionContext);
            break;
        case EGetApplicationLanguage:
            iServer->RequestImpL()->GetApplicationLanguageL(aMessage,this->iSubsessionContext);
            break;
        case EGetDefaultScreenNumber:
            iServer->RequestImpL()->GetDefaultScreenNumberL(aMessage,iAppUid);           
            break;
        case EGetNumberOfOwnDefinedIcons:
            iServer->RequestImpL()->GetNumberOfOwnDefinedIconsL(aMessage,iAppUid,this->iSubsessionContext);
            break;   
		case EGetViewSize:
            iServer->RequestImpL()->GetViewSizeL(aMessage, iAppUid, this->iSubsessionContext);
            break;
        case EGetViewData:
            iServer->RequestImpL()->GetViewDataL(aMessage, this->iSubsessionContext);
            break;
        case EGetAppOwnedFilesSize:
            iServer->RequestImpL()->GetAppOwnedFilesSizeL(aMessage, iAppUid, this->iSubsessionContext);
            break;
        case EGetAppOwnedFilesData:
            iServer->RequestImpL()->GetAppOwnedFilesDataL(aMessage, this->iSubsessionContext);
            break;
        case EGetAppCharacteristics:
            iServer->RequestImpL()->GetAppCharacteristicsL(aMessage, iAppUid);
            break;
        case EGetAppIconForFileName:
            iServer->RequestImpL()->GetAppIconForFileNameL(aMessage, iAppUid, this->iSubsessionContext);
            break;
        case EGetAppViewIconFileName:
            iServer->RequestImpL()->GetAppViewIconFileNameL(aMessage, iAppUid, this->iSubsessionContext);
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    return ETrue;
    }


////////////////////////////////////////////
///CAppRegistrySubsessionContext
///////////////////////////////////////////

CAppRegistrySubsessionContext::~CAppRegistrySubsessionContext()
    {
    iAppUids.Close();
    }

////////////////////////////
///CAppRegistrySubsession
////////////////////////////

CAppRegistrySubsession* CAppRegistrySubsession::NewL(CScrSession& aSession)
    {
    CAppRegistrySubsession* self = new(ELeave) CAppRegistrySubsession(aSession);
    CleanupClosePushL(*self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CAppRegistrySubsession::CAppRegistrySubsession(CScrSession &aSession)
/**
    This constructor is private and prevents direct instantiation. It provides
    a single point of definition from which to call the superclass c'tor.
 */
    :   CScrSubsession(aSession)
    {   
    }

void CAppRegistrySubsession::ConstructL()
/**
    Second-phase constructor.
 */
    {
    iSubsessionContext = new (ELeave) CAppRegistrySubsessionContext;       
    CScrSubsession::ConstructL();
    }

CAppRegistrySubsession::~CAppRegistrySubsession()
/**
    Destructor for the subsession object.
 */
    {
    if(NULL != iApplicationRegistrationInfo)
        delete iApplicationRegistrationInfo;
    delete iSubsessionContext;
    }

TBool CAppRegistrySubsession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
    Implement CFileListSubsession by handling the supplied message.

    @param  aFunction       Function identifier without SCS code.
    @param  aMessage        Standard server-side handle to message.
    @return ETrue means complete client request now. EFalse for asynchronous tasks.
 */
    {
    TScrSessionFunction f = static_cast<TScrSessionFunction>(aFunction);
    
    switch (f)
        {
        case EOpenApplicationRegistrationInfoView:
            {
            iSubsessionContext->iAppRegIndex = 0;
            iSubsessionContext->iAppUids.Reset();
            iServer->RequestImpL()->OpenApplicationRegistrationViewL(aMessage,this->iSubsessionContext);            
            break;
            }
        case EGetApplicationRegistrationSize:
             iServer->RequestImpL()->NextApplicationRegistrationInfoSizeL(aMessage, iApplicationRegistrationInfo, this->iSubsessionContext);
             break;
        case EGetApplicationRegistrationData:
             iServer->RequestImpL()->NextApplicationRegistrationInfoDataL(aMessage, iApplicationRegistrationInfo);
             break;
        case EOpenApplicationRegistrationInfoForAppUidsView:
            {
            iSubsessionContext->iAppRegIndex = 0;
            iSubsessionContext->iAppUids.Reset();
            iServer->RequestImpL()->OpenApplicationRegistrationForAppUidsViewL(aMessage,this->iSubsessionContext);            
            break;
            }
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    return ETrue;
    }
