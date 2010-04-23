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
* This file implements the RSoftwareInstall API.
*
*/


#include <usif/sif/sif.h>
#include "sifservercommon.h"

using namespace Usif;

EXPORT_C RSoftwareInstall::RSoftwareInstall()
	{
	}

EXPORT_C TInt RSoftwareInstall::Connect()
	{
	TVersion ver = Version();
	return iTransport.Connect(KSifServerName, ver, KServerImageName, KSifServerUid);
	}

EXPORT_C void RSoftwareInstall::Close()
	{
	iTransport.Close();
	}

EXPORT_C void RSoftwareInstall::GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	iTransport.GetComponentInfo(aFileName, aComponentInfo, aStatus);
	}

EXPORT_C void RSoftwareInstall::GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	iTransport.GetComponentInfo(aFileHandle, aComponentInfo, aStatus);
	}

EXPORT_C void RSoftwareInstall::Install(const TDesC& aFileName, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	iTransport.Install(aFileName, aStatus, aExclusiveOperation);
	}

EXPORT_C void RSoftwareInstall::Install(const TDesC& aFileName, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus,
										TBool aExclusiveOperation)
	{
	iTransport.Install(aFileName, aArguments, aResults, aStatus, aExclusiveOperation);
	}

EXPORT_C void RSoftwareInstall::Install(RFile& aFileHandle, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	iTransport.Install(aFileHandle, aStatus, aExclusiveOperation);
	}

EXPORT_C void RSoftwareInstall::Install(RFile& aFileHandle, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus,
										TBool aExclusiveOperation)
	{
	iTransport.Install(aFileHandle, aArguments, aResults, aStatus, aExclusiveOperation);
	}

EXPORT_C void RSoftwareInstall::Uninstall(TComponentId aComponentId, TRequestStatus& aStatus, TBool aExclusiveOperation)
	{
	iTransport.Uninstall(aComponentId, aStatus, aExclusiveOperation);
	}

EXPORT_C void RSoftwareInstall::Uninstall(TComponentId aComponentId, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus,
										TBool aExclusiveOperation)
	{
	iTransport.Uninstall(aComponentId, aArguments, aResults, aStatus, aExclusiveOperation);
	}

EXPORT_C void RSoftwareInstall::Activate(TComponentId aComponentId, TRequestStatus& aStatus)
	{
	iTransport.Activate(aComponentId, aStatus);
	}

	EXPORT_C void RSoftwareInstall::Deactivate(TComponentId aComponentId, TRequestStatus& aStatus)
	{
	iTransport.Deactivate(aComponentId, aStatus);
	}

EXPORT_C void RSoftwareInstall::CancelOperation()
	{
	iTransport.CancelOperation();
	}
