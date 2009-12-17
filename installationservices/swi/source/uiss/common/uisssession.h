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
* uisupportsession.h*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __UISUPPORTSESSION_H__
#define __UISUPPORTSESSION_H__

#include <e32base.h>

#include "installclientserver.h"

namespace Swi
{
class CUissServer;
class CUissCmdHandler;
class MUiHandler;
class MCancelHandler;
class CSisRegistryPackage;
class CWriteStream;
class CInstallPrefs;
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
class CNativeComponentInfo;
#endif

/**
 * The UI Support Server session class. This class handles IPC messages, which indicate
 * the commands to perform.
 * 
 * All the commands pass their data in a descriptor, in parameter 0 of the IPC message
 * The return result of the call to the UI is returned in parameter 1 of the IPC message
 * For commands where more data needs to be returned, for example the options
 * selection dialog, this is written into a descriptor and returned in parameter 2
 * of the IPC message.
 * 
 * @internalComponent 
 * @released
 */
class CUissSession : public CSession2
	{
public:
	/**
	 * This enum is used to indicate the message function in the IPC call to the server.
	 */
	enum TMessage
	{
	// Messages used by launcher client
		KMsgSeparatorMinimumLauncherMessage=1, ///< separator 

		// Install message
		KMessageInstall,			///< Message used to start the install process

		KMessageUninstall,
		
		#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
		KMessageGetComponentInfo,
		#endif
		
		// Cancel the install
		KMessageCancel,
		
		/**
		 * Message used to indicate the buffer for the message has been
		 * reallocated, after oveflow error while trying to fit dialog request 
		 * into buffer provided by launcher on the install message
		 */
		KMessageBufferReallocated,
		
		/// Message used to indicate successful return of dialog call
		KMessageCompleteDialog,
	
	// Messages used by SWIS client
	
		// UI Request Messages

		KMsgSeparatorMinimumSwisMessage=128, ///< separator 

		KMessageInstallDialog,
		KMessageQuestionDialog,
		KMessageGrantCapabilitiesDialog,
		KMessageLanguageDialog,
		KMessageDriveDialog,
		KMessageUpgradeDialog,
		KMessageOptionsDialog,
		KMessageHandleInstallEvent,
		KMessageHandleCancellableInstallEvent,
		KMessageTextDialog,
		KMessageErrorDialog,
		KMessageDependencyBreakDialog,
		KMessageApplicationsInUseDialog,
		KMessageCannotOverwriteFileDialog,
		KMessageOcspResultDialog,
		KMessageSecurityWarningDialog,
		KMessageUninstallDialog,
		KMessageDeviceIncompatibility,
		KMessageMissingDependency,

		KMsgSeparatorMaximumSwisMessage, ///< separator 
	};

	static CUissSession* NewL(CUissServer& aServer);
	static CUissSession* NewLC(CUissServer& aServer);

	~CUissSession();

private:
	void ConstructL();
	

	/**
	 * The constructor
	 *
	 * @param aUiHandler		The implementation of the UI Handler to use to display dialogs.
	 * @param aCancelHandler	The implementation of the cancel handler to pass to the UI
	 *							in order for it to cancel asynchronous operations.
	 * @param aTimeout			The time to wait for each command to complete, before
	 *							killing it and returning KErrTimeout.
	 */
	CUissSession(CUissServer& aServer);

	/** 
	 * Function which handles the messages. It calls the UissCmdHandlerFactoryL(), 
	 * passing in the command number, which creates a handler for the command. This handler
	 * is then called to handle the message.
	 * @see Swi::CUissCmdHandler
	 *
	 * @param aMessage The message to service
	 */
	void ServiceL(const RMessage2& aMessage);
	void Disconnect(const RMessage2& aMessage);
	
	void TransferDescriptorL(const RMessage2& aMessageFrom, TInt aParameterFrom,
									const RMessage2& aMessageTo, TInt aParameterTo);							
	
private:
	CUissServer& iUissServer;	
	};

/**
 * Class which is used to kick off the install or uninstall by calling Software
 * Install Server and watches for completion of the installation
 */
class CServerInstallWatcher : public CActive
	{
public:
	static CServerInstallWatcher* NewL(CUissServer& aServer);
	static CServerInstallWatcher* NewLC(CUissServer& aServer);
	
	void StartUninstall(CSisRegistryPackage& aPackage);
	void StartInstallL(const CInstallPrefs& aInstallPrefs, const RArray<TInt>& aDeviceSupportedLanguages);	
	void CancelSwis();
	
	~CServerInstallWatcher();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	void StartGetComponentInfoL(const CInstallPrefs& aInstallPrefs, TDes8& aCompInfoBuffer);
#endif

private:
	CServerInstallWatcher(CUissServer& aServer);
	void ConstructL();
	void SerializePrefsL(const CInstallPrefs& aInstallPrefs);
	
// from CActive
	void DoCancel();
	void RunL();
	
private:
	/**
	 * Session to communicate with SWIS
	 */
	RInstallServerSession iSwisSession;

	CUissServer& iUissServer;		

	TPtrC8 iPtr;
	
	CWriteStream* iWriteStream;	
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	TBool iIsComponentInfoRequest;
	#endif
	
	};

} // namespace Swi
	
#endif // #ifndef __UISUPPORTSESSION_H__
