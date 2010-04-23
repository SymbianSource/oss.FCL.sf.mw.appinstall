/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SIS Installation Result class
*
*/


#include <s32strm.h>
#include "sisinstallationresult.h"

using namespace Swi;

EXPORT_C CInstallationResult* CInstallationResult::NewL()
	{
	CInstallationResult *self = new(ELeave) CInstallationResult();
	return self;
	}

CInstallationResult::~CInstallationResult()
	{
	}

CInstallationResult::CInstallationResult()
	{
	}

EXPORT_C void CInstallationResult::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint16L(iResult);
	aStream.WriteInt32L(iLeaveCode);
	aStream << iProblematicFileName;
	}

EXPORT_C void CInstallationResult::InternalizeL(RReadStream& aStream)
	{
	iResult = static_cast<TResult>(aStream.ReadUint16L());
	iLeaveCode = aStream.ReadInt32L();
	aStream >> iProblematicFileName;
	}
