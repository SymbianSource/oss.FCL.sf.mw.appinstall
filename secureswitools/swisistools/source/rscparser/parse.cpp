// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// 
// 
//
/** 
* @file parse.cpp
*
* @internalComponent
* @released
*/
#include "parse.h"
#include "barsc2.h"

std::string FullNameWithoutDrive(std::string& Buf )
	{
	  std::string iName;
	   std::string::size_type index = Buf.find( ':' );
	   if( index != std::string::npos )
			iName.assign(Buf, index+1, Buf.size());
	
	   return iName;
	}

/**
 * Converts from Ptr8* datatype to wstring
 */
std::wstring Ptr8ToWstring(const Ptr8* aBuf)
{
	std::wstring str2(aBuf->GetLength(), L'\0'); // Make room for characters

	TUint8* temp = aBuf->GetPtr();
	memcpy(str2.begin(),temp,aBuf->GetLength());
	
	return str2;
}

/**
 * Converts from Ptr16* datatype to wstring
 */
std::wstring Ptr16ToWstring(const Ptr16* aBuf)
{
	std::wstring str2(aBuf->GetLength(), L'\0'); // Make room for characters

	TUint16* temp = aBuf->GetPtr();
	std::copy(temp,temp+aBuf->GetLength(),str2.begin());

	return str2;
}

/**
 * Converts from Ptr16* datatype to string
 */
std::string Ptr16ToString(Ptr16* aBuf)
{
		std::string str2(aBuf->GetLength(), '\0'); // Make room for characters

		TUint16* temp = aBuf->GetPtr();
		for(int i =0; i<aBuf->GetLength(); i++)
		{
			str2[i] = *temp++;
		}
		str2[aBuf->GetLength()] = '\0';
		return str2;
}

//Local function to Copy TUint16
void BufCpy(TUint16* aTemp1,TUint16* aTemp2, TInt aLength)
{
	for(TInt i =0 ; i< aLength; i++)
	{
		*aTemp1++ = *aTemp2++;
	}
}

//Local function to Copy TUint8
void BufCpy8(TUint8* aTemp1, const TUint8* aTemp2, TInt aLength)
{
	for(TInt i =0 ; i< aLength; i++)
	{
		*aTemp1++ = *aTemp2++;
	}
}

//Search for either wild character in the input string.
//return 1 if found else return 0
TInt FindWild(PtrC16 *aPtr)
{
	TUint32 i = 0;
	const TUint16* Pt = aPtr->iPtr;
	for( i=0; i < aPtr->iMaxLength; i++)
	{
		if((*Pt=='*') || (*Pt=='?'))
			return 1;
		else
			Pt++;
	}
	return 0;
}

//Find if wild character present in the input string
//return 1 if found else return 0
TInt FindWild(std::string& aRegistrationFileDrive, std::string& aAppFilePath, std::string& aAppFileNameAndExt)
{

	string::size_type index1 = aRegistrationFileDrive.find_last_of("*?");
	if( index1 != string::npos )
		return 1;

	string::size_type index2 = aAppFilePath.find_last_of("*?");
	if( index2 != string::npos )
		return 1;

	string::size_type index3 = aAppFileNameAndExt.find_last_of("*?");
	if( index3 != string::npos )
		return 1;
	return 0;
}

//Default Constructor for class ParsePtrC
ParsePtrC::ParsePtrC() 
		: iNamePresent(0),iPathPresent(0),iExtPresent(0),iDrivePresent(0)
{
}

//Destructor for class ParsePtrC
ParsePtrC::~ParsePtrC() 
{
	delete iNameBuf;
	delete Buf;
	iNameBuf = NULL;
	Buf = NULL;
}

//To Avoid double Delete in destructor.
void ParsePtrC::SetToNull()
{
	iNameBuf = NULL;
}

//Constructor for class ParsePtrC
ParsePtrC::ParsePtrC(const PtrC16* aName)
	: iNamePresent(0),iPathPresent(0),iExtPresent(0),iDrivePresent(0)
	{
	iNameBuf = new PtrC16;
	TUint16* temp = (TUint16*)aName->iPtr;
	if(NULL==iNameBuf)
	{
		std::string errMsg= "Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	iNameBuf->iPtr = aName->iPtr;
	iNameBuf->iMaxLength = aName->iMaxLength;

	Buf = new string(aName->iMaxLength,'\0');

	if(NULL==Buf)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}

	for(TInt i = 0; i<aName->iMaxLength; i++ )
	{
		(*Buf)[i] = *temp;
		temp++;
	}
}

