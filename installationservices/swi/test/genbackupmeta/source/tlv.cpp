/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* For storing data types in Type-Length-Value format.
* @internalComponent
*
*/


#include "tlv.h"


TLV::TLV(TFieldType aExpectedType, CFileContents& aFileContents, TFieldType aPresetType) :
	iFileContents (aFileContents)
	{
	if (aPresetType == EFieldTypeInvalid)
		{
		iType = iFileContents.GetType ();
		}
	else
		{
		iType = aPresetType;
		}

	if (iType != aExpectedType)
		{
		throw new TLVException (ETLVUnexpectedType, aExpectedType, iType);
		}
	iLength = iFileContents.GetLength ();
	iFileContents.SetRemainingLength(iLength);
	iBinary = iFileContents.GetCurrentAddress ();
	aFileContents.Advance (iFileContents.AlignedLength());
	}
