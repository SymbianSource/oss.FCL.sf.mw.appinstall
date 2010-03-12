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
* Description:   This file contains the header file of MIAUpdateBaseNode class 
*
*/



#ifndef IA_UPDATE_BASE_NODE_H
#define IA_UPDATE_BASE_NODE_H


#include <e32cmn.h>

class TIAUpdateVersion;


/**
 *
 */
class MIAUpdateBaseNode
    {    
    
public:

    /**
     * Enumerates the importance of this node
     **/
    enum TImportance
        {
        ENormal,
        ERecommended,
        ECritical,
        EMandatory,
        EHidden
        };


    /**
     * Together with Id, uniquely identifies the node metadata.
     * @return const TDesC& Namespace
     */ 
    virtual const TDesC& MetaNamespace() const = 0;

    /**
     * Together with Namespace, uniquely identifies the node metadata.
     * @return const TDesC& Id
     */ 
    virtual const TDesC& MetaId() const = 0;

    /**
     * @return const TUid& Uid of this node
     **/
    virtual const TUid& Uid() const = 0;
	
	/*
	 * This returns the identifier of widget. Normally it is a string --> "com.yiibu.hue"
	 * This is the unique identifier of widget before installing on top of Symbian platform.
     */
     virtual const TDesC& Identifier() const = 0;    

    /**
     * @return const TDesC& The name of this node
     **/
    virtual const TDesC& Name() const = 0;
    
    /**
     * @return const TDesC& The description for this node.
     */
    virtual const TDesC& Description() const = 0;

    /** 
     * @return TInt The size of the content in bytes.
     * @exception Leaves with system wide error code if 
     * the content size could not be calculated.
     */
    virtual TInt ContentSizeL() const = 0;

    /**
     * @return const TIAUpdateVersion& Specific version information 
     * of this node.
     **/
    virtual const TIAUpdateVersion& Version() const = 0;
    
    /**
     * @return TImportance the importance of this node
     **/
    virtual TImportance Importance() const = 0;
    
    virtual void SetImportance( TImportance aImportance ) = 0;
    
    /**
     * @return const TDesC& the search criteria of the node content.
     * This is mainly used for plug-ins to recognize if the
     * content corresponds the search of some user application.
     **/
    virtual const TDesC& SearchCriteria() const = 0;
    
    /**
     * @return TBool ETrue if reboot is needed after install 
     */
    virtual TBool RebootAfterInstall() const = 0;

    /**
     * @return TBool ETrue if the node should be hidden in the UI. 
     * EFalse if the node can be shown.
     */
    virtual TBool Hidden() const = 0;

    /**
     * @param aSelected ETrue if the node is selected. Else EFalse.
     **/
    virtual void SetSelected( TBool aSelected ) = 0;

    /**
     * @return TBool ETrue if the node is selected. Else EFalse.
     **/
    virtual TBool IsSelected() const = 0;

    /**
     * @return TInt Error code of the last operation whose information 
     * has been set into the purchase history.
     * @exception Leaves with system wide error code
     */
    virtual TInt LastUpdateErrorCodeL() const = 0;

    /**
     * Sets the cancel information into NCD Purchase history.
     * Usually this function can be used when the update flow has been finished 
     * before this node has started its update operation. Also, notice that 
     * the node may have some old history, for example fail or success. But after this
     * operation, the information will be the latest purchase history information.
     *
     * @param aForceVisibleInHistory ETrue means that even if node would be at the moment
     * hidden, it will be forced as visible in purchase history. EFalse means that
     * items are saved normally in history.
     * @exception Leaves with system wide error code
     */
    virtual void SetIdleCancelToPurchaseHistoryL( 
        TBool aForceVisibleInHistory ) = 0;
    
    /**
     * Updates the puchase history from the given info.
     * This function can be used to update the purchase history after self
     * updater has been used.
     *
     * @notice For normal nodes this function should not be used because purchase history
     * is updated automatically. Mainly this can be used for self update nodes and
     * service packs.
     *
     * @param aErrorCode The error code of the install operation of this node.
     * @param aForceVisibleInHistory ETrue means that even if node would be at the moment
     * hidden, it will be forced as visible in purchase history. EFalse means that
     * items are saved normally in history.
     * @exception Leaves with system wide error code
     */
    virtual void SetInstallStatusToPurchaseHistoryL( 
        TInt aErrorCode, TBool aForceVisibleInHistory ) = 0;
    
    /**
    * @return const TDesC& The mime type of this node
    **/
    virtual const TDesC& Mime() const = 0;

    
protected:

    virtual ~MIAUpdateBaseNode() { }
        
    };

#endif  //  IA_UPDATE_BASE_NODE_H
