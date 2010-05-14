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
*
*/


#ifndef	FILEDESCRIPTION_H
#define	FILEDESCRIPTION_H

#include <istream>
#include "serialisable.h"
#include "symbiantypes.h"
#include "hashcontainer.h"

#include <vector>

class Serialiser;
class Deserialiser;
class CSISFileDescription;

/**
* @file FILEDESCRIPTION.H
*
* @internalComponent
* @released
*/
class FileDescription : public Serialisable
	{
public:
	/**
	Constructor
	*/
	FileDescription () {};
	#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	FileDescription (const CSISFileDescription& aFile, TInt aSid, const TInt aTargetDrive, const std::wstring& aFileName);
	#else
	FileDescription (const CSISFileDescription& aFile, TInt aSid, 
						const TInt aTargetDrive, const std::wstring& aFileName, const std::wstring& aLocalName);
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

	virtual ~FileDescription ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	TInt GetSid() const
	{ return iSid; }

	void SetSid(const TInt& aSid)
	{ iSid = aSid; }

	TInt GetIndex() const
	{ return iIndex; }

	void SetIndex(const TInt& aIndex)
	{ iIndex = aIndex; }

	TInt64 GetUncompressedLength() const
	{ return iUncompressedLength; }

	void SetUncompressedLength(const TInt64& aUncompressedLength)
	{ iUncompressedLength = aUncompressedLength; }

	const HashContainer& GetHash() const
	{ return iHash; }

	void SetHash(const HashContainer& aHash)
	{ iHash = aHash; }

	TInt GetOperationOptions() const
	{ return iOperationOptions; }

	void SetOperationOptions(const TInt& aOperationOptions)
	{ iOperationOptions = aOperationOptions; }

	TInt GetOperation() const
	{ return iOperation; }

	void SetOperation(const TInt& aOperation)
	{ iOperation = aOperation; }

	const std::wstring& GetMimeType() const
	{ return iMimeType; }

	void SetMimeType(const std::wstring& aMimeType)
	{ iMimeType = aMimeType; }

	const std::wstring& GetTarget() const
	{ return iTarget; }

	void SetTarget(const std::wstring& aTarget)
	{ iTarget = aTarget; }

	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	const std::wstring& GetLocalFile() const
	{ return iLocalFile; }

	void SetLocalFile(const std::wstring& aLocalFile)
	{ iLocalFile = aLocalFile; }
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

private:
	std::wstring  iTarget;
	#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	// Full file name to the PC location
	std::wstring iLocalFile;
	#endif //SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
	
	std::wstring  iMimeType;
	TInt        iOperation;
	TInt        iOperationOptions;
	HashContainer iHash;
	TInt64        iUncompressedLength;
	TInt        iIndex;
	TInt        iSid;
    };


#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<FileDescription*>& val);

#endif

#endif	/* FILEDESCRIPTION_H */
