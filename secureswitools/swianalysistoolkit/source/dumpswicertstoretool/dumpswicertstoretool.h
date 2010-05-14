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


/**
 @file 
 @internalComponent 
*/

#ifndef __DUMPSWICERTSTORETOOL_H__
#define __DUMPSWICERTSTORETOOL_H__
#pragma warning(disable: 4710)
#pragma warning(disable: 4786)

#include "options.h"
#include "pfsdump.h"
#include "streamreader.h"
#include "swicertstoremetadata.h"
#include "osinterface.h"

class Options;
class Stream;
class StoreFile;
class StreamReader;
class Certificate;
class SwiCertStoreMetaData;

/**
  * The DumpSWICertstoreTool class checks the certstore type(ROM Based,RAM Based or both) and provides 
  * functions which checks whether the dat file is valid or not and accordingly gets the infostream 
  * where the swicertstore metadata is available.
  * @internalComponent 
  */
class DumpSWICertstoreTool
{
public:

	DumpSWICertstoreTool();

	~DumpSWICertstoreTool();

	/**
	   Based on the input provided(ROM Based,C:\Based or both) , accordingly calls functions which 
	   checks for the input file validity(is a permanent file store or not) ,fetches the infostream id
	   containing the certificates and their capabilities information(metadata) and reads the infostream.
	  */
	void Run(const Options& aOptions);

	int TocRevision() const {  return iTocRevision; }

	static ostream& Error();

	static ostream& Warning();

private:

	/**
	   Fetches the infostream id from the rootstream and accordingly calls functions which 
	   extracts information from the infostream.
	  */
	void Initialize(const Options& aOptions , StoreFile& aStore);

	/**
	   Parses each file(from highest numbered file to lowest) present in the C:\Based Directory and 
	   checks for its validity(is a permanent file store or not).The highest numbered valid file
	   is then considered for extracting the certificate and capabilities information.
	  */
	bool InitializeCBasedCertstore(const Options& aOptions);

	/**
	   Checks the validity(is a permanent file store or not) of swicertstore.dat(ROM Based) file and accordingly
	   calls the Initialize() function to extract the infostream.
	  */
	bool InitializeROMCerstore(const Options& aOptions);


private:

	int iTocRevision ;
	int iErrors ;
	string iIssuer ;
	string iMandatory ;
};

extern DumpSWICertstoreTool Tool;
#endif
