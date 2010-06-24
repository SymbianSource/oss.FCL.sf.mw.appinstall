/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the header file of CIAUpdateNodeFilter 
*                class 
*
*/




#ifndef IA_UPDATE_NODE_FILTER_H
#define IA_UPDATE_NODE_FILTER_H

#include <e32base.h>
#include <e32cmn.h>

class MIAUpdateNode;
class MIAUpdateFwNode;
class CIAUpdateParameters;


/**
 * CIAUpdateNodeFilter
 */
class CIAUpdateNodeFilter : public CBase
    {
    
public:

    static CIAUpdateNodeFilter* NewL();
    static CIAUpdateNodeFilter* NewLC();

    ~CIAUpdateNodeFilter();


    /**
     * Sets the parameters that are used for filtering when
     * the update node list is handled.
     * Ownership is transferred.
     * Old parameters are deleted.
     * @param aParmas Filter parameters.
     */
    void SetFilterParams( CIAUpdateParameters* aParams );
    
    
    /**
     * @return CIAUpdateParameters* Pointer to the parameters that
     * this class object uses for filtering.
     * Ownership is not transferred.
     */
    CIAUpdateParameters* FilterParams() const;
    
    
    /**
     * Filters and sorts the nodes in the array according to the 
     * filter parameters of this class object.
     *
     * @param aNodes The array of nodes that will be filtered and sorted.
     */
    void FilterAndSortNodesL( RPointerArray< MIAUpdateNode >& aNodes,
                              RPointerArray< MIAUpdateFwNode >& aFwNodes);

    
    /**
     * Check available update(s) 
     * given list of nodes is compared against filter parameters
     *
     * @param aNodes List of nodes
     * @param aNodes List of firmware nodes
     * @param aCount Returns count of available updates
     * @return TUid pUID of the content that filter parameters are using.
     * Usually, this is the same as the UID of the filter parameters. But,
     * if the filter parameters contain SID instead of pUID, and the pUID
     * corresponding the SID is found, then pUID is returned. If filter 
     * parameters have not been set, then empty UID is returned.
     */
    void CountOfAvailableUpdatesL( 
        const RPointerArray< MIAUpdateNode >& aNodes, 
        const RPointerArray< MIAUpdateFwNode >& aFwNodes,
        TInt& aCount );

    void SetDependenciesSelectedL( MIAUpdateNode& aNode,
                        const RPointerArray<MIAUpdateNode>& aAllNodes);
    
    void StoreNodeListL( const RPointerArray< MIAUpdateNode >& aNodes);  
    
    void RestoreNodeListL( RPointerArray< MIAUpdateNode >& aNodes) const;
    
    void SortSelectedNodesFirstL( const RPointerArray<MIAUpdateNode>& aSelectedNodes, 
                                 RPointerArray< MIAUpdateNode >& aNodes );
    
    void SortThisNodeFirstL( const MIAUpdateNode* aFirstNode, RPointerArray< MIAUpdateNode >& aNodes);

    
private:

    CIAUpdateNodeFilter();
    void ConstructL();
    
    // Prevent these if not implemented
    CIAUpdateNodeFilter( const CIAUpdateNodeFilter& aObject );
    CIAUpdateNodeFilter& operator =( const CIAUpdateNodeFilter& aObject );    

    TBool CompareAndMarkFilterNodesL( 
        MIAUpdateNode& aNode1, 
        MIAUpdateNode& aNode2,
        const TUid& aPackageUid );

    TBool MarkFilterNodeL( 
        MIAUpdateNode& aNode, 
        const TUid& aPackageUid );
        
    TBool FilteredByImportance() const;

    TBool ImportanceMatches( MIAUpdateNode& aNode ) const;
        
    TBool DMSupportL();
    
    TUid PackageUidL( const RPointerArray< MIAUpdateNode >& aNodes );
       
    TBool FilterCheckForServicePackL( const MIAUpdateNode& aNode,
                                      const TUid& aPUid,
                                      const TDesC& aSearchCriteria );
    
    TBool SearchCriteriaMatches( const TDesC& aParamSearchCriteria, 
                                 const TDesC& aNodeSearchCriteria ) const;
    
    
    
private: // data

    CIAUpdateParameters* iFilterParams;
    
    TBool iDmSupportAsked;
    
    TBool iDmSupport;
    
    RPointerArray< MIAUpdateNode > iStoredNodes;

    };

#endif // IA_UPDATE_NODE_FILTER_H
