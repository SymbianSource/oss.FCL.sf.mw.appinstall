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


#ifndef IA_UPDATE_NODE_DETAILS_H
#define IA_UPDATE_NODE_DETAILS_H


#include <e32base.h>
#include <e32cmn.h>
#include "iaupdatenode.h"
#include "iaupdatebasenode.h"

class CIAUpdateNodeDependency;
class CIAUpdatePlatformDependency;
class MNcdNodeMetadata;


class CIAUpdateNodeDetails : public CBase
    {

public:

    /**
     * @param aData If NULL then all the details values will be set to defaults.
     * If metadata is not NULL then its details key-value-pairs are used to create
     * contents of this class object. Ownership is not transferred.
     */
    static CIAUpdateNodeDetails* NewL( const MNcdNodeMetadata* aData );
    static CIAUpdateNodeDetails* NewLC( const MNcdNodeMetadata* aData );

    virtual ~CIAUpdateNodeDetails();

    /**
     * @param aData If NULL then all the details values will be set to defaults.
     * If metadata is not NULL then its details key-value-pairs are used to create
     * contents of this class object. Ownership is not transferred.
     */
    void SetDetailsL( const MNcdNodeMetadata* aData );

    MIAUpdateNode::TPackageType ContentType() const;
    
    MIAUpdateBaseNode::TImportance Importance() const;
    
    void SetImportance( MIAUpdateBaseNode::TImportance aImportance ); 
    
    const TDesC& SearchCriteria() const;
    
    TBool EmbededDegrades() const;

    void GetDependenciesL( 
        RPointerArray< CIAUpdateNodeDependency >& aDependencies ) const;    

    CIAUpdatePlatformDependency& PlatformDependency() const;

    const TDesC& FwVersion1() const;

    const TDesC& FwVersion2() const;

    const TDesC& FwVersion3() const;
    
    TBool RebootAfterInstall() const;


private:

    CIAUpdateNodeDetails();
    void ConstructL( const MNcdNodeMetadata* aData );

    void ClearAll();

    // Prevent these if they are not implemented.
    CIAUpdateNodeDetails( const CIAUpdateNodeDetails& aObject );
    CIAUpdateNodeDetails& operator =( const CIAUpdateNodeDetails& aObject );


private: // data

    MIAUpdateNode::TPackageType iType;
    MIAUpdateBaseNode::TImportance iImportance;
    HBufC* iSearchCriteria;

    RPointerArray< CIAUpdateNodeDependency > iDependencies;
    CIAUpdatePlatformDependency* iPlatformDependency;

    HBufC* iFwVersion1;
    HBufC* iFwVersion2;
    HBufC* iFwVersion3;
    
    TBool iRebootAfterInstall;

    };

#endif // IA_UPDATE_NODE_DETAILS_H
