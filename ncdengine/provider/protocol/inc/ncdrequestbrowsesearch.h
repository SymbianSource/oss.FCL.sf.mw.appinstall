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
* Description:   CNcdRequestBrowseSearch declaration
*
*/


#ifndef CNCDREQUESTBROWSESEARCH_HH
#define CNCDREQUESTBROWSESEARCH_HH

#include <e32std.h>
#include <e32base.h>
#include "ncdrequestbase.h"
#include "ncdrequestconstants.h"
#include "ncdprotocoltypes.h"

class CNcdRequestBrowseSearch : public CNcdRequestBase
    {

public:
    HBufC8* CreateRequestL();

    /**
     * Setter for the request mode (browse/search)
     *
     * @param aDoSearch  Search request if true, browse if false
     */
    void SetSearch( TBool aDoSearch );
       

    /**
     * Adds an entity to browse/search.
     *
     * @param aId
     * @param aIncludeMetadata
     */
    void AddEntityL( const TDesC& aId, TBool aIncludeMetadata );
    
    /**
     * Adds an entity to browse/search.
     *
     * @param aId
     * @param aTimestamp
     * @param aIncludeMetadata
     */
    void AddEntityL( const TDesC& aId, const TDesC& aTimestamp,
                     TBool  aIncludeMetadata );
    
    /**
     * Adds an entity filter to the request.
     * Arrays include only included keywords.
     *
     * @param aIncludedKeywords         Included keywords.
     * @param aIncludedContentPurposes  Included content purposes.
     * @param aReviewScore              Review score (1-5)
     * @param aSubscribableContent      Is content subscribable?
     * @param aFreeContent              Is content free?
     */
    void AddEntityFilterL( 
        const MDesC16Array& aIncludedKeywords,
        const MDesC16Array& aIncludedContentPurposes,        
        TBool         aSubscribableContent,
        TBool         aFreeContent);
        
    
    /**
     * Adds a response filter to the request.
     *
     * @param aPageSize
     * @param aSpageStart
     * @param aStructureDepth
     * @param aMetaDataDepth
     * @param aMetaDataPerLevel
     * @param aIncludedElements
     * @param aExcludedElements
     */
    void AddResponseFilterL(
        TInt          aPageSize,
        TInt          aPageStart,
        TInt          aStructureDepth,
        TInt          aMetaDataDepth,
        TInt          aMetaDataPerLevel,
        const MDesC16Array& aIncludedElements,
        const MDesC16Array& aExcludedElements );
    
public:
    static CNcdRequestBrowseSearch* NewL();
    static CNcdRequestBrowseSearch* NewLC();

    void ConstructL();
    
    ~CNcdRequestBrowseSearch();

private:
    CNcdRequestBrowseSearch();

private:
    TXmlEngString iName;
    TXmlEngString iNamespaceUri;
    TXmlEngString iPrefix;
    TXmlEngString iType;
    TBool iDoSearch;    
    RArray<TNcdRequestRemoteEntity> iEntities;
    
    TNcdRequestEntityFilter iEntityFilter;
    TBool iEntityFilterEnabled;
    
    TNcdRequestResponseFilter iResponseFilter;
    TBool iResponseFilterEnabled;
    
    };

#endif //CNCDREQUESTBROWSESEARCH_HH
    