/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of CNcdDeviceService
*
*/


#ifndef C_NCDTESTCONFIG_H
#define C_NCDTESTCONFIG_H

#include <e32base.h>
#include <f32file.h>

/**
 * Test configuration reader and provider
 *
 * Possible improvement: CNcdTestConfig (or other test class) should implement
 * MNcdDeviceService and it should replace CNcdDeviceService if necessary
 * so that functions that are not configured from a file are rerouted to 
 * CNcdDeviceService
 *
 * Configuration file format:
 * <key1>=<value1>
 * <key2>=<value2>
 *
 * Valid keys: currentmnc, currentmcc, homemnc, homemcc
 *
 * Example:
 * 
 * currentmnc=123
 * currentmcc=456
 * homemnc=678
 * homemcc=987
 *
 * @note The length of MNC/MCC value MUST NOT exceed 3 characters
 * @note There are no validity checks for the values
 * @note If all of the keys are not defined, the missing keys will be 
 * given empty descriptors as their values
 */
class CNcdTestConfig : public CBase
    {
public:

    enum TConfigValue
        {
        EConfigCurrentMcc = 0,
        EConfigCurrentMnc,
        EConfigHomeMcc,
        EConfigHomeMnc,
        EConfigImsi,
        
        // Do NOT use this as a parameter
        // Always keep this as the last enumeration
        EConfigInternal
        };

    static CNcdTestConfig* NewL( RFs& aFs, const TDesC& aConfigFile );
    
    virtual ~CNcdTestConfig();
    
public:

    /**
     * Returns ETrue if the specified value was defined in the
     * config file
     */
    TBool IsSet( TConfigValue aValue ) const;
    
    /**
     * Returns the string defined in the config file
     *
     * @param aValue
     * @panic ENcdPanicNoData if the value had not been set in the config file
     */
    const TDesC& Value( TConfigValue aValue ) const;
    
    
protected:

    CNcdTestConfig();
    
    void ConstructL( RFs& aFs, const TDesC& aConfigFile );        

    void ParseL( const TDesC& aData );
    
    TBool ParseEntityL( 
        const TDesC& aData, const TDesC& aEntityName, TConfigValue aTarget );
        
    HBufC* GetString( TConfigValue aValue ) const;

private:    
    
    RPointerArray<HBufC> iConfigStrings;
    
    };

#endif // C_NCDTESTCONFIG_H

