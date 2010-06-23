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
* Description:   UIDs for the Catalogs engine and subcomponents
*
*/


#ifndef _CATALOGSUIDS_H
#define _CATALOGSUIDS_H

/**
 * Replaced with USE_BUILD_SCRIPT when using build script
 */
#define DUMMY_DEFINE


/**
 * NOTICE: This file contains two versions of the defines.
 *         Ones that are hard coded here and ones that are
 *         replaced by the build script if it is used.
 */


#ifdef USE_BUILD_SCRIPT

    /**
     * Defines when using the build script. For commenting see the
     * defines of the variables of the case when the build script
     * is not used. (later in this file)
     */
 
    #define KCatalogsServerUid 0xCATALOGS_SERVER_UID
    #define KCatalogsUndertakerUid 0xCATALOGS_UNDERTAKER_UID
    #define KCatalogsDefaultClientFamilyUid 0xCATALOGS_DEFAULT_CLIENT_FAMILY_UID
    #define KNcdProviderUid 0xNCD_PROVIDER_UID    
    #define KCatalogsEngineUid 0xCATALOGS_ENGINE_UID
    #define KCatalogsEngineInterfaceUid 0xCATALOGS_ENGINE_INTERFACE_UID
    #define KCatalogsEngineImplementationUid 0xCATALOGS_ENGINE_IMPLEMENTATION_UID

//#warning "pcfront end not varied"

#else
    /**
     * Catalogs default client (NCD) family UID.
     * This is passed by the NCD UI client to the
     * engine during connection as a family ID.
     * Purchase history events created by this NCD UI client is
     * marked by this ID.
     * UI Client with UID KCatalogsUiClientUid must always
     * use this family id when connecting to the engine.
     */
    #define KCatalogsDefaultClientFamilyUid 0x20001A4A

    /**
     * Catalogs engine server exe UID.
     * If you change this, you also need to change the name of the
     * central repository file (currently located in \engine\group\)
     * and change the owner UID in that file
     * and also fix bld.inf's to export it correctly and fix
     * the .pkg file to copy it correctly
     */
    #define KCatalogsServerUid 0x20019119

    /**
     * Debug utility executable uid.
     */
    #define KCatalogsUndertakerUid 0x2001911A

    /**
     * NCD provider uid.
     * This is passed to MCatalogsEngine::CreateProviderL()
     * to create the NCD provider. This is the only provider currently
     * supported.
     */
    #define KNcdProviderUid 0x20008013
      
    /**
     * UID for catalogs engine ECom DLL.
     * Notice that there is a rss file named according
     * to this uid (20019116.rss), it is taken into use in
     * catalogsproxy.mmp and the dll is renamed after this
     * uid in pkg.
     */
    #define KCatalogsEngineUid 0x20019116

    /**
     * UID for catalogs engine ECom interface. 
     */
    #define KCatalogsEngineInterfaceUid 0x20019117

    /**
     * UID for catalogs engine ECom implementation. 
     */
    #define KCatalogsEngineImplementationUid 0x20019118

#endif // USE_BUILD_SCRIPT 


/**
 * UID for the PC client front end server exe. 
 */
#define KCatalogsPCClientFrontEndUid 0x20008012


/**
 * UID for the PC client install launcher exe
 */
#define KCatalogsPCClientLauncherUid 0x2000854D


#endif // _CATALOGSUIDS_H
