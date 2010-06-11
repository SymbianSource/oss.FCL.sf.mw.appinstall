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
* Implementation of RetrieveExecutableSecurityInfoL that provides utility functions for Install and restore machine.
*
*/


/**
 @file
 @released
 @internalTechnology 
*/
#include <hal.h> 
#include "securitycheckutil.h"
#include "log.h"
#include "cleanuputils.h"
#ifndef __WINS__
#include <f32image.h>
#include <e32rom.h>
#else
#include <windows.h>
#endif

#include "securityinfo.h"

using namespace Swi;

namespace SecurityInfo 
{


#ifdef __WINS__
void CleanupWinMemory(TAny* aBuffer)
	{
	LocalFree(LocalHandle ((LPSTR)aBuffer));
	}
#endif

/**
The function is to retrieve the file header from given file in symbian device.
	 
@param aFs a RFs reference
@param aCurrentFileName the file name of the executable
@param aSecurityInfo the security info in the executable header.
@return None
*/
void RetrieveExecutableSecurityInfoL(RFs& aFs, const TDesC& aCurrentFileName, TSecurityInfo& aSecurityInfo)
	{
	DEBUG_PRINTF2(_L("Install Server - Retrieving Executable Security Info for '%S'"),
		&aCurrentFileName);
	
	RFile fileToRead;	
	User::LeaveIfError(fileToRead.Open(aFs, aCurrentFileName, EFileRead | EFileShareReadersOnly | EFileStream));
	CleanupClosePushL(fileToRead);
	
	//Get the file size
	TInt size;
	User::LeaveIfError(fileToRead.Size(size));
	
	//Determine the buffer size for header, and allocate the header buffer
	if (size > RLibrary::KRequiredImageHeaderSize)
		{
		size = RLibrary::KRequiredImageHeaderSize;
		}
#ifndef __WINS__
	HBufC8* memForHeaderStructs = HBufC8::NewLC(size);

   	// Read the executable's header into the previously created memory block.
   	TPtr8 fileHeader(memForHeaderStructs->Des());
#else
	TUint8 *headerBuffer=NULL;
	headerBuffer = (unsigned char*)LocalAlloc(LPTR, size);

	if (headerBuffer == NULL)
		{
		User::Leave(KErrNoMemory);
		}
	  
	CleanupStack::PushL(TCleanupItem(CleanupWinMemory, headerBuffer));
	TPtr8 fileHeader(headerBuffer, size);
#endif
	User::LeaveIfError(fileToRead.Read(fileHeader, size));
	
	// Examine the file attributes to determine if it is an XIP exe image	
	TUint atts;
	User::LeaveIfError(fileToRead.Att(atts));

	RLibrary::TInfoV2 info;
	TPckg<RLibrary::TInfoV2> infoBuf(info);

	if (atts & KEntryAttXIP)
		{
		//it is an XIP exe image
		//No need to check VFPv2 exe here
		User::LeaveIfError(RLibrary::GetInfo(aCurrentFileName, infoBuf));			
		}
	else
		{
		//Retrieve the header from the exes that will be installed to ram and emulator
		TInt ret=RLibrary::GetInfoFromHeader(fileHeader, infoBuf);
		
		if (ret==KErrCorrupt || ret==KErrUnderflow)
			{
			//Invalid header, then we return our defined error code.
			User::Leave(KErrWrongHeaderFormat);	
			}
		else if (ret!=KErrNone)
				{
				User::Leave(ret);		
				}
		
		// This means that the head version less than KImageHdrFormat_V, then it does not contains capability
		if (info.iModuleVersion==0)
			{
			// Reject the install if the header format is not "KImageHdrFmt_V", as we shouldn't be
			// installing old executables and this means that the header has no capabilities in it.	
			DEBUG_PRINTF(_L8("Install Server - Error, executable header format is too old"));
			User::Leave(KErrWrongHeaderFormat);				
			}
		
		// Retrieve the HAL attribute
		TInt VFPHardware=0;
		HAL::Get(HALData::EHardwareFloatingPoint, VFPHardware);
		
		// Check if this is executable is built to use VFPv2 instruction and 
		// the device does not contains VFP hardware
		if (VFPHardware!=EFpTypeVFPv2 && info.iHardwareFloatingPoint==EFpTypeVFPv2)
			{
			DEBUG_PRINTF(_L8("Install Server - Error, attempting to install VFP executable on non-VFP platform"));
			User::Leave(KErrWrongHeaderFormat);			
			}			
		}

	//For exe in ram, ram and emulator
	aSecurityInfo=info.iSecurityInfo;		
#ifndef __WINS__		
	CleanupStack::PopAndDestroy(memForHeaderStructs); //memForHeaderStructs
#else
	CleanupStack::PopAndDestroy(); //TCleanupItem
#endif
	CleanupStack::PopAndDestroy(); //fileToRead		
	}

} //namespace SecurityCheckUtil
