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

#ifndef __SISPROPERTY_H__
#define __SISPROPERTY_H__

#include "structure.h"
#include "numeric.h"


class CSISProperty : public CStructure <CSISFieldRoot::ESISProperty>

	{
public:
	typedef TUint32 TKey;
	typedef TUint32 TValue;

private:
	void InsertMembers ();

public:
	CSISProperty ();
	CSISProperty (const CSISProperty& aInitialiser);
	CSISProperty (const TKey aKey, const TValue aValue);

	virtual std::string Name () const;
#ifdef GENERATE_ERRORS
	virtual void CreateDefects ();
#endif // GENERATE_ERRORS
	
	void AddPackageEntry(std::wostream& aStream, bool aVerbose) const;
	
	inline TKey Key() const;
	inline TValue Value() const;

private:
	CSISInt32	iKey;
	CSISInt32	iValue;

	};



inline void CSISProperty::InsertMembers ()
	{
	InsertMember (iKey);
	InsertMember (iValue);
	}


inline CSISProperty::CSISProperty ()
	{
	InsertMembers ();
	}


inline CSISProperty::CSISProperty (const CSISProperty& aInitialiser) :
		CStructure <CSISFieldRoot::ESISProperty> (aInitialiser),
		iKey (aInitialiser.iKey),
		iValue (aInitialiser.iValue)
	{
	InsertMembers ();
	}


inline CSISProperty::CSISProperty (const TKey aKey, const TValue aValue) :
		iKey (aKey),
		iValue (aValue)
	{
	InsertMembers ();
	}


inline std::string CSISProperty::Name () const
	{
	return "Property";
	}

#ifdef GENERATE_ERRORS
inline void CSISProperty::CreateDefects ()
	{
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iKey = rand ();
		}
	if (CSISFieldRoot::IsBugToBeCreated (CSISFieldRoot::EBugInvalidValues))
		{
		iValue = rand ();
		}
	}
#endif // GENERATE_ERRORS

inline void CSISProperty::AddPackageEntry(std::wostream& aStream, bool aVerbose) const
	{
	(void)aVerbose;
	aStream << iKey << L"=" << iValue;
	}

inline CSISProperty::TKey CSISProperty::Key() const
	{
	return iKey;
	}

inline CSISProperty::TValue CSISProperty::Value() const
	{
	return iValue;
	}



#endif // __SISPROPERTY_H__

