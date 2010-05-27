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
* Note: This file may contain code to generate corrupt files for test purposes.
* Such code is excluded from production builds by use of compiler defines;
* it is recommended that such code should be removed if this code is ever published publicly.
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#include "sisdata.h"



void CSISData::Read (TSISStream& aFile, const CSISFieldRoot::TFieldSize& aContainerSize, const CSISFieldRoot::TFieldType aArrayType)
	{
	CStructure <CSISFieldRoot::ESISData>::Read (aFile, aContainerSize, aArrayType);
	iStub = iDataUnit.size () == 0;
	}


CSISData::TDataUnitCount CSISData::AppendData (const CSISData& aData)
	{
	TDataUnitCount newData = 0;
	TDataUnitCount size = aData.iDataUnit.size ();
	for (unsigned int index = 0; index < size; ++index)
		{
		TDataUnitCount added = AddDataUnit (aData.iDataUnit [index]);
		if (newData == TDataUnitCount ())
			{
			newData = added;
			}
		}
	return newData;
	}

void CSISData::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	iDataUnit.AddPackageEntry(aStream, aVerbose);
	}

