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
* Executable entrypoint and server object factory function.
*
*/


#include <usif/sif/siftransportserverstartup.h>
#include "sifservertask.h"

using namespace Usif;

namespace
	{
	/** A factory function for the SIF Server */
	CSifTransportTask* SifServerTaskFactoryL(TransportTaskFactory::TTaskType aTaskType, TTransportTaskParams& aParams)
		{
		return CSifServerTask::NewL(aTaskType, aParams);
		}
	}

/**
	Executable entrypoint.

	@return	Symbian OS error code where KErrNone indicates
			success and any other value indicates failure.
 */
TInt E32Main()
	{
	return StartTransportServer(KSifServerName, Version(), SifServerTaskFactoryL);
	}
