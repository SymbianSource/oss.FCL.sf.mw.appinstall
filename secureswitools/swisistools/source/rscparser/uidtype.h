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
// uidtype.h
//

/** 
* @file uidtype.h
*
* @internalComponent
* @released
*/

#ifndef __UIDTYPE_H__
#define __UIDTYPE_H__

#include <iostream>
#include "symbiantypes.h"

namespace rscparser
{
/**
@internalComponent
@released

Defines a Null UID value.

@see TUid
*/
const TInt KNullUidValue=0;

/**
@publishedAll
@released

Defines the number of TUids that form a TUidType.

@see TUid
@see TUidType
*/
const TInt KMaxCheckedUid=3;


/**
@publishedAll
@released

A globally unique 32-bit number.
*/
class TUid
	{
public:
	TBool operator==(const TUid& aUid) const;
	TBool operator!=(const TUid& aUid) const;
	static inline TUid Uid(TInt aUid);
	static inline TUid Null();
	TUint32 GetUid();
	
public:
	/**
	The 32-bit integer UID value.
	*/
	TUint32 iUid;
	};


/**
@publishedAll
@released

Defines a null UID
*/
#define NullUid TUid::Null()

/**
Constructs a Null-valued TUid object.

@return The constructed Null-valued TUid object.
*/
inline TUid TUid::Null()
	{TUid uid={KNullUidValue};return uid;}

/**
Constructs the TUid object from a 32-bit integer.

@param aUid The 32-bit integer value from which the TUid object is to be
            constructed.

@return The constructed TUid object.
*/
inline TUid TUid::Uid(TInt aUid)
	{TUid uid={aUid};return uid;}
	
/**
@publishedAll
@released

Encapsulates a set of three unique identifiers (UIDs) which, in combination, 
identify a system object such as a GUI application or a DLL. The three
component UIDs are referred to as UID1, UID2 and UID3.

An object of this type is referred to as a compound identifier or a UID type.
*/
class TUidType
	{
public:

	 TUidType();
	 TUidType(TUid aUid1,TUid aUid2,TUid aUid3);
	 TBool operator==(const TUidType& aUidType) const;
	 const TUid& operator[](TInt anIndex) const;
	
private:
	TUid iUid[KMaxCheckedUid];
	};
}
#endif