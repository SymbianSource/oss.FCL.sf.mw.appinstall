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
* uisupportstream.h
* Definition of the streaming functions used by the UI Support Server
* to internalise and exernalize arrays into and from a descriptor
*
*/


/**
 @file 
 @internalComponent
 @released
*/
 
#ifndef __UISUPPORTSTREAM_H__
#define __UISUPPORTSTREAM_H__

#include <e32std.h>
#include "msisuihandlers.h"

class RReadStream;
class RWriteStream;

namespace Swi
{
/**
 * Serialize an array of TDesC objects to a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamOutArrayL(const RPointerArray<TDesC>& aArray, RWriteStream& aStream);

/**
 * Serialize an array of TDesC objects from a stream.
 *
 * @param aArray	The array to stream into.
 * @param aStream	The stream to read the contents of the array from.
 */
IMPORT_C void StreamInArrayL(RPointerArray<TDesC>& aArray, RReadStream& aStream);

/**
 * Serialize an array of CCertificateInfo objects to a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamOutArrayL(const RPointerArray<CCertificateInfo>& aArray, RWriteStream& aStream);

/**
 * Serialize an array of CPKIXValidationResultBase objects from a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamInArrayL(
	RPointerArray<CPKIXValidationResultBase>& aArray, 
	RReadStream& aStream);

/**
 * Serialize an array of CPKIXValidationResultBase objects to a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamOutArrayL(
	const RPointerArray<CPKIXValidationResultBase>& aArray, 
	RWriteStream& aStream);

/**
 * Serialize an array of TOCSPOutcome objects from a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamInArrayL(RPointerArray<TOCSPOutcome>& aArray,
	RReadStream& aStream);

/**
 * Serialize an array of TOCSPOutcome objects to a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamOutArrayL(const RPointerArray<TOCSPOutcome>& aArray, 
	RWriteStream& aStream);

/**
 * Serialize an array of CCertificateInfo objects from a stream.
 *
 * @param aArray	The array to stream out.
 * @param aStream	The stream to write the contents of the array to.
 */
IMPORT_C void StreamInArrayL(RPointerArray<CCertificateInfo>& aArray, RReadStream& aStream);

/*
 * The following functions and classes allow the streaming of arrays of objects. 
 * These functions work by streaming out the memory of the class directly, so they
 * will only work with T-type classes. In order to restrict the matching of the template
 * arguments, helpers have been added. In order to allow another type, add a 
 * TClassAllowable<> specialization of that type with the Allow enum equal to ETrue.
 */

template <class T>
void StreamOutArrayL(const RArray<T>& aArray, RWriteStream& aStream);

template <class T>
void StreamInArrayL(RArray<T>& aArray, RReadStream& aStream);

namespace StreamHelpers
	{
	// Default to not allowing the streaming
	template <class T>
	struct TClassAllowable
		{
		enum  {Allow = EFalse};
		};

	// Add allowed types here; any PODs (TTypes) are safe
	template <> struct TClassAllowable<TInt> { enum {Allow = ETrue}; };
	template <> struct TClassAllowable<TLanguage> { enum {Allow = ETrue}; };
	template <> struct TClassAllowable<TInt64> { enum {Allow = ETrue}; };
	template <> struct TClassAllowable<TChar> { enum {Allow = ETrue}; };
	template <> struct TClassAllowable<TDesC16> { enum {Allow = ETrue}; };

	// The default StreamArrayHelper does not allow streaming
	template <class T, TBool>
	struct StreamArrayHelper
		{
		};

	// partially specialized version to do streaming
	template <class T>
	struct StreamArrayHelper<T, ETrue>
		{
		static void StreamOutArrayL(const RArray<T>& aArray, RWriteStream& aStream);
		static void StreamInArrayL(RArray<T>& aArray, RReadStream& aStream);
		};
	} // namespace StreamHelpers

#include "uissstream.inl"

} // namespace Swi

#endif // #ifndef __UISUPPORTSTREAM_H__
