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
* Description:   Contains item purpose definitions
*
*/


#ifndef NCD_ITEM_PURPOSE_H
#define NCD_ITEM_PURPOSE_H

/**
 * Defines the flag values indicating the different
 * available purposes an item can be used for.
 *
 * 
 * @see MNcdContentInfo::Purpose()
 */
enum TNcdItemPurpose
    {

    /** Unknown purpose. */
    ENcdItemPurposeUnknown = 1,

    /** Usable as a ringing tone. */
    ENcdItemPurposeRingtone = 2,

    /** Usable as a wall paper. */
    ENcdItemPurposeWallpaper = 4,

    /** Usable as generic music. */
    ENcdItemPurposeMusic = 8,

    /** Usable as an application. */
    ENcdItemPurposeApplication = 16,

    /** Usable as a theme. */
    ENcdItemPurposeTheme = 32,

    /** Usable as an image. */
    ENcdItemPurposeHtmlPage = 64,

    /** Usable as a video. */
    ENcdItemPurposeVideo = 128,

    /** Usable as a game. */
    ENcdItemPurposeGame = 256,

    /** Usable as a screensaver. */
    ENcdItemPurposeScreensaver = 512,

    /** Usable as a stream. */
    ENcdItemPurposeStream = 1024

    };

#endif // NCD_ITEM_PURPOSE_H
