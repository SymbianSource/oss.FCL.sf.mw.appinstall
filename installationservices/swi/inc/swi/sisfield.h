/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of the Swi::Sis::CField
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef __SISFIELD_H__
#define __SISFIELD_H__

#include <e32base.h>
#include <e32def.h>
#include <f32file.h>
#include "sisinstallerrors.h"
#include "sisfieldtypes.h"
#include "sisdataprovider.h"
#include "sisptrprovider.h"

namespace Swi
{
namespace Sis
 {

/**
 * Controls whether the field type is read from the stream and checked
 * or whether we know the field type already.
 *
 * @released
 * @publishedPartner
 */
enum TReadTypeBehaviour 
	{
	EReadType,
	EAssumeType
	};

/**
 * This class represents a Field. Field is a basic structure found in  files.
 * It is described in SGL.GT0188.251.
 *
 * @released
 * @internalTechnology
 */
class CField : public CBase
	{
public:
	enum TArrayStatus { EInArray, ENotInArray };

	virtual ~CField();

protected:

	CField();
	
	/**
	 * The second phase constructor.
	 *
	 * @param aDataProvider 	 An instance of a MSisDataProvider to read the entity from.
	 * @param aFieldType 		 The field type we are attempting to read.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */
	void ConstructL(MSisDataProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	/**
	 * The second phase in place constructor.
	 *
	 * @param aDataProvider 	 An instance of a TPtrProvider to read the entity from.
	 * @param aFieldType 		 The field type we are attempting to read.
	 * @param aTypeReadBehaviour Whether we need to check the sis type or not.
	 */

	void ConstructL(TPtrProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

public:
	/**
	 * This function returns the value of the Length part of the Field.
	 */
	IMPORT_C TFieldLength Length() const;

	/**
	  * Each SISField has and header comprising its type and its length.
	  * This last attribute has variable size.
	  * This method can be used to ascertain the size of the header.
	  *
	  * @return The size of the SISField header in bytes.
	  */	
	TInt HeaderSize(TArrayStatus aInArray = ENotInArray) const;
	
	/**
	  * SISFields can be padded, use this function to learn how many padding
	  * bytes are used in this SISField.
	  *
	  * @return The number of padding bytes for this SISField
	  */
	TInt PaddingSize() const;

	/**
	 * This function returns the value of the Type part of the Field.
	 */
	TFieldType FieldType() const;

protected:

	// Ensures alignment to 4 bytes after reading the field
	void EnsureAlignedL(MSisDataProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	static void CheckedReadL(MSisDataProvider& aDataProvider, TDes8& aBuffer, TInt64& aBytesRead);
	static void CheckedReadL(MSisDataProvider& aDataProvider, TDes8& aBuffer, TInt aSize, TInt64& aBytesRead);
	
	// Skip unknown fields, used to skip unknown fields at the end of a SISField
	void SkipUnknownFieldsL(MSisDataProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	// Read an array into a member variable
	template <typename T>
	void ReadMemberArrayL(MSisDataProvider& aDataProvider, RPointerArray<T>& aArray, TFieldType aExpectedInternalField, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	// Read a collection into a member variable
	template <typename T>
	void ReadMemberCollectionL(MSisDataProvider& aDataProvider, RPointerArray<T>& aArray, TFieldType aExpectedInternalField, TFieldType aTerminatingInternalFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);

	template<typename T>
	static inline void ReadTTypeL(MSisDataProvider& aDataProvider,T& aValue, TInt64& aBytesRead);

	template<typename EnumType, typename StoredType>
	static inline void ReadEnumL(MSisDataProvider& aDataProvider,EnumType& aValue, TInt64& aBytesRead);

	// fill the supplied buffer with the header for the given field type and length
	static void CreateHeader(TFieldType aFieldType, TFieldLength aFieldLength, TDes8& aHeader);

	// called by derived classes to calculate the CRC of their data
	static void CalculateCrcL(MSisDataProvider& aDataProvider, const TInt64 aLength, TUint16& aCrc);
	
	// -----------
	// in place readers
	// -----------
	
	void EnsureAlignedL(TPtrProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);
	void SkipUnknownFieldsL(TPtrProvider& aDataProvider, TInt64 aBytesConsumed, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

	// Read an array into a member variable
	template <typename T>
	void ReadMemberArrayL(TPtrProvider& aDataProvider, RPointerArray<T>& aArray, TFieldType aExpectedInternalField, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
	template<typename T>
	static inline void ReadTTypeL(TPtrProvider& aDataProvider,T& aValue, TInt64& aBytesRead);

	template<typename EnumType, typename StoredType>
	static inline void ReadEnumL(TPtrProvider& aDataProvider,EnumType& aValue, TInt64& aBytesRead);
	
	template <typename T>
	void ReadMemberCollectionL(TPtrProvider& aDataProvider, RPointerArray<T>& aArray, TFieldType aExpectedInternalField, TFieldType aTerminatingInternalFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour = EReadType);
	
private:
	/// protected copy constructor to prevent copying of derived classes
	CField(const CField& aField);

	/// protected assignment operator to prevent copying of derived classes
	CField& operator=(const CField& aField);

	void ReadLengthL(MSisDataProvider& aDataProvider, TInt64& aBytesRead);
	void ReadTypeL(MSisDataProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);
	
	void ReadLengthL(TPtrProvider& aDataProvider, TInt64& aBytesRead);
	void ReadTypeL(TPtrProvider& aDataProvider, TFieldType aFieldType, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour);

private:
	/// Length of the SISField Value (Not including Type, Length fields)
	TFieldLength iLength;
	
	/// Type of the SISField
	TFieldType iType;
	
	/**
	 * Size of the SISField header if it were not stored in an array
	 * so 12 if length is stored in 8 bytes and 8 if length is stored
	 * in 4 bytes
	 */
	TInt iHeaderSize;
	};


// template functions from CField

template<typename T>
void CField::ReadTTypeL(MSisDataProvider& aDataProvider,T& aValue, TInt64& aBytesRead)
	{
	TPckg<T> package(aValue);
	CField::CheckedReadL(aDataProvider, package, aBytesRead);
	}

template<typename EnumType, typename StoredType>
void CField::ReadEnumL(MSisDataProvider& aDataProvider,EnumType& aValue, TInt64& aBytesRead)
	{
	StoredType storedValue=0;
	CField::ReadTTypeL(aDataProvider, storedValue, aBytesRead);
	aValue=EnumType(storedValue);			
	}

template <typename T>
void CField::ReadMemberArrayL(MSisDataProvider& aDataProvider, 
			      RPointerArray<T>& aArray, 
			      TFieldType aExpectedInternalFieldType, 
			      TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	// Arrays are stored as 
	// T = EFieldTypeArray, L = Length , V=
	// {
	//   TUint32 internalFieldType
	//   
	//   Item 1   (stored without Type T)
	//   ....
	//   Item N   (stored without Type T)
	// }	
	
	TInt64 fieldOffset = aBytesRead;
	
	aArray.ResetAndDestroy();
	
	CField* field=new (ELeave) CField();
	CleanupStack::PushL(field);
	
	TInt64 bytesRead=aBytesRead;
	
	field->ReadTypeL(aDataProvider, EFieldTypeArray, aBytesRead, aTypeReadBehaviour);
	field->ReadLengthL(aDataProvider, aBytesRead);

	TFieldType internalFieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, internalFieldType, aBytesRead);

	if (internalFieldType != aExpectedInternalFieldType)
		{
		User::Leave(KErrSISArrayTypeMismatch);
		}

	TFieldLength lengthRead(sizeof(TUint32)); // Internal field type
	
	while (lengthRead < field->Length())
		{
		T* value=T::NewLC(aDataProvider, aBytesRead, EAssumeType);
		
		if (value->FieldType() != internalFieldType)
			{
			User::Leave(KErrSISArrayTypeMismatch);
			}
		
		User::LeaveIfError(aArray.Append(value));
		CleanupStack::Pop(value);

		lengthRead+=value->HeaderSize(EInArray); 
		lengthRead+=value->Length();
		lengthRead+=value->PaddingSize();
		}
	
	if (lengthRead!=field->Length())
		{
		User::Leave(KErrSISArrayReadError);
		}
	
	field->SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead , EReadType);
	field->EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);

	CleanupStack::PopAndDestroy(field);
	}

template <typename T>
	void CField::ReadMemberCollectionL(MSisDataProvider& aDataProvider, 
					RPointerArray<T>& aArray, 
					TFieldType aExpectedInternalFieldType, 
					TFieldType aTerminatingInternalFieldType,
					TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	// Collections are sequences of SISFields all of the same type
	// another different SISfield is used to terminate the collection

	TFieldType fieldType;
	
	// Setup field type, read if necessary, otherwise just set it
	if (aTypeReadBehaviour==EReadType)
		{
		CField::ReadEnumL<TFieldType, TInt32>(aDataProvider, fieldType, aBytesRead);
		}
	else if (aTypeReadBehaviour==EAssumeType)
		{
		fieldType=aExpectedInternalFieldType;
		}

	// while the next field is the type we're expecting, collection is continued	
	while (fieldType==aExpectedInternalFieldType)
		{
		// read the current field
		T* value=T::NewLC(aDataProvider, aBytesRead, EAssumeType);
		User::LeaveIfError(aArray.Append(value));
		CleanupStack::Pop(value);

		// read the next field's type
		CField::ReadEnumL<TFieldType, TInt32>(aDataProvider, fieldType, aBytesRead);
		}
	
	// Check the terminating field's type is as expected
	if (fieldType!=aTerminatingInternalFieldType)
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	}


template <typename T>
	void CField::ReadMemberArrayL(TPtrProvider& aDataProvider, RPointerArray<T>& aArray, TFieldType aExpectedInternalField, TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	TInt64 fieldOffset = aBytesRead;
	
	aArray.ResetAndDestroy();
	
	CField* field=new (ELeave) CField;
	CleanupStack::PushL(field);
	
	TInt64 bytesRead=aBytesRead;
	
	field->ReadTypeL(aDataProvider, EFieldTypeArray, aBytesRead, aTypeReadBehaviour);
	field->ReadLengthL(aDataProvider, aBytesRead);

	TFieldType internalFieldType;
	CField::ReadEnumL<TFieldType,TUint32>(aDataProvider, internalFieldType, aBytesRead);

	if (internalFieldType != aExpectedInternalField)
		{
		User::Leave(KErrSISArrayTypeMismatch);
		}

	TFieldLength lengthRead(sizeof(TUint32)); // Internal field type
	
	while (lengthRead < field->Length())
		{
		T* value=T::NewLC(aDataProvider, aBytesRead, EAssumeType);
		
		if (value->FieldType() != internalFieldType)
			{
			User::Leave(KErrSISArrayTypeMismatch);
			}
		
		User::LeaveIfError(aArray.Append(value));
		CleanupStack::Pop(value);

		lengthRead+=value->HeaderSize(EInArray); 
		lengthRead+=value->Length();
		lengthRead+=value->PaddingSize();
		}
	
	if (lengthRead!=field->Length())
		{
		User::Leave(KErrSISArrayReadError);
		}
	
	field->SkipUnknownFieldsL(aDataProvider, aBytesRead - fieldOffset, aBytesRead , EReadType);
	field->EnsureAlignedL(aDataProvider, aBytesRead - fieldOffset, aBytesRead, aTypeReadBehaviour);

	CleanupStack::PopAndDestroy(field);	
	}
	
template <typename T>
	void CField::ReadMemberCollectionL(TPtrProvider& aDataProvider, 
					RPointerArray<T>& aArray, 
					TFieldType aExpectedInternalFieldType, 
					TFieldType aTerminatingInternalFieldType,
					TInt64& aBytesRead, TReadTypeBehaviour aTypeReadBehaviour)
	{
	// Collections are sequences of SISFields all of the same type
	// another different SISfield is used to terminate the collection

	TFieldType fieldType;
	
	// Setup field type, read if necessary, otherwise just set it
	if (aTypeReadBehaviour==EReadType)
		{
		CField::ReadEnumL<TFieldType, TInt32>(aDataProvider, fieldType, aBytesRead);
		}
	else if (aTypeReadBehaviour==EAssumeType)
		{
		fieldType=aExpectedInternalFieldType;
		}

	// while the next field is the type we're expecting, collection is continued	
	while (fieldType==aExpectedInternalFieldType)
		{
		// read the current field
		T* value=T::NewLC(aDataProvider, aBytesRead, EAssumeType);
		User::LeaveIfError(aArray.Append(value));
		CleanupStack::Pop(value);

		// read the next field's type
		CField::ReadEnumL<TFieldType, TInt32>(aDataProvider, fieldType, aBytesRead);
		}
	
	// Check the terminating field's type is as expected
	if (fieldType!=aTerminatingInternalFieldType)
		{
		User::Leave(KErrSISUnexpectedFieldType);
		}
	}


template<typename T>
	void CField::ReadTTypeL(TPtrProvider& aDataProvider,T& aValue, TInt64& aBytesRead)
	{
	TPckg<T> package(aValue);
	package.Copy(aDataProvider.ReadL(package.Length()));
	aBytesRead += package.Length();
	}

template<typename EnumType, typename StoredType>
	void CField::ReadEnumL(TPtrProvider& aDataProvider,EnumType& aValue, TInt64& aBytesRead)
	{
	StoredType storedValue=0;
	CField::ReadTTypeL(aDataProvider, storedValue, aBytesRead);
	aValue=EnumType(storedValue);	
	}

inline TInt CField::HeaderSize(TArrayStatus aInArray) const
	{
	// Arrays do not store the type so take the size off for this case
	return iHeaderSize - (aInArray==EInArray ? sizeof(TInt32) : 0); 
	}

inline TInt CField::PaddingSize() const
	{
	return (4 - (I64LOW(Length()) & 0x3)) & 0x3;
	};

 } // namespace Sis
} // namespace Swi


#endif
