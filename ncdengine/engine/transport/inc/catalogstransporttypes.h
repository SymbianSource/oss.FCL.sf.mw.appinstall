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


#ifndef _CATALOGSTRANSPORTTYPES_H
#define _CATALOGSTRANSPORTTYPES_H

/** Transport interface IDs */

const TInt KCatalogsTransportSmsInterface 	= 0;
const TInt KCatalogsTransportHttpInterface 	= 1;


enum TCatalogsTransportPriority
    {
    
    /** 
     * Queued priorities
     *
     * Operations with these priorities are are queued in
     * general queue. Operations are executed one at a time.
     */
    ECatalogsPriorityQueuedLow = -3,
    ECatalogsPriorityQueuedMedium = -2,
    ECatalogsPriorityQueuedHigh = -1,
    
    /**
     * Separates queued and normal priorities
     */
    ECatalogsPriorityQueued = ECatalogsPriorityQueuedHigh,
    
    /**
     * Normal priorities 
     *
     * Operations with these priorities are queued either in
     * transaction queue or download queue. Depending on the
     * implementation, more than one operation can be in
     * execution at the same time.
     */
    ECatalogsPriorityLow = 0,
    ECatalogsPriorityMedium,
    ECatalogsPriorityHigh
    };
    
    

                
#endif // _CATALOGSTRANSPORTTYPES_H