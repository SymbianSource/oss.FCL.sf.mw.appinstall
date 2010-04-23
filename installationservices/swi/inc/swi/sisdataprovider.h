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
* Definition of the MSisDataProvider
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __SISDATAPROVIDER_H__
#define __SISDATAPROVIDER_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <caf/caf.h>

namespace Swi
{

/**
 * Interface that provides package file data to Software Install.
 * @publishedPartner
 * @released
 */
class MSisDataProvider 
	{
public:
	/** 
	Reads as much data as fits into the supplied descriptor.
	Note, that if descriptor is larger than the data available, no error is returned.
	The descriptor's length is set according to the number of bytes read into it.
	@param aDes Descriptor that receives the data
	@return KErrNone in the case of success, or one of the system-wide errors in the case of failure
	*/
    virtual TInt Read(TDes8& aDes)=0;
    
    /**
    Reads the specified amount of data into a descriptor.
	Note, that if aLength is larger than the data available, no error is returned.
	The descriptor's length is set according to the number of bytes read into it.    
	@param aDes    Descriptor that receives the data
	@param aLength Amount of data to read in bytes
	@return KErrNone in the case of success, KErrOverflow if aLength is larger than the descriptor's size, or one of the other system-wide errors in case of failure
    */
    virtual TInt Read(TDes8& aDes, TInt aLength)=0;

    /**
    Seeks to the specified position in the data stream.
    @param aMode Seek mode:
		   ESeekCurrent Seek from the current position
		   ESeekStart   Seek from the beginning of the file
		   ESeekEnd     Seek from the end of the file
		   ESeekAddress Not supported
	@param aPos The offset relative to aMode to which to seek.  On return,
	this is set to the offset relative to the start of the file.
	@return Error code, KErrNone on success
	@see RFile::Seek
    */
    virtual TInt Seek(TSeek aMode, TInt64& aPos)=0;

	/**
	Empty virtual destructor allows us to delete implementations via an
	interface pointer.
	*/
	virtual ~MSisDataProvider() {}

	/**
	This function is called by Software Install in order to open any DRM content.
	A DRM enabled implementation of this interface should open the content, using
	the intent provided, and evaluate the rights. This function should leave with
	an error if the rights evaluation fails.	 An implementation of this interface
	that does not support DRM does not need to implement this function.

	@param aIntent	The intent which the DRM enabled data provider should use
						when evaluating the rights of the content.
	*/
	virtual void OpenDrmContentL(ContentAccess::TIntent aIntent);
	
	
	/**
	This function is called by Software Install, at the end of a successful
	installation, in order to update the rights of the installed SISX file. A DRM
	enabled implementation of this interface should execute the rights on the content
	using the intent provided. If DRM is not intended to be supported then this
	function does not need to be implemented. 

	@param aIntent	The intent which the DRM enabled data provider should use
						when executing the rights of the content.
	@return			KErrNone, if the rights execution is successful, otherwise one of the system wide error codes
	*/
	virtual TInt ExecuteDrmIntent(ContentAccess::TIntent aIntent);
	};

// inline functions from MSisDataProvider, which are implemented to do nothing
inline void MSisDataProvider::OpenDrmContentL(ContentAccess::TIntent /*aIntent*/)
	{
	// do nothing
	}

inline TInt MSisDataProvider::ExecuteDrmIntent(ContentAccess::TIntent /*aIntent*/)
	{
	return KErrNone; // do nothing , and return success
	}
 
} // namespace Swi

#endif // __SISDATAPROVIDER_H__
