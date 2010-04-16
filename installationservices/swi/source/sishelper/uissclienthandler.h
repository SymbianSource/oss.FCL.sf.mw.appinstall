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
* Declaration of Swi::CUissClientHandler class which is the client-side part 
* of the reverse completion mechanism used by SWI to communicate with the UI
*
*/


/**
 @file
 @internalComponent
*/

#include <e32std.h>
#include "../uiss/client/uissclient.h"
#include "swi/msisuihandlers.h"
#include "swi/launcher.h"
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sif/sifcommon.h>
#include "swi/nativecomponentinfo.h"
#endif

namespace Swi
{
class CSisRegistryPackage;
class CUissCmdHandler;
class CWriteStream;

/**
 * This class communicates asynchronously with UISS using reverse completion 
 * mechanism, making requests to UISS and receiving completions with requests
 * to display dialogs to the user, which it does by calling back to UI 
 * implementation (see Swi::MUiHandler and Swi::Launcher).
 *
 * @note This class is similar to the class CInstCoordClient from Jetstream
 *
 * @see Swi::MUiHandler
 * @see Swi::Launcher
 *
 * @internalComponent
 * @released
 */
class CUissClientHandler : public CActive
	{
public:
	static CUissClientHandler* NewL(MUiHandler& aUiHandler, TBool aActiveObjectMode = true);
	static CUissClientHandler* NewLC(MUiHandler& aUiHandler, TBool aActiveObjectMode = true);
	
	virtual ~CUissClientHandler();
	
	// client methods
	
	/**
	 * Performs installation asynchronously. The r/c mechanism is handled 
	 * internally.
	 */
	void InstallL(const CInstallPrefs& aInstallPrefs, const RArray<TInt>& aDeviceSupportedLanguages, TRequestStatus& aStatus, RThread& aServer);
		
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Retrieves component info asynchronously. The buffer management is handled internally.
	 */
	void GetComponentInfoL(const CInstallPrefs& aInstallPrefs, Usif::CComponentInfo& aComponentInfo,  TRequestStatus& aStatus, RThread& aServer);
	#endif
		
	/**
	 * Performs uninstallation asynchronously. The r/c mechanism is handled 
	 * internally.
	 *
	 * @param aPackage Package to uninstall
	 */
	void UninstallL(const CSisRegistryPackage& aPackage, TRequestStatus& aStatus);
	
	/**
	 * Cancels operation in progress
	 */
	void CancelOperation();

	/**
	 * Used during synchronous operation to issue/process all
	 * requests until operation is complete.
	 */
	void WorkUntilCompleteL();

	/**
	 * Returns true if an install/uninstall operation is in
	 * progress. This is different to IsActive() which just returns
	 * true if we have an outstanding request on the UISS.
	 */
	TBool IsBusy();
		
private:
	//	static const TInt KBufSize=10*1024; ///< Initial size of the command buffer
	static const TInt KBufSize=10; ///< Initial size of the command buffer

	void Cancel(); // Weak attempt to stop any user calling Cancel directly.
	
	CUissClientHandler(MUiHandler& aUiHandler, TBool aActiveObjectMode);

	CUissClientHandler(const CUissClientHandler&); // Disable copy construction
	const CUissClientHandler &operator=(const CUissClientHandler&); // Disable assignment
	
	void ConstructL();             ///< Second phase construction function
	
	/**
	 * Starts UISS in a new thread of the same SISHelper process. Later SWIS 
	 * will connect to UISS to show dialogs.
	 *
	 * @return KErrNone or an error code
	 */
	static TInt StartUiss();
	
	/**
	 * UISS thread function to run UISS in the same process.
	 *
	 * @return KErrNone or an error code
	 */
	static TInt UissThreadFunction(TAny *);
	
	/**
	 * Creates a command handler object corresponding to the specified UI 
	 * dialog request from SWIS
	 *
	 * @param aCode Dialog code (see CUissSession::TMessage)
	 *
	 * @return Command handler object (caller must destroy it)
	 */
	CUissCmdHandler* UissCmdHandlerFactoryL(TInt aCommand) const;
	void AllocBufL(TInt aSize);    ///< [Re]allocates buffer to aSize
	void HandleOverflowL();        ///< Increases UISS r/c command buffer size
	void InitializeArgStreamL(const CInstallPrefs& aInstallPrefs);
	
	/**
	 * Process a completion of an internal request to the UISS server.
	 */
	virtual void RunL();


	/**
	 * Propogate RunL failures to client via its TRequestStatus
	 */
	virtual TInt RunError(TInt aError);
	
	/**
	 * Cancel any internal request and complete the client request.
	 */
	virtual void DoCancel();
	
	void WaitForSisHelperShutdown();

	MUiHandler& iUiHandler;        ///< UI implementation
	MCancelHandler* iCancelHandler;///< Internal cancel handler
	HBufC8* iBuf;                  ///< Command buffer
	TPtr8 iPtrIntoBuf;
	RUissSession iUissSession;     ///< UISS session

	TBool iActiveObjectMode;		///< Normally true for CActive object mode, set to false for sync install
	TRequestStatus* iClientStatus; ///< Client request status
    CWriteStream* iArgsStream;    ///< Saved copy of arguments stream
	TPtrC8 iPtrIntoArgsStream;
    enum { KUissClientIdle, 
    	   KUissClientInstalling,  
    	   KUissClientUninstalling
    	   #ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    	   , KUissClientGettingCompInfo
    	   #endif
		} iState;
    RThread iSisHelper;
   
   	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK 
	void AllocCompInfoBufL(TInt aBufSize);
	void ConstructCompInfoFromBufferL();
	Usif::CComponentInfo::CNode* MapToComponentInfoL(CNativeComponentInfo& aNativeComponentInfo);
	void MapToApplicationInfoL(RCPointerArray<Swi::CNativeComponentInfo::CNativeApplicationInfo>& aNativeApplicationsInfo, RPointerArray<Usif::CComponentInfo::CApplicationInfo>& aApplicationsInfo);
	HBufC8* iCompInfoBuffer;		//< Component Information storage buffer
	TPtr8   iCompInfoBufPtr;
	
	Usif::CComponentInfo* iComponentInfo;
	#endif
	};
} // namespace Swi
