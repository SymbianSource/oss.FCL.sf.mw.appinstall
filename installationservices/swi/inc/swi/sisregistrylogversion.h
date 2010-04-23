/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The access to a specific functionality depends on the client capabilities 
* and may be restricted.             
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __SISREGISTRYLOGVERSION_H__
#define __SISREGISTRYLOGVERSION_H__

#include "sisregistrylog.h"
#include <e32base.h>
#include <f32file.h>
#include <s32strm.h>

class RReadStream;
class RWriteStream;

using namespace Swi;

/**
 * @internalComponent
 * @released
 */
namespace Swi
{
class CLogFileVersion : public CBase
    {
public:
    CLogFileVersion()
   		{
   		iLogFileMajorVersion = KLogFileMajorVersion;
		iLogFileMinorVersion = KLogFileMinorVersion;
	    }
	
    static CLogFileVersion* NewL(RReadStream& aStream);
	static CLogFileVersion* NewLC(RReadStream& aStream);
	
	/**
	 * Write the object to a stream 
	 *
	 * @param aStream The stream to write to
	 */
	 void ExternalizeL(RWriteStream& aStream) const;
	
	/**
	 * Read the object from a stream
	 *
	 * @param aStream The stream to read from.
	 */
	 void InternalizeL(RReadStream& aStream) ;
	 
     void ConstructL();
	 void ConstructL(RReadStream& aStream);
	 
	/**
	 * Copy Constructor 
	 */
	CLogFileVersion(const CLogFileVersion& aObject1);
	
private:
	TUint8 iLogFileMajorVersion;
	TUint8 iLogFileMinorVersion;  
  	 
    };
} //namespace

#endif


