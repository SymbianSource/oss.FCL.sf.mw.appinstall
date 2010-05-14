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
* Description:   ?Description
*
*/



#ifndef IA_UPDATE_PROTOCOL_CONSTS_H
#define IA_UPDATE_PROTOCOL_CONSTS_H

/**
 * IAUpdateProtocolConsts
 * Gives constants that are used when details elements are interpreted
 * from the server responses.
 */
namespace IAUpdateProtocolConsts
    {
    // Node details elements and their possible values

    /**
     * Importance informs how important the node updata is.
     */
    _LIT( KImportanceKey, "iaimportance" );
    _LIT( KImportanceMandatory, "mandatory" );
    _LIT( KImportanceCritical, "critical" );
    _LIT( KImportanceRecommended, "recommended" );
    _LIT( KImportanceNormal, "normal" );
    _LIT( KImportanceHidden, "hidden" );

    /**
     * Package type informs what kind of update package the node
     * content will provide.
     */
    _LIT( KPackageTypeKey, "iatype" );
    _LIT( KPackageTypeSA, "SA" );
    _LIT( KPackageTypeSP, "SP" );
    _LIT( KPackageTypePU, "PU" );

    /**
     * Search criteria can be used to filter nodes.
     */
    _LIT( KSearchCriteriaKey, "iasearchcriteria" );


    // Firmware item related information that is given as normal
    // key-value-pairs is defined here.

    /**
     * Firmware version 1 key.
     */
    _LIT( KFirmwareVersion1Key, "fwVersion1" );

    /**
     * Firmware version 2 key.
     */
    _LIT( KFirmwareVersion2Key, "fwVersion2" );
    
    /**
     * Firmware version 3 key.
     */
    _LIT( KFirmwareVersion3Key, "fwVersion3" );
    
    /**
     * Reboot after install 
     */
    _LIT( KRebootAfterInstallKey, "rebootafterinstall" );
    _LIT( KRebootAfterInstallNeeded, "1" );
    
    

    // The actual dependency information is included into the value field
    // that corresponds to the details dependency key.

    /*
     * Dependency key, whose value contains all dependency information in
     * an xml format.    
     */
    _LIT( KDependencyKey, "iadependencies" );


    // The dependency elements local names are defined here


    /**
     * This element does not have any local name, because this is data is
     * directly from the protocol details value field.
     */
    _LIT8( KDependencies, "iaupdatedependencies" );
    _LIT( KDependenciesPrefix, "<iaupdatedependencies>" );
    _LIT( KDependenciesPostfix, "</iaupdatedependencies>" );

    // Node related dependencies

    /**
     * Local name for the element that contains node depenendency elements.
     */
    _LIT8( KNodeDependencies, "interDeps" );
    
    /**
     * Local name for the node dependency element
     */
    _LIT8( KNodeDependency, "interDep" );

    /**
     * Local name for the node uid element.
     */
    _LIT8( KNodeUid, "sisxUid" );

    /**
     * Local name for the node embedded element.
     */
    _LIT8( KNodeEmbedded, "embedded" );

    // Node version infos
    _LIT8( KNodeVersionFrom, "versionFrom" );
    _LIT8( KNodeVersionTo, "versionTo" );


    // Platform related dependencies

    /**
     * This contains the platform dependency information
     */ 
    _LIT8( KPlatformDependency, "swPlatformDep" );

    
    /**
     * This is the platform tag.
     */
    _LIT8( KPlatformDescription, "platform" );

    /**
     * This is the only acceptable platform string for s60.
     */
    _LIT8( KPlatformDescriptionValue, "S60" );


    // Platform version infos
    _LIT8( KPlatformVersionFrom, "versionFrom" );
    _LIT8( KPlatformVersionTo, "versionTo" );
    _LIT8( KPlatformVersionDate, "date" );
    _LIT8( KPlatformVersionDateYear, "year" );
    _LIT8( KPlatformVersionDateWeek, "week" );


    // Generally used constants

    // Version major, minor, and build are common
    // for all the versions.
    _LIT8( KVersionMajor, "major" );
    _LIT8( KVersionMinor, "minor" );
    _LIT8( KVersionBuild, "build" );


    // IAD specific key values that are used in the 
    // protocol requests.

    /**
     * The key of the key-value pair for the client role. 
     * The value that will correspond the key will define 
     * the role of this client.
     */
    _LIT( KIAClientRole, "clientRole" );


    // IAD specific MIME constants.
    // These are used to identify the type of the node
    // from the server defined MIME.

    /**
     * MIME for service pack.
     *
     * Pattern constant is used for MIME comparisons.
     * So, strings should be equal and only difference is 
     * wildcard characters. The pattern is not exactly defined
     * in protocol but it is defined for convenience.
     */
    _LIT( KMimeServicePack, "application/x-arrow-metapackage" );
    _LIT( KMimeServicePackPattern, "application/x-arrow-metapackage*" );

    /**
     * MIME for FOTA DP2 firmware update.
     */
    _LIT( KMimeFotaDp2, "application/x-nokia-fota-dp2" );

    /**
     * MIME for FOTA DP2 firmware update.
     */
    _LIT( KMimeFotiNsu, "application/x-nokia-foti-nsu" );
    
    /**
     * MIME for widgets
     */
    _LIT( KMimeWidget, "application/x-nokia-widget" );

    }
    
#endif // IA_UPDATE_PROTOCOL_CONSTS_H
