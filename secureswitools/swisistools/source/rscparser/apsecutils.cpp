// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Security issues related methods used across apparc
// 
// apsecutils.cpp
//

#include "apsecutils.h"
#include <stdlib.h> 
#include <iostream.h> 
#include <fstream>
#include <string> 
#include "parse.h"

using namespace std;

TUint32 CApaSecurityUtils::iCapabilities = 0;
TUint32 CApaSecurityUtils::iSecureID = 0;

/** 
 * Determines whether an EXE SID is within the protected range\d
 * 
 * @param aSid The SID to check
 * @return ETrue if it is protected
 */
inline TBool CApaSecurityUtils::HasWriteDeviceDataCap( const TUint32 aCapability )
{
     // If bit 6 is set then WriteDeviceData capability is enabled
     return (aCapability & 0x20) ? 1 : 0;
} 

/** 
 * Determines whether an EXE SID is within the protected range\d
 * 
 * @param aSid The SID to check
 * @return ETrue if it is protected
 */
inline TBool CApaSecurityUtils::IsSidProtected( const TUint32 aSid )
{
     // If bit 31 is set then Sid is unprotected
     return (aSid & 0x80000000) ? 0 : 1;
} 

TInt CApaSecurityUtils::GetInfo(std::string& aFilename)
{
	const TUint8 kHeaderSize= 4;
	TUint8 header[kHeaderSize];

	std::ifstream* iFileContents;	
	iFileContents= new std::ifstream(aFilename.c_str(), std::ios::in|std::ios::binary);

	if(!iFileContents->good())
	{
		if (iFileContents->is_open())
		{
			iFileContents->close();
		}
		if(iFileContents)
			delete iFileContents;
		return 1;
	}
	
	TInt aPos=0x80;
	// Seek to the offset specified by "aPos"
	iFileContents->seekg(aPos, std::ios_base::beg);
	iFileContents->read((char*)header, kHeaderSize); 
	memcpy((TUint8*)&iSecureID,header,4);

	aPos=0x88;
	// Seek to the offset specified by "aPos"
	iFileContents->seekg(aPos, std::ios_base::beg);
	iFileContents->read((char*)header, kHeaderSize); 
	memcpy((TUint8*)&iCapabilities,header,4);

	if (iFileContents->is_open())
	{
		iFileContents->close();
	}

	if(iFileContents)
		delete iFileContents;
	
	return 0;
}

/**
 * Check if application has a WriteDeviceData capability
 * and if it's SID is in the protected range
 * 
 * @param aAppFilename path to application exe file
 * @param aHasWriteDeviceDataCap returns ETrue if app has WriteDeviceData cap
 * @param aIsSidProtected returns ETrue if application SID is in the protected range
 * @return KErrNone if succesful, error code otherwise
 */
TInt CApaSecurityUtils::CheckAppSecurity( const Ptr16& aAppFilename, 
                                           TBool& aHasWriteDeviceDataCap, 
                                           TBool& aIsSidProtected,
                                           const std::string& aDerivedPath)

{ 
    aHasWriteDeviceDataCap = EFalse;
    aIsSidProtected = EFalse;

	std::string Filename = Ptr16ToString(&aAppFilename);
	std::string Path(aDerivedPath);
	#ifdef __LINUX__
	Path.append("/sys/bin/");
	#else
	Path.append("\\sys\\bin\\");
	#endif

	Path.append(Filename);
	Path.append(".exe");
	
    TInt ret = CApaSecurityUtils::GetInfo(Path);

    if ( KErrNone == ret )
    {
        if( HasWriteDeviceDataCap( TUint32( iCapabilities) ) )
        {
            aHasWriteDeviceDataCap = ETrue;
        }

        if( IsSidProtected( TUint32( iSecureID) ) )
        {
            aIsSidProtected = ETrue;
        }
    }

    return ret;
}

//End of file
