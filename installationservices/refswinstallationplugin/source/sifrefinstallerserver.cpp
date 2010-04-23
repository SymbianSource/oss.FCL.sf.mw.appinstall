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
* This file implements a server for the Reference Installer.
*
*/


#include <usif/sif/siftransportserverstartup.h>
#include "sifrefinstallerclientserver.h"
#include "sifrefinstallertask.h"
#include "sifrefuihandler.h"

using namespace Usif;

namespace
	{

	/**
	A factory function that instantiates a UI handler. Reference Installer tasks
	call this function when they need UI interaction.
	
	@return Pointer to a MInstallerUIHandler object.
	@leave System wide error code
	*/
	MInstallerUIHandler* NewSifRefUiHandlerL()
		{
		return new (ELeave) MSifRefUIHandler;
		}

	/**
	A factory function that instantiates Reference Installer tasks. The CSifTransportRequest
	class calls this function in order to process incoming software management requests.
	*/
	CSifTransportTask* RefInstallerTaskFactoryL(TransportTaskFactory::TTaskType aTaskType, TTransportTaskParams& aParams)
		{
		// Security check - the client must have the SIF Server SID
		if (aParams.iSecurityContext->SecureId() != KSifServerUid)
			{
			User::Leave(KErrPermissionDenied);
			}

		// Instantiate an appropriate task
		switch (aTaskType)
			{
			case TransportTaskFactory::EGetComponentInfo:
				return CSifRefGetComponentInfoTask::NewL(aParams);

			case TransportTaskFactory::EInstall:
				return CSifRefInstallTask::NewL(aParams, NewSifRefUiHandlerL);

			case TransportTaskFactory::EUninstall:
				return CSifRefUninstallTask::NewL(aParams, NewSifRefUiHandlerL);

			case TransportTaskFactory::EActivate:
				return new (ELeave) CSifRefActivateDeactivateTask(aParams, EActivated);

			case TransportTaskFactory::EDeactivate:
				return new (ELeave) CSifRefActivateDeactivateTask(aParams, EDeactivated);

			default:
				User::Leave(KErrArgument);
			}
		return NULL;
		}

	}

/**
Executable entrypoint of the Reference Installer Server.

@return	Symbian OS error code where KErrNone indicates
		success and any other value indicates failure.
 */
TInt E32Main()
{
return StartTransportServer(KSifRefInstallerServerName, Version(), RefInstallerTaskFactoryL);
}
