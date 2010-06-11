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


#ifndef	CARDINALITY_H
#define	CARDINALITY_H

#include <istream>
#include "serialisable.h"
#include "symbiantypes.h"
#include <vector> 

class Serialiser;
class Deserialiser;
/** 
* @file CARDINALITY.H
*
* @internalComponent
* @released
*/
class Cardinality : public Serialisable//<Cardinality>
	{
public:
	/**
	Constructor
	*/
	Cardinality ();
	virtual ~Cardinality ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	operator TUint32() const { return iSize; }
	
	TUint32 GetSize() const
	{ return iSize; }

	void SetSize(TUint32 aSize)
	{ iSize = aSize; }

private:
	TUint32             iSize;
	};

#endif	/* CARDINALITY_H */
