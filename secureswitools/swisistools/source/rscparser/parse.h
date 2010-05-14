// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// parse.h
//
/** 
* @file parse.h
*
* @internalComponent
* @released
*/
#ifndef __PARSE_H__
#define __PARSE_H__

#include <iostream>
#include <string>
#include "commontypes.h"
using namespace std;

#define KUidAppRegistrationResourceFile 0x101f8021

//Local function to Copy TUint16
void BufCpy(TUint16* aTemp1,TUint16* aTemp2, TInt aLength);
//Local function to Copy TUint8
void BufCpy8(TUint8* aTemp1, const TUint8* aTemp2, TInt aLength);
//Find if wild character present in the input string
TInt FindWild(PtrC16 *aPtr);
//Find if wild character present in the input string
TInt FindWild(std::string& aRegistrationFileDrive, std::string& aAppFilePath, std::string& aAppFileNameAndExt);

std::string FullNameWithoutDrive(std::string& Buf );

/**
 * Converts from Ptr8* datatype to wstring
 */
std::wstring Ptr8ToWstring(const Ptr8* aBuf);
/**
 * Converts from Ptr16* datatype to wstring
 */
std::wstring Ptr16ToWstring(const Ptr16* aBuf);
/**
 * Converts from Ptr16* datatype to string
 */
std::string Ptr16ToString(Ptr16* aBuf);


class ParsePtrC 
	{
public:
	ParsePtrC();
	~ParsePtrC();
	ParsePtrC(std::string aStr);
	ParsePtrC(const PtrC16* aName);
	
	PtrC16* FullName();
	std::string StrName();
	std::string Name();
	std::string NameAndExt();
	std::string FullPath();
	std::string Path();
	std::string Drive();
	std::string FullNameWithoutDrive();
	TInt NamePresent();
	TInt PathPresent();
	TInt ExtPresent();
	TInt DrivePresent();
	TInt IsWild();
	TInt IsValidName();
	void SetToNull();

private:
	PtrC16* iNameBuf;
	TInt iNamePresent;
	TInt iPathPresent;
	TInt iExtPresent;
	TInt iDrivePresent;
	std::string* Buf;
	};

#endif