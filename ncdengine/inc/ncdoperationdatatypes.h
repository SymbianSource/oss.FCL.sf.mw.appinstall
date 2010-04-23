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
	

#ifndef NCDOPERATIONDATATYPES_H
#define NCDOPERATIONDATATYPES_H


/**
 * Data types for download operations
 */
enum TNcdDownloadDataType
    {
    /**
     * Operation is downloading a skin.
     */
    ENcdSkinDownload,
        
    /**
     * Operation is downloading an icon.
     */
    ENcdIconDownload,

    /**
     * Operation is downloading a screenshot.
     */
    ENcdScreenshotDownload,
       
    /**
     * Operation is downloading content.
     */    
    ENcdContentDownload,
    
    /**
     * Operation is downloading a preview.
     */    
    ENcdPreviewDownload,
    
    /**
     * Operation is downloading a file of undetermined type.
     */
    ENcdGenericFileDownload
    };

#endif // NCDOPERATIONDATATYPES_H
