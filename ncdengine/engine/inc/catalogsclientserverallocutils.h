/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#ifndef CATALOGS_CLIENTSERVER_ALLOC_UTILS_H
#define CATALOGS_CLIENTSERVER_ALLOC_UTILS_H

#include <badesca.h>

/**
 * Function that allocates a descriptor of given expected length. If
 * the given length is less than minimum length acceptable when using
 * return message allocation then this minimum allocation length is
 * used.
 * 
 * @param aExpectedLength Expected length of return message.
 * @param aAllocatedBuf Reference to a pointer which is used to
 *                      return the allocated descriptor.
 * @return TInt KErrNone on success.
 */
TInt AllocReturnBuf( TInt aExpectedLength, HBufC8*& aAllocatedBuf );

/**
 * Function that allocates a descriptor of given expected length. If
 * the given length is less than minimum length acceptable when using
 * return message allocation then this minimum allocation length is
 * used.
 * 
 * @param aExpectedLength Expected length of return message.
 * @param aAllocatedBuf Reference to a pointer which is used to
 *                      return the allocated descriptor.
 * @return TInt KErrNone on success.
 */
TInt AllocReturnBuf( TInt aExpectedLength, HBufC16*& aAllocatedBuf );
                         


/**
 * Retrieves realloc-information from the given descriptor and
 * reallocates the descriptor according to it. Also makes the
 * descriptor empty. Realloc-information is expected to be sent from
 * the server-side to inform of a too small return buffer-size.
 * This function also returns the handle of the incomplete message
 * in a reference. In error situations remember to delete server-side
 * incomplete message if this function gives the handle to it.
 * If not given, -1 is set to aIncompleteMessageHandle.
 * 
 * @param aTempReturnBuf Buffer which contains realloc info sent
 *                       from server side and which is reallocated
 *                       according to it.
 * @param aIncompleteMessageHandle Handle of the incomplete message.
 * @return TInt KErrNone on success.
 */
TInt RetrieveNewDescLengthAndReAlloc( HBufC8*& aTempReturnBuf,
                                      TInt& aIncompleteMessageHandle );

/**
 * Retrieves realloc-information from the given descriptor and
 * reallocates the descriptor according to it. Also makes the
 * descriptor empty. Realloc-information is expected to be sent from
 * the server-side to inform of a too small return buffer-size.
 * This function also returns the handle of the incomplete message
 * in a reference. In error situations remember to delete server-side
 * incomplete message if this function gives the handle to it.
 * If not given, -1 is set to aIncompleteMessageHandle.
 * 
 * @param aTempReturnBuf Buffer which contains realloc info sent
 *                       from server side and which is reallocated
 *                       according to it.
 * @param aIncompleteMessageHandle Handle of the incomplete message.
 * @return TInt KErrNone on success.
 */
TInt RetrieveNewDescLengthAndReAlloc( HBufC16*& aTempReturnBuf,
                                      TInt& aIncompleteMessageHandle );




/**
 * Function that retrieves handle of a incomplete message and a
 * new descriptor maximum length from the given descriptor. The message
 * in descriptor is expected to be sent from the server-side to
 * inform of a too small return buffer-size.
 * 
 * @param aTempReturnBuf Buffer which contains realloc info sent
 *                       from server side and which is reallocated
 *                       according to it.
 * @param aIncompleteMessageHandle Handle of the incomplete message.
 * @return TInt KErrNone on success.
 */
TInt InterpretNewAllocInfo( const HBufC8& aTempReturnBuf,
                            TInt& aIncompleteMessageHandle,
                            TInt& aNewDescriptorMaxLength );
                            
/**
 * Function that retrieves handle of a incomplete message and a
 * new descriptor maximum length from the given descriptor. The message
 * in descriptor is expected to be sent from the server-side to
 * inform of a too small return buffer-size.
 * 
 * @param aTempReturnBuf Buffer which contains realloc info sent
 *                       from server side and which is reallocated
 *                       according to it.
 * @param aIncompleteMessageHandle Handle of the incomplete message.
 * @return TInt KErrNone on success.
 */
TInt InterpretNewAllocInfo( const HBufC16& aTempReturnBuf,
                            TInt& aIncompleteMessageHandle,
                            TInt& aNewDescriptorMaxLength );


#endif // CATALOGS_CLIENTSERVER_ALLOC_UTILS_H
