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



#ifndef	CONTROLLERINFO_H
#define	CONTROLLERINFO_H

// System includes
#include <istream>

// User includes
#include "serialisable.h"
#include "symbiantypes.h"
#include "version.h"
#include "hashcontainer.h"


// Forward Declarations
class CSISController;
class Serialiser;
class Deserialiser;
/**
* @file CONTROLLERINFO.H
*
* @internalComponent
* @released
*/
class ControllerInfo : public Serialisable//<ControllerInfo>
	{
public:
	/**
	Constructor
	@param aFileContents The input stream
	*/
	ControllerInfo () {};
	virtual ~ControllerInfo ();

	virtual void Internalize(Deserialiser& des);
	virtual void Externalize(Serialiser& ser);
	
	void CalculateAndSetHash(const CSISController& aController, 
							TUint16 aRegFileMajorVersion, 
							TUint16 aRegFileMinorVersion);

	const HashContainer& GetHashContainer() const
	{ return iHashContainer; }

	void SetHashContainer(const HashContainer& aHashContainer)
	{ iHashContainer = aHashContainer; }

	TInt GetOffset() const
	{ return iOffset; }

	void SetOffset(TInt aOffset)
	{ iOffset = aOffset; }

	const Version& GetVersion() const
	{ return iVersion; }

	void SetVersion(const Version& aVersion)
	{ iVersion = aVersion; }

private:
	Version       iVersion;
	TInt        iOffset;
	HashContainer iHashContainer;
	};


#ifdef _MSC_VER

Deserialiser& operator>>(Deserialiser& aInput, std::vector<ControllerInfo*>& val);

#endif

#endif	/* CONTROLLERINFO_H */
