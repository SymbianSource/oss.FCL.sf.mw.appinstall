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
* This file implements the client side of the Reference Installer.
*
*/


#include <f32file.h>
#include <scs/scscommon.h>
#include <scs/scsclient.h>
#include "sifrefinstallerclientserver.h"
#include "sifrefinstallerclient.h"

using namespace Usif;

namespace
	{
	/** SIF server SID. */
	const TUid KSifRefInstallerServerUid = {0x1028634E};

	/** Executable image which runs the server process. */
	_LIT(KRefInstallerServerImageName, "sifrefinstallerserver.exe");
	}

RSifRefInstallerClient::RSifRefInstallerClient()
	{
	}

TInt RSifRefInstallerClient::Connect()
	{
	return iTransporter.Connect(KSifRefInstallerServerName, Version(), KRefInstallerServerImageName, KSifRefInstallerServerUid);
	}

void RSifRefInstallerClient::Close()
	{
	iTransporter.Close();
	}

void RSifRefInstallerClient::GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	iTransporter.GetComponentInfo(aFileName, aComponentInfo, aStatus);
	}

void RSifRefInstallerClient::GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	iTransporter.GetComponentInfo(aFileHandle, aComponentInfo, aStatus);
	}

void RSifRefInstallerClient::Install(const TDesC& aFileName, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus)
	{
	// Fifth argument of the SifTransportClient::Install() and SifTransportClient::Uninstall() methods,
	// if set to ETrue, instructs an instance of CSifTransportServer to work in the ExclusiveOperation
	// mode. In this mode only one request may be processed at a time. In our case an instance of
	// CSifTransportServer means the Refrence Installer Server. Hence, we have to set this param to
	// ETrue because the Reference Installer doesn't support concurrent operations.
	iTransporter.Install(aFileName, aArguments, aResults, aStatus, ETrue);
	}

void RSifRefInstallerClient::Install(RFile& aFileHandle, const COpaqueNamedParams& aArguments,
										COpaqueNamedParams& aResults, TRequestStatus& aStatus)
	{
	// Please see above for a comment about ETrue passed as a fifth param.
	iTransporter.Install(aFileHandle, aArguments, aResults, aStatus, ETrue);
	}

void RSifRefInstallerClient::Uninstall(TComponentId aComponentId, const COpaqueNamedParams& aArguments,
											COpaqueNamedParams& aResults, TRequestStatus& aStatus)
	{
	// Please see above for a comment about ETrue passed as a fifth param.
	iTransporter.Uninstall(aComponentId, aArguments, aResults, aStatus, ETrue);
	}

void RSifRefInstallerClient::Activate(TComponentId aComponentId, TRequestStatus& aStatus)
	{
	iTransporter.Activate(aComponentId, aStatus);
	}

void RSifRefInstallerClient::Deactivate(TComponentId aComponentId, TRequestStatus& aStatus)
	{
	iTransporter.Deactivate(aComponentId, aStatus);
	}

void RSifRefInstallerClient::CancelOperation()
	{
	iTransporter.CancelOperation();
	}
