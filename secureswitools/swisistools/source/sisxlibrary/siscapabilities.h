/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @publishedPartner 
*/

#ifndef __SISCAPABILITIES_H__
#define __SISCAPABILITIES_H__
 
#include "structure.h"
#include "numeric.h"
#include "sisfiledata.h"

#include <vector>

static const wchar_t* KCapabilityNames[] =
	{	
	L"TCB",
	L"CommDD",
	L"PowerMgmt",
	L"MultimediaDD",
	L"ReadDeviceData",
	L"WriteDeviceData",
	L"DRM",
	L"TrustedUI",
	L"ProtServ",
	L"DiskAdmin",
	L"NetworkControl",
	L"AllFiles",
	L"SwEvent",
	L"NetworkServices",
	L"LocalServices",
	L"ReadUserData",
	L"WriteUserData",
	L"Location",
	L"SurroundingsDD",
	L"UserEnvironment"
	};

#define NUMOFCAPABILITIES (sizeof(KCapabilityNames) / sizeof(KCapabilityNames[0]))

class CSISCapabilities : public CStructure <CSISFieldRoot::ESISCapabilities>
	{
public:
	/**
	 * Default constructor
	 */
	explicit CSISCapabilities(const bool aRequired = false);
	/**
	 * Copy constructor.
	 */
	CSISCapabilities(const CSISCapabilities& aInitialiser);

	/**
	 * This function will verify the class content. Throws
	 * an exception if found invalid.
	 * @param aLanguages Number of languages present in the sis file.
	 */
	virtual void Verify (const TUint32 aLanguages) const;
	/**
	 * Class Name
	 */
	virtual std::string Name () const;
	/**
	 * Function checks if this class should be used or not.
	 * Mainly used in externalization.
	 * @return 	if true, don't bother writing this record to disc
	 * 			if false, when reading, it'd better be there
	 */
	virtual bool WasteOfSpace() const;

	/**
	 * Function will extract capabilities of the file passed
	 * and store it in the class.
	 * @param aFileName filename of the file whose capability needs to be extracted.
	 */
	void ExtractCapabilities(const std::wstring& aFileName);
	/**
	 * Adds the write the package details into the stream.
	 * @param aStream - Stream in which the package entries need to be written.
	 * @param aVerbose - If this option is set then detail description of pkg
	 * 			will be written into the stream.
	 */
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	/**
	 * Get the stored capability list in human readable format.
	 * @param aCapList will be filled by this function.
	 */
	void GetCapabilityList(std::vector<std::wstring>& aCapList) const;
	/**
	 * Returns the stored capability.
	 */
	inline TUint32 Capabilities() const;

private:
	void InsertMembers ();

private:
	CSISUInt32 iCapabilities;
	int	iCapabilitySetSize;
	
	};


inline void CSISCapabilities::InsertMembers()
	{
	InsertMember(iCapabilities);
	}

inline CSISCapabilities::CSISCapabilities(const bool aRequired)
	: CStructure<CSISFieldRoot::ESISCapabilities> (aRequired), 
	iCapabilities(0),
	iCapabilitySetSize(0)
	{
	InsertMembers();
	}

inline TUint32 CSISCapabilities::Capabilities() const
	{
	return iCapabilities.Value();
	}


#endif /* __SISCAPABILITIES_H__ */
