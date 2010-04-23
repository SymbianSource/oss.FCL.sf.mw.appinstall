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
* sisrerevocationentry.cpp
* sisrerevocation - exported client revocation session interface implementation
*
*/


/**
 @file 
 @released
 @internalComponent
*/
 
#include "sisregistryclientserver.h"
#include "sisrevocationentry.h"

using namespace Swi;

EXPORT_C void RSisRevocationEntry::CheckRevocationStatusL(const TDesC8& aOcspUri)
	{
	User::LeaveIfError(SendReceive(ERevocationStatus, TIpcArgs(&aOcspUri)));
	}

EXPORT_C void RSisRevocationEntry::CheckRevocationStatus(const TDesC8& aOcspUri,
                                                          TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	SendReceive(ERevocationStatus, TIpcArgs(&aOcspUri), aStatus);
	}

EXPORT_C void RSisRevocationEntry::CancelRevocationStatusL()
  	{
 	User::LeaveIfError(SendReceive(ECancelRevocationStatus)); 
  	}
