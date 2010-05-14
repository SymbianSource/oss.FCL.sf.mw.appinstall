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
	

#ifndef M_NCD_SEARCH_FILTER_H
#define M_NCD_SEARCH_FILTER_H

#include <e32cmn.h>
#include <bamdesca.h>

#include "ncditempurpose.h"



/**
 *  Search filter interface.
 *
 *  Allows setting of keywords and/or content types to limit searches.
 *
 *  @note Default implementation class can be found in ncdutils.h.
 *  
 */
class MNcdSearchFilter
    {
public:

    /**
     * Search mode
     */
    enum TSearchMode 
        {
        /**
         * Normal search
         *
         * Items returned by the server in the search response are ignored. 
         * This mode should be used always when doing recursive searches
         */
        ENormal = 0,
        
        /**
         * Accept items in the search response
         *
         * This mode should be used if the server returns items in its
         * search responses.
         * @note Recursive searching is discouraged because it can cause
         * performance issues.
         */
        EAcceptItems
        };
public:

    /**
     * Returns keywords set for the filter. Items must match all keywords in order
     * to be included in search results.
     *
     * 
     * @return Array of keywords. If empty, all items match for keywords.
     */
    virtual const MDesCArray& Keywords() const = 0;

    /**
     * Returns content types set for the filter. Items must match one of the types
     * in order to be included in search results.
     *  
     * 
     * @return Bit field defining the content type(s) to search for. TNcdItemPurpose
     *  defines the bit values used.
     * @see TNcdItemPurpose
     */
    virtual TUint ContentPurposes() const = 0;
    
    
    /**
     * Returns search mode
     *
     * 
     * @return Search mode
     */
    virtual TSearchMode SearchMode() const = 0;
    
    
    /**
     * Depth of recursion
     *
     * @return Depth of recursion
     */
    virtual TUint RecursionDepth() const = 0;

protected:

    /**
     * Destructor.
     *
     */
    virtual ~MNcdSearchFilter() {}

    };
	
	
#endif //  M_NCD_SEARCH_FILTER_H
