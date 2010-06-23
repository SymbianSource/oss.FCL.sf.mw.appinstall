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
* Description:   Catalogs Engine error code definitions.
*
*/


#ifndef CATALOGS_PANICS_H
#define CATALOGS_PANICS_H

/** Catalogs Engine panic category name */
_LIT( KCatalogsPanicCategory, "Catalogs" );


/** Catalogs Engine panic category panic codes */
enum TCatalogsPanic
    {
    /**
     * Raised when the client releases its last reference to the
     * Catalogs Engine main object (MCatalogsEngine interface),
     * while the session opened with the object's MCatalogsEngine::Connect()
     * is still open.
     */
    ECatalogsPanicSessionNotClosed = 1,

    /**
     * Raised when the client attempts to close its session to the Catalogs
     * Engine while still holding references to other objects implemented
     * in Catalogs Engine or the providers (e.g. NCD) therein.
     */
    ECatalogsPanicUnreleasedReferencesExist
    };


#endif // CATALOGS_PANICS_H
