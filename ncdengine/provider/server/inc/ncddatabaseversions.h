/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains database version definitions
*
*/


#ifndef NCDDATABASEVERSIONS_H
#define NCDDATABASEVERSIONS_H

/**
 * Base value for Altair database version numbers
 */
const TUint32 KNcdAltairVersionBase = 1000;

/**
 * Purchase history version number
 *
 * @note This MUST be increased always when purchase history's
 * binary format is changed
 */
const TUint32 KNcdPurchaseHistoryVersion = KNcdAltairVersionBase + 4;


/**
 * General database version. This affects node cache, 
 * subscriptions, etc.
 *
 * @note This MUST be increased always when the binary format
 * of any database except purchase history changes
 */
const TUint32 KNcdGeneralDatabaseVersion = KNcdAltairVersionBase + 18;

#endif // NCDDATABASEVERSIONS_H
