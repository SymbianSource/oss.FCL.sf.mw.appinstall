/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifdef CATALOGS_BUILD_CONFIG_OVERRIDE_DEVICE

// Overrides device information sent by the client
#ifndef CATALOGS_DEVICE_OVERRIDE_HRH
#define CATALOGS_DEVICE_OVERRIDE_HRH  
    
    #define CATALOGS_OVERRIDE_NETWORK
    
    #ifdef CATALOGS_OVERRIDE_NETWORK
        _LIT( KCatalogsOverrideHomeMcc, "244" );
        _LIT( KCatalogsOverrideHomeMnc, "21" );        
        _LIT( KCatalogsOverrideCurrentMcc, "244" );
        _LIT( KCatalogsOverrideCurrentMnc, "05" );        

        // The length of service provider must not exceed
        // RMobilePhone::KMaxSPNameSize (=16)
        _LIT( KCatalogsOverrideServiceProvider, "Saunalahti" );
    #endif // CATALOGS_OVERRIDE_NETWORK
        
    
    // This is not necessary if user agent is correctly overridden
    //#define CATALOGS_OVERRIDE_MODEL    
    #ifdef CATALOGS_OVERRIDE_MODEL
        _LIT( KCatalogsOverrideDeviceId, "Nokia6290" );
        _LIT( KCatalogsOverrideDeviceModel, "6290" );
        
    #endif // CATALOGS_OVERRIDE_MODEL
    
    
    // Comment this if you don't want to override the user agent string
    #define CATALOGS_OVERRIDE_USERAGENT
    
    // User agent string
    #ifdef CATALOGS_OVERRIDE_USERAGENT
        // N6290
        //_LIT8( KCatalogsOverrideUserAgent, "Mozilla/5.0 (SymbianOS/9.2; Series60/3.1 Nokia6290/3.04; Profile/MIDP-2.0 Configuration/CLDC-1.1 ) U; AppleWebKit/413 (KHTML, like Gecko) Safari/413");
                
        // N5700
        //_LIT8( KCatalogsOverrideUserAgent, "Mozilla/5.0 (SymbianOS/9.2; Series60/3.1 Nokia5700/3.04; Profile/MIDP-2.0 Configuration/CLDC-1.1 ) U; AppleWebKit/413 (KHTML, like Gecko) Safari/413");
        
        // N6110 Navigator
        //_LIT8( KCatalogsOverrideUserAgent, "Mozilla/5.0 (SymbianOS/9.2; Series60/3.1 Nokia6110Navigator/3.68; Profile/MIDP-2.0 Configuration/CLDC-1.1 ) U; AppleWebKit/413 (KHTML, like Gecko) Safari/413");
        
        // N76
        //_LIT8( KCatalogsOverrideUserAgent, "Mozilla/5.0 (SymbianOS/9.2; Series60/3.1 NokiaN76/11.0.026; Profile/MIDP-2.0 Configuration/CLDC-1.1 U; ) AppleWebKit/413 (KHTML, like Gecko) Safari/413");
        
        // N95
        _LIT8( KCatalogsOverrideUserAgent, "Mozilla/5.0 (SymbianOS/9.2; Series60/3.1 NokiaN95/12.0.013; Profile/MIDP-2.0 Configuration/CLDC-1.1 U; ) AppleWebKit/413 (KHTML, like Gecko) Safari/413");
        
    #endif // CATALOGS_OVERRIDE_USERAGENT
    
    
    #define CATALOGS_OVERRIDE_LANGUAGE   
    #ifdef CATALOGS_OVERRIDE_LANGUAGE   
        _LIT( KCatalogsOverrideDeviceLanguage, "en_CN" );
        _LIT( KCatalogsOverrideSoftwareLanguage, "en_CN" );
    #endif // CATALOGS_OVERRIDE_LANGUAGE   

    //#define CATALOGS_OVERRIDE_FIRMWARE
    #ifdef CATALOGS_OVERRIDE_FIRMWARE
        _LIT( KCatalogsOverrideFirmware, "emulator" );
		//_LIT( KCatalogsOverrideFirmware, "V 20.0.001 27-07-07 RM-159 Nokia N95" );
    #endif // CATALOGS_OVERRIDE_FIRMWARE

#endif // CATALOGS_DEVICE_OVERRIDE_HRH

#endif // CATALOGS_BUILD_CONFIG_OVERRIDE_DEVICE
