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
* This file creates a dll with a large SmallArray
*
*/


/**
 @file
 SmallArray.cpp
*/


// User Include
#include "smallarray.h"
// System Include
#include <e32uid.h>



/**
Description:  constructor      
@internalTechnology
@param aConsole - console on which messages are to be written  															
@param aString  - Description of console like name.
@return CSmallArray* - pointer to newly created object of CSmallArray
@test
*/

EXPORT_C CSmallArray* CSmallArray::NewLC(const TDesC& aString)
	{
	CSmallArray* self=new (ELeave) CSmallArray();
	CleanupStack::PushL(self);
	self->ConstructL(aString);  
	return self;
	}

/**
Description:  Destructor - no export
@internalTechnology
@test
*/
CSmallArray::~CSmallArray() 
	{
	delete iString; 
	}

/**  
Description:  Access every aNumber th  element in the SmallArray starting from 0        
@internalTechnology
@param aNumber  - It is the increment value
@return void 
@test
*/

EXPORT_C TBool CSmallArray::ReadSmallArray(TUint32 aNumber)
	{
	TBool status = ETrue;
		for (TUint32 index=0; index<KSmallArrayMaxSize ;index=(index+aNumber) )  
		{
		if(index != SmallArray[index]) 
			{
			status  = EFalse ;
			break;
			}
		}

	return status;
					
	} 
//don't export these, because used only by functions in this DLL, eg our NewLC()

/**
Description: first-phase C++ constructor         
@internalTechnology
@param aConsole - console to print messages
@test
*/

CSmallArray::CSmallArray() 
	{
	}

//don't export these, because used only by functions in this DLL, eg our NewLC()
/**
Description: second-phase constructor   
@internalTechnology
@param aString  - Description of the SmallArray
@return void 
@test
*/
 
void CSmallArray::ConstructL(const TDesC& aString) 
	{
	// copy given string into own descriptor
	iString=aString.AllocL(); 
    }

/**
Description:   DLL entry point
@internalTechnology
@return TInt- KErrNone - Code for no error.
@test
*/

GLDEF_C TInt E32Dll()
	{
	return(KErrNone);
	}
//	SmallArray

