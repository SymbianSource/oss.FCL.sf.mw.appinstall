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
*
*/


#include "uissclient.h"
#include "uisssession.h"
#include "writestream.h"

#include "sisregistrypackage.h"

/**
 * @internalComponent
 */
namespace Swi
{

EXPORT_C TInt RUissSession::Connect()
	{
	// Assume UISS is running
	TInt err=CreateSession(Swi::KUissServerName, TVersion(0,1,0));
	return err;
	}

EXPORT_C void RUissSession::Install(const TDesC8& aParams,
                                    TDes8& aDialogBuffer,
                                    TRequestStatus& aRequestStatus)
	{
	TIpcArgs args(&aDialogBuffer,&aParams);
	SendReceive(CUissSession::KMessageInstall, args, aRequestStatus);
	}

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
EXPORT_C void RUissSession::GetComponentInfo(const TDesC8& aParams,
                                    TDes8& aCompInfoBuffer,
                                    TRequestStatus& aRequestStatus)
	{	
	TIpcArgs args(&aCompInfoBuffer, &aParams);
	SendReceive(CUissSession::KMessageGetComponentInfo, args, aRequestStatus);	
	}
#endif

EXPORT_C void RUissSession::Uninstall(const TDesC8& aParams,
                                      TDes8& aDialogBuffer,
                                      TRequestStatus& aRequestStatus)
	{
    TIpcArgs args(&aDialogBuffer, &aParams);
    SendReceive(CUissSession::KMessageUninstall, args, aRequestStatus);
	}

EXPORT_C TInt RUissSession::Cancel()
	{
	return SendReceive(CUissSession::KMessageCancel);
	}
	
EXPORT_C void RUissSession::CompleteDialog(TInt aError, TDes8& aDialogBuffer,
										   TRequestStatus& aRequestStatus)
	{
	TIpcArgs args(&aDialogBuffer, TIpcArgs::ENothing, TIpcArgs::ENothing, aError);
	SendReceive(CUissSession::KMessageCompleteDialog, args, aRequestStatus);
	}

EXPORT_C void RUissSession::BufferReallocated(TDes8& aDialogBuffer,
											  TRequestStatus& aRequestStatus)
	{
	TIpcArgs args(&aDialogBuffer);
	SendReceive(CUissSession::KMessageBufferReallocated, args, aRequestStatus);
	}

} // namespace Swi
