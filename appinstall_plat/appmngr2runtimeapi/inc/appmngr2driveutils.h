/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility definitions for drive and file handling
*
*/


#ifndef C_APPMNGR2DRIVEUTILS_H
#define C_APPMNGR2DRIVEUTILS_H

#include <appmngr2infobase.h>           // TAppMngr2Location

/**
 * Utility functions to get TAppMngr2Location values and to
 * construct resource and bitmap file names. 
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class TAppMngr2DriveUtils
	{
public:     // new functions
    /**
     * Returns location info (TAppMngr2Location) based on given file name (aFileName).
     * 
     * @param aFileName  File which location is needed
     * @param aFs  File server session
     * @return TAppMngr2Location  Location of the file
     */ 
    IMPORT_C static TAppMngr2Location LocationFromFileNameL( const TDesC& aFileName, RFs& aFs );
    
    /**
     * Returns location info (TAppMngr2Location) based on given drive (aDrive).
     * 
     * @param aDrive  Drive which location is needed
     * @param aFs  File server session
     * @return TAppMngr2Location  Location of the drive
     */ 
    IMPORT_C static TAppMngr2Location LocationFromDriveL( TInt aDrive, RFs& aFs );
    
    /**
     * Returns the nearest resource file using for given file (aFileName) using
     * EDefaultRom drive, KDC_RESOURCE_FILES_DIR, and BaflUtils::NearestLanguageFile().
     * 
     * TFileName object is allocated on heap and it is left in the cleanup stack.
     * The caller of this method is responsible to delete the allocated TFileName object.
     * 
     * @param aFileName  Resource file name
     * @param aFs  File server session
     * @return TFileName*  Full name of the nearest resource file to load
     */
    IMPORT_C static TFileName* NearestResourceFileLC( const TDesC& aFileName, RFs& aFs );
    
    /**
     * Returns full bitmap file name. Constructs file name using EDefaultRom
     * drive, KDC_APP_BITMAP_DIR directory, and given MBM/MIF file name.
     * 
     * Full file name is allocated on heap and it is left in the cleanup stack.
     * The caller of this method is responsible to delete the allocated HBufC object.
     *  
     * @param aBitmapFile  MBM or MIF file name
     * @return HBufC*  Full file name for aBitmapFile
     */
    IMPORT_C static HBufC* FullBitmapFileNameLC( const TDesC& aBitmapFile, RFs& aFs );

	};

#endif  // C_APPMNGR2DRIVEUTILS_H

