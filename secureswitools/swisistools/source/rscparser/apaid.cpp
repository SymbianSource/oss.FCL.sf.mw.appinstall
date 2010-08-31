// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: class TAppServiceInfo represents the Service Info
// structure of Resource File. Member Function of this Class are defined. 
//

/** 
* @file apaid.cpp
*
* @internalComponent
* @released
*/

#include "apaid.h"
#include <cassert>

//
// class TAppServiceInfo
//

//Default Constructor for class TAppServiceInfo
TAppServiceInfo::TAppServiceInfo()
	: iUid(NullUid),
	  iDataTypes(0)
{
}

//Destructor for class TAppServiceInfo
TAppServiceInfo::~TAppServiceInfo()
{
}

//Constructor for class TAppServiceInfo
TAppServiceInfo::TAppServiceInfo(TUid aUid, 
	std::vector<TDataTypeWithPriority*> aDataTypes)
	: iUid(aUid),
	  iDataTypes(aDataTypes)
{
}

//Releases the data member
void TAppServiceInfo::Release()
{

	if(0 < iDataTypes.size())
		iDataTypes.clear();
}

/*
* Member function of Class TAppServiceInfo to return private member
* variable iUid
*/
TUid TAppServiceInfo::GetUid()
{
	return iUid;
}

/*
* Member function of Class TAppServiceInfo to return private member
* variable iDataTypes
*/
std::vector<TDataTypeWithPriority*> TAppServiceInfo::GetDataType()
{
	return iDataTypes;
}