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


#ifndef	SISREGISTRYPROPERTY_H
#define	SISREGISTRYPROPERTY_H

#include <istream>
#include "serialisable.h"
#include "symbiantypes.h"
#include <vector>

class Serialiser;
class Deserialiser;
class CSISProperty;
/**
* @file SISREGISTRYPROPERTY.H
*
* @internalComponent
* @released
*/
class SisRegistryProperty : public Serialisable//<SisRegistryProperty>
	{
public:
	/**
	Constructor
	*/
	SisRegistryProperty () {};
	SisRegistryProperty(const CSISProperty& aProp);

	virtual ~SisRegistryProperty ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);
	//virtual Serialisable* Make() { return new SisRegistryProperty; }

	TInt GetValue() const
	{ return iValue; }

	void SetValue(TInt aValue)
	{ iValue = aValue; }

	TInt GetKey() const
	{ return iKey; }

	void SetKey(TInt aKey)
	{ iKey = aKey; }

private:
	TInt iKey;
	TInt iValue;
    };


#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<SisRegistryProperty*>& val);

#endif

#endif	/* SISREGISTRYPROPERTY_H */
