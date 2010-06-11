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
* Description:   MNcdProtocolElementEntity declaration
*
*/


#ifndef NCDQUERYOPTIONIMPL_H
#define NCDQUERYOPTIONIMPL_H

#include <e32base.h>

class RWriteStream;
class RReadStream;
class CNcdString;

class MNcdConfigurationProtocolQueryOption;

/**
 * Defines a selectable option in a query item.
 */
class CNcdQueryOption : public CBase
    {
public:
    
    static CNcdQueryOption* NewL( RReadStream& aReadStream );
    
    static CNcdQueryOption* NewLC( RReadStream& aReadStream );
    
    static CNcdQueryOption* NewL(
        const MNcdConfigurationProtocolQueryOption& aOption );
    
    static CNcdQueryOption* NewLC(
        const MNcdConfigurationProtocolQueryOption& aOption );

    static CNcdQueryOption* NewL( const CNcdString& aPaymentMethodName,
                                  const TDesC8& aPaymentMethodType );
    static CNcdQueryOption* NewLC( const CNcdString& aPaymentMethodName,
                                  const TDesC8& aPaymentMethodType );
        
    virtual ~CNcdQueryOption();
    
    void InternalizeL( RReadStream& aReadStream );

    void InternalizeL( const MNcdConfigurationProtocolQueryOption& aOption );
    
    void ExternalizeL( RWriteStream& aWriteStream );
    
//    const TDesC& Key() const;
    
    const TDesC& Value() const;
    
    const CNcdString& Name() const;

    const TDesC8& Data() const;
    void SetDataL( const TDesC8& aData );

private:

    CNcdQueryOption(){}

    virtual void ConstructL( const CNcdString& aPaymentMethodName,
                             const TDesC8& aPaymentMethodType );

private:

    HBufC* iValue;
    CNcdString* iName;
    HBufC8* iData;
    };


#endif // NCDQUERYOPTIONIMPL_H
