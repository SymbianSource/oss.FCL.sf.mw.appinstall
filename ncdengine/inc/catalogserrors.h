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


#ifndef CATALOGS_ERRORS_H
#define CATALOGS_ERRORS_H


const TInt KCatalogsErrorBase = -1000;


/** 
 * Client is not authorized to start maintenance on Catalogs Engine.
 * @see MCatalogsEngine::StartMaintenanceL()
 */
const TInt KCatalogsErrorMaintenanceNotAuthorized = KCatalogsErrorBase - 1;


/** 
 * Client has not started maintenance.
 * @see MCatalogsEngine::StartMaintenanceL()
 * @see MCatalogsEngine::EndMaintenanceL()
 */
const TInt KCatalogsErrorMaintenanceNotStarted = KCatalogsErrorBase - 2;


/**
 * The requested functionality is not available because the Catalogs Engine is
 * in maintenance mode.
 */
const TInt KCatalogsErrorMaintenanceMode = KCatalogsErrorBase - 3;


/**
 * Base value used for HTTP errors. Eg. HTTP 400 would be given as -100400
 */
const TInt KCatalogsErrorHttpBase = -100000;


/**
 * Connection was denied by the user
 */
const TInt KCatalogsErrorHttpConnectionDenied = KCatalogsErrorHttpBase - 1;

/**
 * No access point defined at all
 */
const TInt KCatalogsErrorHttpNoApn = KCatalogsErrorHttpBase - 2;

#endif // CATALOGS_ERRORS_H
