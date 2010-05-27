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
* This file declares the Swi::Launcher class that is used to launch
* a synchronous install or uninstall operation.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __SWILAUNCHER_H__
#define __SWILAUNCHER_H__

#include <e32base.h>
#include <swi/msisuihandlers.h>
#include <swi/sisdataprovider.h>


// Software Install namespace

namespace Swi
{
class CSisRegistryPackage;

/**
 * Class that holds installation preferences that are stored externally and 
 * passed to the software install by the client.
 *
 * @publishedPartner
 * @released
 */
class CInstallPrefs : public CBase
	{
public:
	/**
	 * Constructs a default preferences object
	 * @return Pointer to a new default preferences object
	 */
	static IMPORT_C CInstallPrefs* NewL();
	
	/**
	 * Constructs a default preferences object and puts it on the cleanup stack
	 * @return Pointer to a new object on the cleanup stack
	 */
	static IMPORT_C CInstallPrefs* NewLC();
	
	/**
	 * Reads object from stream
	 * @param aStream Stream to read the object from
	 * @return        New object
	 */
	static IMPORT_C CInstallPrefs* NewL(RReadStream& aStream);
	
	/**
	 * Reads object from stream and puts it on the cleanup stack
	 * @param aStream Stream to read the object data from
	 * @return        New object on the cleanup stack
	 */
	static IMPORT_C CInstallPrefs* NewLC(RReadStream& aStream);
	/**
	 * Destructor of CInstallPrefs. 
	 */
	IMPORT_C virtual ~CInstallPrefs();
	
	/**
	 * Reads object data from stream
	 * @param aStream Stream to read the object from
	 */
	IMPORT_C void  InternalizeL(RReadStream& aStream);
	
	/**
	 * Writes object data to stream
	 * @param aStream Stream to write the object data to
	 */
	IMPORT_C void  ExternalizeL(RWriteStream& aStream) const;
	
	/**
	 * Sets OCSP server URI
	 * @param aUri New OCSP server URI
	 */
	void  SetRevocationServerUriL(const TDesC8& aUri);
	
	/**
	 * Sets OCSP check flag
	 * @param aCheck Whether or not OCSP check should be performed
	 */
	void  SetPerformRevocationCheck(TBool aCheck);

	/**
	 * Returns OCSP server URI
	 * @return OCSP server URI
	 */
	const TDesC8&  RevocationServerUri() const;
	
	/**
	 * Returns global OCSP check flag
	 * @return Global OCSP status flag:
	 *         ETrue  OCSP check should be performed
	 *         EFalse OCSP check should not be performed
	 */
	TBool          PerformRevocationCheck() const;
	
private:
	CInstallPrefs();
	
	void ConstructL();
	void ConstructL(RReadStream& aStream);

	TBool   iPerformRevocationCheck;
	HBufC8* iRevocationServerUri;
	};

inline const TDesC8& CInstallPrefs::RevocationServerUri() const
	{
	return *iRevocationServerUri;
	}

inline TBool CInstallPrefs::PerformRevocationCheck() const
	{
	return iPerformRevocationCheck;
	}

inline void CInstallPrefs::SetRevocationServerUriL(const TDesC8& aUri)
	{
	delete iRevocationServerUri;
	iRevocationServerUri = 0;
	iRevocationServerUri = aUri.AllocL();
	}
	
inline void CInstallPrefs::SetPerformRevocationCheck(TBool aCheck)
	{
	iPerformRevocationCheck = aCheck;
	}

struct TSisHelperStartParams;

/**
 * Synchronous installation or removal launcher static class.
 *
 * If you want to perform asynchronous installation, investigate the
 * CAsyncLauncher class defined in asynclauncher.h.
 *
 * These functions are synchronous, when the function returns the
 * operation is complete.
 *
 * During the operation and the client does not need to save any
 * status or do anything else after starting installation because all
 * notifications will come via the UI implementation interface.
 *
 * It should be noted by users of this interface that starting an
 * install causes an instance of a "helper" server to be started 
 * in process, sharing the same heap as the client process. This may
 * lead to a number of side effects, including the heap being temporarily
 * inbalanced when the install method returns, due to the delay in the server
 * thread shutting down.
 *
 * An installer UI should call REComSession::FinalClose() as part of clean up.
 *
 * @see CAsyncLauncher
 *
 * @publishedPartner
 * @released
 */
class Launcher
	{
public:
	/**
	 * Starts software installation in case of a locally available SISX file.
	 *
	 * An installer UI should call REComSession::FinalClose() as part of clean up.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aFileName     Local name of the SISX file
	 * @param aInstallPrefs Installation preferences
	 * @return              KErrNone or an error code in case installation did
	 *                      not start
	 * @see MUiHandler
	 * @see CInstallPrefs
	 */
	static IMPORT_C TInt Install(MUiHandler& aUiHandler,
		const TFileName& aFileName, const CInstallPrefs& aInstallPrefs);
	
	/**
	 * Starts software installation with package data provided by means of IPC.
	 *
	 * An installer UI should call REComSession::FinalClose() as part of clean up.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aDataProvider SIS data provider implementation, which may supply 
	 *                      data either by reading a file or by streaming
	 * @param aInstallPrefs Installation preferences
	 * @return              KErrNone or an error code in case installation did 
	 *                      not start
	 * @see MUiHandler
	 * @see MSisDataProvider
	 * @see CInstallPrefs
	 */
	static IMPORT_C TInt Install(MUiHandler& aUiHandler, 
		MSisDataProvider& aDataProvider, const CInstallPrefs& aInstallPrefs);

