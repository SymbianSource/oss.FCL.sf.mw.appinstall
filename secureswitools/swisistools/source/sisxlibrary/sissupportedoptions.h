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

#ifndef __SISSUPPORTEDOPTIONS_H__
#define __SISSUPPORTEDOPTIONS_H__


#include "sissupportedoption.h"
#include "sisarray.h"

class CSISSupportedOptions : public CStructure <CSISFieldRoot::ESISSupportedOptions>

	{
public:
	CSISSupportedOptions ();
	CSISSupportedOptions (const CSISSupportedOptions& aInitialiser);

	virtual std::string Name () const;

	void AddOption ();
	void AddOptionName (const std::wstring& aName);
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	inline TUint32 SupportedOptionCount() const;
	
private:
	CSISArray <CSISSupportedOption, CSISFieldRoot::ESISSupportedOption>	iSupportedOption;
	};



inline CSISSupportedOptions::CSISSupportedOptions ()
	{
	InsertMember (iSupportedOption); 
	}


inline CSISSupportedOptions::CSISSupportedOptions (const CSISSupportedOptions& aInitialiser) :
		CStructure <CSISFieldRoot::ESISSupportedOptions> (aInitialiser),
		iSupportedOption (aInitialiser.iSupportedOption)
	{
	InsertMember (iSupportedOption); 
	}


inline void CSISSupportedOptions::AddOption ()
	{
	iSupportedOption.Push (CSISSupportedOption ()); 
	}

inline void CSISSupportedOptions::AddOptionName (const std::wstring& aName)
	{
	assert (iSupportedOption.size () > 0);
	iSupportedOption.Last ().AddOptionName (aName);
	}

inline std::string CSISSupportedOptions::Name () const
	{
	return "Supported Options";
	}

inline TUint32 CSISSupportedOptions::SupportedOptionCount() const
	{
	return iSupportedOption.size();
	}

#endif // __SISSUPPORTEDOPTIONS_H__

