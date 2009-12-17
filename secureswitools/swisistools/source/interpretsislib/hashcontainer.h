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


#ifndef	HASHCONTAINER_H
#define	HASHCONTAINER_H

#include <istream>
#include "serialisable.h"
#include "symbiantypes.h"
#include <vector>

class Serialiser;
class Deserialiser;
/**
* @file HASHCONTAINER.H
*
* @internalComponent
* @released
*/
class HashContainer : public Serialisable
	{
public:

	enum HashId
		{
		EHashMD2,
		EHashMD5,
		EHashSHA,
		EHashIllegal
		};

	/**
	Constructor
	@param aFileContents The input stream
	*/
	HashContainer ();
	virtual ~HashContainer ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);

	const std::string& GetData() const
	{ return iData; }

	void SetData(const std::string& aData)
	{ iData = aData; }

	TInt GetHashId() const
	{ return iHashId; }

	void SetHashId(TInt aHashId)
	{ iHashId = aHashId; }

private:
	TInt             iHashId;
	std::string        iData;
	};

#endif	/* HASHCONTAINER_H */
