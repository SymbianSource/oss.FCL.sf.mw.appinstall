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


#ifndef C_NCDCONFIGURATIONPROTOCOLCOOKIEIMPL_H
#define C_NCDCONFIGURATIONPROTOCOLCOOKIEIMPL_H

#include "ncd_cp_cookie.h"
#include "ncdstoragedataitem.h"

class CNcdConfigurationProtocolCookie : public CBase,
    public MNcdConfigurationProtocolCookie,
    public MNcdStorageDataItem
    {
public:
    
    static CNcdConfigurationProtocolCookie* NewL();
    static CNcdConfigurationProtocolCookie* NewLC();
    
    static CNcdConfigurationProtocolCookie* NewLC( 
        const MNcdConfigurationProtocolCookie& aCookie );

    virtual ~CNcdConfigurationProtocolCookie();

public: // From MNcdConfigurationProtocolCookie
    const TDesC& Key() const;
    const TDesC& Type() const;
    const TDesC& Scope() const;
    TInt ExpirationDelta() const;
    TInt ValueCount() const;
    const TDesC& Value( TInt aIndex ) const;    
    const TDesC8& Sim() const;           

    TTime ExpirationTime() const;
    
public:

    static TBool Identity( 
        const MNcdConfigurationProtocolCookie& aFirst,
        const MNcdConfigurationProtocolCookie& aSecond );
        
public: // From MNcdStorageDataItem

    void ExternalizeL( RWriteStream& aStream );


    void InternalizeL( RReadStream& aStream );


private:    
    CNcdConfigurationProtocolCookie();
    
    void ConstructL();
    void ConstructL( const MNcdConfigurationProtocolCookie & aCookie );

public:
    HBufC* iKey;
    HBufC* iType;
    HBufC* iScope;
    TInt iExpirationDelta;
    RPointerArray<HBufC> iValues;
    HBufC8* iSim;
    TTime iExpirationTime;
    
    };

#endif // C_NCDCONFIGURATIONPROTOCOLCOOKIEIMPL_H
