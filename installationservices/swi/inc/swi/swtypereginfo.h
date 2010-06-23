/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines the CSoftwareTypeRegInfo class used for installation of a Layered Eexecution Environment.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SWTYPEREGINFO_H
#define SWTYPEREGINFO_H

#include <e32base.h>
#include <s32mem.h>
#include <ct/rcpointerarray.h>
#include <usif/scr/screntries_platform.h>

namespace Swi
	{
	_LIT(KTxtSoftwareTypeComponentProperty, "softwareTypes");

	class SoftwareTypeRegInfoUtils
		{
	public:
		IMPORT_C static void SerializeArrayL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf8& aSerializedArray);
		IMPORT_C static void UnserializeArrayL(RReadStream& aStream, RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray);
		IMPORT_C static void SerializeUniqueSwTypeNamesL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RBuf& aSerializedNames);
		IMPORT_C static void UnserializeUniqueSwTypeNamesL(const TDesC& aSerializedNames, RArray<TPtrC>& aUniqueSwTypeNames);
		IMPORT_C static void ExtractMimeTypesL(const RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray, RPointerArray<HBufC8>& aMimeTypes);
	public:
		enum { KUniqueNameSeparator = 0x1E };  // 'record separator'
		};
	}

#endif // SWTYPEREGINFO_H
