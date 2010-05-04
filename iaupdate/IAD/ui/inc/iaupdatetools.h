/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateTools functions
*
*/



#ifndef IA_UPDATE_TOOLS_H
#define IA_UPDATE_TOOLS_H

#include <e32base.h>
#include <e32cmn.h>

class RReadStream;
class RWriteStream;


/**
 * IAUpdateTools provides general tool functions
 *
 * @since S60 v3.2
 */
namespace IAUpdateTools
    {

    /**
     * @param aTarget The old pointer data will be deleted and the pointer will point to the new data 
     * that will contain externalization of the CIAUpdateParameters object.
     * @param aSource CIAUpdateParameters object that will be externalized to a descriptor.
     * @exception Leaves with system wide error code.
     */
    void ExternalizeParametersL( HBufC8*& aTarget,
                                 const CIAUpdateParameters& aSource );

    /**
     * Internalizes CIAUpdateParameters from the source descriptor data.
     *
     * @param aTarget CIAUpdateParameters object that will be internalized.
     * @param aSource Data that is used to internalize CIAUpdateParameters object.
     * @exception Leaves with system wide error code.
     */
    void InternalizeParametersL( CIAUpdateParameters& aTarget, 
                                 const TDesC8& aSource );


    /**
     * Externalizes the descriptor to the stream with length information
     *
     * @param aDes Descriptor to externalize
     * @param aStream Target stream
     * @exception Leaves with system wide error code.
     */
    void ExternalizeDesL( const TDesC& aDes, 
                          RWriteStream& aStream );

    /**
     * Internalizes a descriptor written with ExternalizeDesL() from the stream
     *
     * @param Target descriptor pointer. Old descriptor is deleted if the read is 
     * successful.
     * @param aStream Source stream
     * @return Length of the read data
     * @exception Leaves with system wide error code. 
     */
    TInt InternalizeDesL( HBufC*& aDes, 
                          RReadStream& aStream );


    /**
     * @see ExternalizeDesL
     */
    void ExternalizeDes8L( const TDesC8& aDes, 
                           RWriteStream& aStream );

    /**
     * @see InternalizeDesL
     */
    TInt InternalizeDes8L( HBufC8*& aDes, 
                           RReadStream& aStream );

    }

#endif // IA_UPDATE_TOOLS_H
