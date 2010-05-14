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
// Description:
/** 
* @file uidtype.cpp
*
* @internalComponent
* @released
*/
#include <cassert>
#include "uidtype.h"
using namespace rscparser;
// Class TUid

TBool TUid::operator==(const TUid& aUid) const
/**
Compares two UIDs for equality.

@param aUid The UID to be compared with this UID.

@return True, if the two UIDs are equal; false otherwise. 
*/
	{

	return(iUid==aUid.iUid);
	}

TBool TUid::operator!=(const TUid& aUid) const
/**
Compares two UIDs for inequality.

@param aUid The UID to be compared with this UID.

@return True, if the two UIDs are unequal; false otherwise. 
*/
	{

	return(iUid!=aUid.iUid);
	}

/**
Get  UID.

@param Void

@return UID. 
*/
TUint32 TUid::GetUid()
{
	return iUid;
}

TUidType::TUidType()
/**
Default constructor.

Creates a UID type, and sets all three component UIDs to NullUid.
*/
    {

	memset(this,0,sizeof(TUidType));
    }

TUidType::TUidType(TUid aUid1,TUid aUid2,TUid aUid3)
/**
Constructor that creates a UID type and sets all three UID components
to the specified values.

@param aUid1 Value for UID1.
@param aUid2 Value for UID2.
@param aUid3 Value for UID3.
*/
    {


    iUid[0]=aUid1;
    iUid[1]=aUid2;
    iUid[2]=aUid3;
    }


const TUid& TUidType::operator[](TInt aIndex) const
/**
Gets the UID component as identified by the specified index.

@param aIndex Index value indicating which UID component to return.
               0 specifies UID1,
               1 specifies UID2,
               2 specifies UID3.

@return A reference to the required UID component.

@panic USER 37 if aIndex is not in the range 0 to 2, inclusive.
*/
    {

	assert(aIndex>=0 && aIndex<KMaxCheckedUid);
	return(iUid[aIndex]);
    }

TBool TUidType::operator==(const TUidType& aUidType) const
/**
Compares this UID type for equality with the specified UID type.

@param aUidType The UID type to be compared. 

@return True, if each component UID is equal to the corresponding component 
        UID in the specified UID type; false, otherwise.
*/
    {

    return(iUid[0]==aUidType.iUid[0] &&
           iUid[1]==aUidType.iUid[1] &&
           iUid[2]==aUidType.iUid[2]);
    }

TCheckedUid::TCheckedUid()
//
// Constructor
//
	{

	memset(this,0,sizeof(TCheckedUid));
	}

TCheckedUid::TCheckedUid(const TUidType& aUidType)
//
// Constructor
//
	{

	Set(aUidType);
	}

void TCheckedUid::Set(const TUidType& aUidType)
//
// Set from a aUidType
//
	{
	iType=aUidType;
	}

const TUidType&  TCheckedUid::UidType() const
{
	return (iType);
}

void TCheckedUid::Set(const TUint8* aDes, TInt aLen)
    {
   	assert(aLen == sizeof(TCheckedUid));
    memcpy(this,aDes,aLen);
    }

TCheckedUid::TCheckedUid(const TUint8* aDes, TInt aLen)
    {
    memset(this,0,sizeof(TCheckedUid));
    Set(aDes, aLen);
    }
