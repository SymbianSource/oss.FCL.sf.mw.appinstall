/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file declares Swi::AsyncLauncher class that is used to launch installation
* or removal.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __SWIASYNCLAUNCHER_H__
#define __SWIASYNCLAUNCHER_H__

#include <swi/launcher.h>
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <usif/sif/sifcommon.h>
#endif

// Software Install namespace
 
namespace Swi
{

class CUissClientHandler;

/**
 * Installation or removal CAsyncLauncher class.
 * 
 * This class defines an asynchronous API to the software install
 * which is intended to be used by a CActive object running within an
 * active scheduler. @see CActiveScheduler
 *
 * The client needs to wait for the TRequestStatus object passed in to
 * be updated, but MUST do so by using the active scheduler, otherwise
 * the installer will be unable to initiate/process the multiple
 * internal requests which make up an (un)install procedure.
 *
 * Within a process, all calls must originate from the same thread.
 *
 * It should be noted by users of this interface that starting an
 * install causes an instance of a "helper" server to be started 
 * in process, sharing the same heap as the client process. This may
 * lead to a number of side effects, including the heap being temporarily
 * inbalanced after the client request is completed, due to the delay in 
 * the server thread shutting down.
 *
 * Note that the EEventAbortedInstall and EEventAbortedUnInstall
 * callbacks are no longer generated.
 *
 * @publishedPartner
 * @released
 */
class CAsyncLauncher : public CBase
	{
public:
	/**
	 * Creates a new CAsyncLauncher object and places it on the cleanup stack.
	 *
	 * @return An instance of CAsyncLauncher
	 */
	IMPORT_C static CAsyncLauncher *NewLC();
	
	/**
	 * Creates a new CAsyncLauncher object.
	 *
	 * @return An instance of CAsyncLauncher
	 */
	IMPORT_C static CAsyncLauncher *NewL();
	
	/**
	 * Starts software installation in case of a locally available SISX file.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aFileName     Local name of the SISX file
	 * @param aInstallPrefs Installation preferences
	 * @param aRequest		Client's CActive object's iStatus
	 * @see MUiHandler
	 * @see CInstallPrefs
	 */
	IMPORT_C void InstallL(MUiHandler& aUiHandler,
						   const TFileName& aFileName, 
						   const CInstallPrefs& aInstallPrefs,
						   TRequestStatus &aRequest);
	
	/**
	 * Starts software installation with package data provided by means of IPC.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aDataProvider SIS data provider implementation, which may supply 
	 *                      data either by reading a file or by streaming
	 * @param aInstallPrefs Installation preferences
	 * @param aRequest		Client's CActive object's iStatus
	 * @see MUiHandler
	 * @see MSisDataProvider
	 * @see CInstallPrefs
	 */
	IMPORT_C void InstallL(MUiHandler& aUiHandler, 
						   MSisDataProvider& aDataProvider,
						   const CInstallPrefs& aInstallPrefs,
						   TRequestStatus &aRequest);
	
	/**
	 * Starts software installation with package data provided by means of IPC.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aFileHandle 	File handle of the file to install.
	 * @param aInstallPrefs Installation preferences
	 * @param aRequest		Client's CActive object's iStatus
	 * @see MUiHandler
	 * @see CInstallPrefs
	 */
	IMPORT_C void InstallL(MUiHandler& aUiHandler, 
						   RFile& aFileHandle,
						   const CInstallPrefs& aInstallPrefs,
						   TRequestStatus &aRequest);
	
	/**
	 * Uninstalls the main package identified by UID, and all the augmentations
	 * associated with this package.
	 *
	 * @param aUiHandler UI implementation
	 * @param aUid       UID of the application to remove
	 * @param aRequest	 Client's CActive object's iStatus
	 * @see MUiHandler
	 */
	IMPORT_C void UninstallL(MUiHandler& aUiHandler, 
							 const TUid& aUid,
							 TRequestStatus &aRequest);
	
	/**
	 * Uninstalls a specific package.
	 *
	 * @param aUiHandler 	UI implementation
	 * @param aPackage	 	The specific package to uninstall.
	 * @param aRequest		Client's CActive object's iStatus
	 * @see MUiHandler
	 */
	IMPORT_C void UninstallL(MUiHandler& aUiHandler,
							 const CSisRegistryPackage& aPackage,
							 TRequestStatus &aRequest);
	
	/**
	 * Calling Cancelperation cancels the current operation of
	 * Software Install. On cancellation Software Install performs
	 * full rollback of the current operation.
	 *
	 * Note that the cancellation is asynchronous and its completion
	 * will be notified by the completion of the original
	 * install/uninstall request (with a status of KErrCancel).
	 */
	virtual void CancelOperation();

	
	/**
	 * Destructor for this class
	 */
	IMPORT_C virtual ~CAsyncLauncher();
	
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	/**
	 * Starts getting the component info in case of a locally available SISX file.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aFileName     Local name of the SISX file
	 * @param aInstallPrefs Installation preferences
	 * @param aComponentInfo Component information has to be returned back
	 * @param aRequest		Client's CActive object's iStatus
	 * @see MUiHandler
	 * @see CInstallPrefs
	 */
	IMPORT_C void GetComponentInfoL(MUiHandler& aUiHandler,
						   const TFileName& aFileName, 
						   const CInstallPrefs& aInstallPrefs,
						   Usif::CComponentInfo& aComponentInfo, 
						   TRequestStatus &aRequest);
			
