// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
/** 
* @file APMSTD.cpp
*
* @internalComponent
* @released
*/
#include "apmstd.h"
#include "barsc2.h"
#include "parse.h"

//
// class TDataType
//

TDataType::TDataType()
	:iDataType(),
	iUid(NullUid)
/** Default constructor.

Sets an empty data type string and sets the associated UID to NullUid. */
	{}


TDataType::~TDataType()
/** Default destructor.

Sets an empty data type string and sets the associated UID to NullUid. */
{
	delete iDataType;
}

TDataType::TDataType(const PtrC8* aDataType)
	:iUid(NullUid)
/** Copy constructor.

@param aDataType The data type to be copied. */
{
	iDataType = new Ptr8(aDataType->iMaxLength);
	if(NULL==iDataType)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	iDataType->UpdateLength(aDataType->iMaxLength);
	BufCpy8(iDataType->GetPtr(), aDataType->iPtr, aDataType->iMaxLength);
}


Ptr8* TDataType::GetDataType()
{
	return iDataType;
}


//
// class TDataTypeWithPriority
//

/** Default constructor */
TDataTypeWithPriority::TDataTypeWithPriority()
{
}

/** Constructor taking a data type and a priority value.

@param aDataType The data type.
@param aPriority The priority value. */
TDataTypeWithPriority::TDataTypeWithPriority(const TDataType& aDataType, TDataTypePriority aPriority)
	: iDataType(aDataType),
	iPriority(aPriority)
	{}
