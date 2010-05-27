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
* Description:   ?Description
*
*/
	

#ifndef NCD_CHILD_LOAD_MODE_H
#define NCD_CHILD_LOAD_MODE_H

/** 
 * Defines the mode for child node loading.
 */
enum TNcdChildLoadMode
    {
    /** 
     * Loads structure data for child nodes on the given page.
     *
     * @note Loads structure for a minimum of 48 nodes regardless of page size.
     * (i.e. the range: page size -> page size + 48). This is needed for
     * aqcuiring seen/new information.
     * @note When a node has structure data but no metadata it's state is
     *  EStateNotInitialized.
     * @see MNcdNodeSeen
     * @see MNcdNode::TState
     */
    ELoadStructure,
    
    /**
     * Loads both structure- and metadata for child nodes on the given page.
     *
     * @note Loads structure for a minimum of 48 nodes regardless of page size.
     * (i.e. the range: page size -> page size + 48). This is needed for
     * aqcuiring seen/new information.
     * @see MNcdNodeSeen
     */
    ELoadMetadata,
    
    /**
     * Obsolete.
     * @note DON'T USE THIS! NOT SUPPORTED IN CURRENT IMPLEMENTATION!
     */
    EForceRefresh
    };
    
#endif //  NCD_CHILD_LOAD_MODE_H
