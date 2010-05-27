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
* Interface for the RUissSession class
*
*/


/**
 @file 
 @internalTechnology 
*/
 
#ifndef __UISSCLIENT_H__
#define __UISSCLIENT_H__

#include <e32std.h>

namespace Swi
{
_LIT(KUissServerName, "!UISS");
_LIT(KUissServerImage, "UISS");

class CSisRegistryPackage;

const TUid KUissServerUid3 = { 0x10202DC8 };

/**
 * Session to communicate with UISS in order to initiate installation or
 * uninstallation. Used from SISHelper.
 *
 * @released
 * @internalTechnology
 */
class RUissSession : public RSessionBase
	{
public:
	/**
	 * Connect to the server. Assumes the server is already running, because it 
	 * is started from SISHelper and runs in the same process, and this class is 
	 * used by SWIS. Starting a server requires linking against the module 
	 * containing UISS thread function, and SWIS cannot do that because it is 
	 * TCB and UISS is not, nor is SISHelper. So the result is that UISSClient 
	 * module may not contain UISS thread function or any function that starts 
	 * UISS.
	 *
	 * @return KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Connect();
	
	/**
	 * Start asynchronous installation.
     *
     * @param aParms Serialised CInstallPrefs specifying package to install.
     * @param aDialogBuffer Response buffer for dialogue to invoke information.
     *
     * Note that the caller is responsible for making sure that the
     * parameters persist accross the duration of the servicing of the
     * request.
	 */
	IMPORT_C void Install(const TDesC8& aParams,
                          TDes8& aDialogBuffer,
                          TRequestStatus& aRequestStatus);

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	/**
	 * Start asynchronous component information retrieval.
     *
     * @param aParms Serialised CInstallPrefs specifying SWI parameters.
     * @param aCompInfoBuffer Buffer for retrieving the component information.
     *
     * Note that the caller is responsible for making sure that the
     * parameters persist accross the duration of the servicing of the
     * request.
	 */
	IMPORT_C void GetComponentInfo(const TDesC8& aParams,
                          TDes8& aCompInfoBuffer,
                          TRequestStatus& aRequestStatus);
	#endif
	
	/**
	 * Start asynchronous uninstallation
	 *
     * @param aParms Serialised CSisRegistryPackage specifying package to uninstall.
     * @param aDialogBuffer Response buffer for dialogue to invoke information.
     *
     * Note that the caller is responsible for making sure that the
     * parameters persist accross the duration of the servicing of the
     * request.
	 */
	IMPORT_C void Uninstall(const TDesC8& aParams,
                            TDes8& aDialogBuffer,
                            TRequestStatus& aRequestStatus);
	
	/**
	 * Cancel current activity of SWIS. This is meaningful during file 
	 * installation phase only because to cancel installation planning it is
	 * only necessary to cancel a dialog box.
	 *
	 * @return KErrNone if successful, or an error code
	 */
	IMPORT_C TInt Cancel();

	/**
	 * Issue a dialog request, which will complete with either dialog
	 * callback information or the final status of the current
	 * operation.
	 */
	IMPORT_C void CompleteDialog(TInt aError, TDes8& aDialogBuffer,
								 TRequestStatus& aRequestStatus);

	/**
	 * If a dialog request has been received from swis which does not
	 * fit in the client request then the client should re-issue the
	 * request with a bigger buffer using this function.
	 */
	IMPORT_C void BufferReallocated(TDes8& aDialogBuffer, TRequestStatus& aRequestStatus);
    };

}

#endif // __UISSCLIENT_H__
