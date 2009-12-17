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
* Description:   Declaration of CCatalogsBigDes
*
*/


#ifndef C_CATALOGS_BIG_DES_H
#define C_CATALOGS_BIG_DES_H

#include <e32base.h>
#include <badesca.h>

/**
 * A buffer that grows automatically and can be retrieved in
 * descriptor format.
 */
class CCatalogsBigDes : public CBase
    {
public:

    /**
     * Constructor.
     * @return CCatalogsBigDes* Big Des.
     */
    static CCatalogsBigDes* NewL();
    
    /**
     * Constructor.
     * @return CCatalogsBigDes* Big Des.
     */
    static CCatalogsBigDes* NewLC();
    
    /**
     * Destructor.
     */
    ~CCatalogsBigDes();
    
    /**
     * Appends data to the buffer.
     * @param aDes Descriptor.
     */
    void AppendL( const TDesC& aDes );
    
    /**
     * Retrieves the data as a heap desc.
     * @return HBufC* Buffer, ownership transferred.
     */
    HBufC* DesL() const;
    
    /**
     * Retrieves the data as a heap desc.
     * @return HBufC* Buffer, ownership transferred.
     */
    HBufC* DesLC() const;
    
private:
    /**
     * Private constructor.
     */
    void ConstructL();
    
    /**
     * Private constructor.
     */
    CCatalogsBigDes();
    
private:

    /**
     * Actual data is stored in an array.
     */
    CDesCArray* iArray;
    
    };

#endif // C_CATALOGS_BIG_DES_H
