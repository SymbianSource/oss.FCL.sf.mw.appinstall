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


#include "siscontrollerinfo.h"

SISControllerInfo::SISControllerInfo()
	:iMajor (0) ,
	iMinor (0),
	iBuild( 0)  ,
	iOffset (0),
	iAlgorithmType (0)
	{
	iAlgorithms[0] = "MD2" ;
	iAlgorithms[1] = "MD5" ;
	iAlgorithms[2] = "SHA1" ;
	iAlgorithms[3] = "HMAC" ;
	}

void SISControllerInfo::ExtractControllerInfo(StreamReader& aReader )
	{	
	iMajor = aReader.ReadInt32();
	iMinor = aReader.ReadInt32();
	iBuild = aReader.ReadInt32();
	iOffset = aReader.ReadInt32();
	iAlgorithmType = aReader.ReadInt32();
	char* controllerHash = aReader.ReadDescriptor();
	delete [] controllerHash;
	}


void SISControllerInfo::DisplayControllerInfo(int i)
	{
	cout << endl ;
	cout << "Controller" << i+1 << " Version :" << iMajor << "." << iMinor << "." << iBuild << endl;
	if(iAlgorithms.find(iAlgorithmType) != iAlgorithms.end())
		{
		cout << "Controller" << i+1 << " Algorithm :" << iAlgorithms[iAlgorithmType] << endl << endl;
		}
	else
		{
		cout << "Controller" << i+1 << " Algorithm :" << "Unknown" << endl << endl;
		}
		
	}


SISControllerInfo::~SISControllerInfo()
	{
	}
	


