/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISLOGO_H__
#define __SISLOGO_H__

#include "sisfiledescription.h"

class CSISData;

class CSISLogo : public CStructure <CSISFieldRoot::ESISLogo>

	{
public:
	CSISLogo (const bool bRequired = true);
	CSISLogo (const CSISLogo& aInitialiser);

	virtual std::string Name () const;
	virtual bool WasteOfSpace () const;

	void Set (const std::wstring& aMime, const std::wstring& aTarget, const TUint32 aFileIndex, const CSISDataUnit& aDataUnit);
	void SetSizes (const TUint64 aCompressedSize, const TUint64 aUncompressedSize);
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	inline const CSISFileDescription& FileDesc() const; 

private:
	CSISFileDescription iLogoFile;

	};


inline CSISLogo::CSISLogo (const bool bRequired) :
		CStructure <CSISFieldRoot::ESISLogo> (bRequired),
		iLogoFile()
	{
	InsertMember (iLogoFile); 
	}


inline CSISLogo::CSISLogo (const CSISLogo& aInitialiser) :
		CStructure <CSISFieldRoot::ESISLogo> (aInitialiser),
		iLogoFile (aInitialiser.iLogoFile)
	{
	InsertMember (iLogoFile); 
	}

inline void CSISLogo::SetSizes (const TUint64 aCompressedSize, const TUint64 aUncompressedSize)
	{
	iLogoFile.SetLengths (aUncompressedSize, aCompressedSize);
	}

inline const CSISFileDescription& CSISLogo::FileDesc() const
	{
	return iLogoFile;
	}


#include "sisdata.h"

#endif // __SISLOGO_H__