	/**
	 * Starts getting the component info with package data provided by file handle.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aFileHandle 	File handle of the file to install.
	 * @param aInstallPrefs Installation preferences
	 * @param aComponentInfo Component information has to be returned back
	 * @param aRequest		Client's CActive object's iStatus
	 * @see MUiHandler
	 * @see CInstallPrefs
	 */
	IMPORT_C void GetComponentInfoL(MUiHandler& aUiHandler, 
						   RFile& aFileHandle,
						   const CInstallPrefs& aInstallPrefs,
						   Usif::CComponentInfo& aComponentInfo,
						   TRequestStatus &aRequest);
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK	
	
	/**
	* Starts software installation in case of a locally available SISX file.
	*
	* @param aUiHandler                UI implementation
	* @param aFileName     			   Local name of the SISX file
	* @param aInstallPrefs			   Installation preferences
	* @param aRequest				   Client's CActive object's iStatus
	* @param aDeviceSupportedLanguages Set of languages which device supports.If the array 
	*								   passed is empty, then the behavior will be same
	*								   as the InstallL API without aDeviceSupportedLanguages 
	*								   option.
	* @see MUiHandler
	* @see CInstallPrefs
	*/
	IMPORT_C void InstallL(MUiHandler& aUiHandler, 
						   const TFileName& aFileName,
						   const CInstallPrefs& aInstallPrefs,
						   const RArray<TInt>& aDeviceSupportedLanguages, 
						   TRequestStatus &aRequest);
		
	/**
	* Starts software installation with package data provided by means of IPC.
	*
	* @param aUiHandler    				UI implementation
	* @param aFileHandle 				File handle of the file to install.
	* @param aInstallPrefs 				Installation preferences
	* @param aRequest					Client's CActive object's iStatus
	* @param aDeviceSupportedLanguages 	Set of languages which device supports.If the array 
	*									passed is empty, then the behavior will be same
	*									as the InstallL API without aDeviceSupportedLanguages 
	*									option.
	* @see MUiHandler
	* @see CInstallPrefs
	*/
	IMPORT_C void InstallL(MUiHandler& aUiHandler, 
						   RFile& aFileHandle,
						   const CInstallPrefs& aInstallPrefs,
						   const RArray<TInt>& aDeviceSupportedLanguages, 
						   TRequestStatus &aRequest);
	
	/**
	* Starts software installation with package data provided by means of IPC.
	*
	* @param aUiHandler    			   UI implementation
	* @param aDataProvider 			   SIS data provider implementation, which may supply 
	*                      			   data either by reading a file or by streaming
	* @param aInstallPrefs 			   Installation preferences
	* @param aRequest				   Client's CActive object's iStatus
	* @param aDeviceSupportedLanguages Set of languages which device supports. If the array 
	*									passed is empty, then the behavior will be same
	*									as the InstallL API without aDeviceSupportedLanguages 
	*									option.
	* @see MUiHandler
	* @see MSisDataProvider
	* @see CInstallPrefs
	*/
	IMPORT_C void InstallL(MUiHandler& aUiHandler,
						   MSisDataProvider& aDataProvider, 
						   const CInstallPrefs& aInstallPrefs, 
						   const RArray<TInt>& aDeviceSupportedLanguages, 
						   TRequestStatus &aRequest);
		

	
private:
	CAsyncLauncher(const CAsyncLauncher&); // Disable copy construction
	const CAsyncLauncher &operator=(const CAsyncLauncher&); // Disable assignment
    
	/**
	 * Constructor for CAsyncLauncher is private - instance can
	 * only be created via NewLC or NewL.
	 */
	CAsyncLauncher();
	
	TBool VerifyBusyL(TRequestStatus &aRequest);
	
	void DoInstallL(TSisHelperStartParams& aHelperParams, 
					MUiHandler& aUiHandler, 
					const CInstallPrefs& aInstallPrefs,
					const RArray<TInt>& aDeviceSupportedLanguages, 
				    TRequestStatus &aRequest);
	
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK						 
	void DoGetComponentInfoL(TSisHelperStartParams& aHelperParams, MUiHandler& aUiHandler, const CInstallPrefs& aInstallPrefs,
							 Usif::CComponentInfo& aComponentInfo, TRequestStatus &aRequest);
	#endif

	/**
	 * Pointer to our UISS handler.
	 *
	 * Owned by this class.
	 */
	CUissClientHandler *iUissClientHandler;
	};

} // namespace Swi

#endif // __SWIASYNCLAUNCHER_H__
