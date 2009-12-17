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


#include "sisdependency.h"

SISDependency::SISDependency()
	:iDependencyUid (0) ,
	iFromMajorVersion (0) ,
	iFromMinorVersion (0) ,
	iFromBuild (0) ,
	iToMajorVersion (0) ,
	iToMinorVersion (0) ,
	iToBuild (0)
	{
	}

void SISDependency::ExtractDependency(StreamReader& aReader )
	{
	//uid of SISX package which needs to be installed in order to satisfy the dependency
	iDependencyUid = aReader.ReadInt32();
	//range of versions of the SISX package that needs to be installed on the device. 
	iFromMajorVersion = aReader.ReadInt32();
	iFromMinorVersion = aReader.ReadInt32();
	iFromBuild = aReader.ReadInt32();
	iToMajorVersion = aReader.ReadInt32();
	iToMinorVersion = aReader.ReadInt32();
	iToBuild = aReader.ReadInt32();
	}

void SISDependency::DisplayDependencyInfo()
	{
	cout << "Dependent Package UID : " << std::hex << "0x" << iDependencyUid << endl;
	cout << "Dependent Package From Version : " << std::dec << iFromMajorVersion << "." << iFromMinorVersion << "." << iFromBuild << endl;
	cout << "Dependent Package To Version : " <<std::dec << iToMajorVersion << "." << iToMinorVersion << "." << iToBuild << endl << endl;
	}

SISDependency::~SISDependency()
	{
	}






























