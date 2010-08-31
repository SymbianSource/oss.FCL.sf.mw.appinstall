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
* SisLauncher - session definition
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <e32base.h>
#include "sislauncherserver.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK  
#include <usif/scr/appregentries.h>
#include "nativecomponentinfo.h"
#endif
namespace Swi
{
class CSisRegistryFileDescription;
class CThreadMonitor;


class CSisLauncherSession : public CSession2
	{
public:
	CSisLauncherSession();
	void CreateL();
	
private:
	
	~CSisLauncherSession();
	inline CSisLauncherServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	inline TBool ReceivePending() const;

	void CheckApplicationInUseL(TUid aUid);

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void ParseSwTypeRegFileL(const RMessage2& aMessage);
    TInt AsyncParseResourceFileSizeL(const RMessage2& aMessage);
    void AsyncParseResourceFileDataL(const RMessage2& aMessage);    
#endif

#ifndef SWI_TEXTSHELL_ROM
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void RegisterSifLauncherMimeTypesL(const RMessage2& aMessage);
	void UnregisterSifLauncherMimeTypesL(const RMessage2& aMessage);
	void RegisterSifLauncherMimeTypesImplL(const RMessage2& aMessage, TBool aRegister);
	void NotifyApparcForAppsL(const RMessage2& aMessage);
#endif
#endif
	void DoRunExecutableL(const RMessage2& aMessage);
	void DoStartDocumentL(const RMessage2& aMessage);
	void DoStartDocumentByHandleL(const RMessage2& aMessage);
	void DoStartByMimeL(const RMessage2& aMessage);
	void DoStartByMimeByHandleL(const RMessage2& aMessage);


private:
	RMessagePtr2 iReceiveMsg;
	TInt iReceiveLen;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
    Usif::CApplicationRegistrationData* iCurrentAppRegData;
    TBool iInAsyncParseResourceFile;
#endif

	};

// CSisLauncherSession inlines
inline TBool CSisLauncherSession::ReceivePending() const
	{return !iReceiveMsg.IsNull();}

inline CSisLauncherServer& CSisLauncherSession::Server()
	{return *static_cast<CSisLauncherServer*>(const_cast<CServer2*>(CSession2::Server()));}

} //namespace
