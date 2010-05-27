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
* @file
* Simple Device Id list container.
* @internalComponent
* @released
*
*/


#ifndef __DEVICEIDLIST_H__
#define __DEVICEIDLIST_H__

#include <e32base.h>

class RReadStream;
class RWriteStream;

namespace Swi
{

/**
Simple wrapper class to contain and own an array of heap descriptors
representing device identifiers.  Acts as a convenient place to
implement InternalizeL() and ExternalizeL() methods for such arrays.
*/
class CDeviceIdList : public CBase
	{
public:
	/**
	Construct a new empty list.
	@return the empty list
	*/
	IMPORT_C static CDeviceIdList* NewL();
	/**
	Construct a new empty list, and place it on the cleanup stack.
	@return the empty list
	*/
	IMPORT_C static CDeviceIdList* NewLC();
	/**
	Construct a new list unpacked from the supplied descriptor.
	@param aPackedList The data from which the list should be constructed.
	@return the new list
	*/
	IMPORT_C static CDeviceIdList* NewL(const TDesC8& aPackedList);
	/**
	Construct a new list unpacked from the supplied descriptor, and
	place it on the cleanup stack.
	@param aPackedList The data from which the list should be constructed.
	@return the new list
	*/
	IMPORT_C static CDeviceIdList* NewLC(const TDesC8& aPackedList);
	/**
	Cleanup resources used by the instance.
	*/
	IMPORT_C ~CDeviceIdList();

	/**
	Write out the contents of this instance to the given stream.
	@param aStream the stream to which the object should eb written.
	*/
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/**
	Create the list of device ids from the given stream.  Any device
	ids already in the list are destroyed first.
	*/
	IMPORT_C void InternalizeL(RReadStream& aStream);

	/**
	Pack the array of device ids into a buffer.  The memory returned
	by this method must be managed by the client of the class.
	@return descriptor into which the array of device ids has been packed.
	*/
	IMPORT_C HBufC8* PackL() const;

	/**
	Append a descriptor to the list.
	@param aDeviceId Descriptor to append.
	*/
	IMPORT_C void AppendL(const TDesC& aDeviceId);
	
	/**
	Get the array of ids.  The memory returned by this method is
	managed by this class (and has the same lifetime) and must not be
	destroyed by clients.
	@return Array of device ids.
	*/
	IMPORT_C const RPointerArray<HBufC>& DeviceIds() const;

private:
	/**
	Private constructor generation to prevent implicit constructor
	generation and export.
	*/
	CDeviceIdList();

	/**
	Array of device ids managed by this class.
	*/
	RPointerArray<HBufC> iDeviceIds;
	};

}

#endif // __DEVICEIDLIST_H__
