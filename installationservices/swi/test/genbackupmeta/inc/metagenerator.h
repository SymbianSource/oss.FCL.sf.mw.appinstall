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
* For generating meta-data of a SIS file.
* @internalComponent
*
*/



#ifndef	METAGENERATOR_H
#define	METAGENERATOR_H

#pragma warning( disable : 4786)

#include "symbiantypes.h"
#include "sisfieldtypes.h"

#include <vector>
#include <iostream>

class TExceptionHandler;
class CCmdParser;
class CStreamWriter;
class CFileContents;
class CSISCompressed;

class CMetaGenerator
	{
	public:
		CMetaGenerator(	const std::string& aUid, const int aDriveName, 
						const std::vector<std::string>& aSISFileNames,
						const std::vector<std::string>& aFileNames, 
						const std::string& aMetaFileName );

		void GenerateMetaData () throw (TExceptionHandler);
		void CleanUp();
		~CMetaGenerator();

	private:
		CMetaGenerator (const CMetaGenerator&);
		CMetaGenerator& operator= (const CMetaGenerator&);
		
		// Ownership of memory cleanup of the caller
		const char* GetControllerDetails(TUint32& aControllerLength, CFileContents aSISReader) const;

		void ValidateSISFiles(); 
		void WriteControllers();

	public:
		std::string iMetaFileName;
		static std::string FilePrefix;

	private:
		CStreamWriter* iStreamWriter;

		std::string iUid;
		int iDriveName;
		std::vector<std::string> iSISFileNames;
		std::vector<std::string> iFileNames;
	};

#endif // MAKEMETA_H