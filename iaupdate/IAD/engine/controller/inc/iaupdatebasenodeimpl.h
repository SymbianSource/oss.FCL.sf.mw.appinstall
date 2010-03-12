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
* Description:   This file contains the header file of CIAUpdateBaseNode class 
*
*/


#ifndef IA_UPDATE_BASE_NODE_IMPL_H
#define IA_UPDATE_BASE_NODE_IMPL_H


#include <e32base.h>

#include "iaupdatebasenode.h"
#include "iaupdateversion.h"


class MNcdNode;
class MNcdNodeMetadata;
class CNcdPurchaseDetails;
class CIAUpdateNodeDetails;
class CIAUpdateController;


/**
 *
 */
class CIAUpdateBaseNode : public CBase, 
                          public MIAUpdateBaseNode
    {

public:

    /**
     * Destructor
     **/
    virtual ~CIAUpdateBaseNode();
    
    
public: // MIAUpdateBaseNode

    /**
     * @see MIAUpdateBaseNode::MetaNamespace
     */ 
    virtual const TDesC& MetaNamespace() const;

    /**
     * @see MIAUpdateBaseNode::MetaId
     */ 
    virtual const TDesC& MetaId() const;

    /**
     * @see MIAUpdateBaseNode::Uid
     **/
    virtual const TUid& Uid() const;
	
	/**
     * @see MIAUpdateBaseNode::Identifier
     **/
    virtual const TDesC& Identifier() const;

    /**
     * @see MIAUpdateBaseNode::Name
     **/
    virtual const TDesC& Name() const;

    /**
     * @see MIAUpdateBaseNode::Description
     */
    virtual const TDesC& Description() const;

    /** 
     * @see MIAUpdateBaseNode::ContentSizeL
     */
    virtual TInt ContentSizeL() const;

    /**
     * @see MIAUpdateBaseNode::Version
     **/
    virtual const TIAUpdateVersion& Version() const;
    
    /**
     * @see MIAUpdateBaseNode::Importance
     **/
    virtual TImportance Importance() const;
    virtual void SetImportance( TImportance aImportance );

    /**
     * @see MIAUpdateBaseNode::SearchCriteria
     */
    virtual const TDesC& SearchCriteria() const;
    
    /**
     * @see MIAUpdateBaseNode:: RebootAfterInstall
     */
    virtual TBool RebootAfterInstall() const;
    
    /**
     * @see MIAUpdateBaseNode::Hidden
     */
    virtual TBool Hidden() const;

    /**
     * @see MIAUpdateBaseNode::SetSelected
     */
    virtual void SetSelected( TBool aSelected );

    /**
     * @see MIAUpdateBaseNode::IsSelected
     */
    virtual TBool IsSelected() const;

    /**
     * @note This function decodes the error code from the purchase history.
     *
     * @see MIAUpdateBaseNode::LastUpdateErrorCodeL
     * @see CIAUpdateBaseNode::LastUpdateErrorCodeFromPurchaseHistoryL
     **/
    virtual TInt LastUpdateErrorCodeL() const;
            
    /**
     * @see MIAUpdateBaseNode::SetIdleCancelToPurchaseHistoryL
     */
    virtual void SetIdleCancelToPurchaseHistoryL( 
        TBool aForceVisibleInHistory );

    /**
     * @see MIAUpdateBaseNode::SetInstallStatusToPurchaseHistoryL
     */
    virtual void SetInstallStatusToPurchaseHistoryL( 
        TInt aErrorCode, TBool aForceVisibleInHistory );
    
    /**
     * @see MIAUpdateBaseNode::Mime
     */
    virtual const TDesC& Mime() const;
    
    
public: // ** New functions

    /**
     * Sets the error information into NCD Purchase history.
     * Usually this function can be used when the update flow has been finished 
     * before this node has started its update operation. Also, notice that 
     * the node may have some old history, for example fail or success. But after this
     * operation, the information will be the latest purchase history information.
     * @param aError Error code of the operation.
     * @param aForceVisibleInHistory ETrue means that even if node would be at the moment
     * hidden, it will be forced as visible in purchase history. EFalse means that
     * items are saved normally in history.
     */
    virtual void SetIdleErrorToPurchaseHistoryL( 
        TInt aError, TBool aForceVisibleInHistory );


    /**
     * Checks if the node identifications or contents are equal.
     *
     * @param aNode Node that is compared with this class object.
     * @return TBool ETrue if the node or node content is thought to be same.
     * Else EFalse.
     */
    TBool Equals( const CIAUpdateBaseNode& aNode ) const;


    /**
     * Noramlly hidden info comes from the details gotten from the server.
     * But, by using this function the node can be forced hidden for 
     * specific purposes.
     *
     * @param aHidden ETrue if this node should be thought as hidden. 
     */
    void ForceHidden( TBool aHidden );


    /** 
     * @return TInt The content size of this node. This value is gotten from the
     * backend server. Does not include dependency contents.
     * @exception Leaves with system wide error code.
     */
    TInt OwnContentSizeL() const;
        

    /**
     * @return CIAUpdateNodeDetails& Additional node details info.
     */
    CIAUpdateNodeDetails& Details() const;


    /**
     * @return CIAUpdateController& Controller
     */
    CIAUpdateController& Controller() const;    


    /**
     * @return MNcdNode& Actual NCD Node that this iaupdate node wraps.
     */
    MNcdNode& Node() const;
    

    /**
     * This gives the exact error code from the purchase history without
     * first decoding IAD specific base codes from it.
     *
     * @see MIAUpdateBaseNode::LastUpdateErrorCodeL
     *
     * @return TInt Error code that has been saved into the purchase history.
     */
    TInt LastUpdateErrorCodeFromPurchaseHistoryL() const;

    
protected:

    /**
     * @param aController Controller of the IAUpdate engine.
     */
    CIAUpdateBaseNode( CIAUpdateController& aController );

    /**
     * @param aNode NCD Engine node that this IAUpdate node is wrapping.
     */    
    virtual void ConstructL( MNcdNode* aNode );


    /**
     * @note This function sets the purchase detail MIME by using 
     * CIAUpdateBaseNode::MimeL function instead of using the purchase 
     * history value. This way the latest value can be put automatically
     * into the purchase history if purchase detail is saved after this.
     *
     * @see CIAUpdateBaseNode::MimeL
     *
     * @notice Even if purhcase details are updated and saved into the
     * purchase history, information may not necessary be updated into
     * NCD Nodes before they are released and reloaded. So, the values in
     * IAD node objects may differ from the actual NCD Engine node value
     * until the NCD Nodes are reloaded. So, be careful when using this.
     *
     * @return CNcdPurchaseDetails* Purchase details that was found from the
     * purchase history. If right purchase details was not found, then new one
     * is created.
     * Ownership is transferred.
     */
    CNcdPurchaseDetails* PurchaseDetailsLC(); 


    /**
     * @note This function will check if MIME should 
     * contain certain postfixes that relate to the node
     * settings. If necessary, original MIME value is
     * replaced here by new value that is returned.
     *
     * @return const TDesC& MIME type of the content.
     */
    const TDesC& MimeL();
    
    virtual void SetNameL( const MNcdNodeMetadata& aMetaData );


private:

    // Prevent these if not implemented
    CIAUpdateBaseNode( const CIAUpdateBaseNode& aObject );
    CIAUpdateBaseNode& operator =( const CIAUpdateBaseNode& aObject );
 
protected: // data
    HBufC*                              iName;
    
private: // data

    MNcdNode*                           iNode;
    CIAUpdateController&                iController;
    HBufC*                              iMetaNamespace;
    HBufC*                              iMetaId;
    HBufC*                              iDescription;
    HBufC*                              iMime;
    TUid                                iUid;
    HBufC*                              iIdentifier;
    TIAUpdateVersion                    iVersion;
    CIAUpdateNodeDetails*               iDetails;
    TBool                               iSelected;
    TBool                               iForcedHidden;

    };
    
    
#endif  //  IA_UPDATE_BASE_NODE_IMPL_H
