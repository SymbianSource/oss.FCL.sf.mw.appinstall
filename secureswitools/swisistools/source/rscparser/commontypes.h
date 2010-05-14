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
* @file commontypes.h
*
* @internalComponent
* @released
*/
#ifndef	COMMONTYPES_H
#define	COMMONTYPES_H

#include "symbiantypes.h"
#include "uidtype.h"
#include <vector>
using namespace std;
using namespace rscparser;

/** 
Returns the smaller of two values.
 
@param aLeft  The first value to be compared.
@param aRight The second value to be compared.

@return The smaller value.
*/
template <class T>
inline T Min(T aLeft,T aRight)
    {return(aLeft<aRight ? aLeft : aRight);}

/**
Returns the smaller of two objects, where the right hand object is a treated
as a TInt for the  purpose of comparison.

@param aLeft  The first value to be compared.
@param aRight The second value to be compared.

@return The smaller value.
*/
template <class T>
inline T Min(T aLeft,TUint aRight)
    {return(aLeft<(TInt)aRight ? aLeft : (T)aRight);}

/** 
Returns the larger of two values.

@param aLeft  The first value to be compared.
@param aRight The second value to be compared.

@return The larger value.
*/
template <class T>
inline T Max(T aLeft,T aRight)
    {return(aLeft<aRight ? aRight : aLeft);}

/**
Returns the larger of two objects, where the right hand object is a treated
as a TInt for the  purpose of comparison.

@param aLeft  The first value to be compared.
@param aRight The second value to be compared.

@return The larger value.
 */
template <class T>
inline T Max(T aLeft,TUint aRight)
    {return(aLeft<(TInt)aRight ? (TInt)aRight : aLeft);}

/**
Class to allocate heap memory and store a pointer to it 
for unicode data.
The maximun allocated length and current length would also 
be maintained.
*/
class Ptr8
{

public:
	/**
	Allocate heap of the specified size and 
	store the pointer to it.
	@param aSize Size of the heap to be allocated.
	*/
	Ptr8(const TUint32& aSize);

	/**
	Free the allocated heap 
	*/
	~Ptr8();

	/**
	Get the pointer to the allocated heap
	*/
	TUint8* GetPtr() const;
	/**
	Set the pointer
	*/
	void SetPtr(TUint8* aPtr);

	/**
	Set the length
	*/
	void SetLength(const TUint32& aLength);
	void ForceLength(const TUint32& aLength);
	
	/**
	Update the length of the current pointer
	with the length provided
	@param aLength Length to be updated
	*/
	TBool UpdateLength(const TUint32& aLength);

	/**
	To fetch the current length
	*/
	TUint32 GetLength() const;

	/**
	To fetch the Max length
	*/
	TUint32 GetMaxLength() const;

 	TUint8 operator[](TInt anIndex) const;

	void Append(TUint8* aBuffer, TInt aLen);

private:
	TUint8* iPtr;
	TUint32 iLength;
	const TUint32 iMaxLength;
};


/**
Class to allocate heap memory and store a pointer to it 
for non-unicode data.
The maximun allocated length and current length would also 
be maintained.
*/
class Ptr16
{

public:
	/**
	Allocate heap of the specified size and 
	store the pointer to it.
	@param aSize Size of the heap to be allocated.
	*/
	Ptr16(const TUint32& aSize);

	/**
	Free the allocated heap 
	*/
	~Ptr16();

	/**
	Get the pointer to the allocated heap
	*/
	TUint16* GetPtr() const;

	/**
	Set the pointer
	*/
	void SetPtr(TUint16* aPtr);

	/**
	Update the length of the current pointer
	with the length provided
	@param aLength Length to be updated
	*/
	TBool UpdateLength(const TUint32& aLength);
	
	/**
	To fetch the current length
	*/
	TUint32 GetLength() const;

 	TUint16 operator[](TInt anIndex) const;

private:
	TUint16* iPtr;
	TUint32 iLength;
	const TUint32 iMaxLength;
};


/**
Structure to represent TPtrC8 from symbian data type
*/
struct PtrC8
{	
	PtrC8();
	~PtrC8();
	const TUint8* iPtr;
	TUint32 iMaxLength;
};


/**
Structure to represent TPtrC16 from symbian data type
*/
struct PtrC16
{	
	PtrC16();
	~PtrC16();
	TUint16* iPtr;
	TUint32 iMaxLength;
};


/**
Structure to hold the UIDs of the RSC file
*/

struct sTUid
{	
	sTUid();
	TUint32 iUid1;
	TUint32 iUid2;
	TUint32 iUid3;
};

/**
System wide error code -1 : item not found.

A system wide error code indicates an error in the environment, or in
user input from which a program may recover.
*/
const TInt KErrNotFound=(-1); // Must remain set to -1

/**
System wide error code 0 : this represents the no-error condition.

A system wide error code indicates an error in the environment, or in
user input from which a program may recover.
*/
const TInt KErrNone=0; 

/**
System wide error code -25 : indicates that end of file has been reached.

A system wide error code indicates an error in the environment, or in
user input from which a program may recover.

Note that RFile::Read() is a higher-level interface. When the end of
the file is reached, it returns zero bytes in the destination descriptor, and
a KErrNone return value. KErrEof is not used for this purpose; other error
conditions are returned only if some other error condition was indicated on
the file.
*/
const TInt KErrEof=(-25);


const TInt KExecutableImageUidValue=0x1000007a;

const TUid KExecutableImageUidVal={KExecutableImageUidValue};

/** The hexadecimal value of the 2nd UID that defines a DLL as being a Unicode 
UI application. In .mmp files, the hexadecimal number is explicitly inserted 
as the first value following the UID keyword.

@publishedAll 
@released
@see KAppUidValue */
const TInt KAppUidValue16 = 0x100039CE;

/** The 2nd UID that defines a DLL as being a Unicode UI application.

@publishedAll 
@released
@see KUidApp */
const TUid KUidApp16={KAppUidValue16};

/** The type-independent 2nd UID that identifies a DLL as being a UI application.

@publishedAll 
@released
@see KUidApp16 */
#define KUidApp KUidApp16

/** 
The uid for the Open service.

@publishedPartner
@released
*/
const TUid KOpenServiceUid = { 0x10208DCA };

/** An application group name.

This is a name that allows applications to be categorized, for instance "Games" 
or "Utilities". 

@publishedAll 
@released */
typedef Ptr16* TAppGroupName;
#endif	/* COMMONTYPES_H */
