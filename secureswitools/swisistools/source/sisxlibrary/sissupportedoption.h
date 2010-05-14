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
* As specified in SGL.GT0188.251
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __SISSUPPORTEDOPTION_H__
#define __SISSUPPORTEDOPTION_H__


#include "structure.h"
#include "stringarray.h"


class CSISSupportedOption : public CStructure <CSISFieldRoot::ESISSupportedOption>

	{
public:
	CSISSupportedOption ();
	CSISSupportedOption (const CSISSupportedOption& aInitialiser);

	virtual std::string Name () const;
	virtual void Verify (const TUint32 aLanguages) const;

	void AddOptionName (const std::wstring& aName);
	TUint32 LanguageCount () const;
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;

private:
	CSISArrayOfStrings iNames;
	
	};




inline CSISSupportedOption::CSISSupportedOption ()
	{ 
	InsertMember (iNames); 
	}


inline CSISSupportedOption::CSISSupportedOption (const CSISSupportedOption& aInitialiser) :
		CStructure <CSISFieldRoot::ESISSupportedOption> (aInitialiser),
		iNames (aInitialiser.iNames)
	{ 
	InsertMember (iNames); 
	}


inline void CSISSupportedOption::AddOptionName (const std::wstring& aName)
	{
	iNames.Push (CSISString (aName)); 
	}


inline TUint32 CSISSupportedOption::LanguageCount () const
	{ 
	return iNames.size (); 
	}


#endif // __SISSUPPORTEDOPTION_H__

