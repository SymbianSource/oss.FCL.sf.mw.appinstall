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

#ifndef	__SWICERTSTOREMETADATA_H__
#define	__SWICERTSTOREMETADATA_H__

#include "pfsdump.h"
#include "options.h"
#include "certificate.h"
#include "capabilities.h"
#include <math.h>

class Stream;
class StoreFile;
/**
 * The SwiCertStoreMetaData class provides functions which extracts the contents of infostream.
 * Infostream in a dat file is a stream containing the metadata about all the certificates 
 * and their capabilities.
 * @internalComponent 
 */
class SwiCertStoreMetaData
{
public:

	SwiCertStoreMetaData (Stream& aStream, StoreFile& aStore);

	~SwiCertStoreMetaData();

	/**
	  Extracts information from the infostream.
	 */
	void Read(const Options& aOptions);

	const string& GetIssuer() const { return iIssuer ;}

	const string& GetMandatoryStatusOfAllCertstoreCerts() const { return iMandatoryCert ;}

private:

	/**
	  The capabilities of the certstore certificate are obtained as char* from Read() function. 
	  This function converts the char* into a string.
	  @param	aCapabilityString	certificate capabilities .
	  @param	aCapsLength			length of the capability string.
	  @return capabilities
	  @deprecated Interface is deprecated and is available for backward compatibility reasons 
				  only. Interface may in future be withdrawn. Use ExtractDetailCapabilities instead.
	 */
	const string ExtractCapabilities(char* aCapabilityString, int aCapsLength);
	
	/**
	  The capabilities of the certstore certificate are obtained as char* from Read() function. 
	  This function converts the char* into a string.
	  @param	aCapabilityString	certificate capabilities .
	  @param	aCapsLength			length of the capability string.
	  @return capabilities(human readable form)
	 */
	const string ExtractDetailCapabilities(char* aCapabilityString, int aCapsLength);

	/**
	  Extracts the certificates from the datastream in the dat file. 
	  @param	aCertLabel	label of the certificate to be extracted.
	  @param	aStreamId	datastream id
	  @param	aMandatory	mandatory status of the corressponding certificate.
	 */
	bool ExtractCertificate(string& aCertLabel , const Options& aOptions ,int aStreamId, unsigned char aMandatory);
	
	/**
	  Displays the infostream (certificate label,it's capabilities,mandatory status
	  and the application it supports.
	  */
	void Display(const Options& aOptions);

public:

	static int iCount;
	
private:

	Stream& iStream;
	StoreFile& iStore;
	// This value contains both mandatory and system upgrade values
	unsigned char iValue;
	vector<unsigned int> iAppId;
	string iCertName ;
	string iCertTag ;
	string iCertCapabilities ;
	string iIssuer ;
	string iMandatoryCert ;
	string iFileWrite ;
};
#endif