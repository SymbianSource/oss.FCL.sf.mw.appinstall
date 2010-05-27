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
//
/** 
* @file barsc2.h
*
* @internalComponent
* @released
*/
#ifndef __BARSC2_H__
#define __BARSC2_H__

#include <string>
#include "barscimpl.h"

class RResourceFileImpl;

/**
This class accesses a resource file and reads the resource data into a buffer.
Interpretation of the data is achieved using the RResourceReader class.
*/
class CResourceFile 
	{
public:
	/**
	Initializes in place the implementation object and 
	opens the resource file for reading.
	@param aName Resource file name
	@param aFileOffset Resource file offset
	@param aFileSize Resource file size	
	*/
	CResourceFile(const std::string& aName, TUint32 aFileOffset, TInt aFileSize);
	
	/**
	Cleanup the allocated memory
	*/
	~CResourceFile();
	
	/** 
	Reads a resource into a heap and returns the correspoding pointer to it.
	
	Ownership of the allocated heap is passed to the caller who must destroy 
	it when it is no longer needed.

	The search for the resource uses the following algorithm:

	@param aResourceId The numeric id of the resource to be read.
	@return Pointer to the allocated heap memory containing the resource.
	*/
	Ptr8* AllocReadL(const TInt& aResourceId);

	/** Initialises the offset value from the first resource.

	The function assumes that the first resource in the file consists of
	two 32-bit integers. The first integer contains the version number and
	the second is a self-referencing link whose value is the offset for
	the resources in the file, plus 1.This function must be called before
	calling Offset(), AllocReadL(), AllocReadLC() or ReadL().

	@param aSignature This argument value is not used by the function.
	@leave The function leaves if this resource id is not in this
	resource file or the file is corrupted. */
	void ConfirmSignatureL();
	
	/** 
	Tests whether the resource file owns the specified resource id.
	@param aResourceId The resource id to test.
	@return True, if the resource file owns the id, false otherwise.
	*/
	TBool OwnsResourceId(const TInt& aResourceId) const;

	/** 
	Reads Resource File Uid3.
	@param  Void
	@return Uid3 of Resource File.
	*/

	TUid ReadAppUidL();

	/** 
	Reads Resource File Uid2.
	@param  Void
	@return Uid2 of Resource File.
	*/

	TUid ReadFileUidL();
	 
private:
	/**
	prevent default copy constructor
	*/
	CResourceFile(const CResourceFile&);
	/**
	prevent default assignment operator
	*/
	CResourceFile& operator=(const CResourceFile&);
	/** 
	@return Non-const pointer to the implementation. 
	*/
	RResourceFileImpl* Impl();
	/** 
	@return Const pointer to the implementation. 
	*/
	const RResourceFileImpl* Impl() const;

private:
	enum
		{
		KRscFileSize = 24,
		};
	TUint8 iImpl[KRscFileSize];
	};


/**
This class implements the exception handling mechanism
required for resource file parsing.
*/
class CResourceFileException
{
public:

	/**
	Initializes the CResourceFileException class with the required
	exception message.
	@param aValue Exception message that is to be displayed.
	*/
	inline CResourceFileException(const std::string& aValue)
							   :iValue(aValue)
		{
		}

	/**
	Displays the exception message
	*/
	void Display() const;

	/**
	Get the exception message
	*/
	std::string GetMsg();

private:
	std::string iValue;
};

#endif//__BARSC2_H__