PtrC16* ParsePtrC::FullName()
{
	return iNameBuf;
}

std::string ParsePtrC::StrName()
{
	return *Buf;
}

//Constructor for class ParsePtrC
ParsePtrC::ParsePtrC(std::string aStr)
	: iNamePresent(0),iPathPresent(0),iExtPresent(0),iDrivePresent(0)
{
	Buf = new string;
	if(NULL==Buf)
	{
		std::string errMsg= "Failed : Error in Reading File. Memory Allocation Failed";
		throw CResourceFileException(errMsg);
	}
	*Buf = aStr;
}

std::string ParsePtrC::Name()
{
  string iName;

  #ifdef __LINUX__
  TInt i = Buf->rfind("/");
  #else
  TInt i = Buf->rfind("\\");
  #endif

  if(i!=string::npos)
     iName.assign(*Buf, i, Buf->size()-4);
  else
  {
	  if(ExtPresent())
		iName.assign(*Buf, i, Buf->size()-4);
	  else
		iName.assign(*Buf, 0, Buf->size());
  }
	  
   return iName;
}

std::string ParsePtrC::NameAndExt()
{
  string iName;

  #ifdef __LINUX__
  TInt i = Buf->rfind("/");
  #else
  TInt i = Buf->rfind("\\");
  #endif
  
  if(i!=string::npos)
     iName.assign(*Buf, i, Buf->size());

   return iName;
}

std::string ParsePtrC::FullPath()
{
  string iName;

  #ifdef __LINUX__
  TInt i = Buf->rfind("/");
  #else
  TInt i = Buf->rfind("\\");
  #endif
  
  if(i!=string::npos)
     iName.assign(*Buf, 0, i);

   return iName;
}

std::string ParsePtrC::Path()
{
  string iName;

  #ifdef __LINUX__
  TInt j = Buf->rfind("/");
  TInt i = Buf->find("/");
  #else
  TInt j = Buf->rfind("\\");
  TInt i = Buf->find("\\");
  #endif

  if(i!=string::npos)
     iName.assign(*Buf, i, Buf->size()-j);

   return iName;
}

std::string ParsePtrC::Drive()
{
  string iName;

   string::size_type index = Buf->find( ':' );
   if( index != string::npos )
		iName.assign(*Buf, index-1, 2);

   return iName;
}

std::string ParsePtrC::FullNameWithoutDrive()
{
  string iName;

   string::size_type index = Buf->find( ':' );
   if( index != string::npos )
		iName.assign(*Buf, index+1, Buf->size());

   return iName;
}

TInt ParsePtrC::NamePresent()
{
	#ifdef __LINUX__
	TInt i = Buf->rfind("/");
    #else
	TInt i = Buf->rfind("\\");
    #endif

	if(i!=string::npos)
		iNamePresent = 1;
	else
	{
		if(Buf!= NULL)
			iNamePresent = 1;
		else
			iNamePresent = 0;
	}
	return iNamePresent;
}

TInt ParsePtrC::PathPresent()
{
	#ifdef __LINUX__
	TInt i = Buf->find("/");
	#else
	TInt i = Buf->find("\\");
	#endif

	if(i!=string::npos)
		iPathPresent = 1;
	else
		iPathPresent = 0;
	return iPathPresent;
}

TInt ParsePtrC::ExtPresent()
{
	TInt i = Buf->find(".");
	if(i!=string::npos)
		iExtPresent = 1;
	else
		iExtPresent = 0;
	return iExtPresent;
}

TInt ParsePtrC::DrivePresent()
{
	TInt i = Buf->find(":");
	if(i!=string::npos)
		iDrivePresent = 1;
	else
		iDrivePresent = 0;
	return iDrivePresent;
}

TInt ParsePtrC::IsWild()
{
	string::size_type index = Buf->find_last_of("*?");
	if( index != string::npos )
		return 1;
	return 0;
}

TInt ParsePtrC::IsValidName()
{
	#ifdef __LINUX__
	    string::size_type index = Buf->find_last_of("*?< > : \" \\ |");
	#else
	    string::size_type index = Buf->find_last_of("*?< > : \" |");
	#endif

	if( index != string::npos )
		return 1;
	return 0;
}
