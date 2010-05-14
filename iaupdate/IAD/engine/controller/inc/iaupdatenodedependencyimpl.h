/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef IA_UPDATE_NODE_DEPENDENCY_IMPL_H
#define IA_UPDATE_NODE_DEPENDENCY_IMPL_H


#include <e32std.h>

#include "iaupdateversion.h"

class CIAUpdateNode;


class CIAUpdateNodeDependency : public CBase
    {
    
public:

    static CIAUpdateNodeDependency* NewLC();                                     
    static CIAUpdateNodeDependency* NewL();
    
    virtual ~CIAUpdateNodeDependency();
    
    
    /**
     * @return const TIAUpdateVersion& The version floor of the dependency. 
     * Can be the same as version roof, in that case
     * dependency is to a distinct SW revision.
     **/
    const TIAUpdateVersion& VersionFloor() const;

    /**
     * @param aVersion The version floor of the dependency. 
     * Can be the same as version roof, in that case
     * dependency is to a distinct SW revision.
     **/
    void SetVersionFloor( const TIAUpdateVersion& aVersion );
    

    /**
     * @return const TIAUpdateVersion& The version roof of the dependency. 
     * Can be the same as version floor, in that case
     * dependency is to a distinct SW revision.
     **/
    const TIAUpdateVersion& VersionRoof() const;

    /**
     * @param aVersion The version roof of the dependency. 
     * Can be the same as version floor, in that case
     * dependency is to a distinct SW revision.
     **/
    void SetVersionRoof( const TIAUpdateVersion& aVersion );
    

    /**
     * @return TBool ETrue if this dependency is embedded.
     **/
    TBool IsEmbedded() const;

    /**
     * @param aEmbedded ETrue if this dependency is embedded.
     **/
    void SetEmbedded( TBool aEmbedded );
    

    /**
     * @return MIAUpdateNode* The node present that matches this dependency best. 
     * Is NULL if dependency cannot be, or doesn't need to be, fulfilled by IAD.
     * Ownership of the node is not transferred
     **/
    CIAUpdateNode* BestMatch() const;

    /**
     * Sets the best match for this dependency.
     *
     * @param aBestMatch Node that matches best the dependency.
     * Ownership is not transferred.
     **/
    void SetBestMatch( CIAUpdateNode* aBestMatch );
    

    /**
     * @return const TUid& The uid of this node dependency
     **/
    const TUid& Uid() const;    

    /**
     * @param aUid The uid of this node dependency
     **/
    void SetUid( const TUid& aUid );


    /**
     * Resets all the member variables to their default values.
     */
    void Reset();    

    
private:

    /**
     * Constructor
     */
    CIAUpdateNodeDependency();
    
    /**
     * ConstructL
     */
    void ConstructL();
    
    
private: // data

    TUid                iUid;    
    TIAUpdateVersion    iVersionFloor;    
    TIAUpdateVersion    iVersionRoof;    
    TBool               iEmbedded;
    CIAUpdateNode*      iBestMatch;
    
    };

#endif  //  IA_UPDATE_NODE_DEPENDENCY_IMPL_H