	/**
	 * Starts software installation with package data provided by means of IPC.
	 *
     * An installer UI should call REComSession::FinalClose() as part of clean up.
	 *
	 * @param aUiHandler    UI implementation
	 * @param aFileHandle 	File handle of the file to install.
	 * @param aInstallPrefs Installation preferences
	 * @return              KErrNone or an error code in case installation did 
	 *                      not start
	 */
	static IMPORT_C TInt Install(MUiHandler& aUiHandler, 
		RFile& aFileHandle, const CInstallPrefs& aInstallPrefs);
	
	/**
	 * Uninstalls the main package identified by UID, and all the augmentations
	 * associated with this package.
	 *
	 * @param  aUiHandler UI implementation
	 * @param  aUid       UID of the application to remove
	 * @return KErrNone or an error code in case uninstallation did not start
	 * @see MUiHandler
	 */
	static IMPORT_C TInt Uninstall(MUiHandler& aUiHandler, 
		const TUid& aUid);

	/**
	 * Uninstalls a specific package.
	 *
	 * @param  aUiHandler UI implementation
	 * @param  aPackage	 The specific package to uninstall.
	 * @return KErrNone or an error code in case uninstallation did not start
	 * @see MUiHandler
	 */
	static IMPORT_C TInt Uninstall(MUiHandler& aUiHandler,
 		const CSisRegistryPackage& aPackage);

	/**
	 * Cancels the current operation of Software Install. On cancellation 
	 * Software Install performs full rollback of the current operation.
	 *
	 * @return KErrNone or an error code in case SWIS did not connect
	 */
	static IMPORT_C int Cancel();
	
	/**
	* Starts software installation in case of a locally available SISX file.
	*
	* An installer UI should call REComSession::FinalClose() as part of clean up.
	*
	* @param aUiHandler    				UI implementation
	* @param aFileName     				Local name of the SISX file
	* @param aInstallPrefs 				Installation preferences
	* @param aDeviceSupportedLanguages  Set of languages which device supports. If the array 
	*									passed is empty, then the behavior will be same 
	*									as the Install API without aDeviceSupportedLanguages 
	*									option.
	* @return             				KErrNone or an error code in case installation did
	*                     				not start
	* @see MUiHandler
	* @see CInstallPrefs
	*/
	static IMPORT_C TInt Install(MUiHandler& aUiHandler, 
									const TFileName& aFileName,
									const CInstallPrefs& aInstallPrefs,
									const RArray<TInt>& aDeviceSupportedLanguages);
	
	/**
	* Starts software installation with package data provided by means of IPC.
	*
	* An installer UI should call REComSession::FinalClose() as part of clean up.
	*
	* @param aUiHandler    				UI implementation
	* @param aFileHandle 				File handle of the file to install.
	* @param aDeviceSupportedLanguages 	Set of languages which device supports. If the array 
	*									passed is empty, then the behavior will be same 
	*									as the Install API without aDeviceSupportedLanguages 
	*									option.
	* @param aInstallPrefs 				Installation preferences
	* @return              				KErrNone or an error code in case installation did 
	*                      				not start
	*/
	static IMPORT_C TInt Install(MUiHandler& aUiHandler, 
									RFile& aFileHandle,
									const CInstallPrefs& aInstallPrefs,
									const RArray<TInt>& aDeviceSupportedLanguages);
	
	/**
	* Starts software installation with package data provided by means of IPC.
	*
	* An installer UI should call REComSession::FinalClose() as part of clean up.
	*
	* @param aUiHandler    				UI implementation
	* @param aDataProvider 				SIS data provider implementation, which may supply 
	*                      				data either by reading a file or by streaming
	* @param aInstallPrefs 				Installation preferences
	* @param aDeviceSupportedLanguages 	Set of languages which device supports. If the array 
	*									passed is empty, then the behavior will be same 
	*									as the Install API without aDeviceSupportedLanguages 
	*									option.
	* @return              				KErrNone or an error code in case installation did 
	*                      				not start
	* @see MUiHandler
	* @see MSisDataProvider
	* @see CInstallPrefs
	*/
	static IMPORT_C TInt Install(MUiHandler& aUiHandler,
									MSisDataProvider& aDataProvider, 
									const CInstallPrefs& aInstallPrefs, 
									const RArray<TInt>& aDeviceSupportedLanguages);

	
private:
	static TInt InstallL(MUiHandler& aUiHandler, 
						 const TFileName& aFileName,
						 const CInstallPrefs& aInstallPrefs,
						 const RArray<TInt>& aDeviceSupportedLanguages);
	
	static TInt InstallL(MUiHandler& aUiHandler, 
                             RFile& aFileHandle,
						 const CInstallPrefs& aInstallPrefs,
						 const RArray<TInt>& aDeviceSupportedLanguages);
	
	static TInt InstallL(MUiHandler& aUiHandler,
						 MSisDataProvider& aDataProvider,
						 const CInstallPrefs& aInstallPrefs,
						 const RArray<TInt>& aDeviceSupportedLanguages);
	
	static TInt UninstallL(MUiHandler& aUiHandler, const CSisRegistryPackage& aPackage);
	};
	
} // namespace Swi

#endif // __SWILAUNCHER_H__
