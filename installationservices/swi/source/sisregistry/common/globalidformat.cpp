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
* Provides the functionality of forming the component global ID
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#include "scrhelperutil.h"
#include "scrdbconstants.h"

// Formates and returns the global ID for the component based on it's installation type.
// Only the patch applications will have the name as part of the global Id,
// all other type packages will have only the Uid as global Id.
HBufC* Swi::FormatGlobalIdLC(const TUid& aCompUid, const TDesC& aCompName, Sis::TInstallType aInstallType)
	{
	HBufC* compGlobalIdString = NULL;
	if ((aInstallType & Sis::EInstAugmentation) || (aInstallType & Sis::EInstPreInstalledPatch))
		{
		compGlobalIdString = HBufC::NewLC(KCompUidSize + aCompName.Size() + 1); // The 1 extra char for the '_' seperator in the global Id structure.
		TPtr ptr(compGlobalIdString->Des());
		ptr.AppendFormat(KPatchGlobalIdFormat, aCompUid, &aCompName);
		}
	else
		{
		compGlobalIdString = HBufC::NewLC(KCompUidSize);
		TPtr ptr(compGlobalIdString->Des());
		ptr.AppendFormat(KGlobalIdFormat, aCompUid);
		}

	return compGlobalIdString;
	}
