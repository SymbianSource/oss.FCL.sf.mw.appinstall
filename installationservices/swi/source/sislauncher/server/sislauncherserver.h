/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* SisLauncher - server classes
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include <e32uid.h>
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/scr/appregentries.h>
#endif
#ifndef _CSISLAUNCHERSERVER_H_
#define _CSISLAUNCHERSERVER_H_

#ifndef SWI_TEXTSHELL_ROM
#include <w32std.h>
#endif

const TInt KTextShell = 1;
enum TSisLauncherPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessagePtr2& aMessage,TSisLauncherPanic aPanic);

namespace Swi
{

class CQueueProcessor;

class CSisLauncherServerShutdown : public CTimer
	{
private:
	/** Server timeout shutdown delay (approximately 2 seconds) */
	enum {KSisLauncherShutdownDelay=0x200000};
	/** Server long timeout shutdown delay (approximately 10 seconds) */
	enum {KSisLauncherLongShutdownDelay=10000000};
public:
	virtual ~CSisLauncherServerShutdown();
	/** Constructs shutdown timer */
	inline CSisLauncherServerShutdown();
	/** Second-phase construction */
	inline void ConstructL();
	/** Starts server shutdown when the timer goes off */
	void StartShort(TBool aCancelLong);
	/** Starts server shutdown when the long timer goes off */
	void StartLong();
private:
	void RunL();
	TBool iLongTimerActive;
	};

class CSisLauncherServer : public CPolicyServer
	{
public:
	static CServer2* NewLC();
	virtual ~CSisLauncherServer();
	void AddSession();
	void DropSession();
	inline CFileMan& FileMan();
	inline TInt BootUpMode();
	inline CQueueProcessor& RunQueue();

#ifndef SWI_TEXTSHELL_ROM
	void StartDocumentL(RFile& aFile, TBool aWait);
	void StartByMimeL(RFile& aFile, TDesC8& aMimeType, TBool aWait);
	void StartDocumentL(const TDesC& aFileName, TBool aWait);
	void StartByMimeL(const TDesC& aFileName, TDesC8& aMimeType, TBool aWait);
	void ShutdownL();
	void ShutdownL(TUid aUid, TInt aTimeout); 
	void NotifyNewAppsL(const RPointerArray<TDesC>& aFiles);
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void NotifyNewAppsL(const RPointerArray<Usif::CApplicationRegistrationData>& aApplicationRegistrationData);	
	#endif
#endif		
	void HandleShutdownL(TThreadId aThread, TBool aKillOnTimeout = EFalse);
	void ForceShutdownL(TUid aUid);
	void RunExecutableL(const TDesC& aFileName, TBool aWait);
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	const RArray<TLanguage>& DeviceSupportedLanguages() const;
	// Get and Set the device supported languages  
	void ResetInstalledLanguagesL();
#endif
	
private:
	CSisLauncherServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	void LongServerShutdown();
	void ShortServerShutdown();
	void CancelShutdown();
    #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	virtual TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);	
    #endif	
	// Server Policies
	// separate the policies into handled and not handled
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	static const TUint iRangeCount=6;
	RArray<TLanguage> iDeviceSupportedLanguages;
#else
	static const TUint iRangeCount=2;
#endif
	static const TInt iRanges[iRangeCount];
	static const TUint8 iElementsIndex[iRangeCount];
	static const CPolicyServer::TPolicyElement iPolicyElements[iRangeCount];
	static const CPolicyServer::TPolicy iPolicy;

private:

	friend class CQueueProcessor;
	
	TInt iSessionCount;
	
	CSisLauncherServerShutdown* iShutdown;
	TInt iBootMode;
	CQueueProcessor* iQueueProcessor;
	
#ifndef SWI_TEXTSHELL_ROM

private:
	RWsSession iWsSession;
#endif
	};


// CSisLauncherServer inlines

inline CSisLauncherServer::CSisLauncherServer()
:	CPolicyServer(CActive::EPriorityStandard, iPolicy, ESharableSessions)
{
}

inline TInt CSisLauncherServer::BootUpMode()
	{
	return iBootMode;
	}

inline CQueueProcessor& CSisLauncherServer::RunQueue()
	{
	return *iQueueProcessor;
	}

// CSisLauncherServerShutDown inlines
inline CSisLauncherServerShutdown::CSisLauncherServerShutdown()
:	CTimer(-1), iLongTimerActive(false)
	{
	CActiveScheduler::Add(this);
	}
	
	
inline void CSisLauncherServerShutdown::ConstructL()
	{
	CTimer::ConstructL();
	StartShort(ETrue);
	}


}

#endif
