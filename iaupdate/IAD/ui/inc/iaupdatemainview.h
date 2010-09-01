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
* Description:   This file contains the header file of the CIAUpdateMainView class 
*
*/



#ifndef IAUPDATEMAINVIEW_H
#define IAUPDATEMAINVIEW_H

// INCLUDES
#include <aknview.h> 
#include <akntoolbarobserver.h> 
#include "iaupdatecontainerobserver.h"

// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknToolbar;
class CIAUpdateMainContainer;
class CIAUpdateFWUpdateHandler;

class MIAUpdateAnyNode;
class MIAUpdateNode;
class MIAUpdateFwNode;

// CLASS DECLARATION
/**
* This class represents the main view of IAUpdate
*
*/
class CIAUpdateMainView : public CAknView, 
                          public MAknToolbarObserver, 
                          public MIAUpdateContainerObserver
    {
public:
    /**
    * Symbian two phased constructor.
    * @return Instance of the created server object.
    */
    static CIAUpdateMainView* NewL(const TRect& aRect );

    /**
    * Symbian two phased constructor.
    * @return Instance of the created server object.
    */
    static CIAUpdateMainView* NewLC(const TRect& aRect );

    /**
    * Destructor
    */
    ~CIAUpdateMainView();
    
    /**
    * Refresh the list
    * @param aNodes  Array of nodes 
    * @param aError  Error code
    */    
    void RefreshL( const RPointerArray<MIAUpdateNode>& aNodes,
                   const RPointerArray<MIAUpdateFwNode>& aFwNodes,
                   TInt aError );       
            
    /**
    * Returns node that's highlighted in a list
    * @return Current (highlighted) node
    */     
    MIAUpdateAnyNode* GetCurrentNode();
    
    /**
    * Returns node that's selected (marked) in a list
    * Can be used only when it's known that only one node is selected (marked)
    * @return selected (marked) node
    */  
    MIAUpdateAnyNode* GetSelectedNode();
    
    
private:  // from CAknView
    
    /**
    * Returns views id, intended for overriding by sub classes.
    * @return id for this view.
    */
    TUid Id() const;
    
    /** 
    * Command handling function intended for overriding by sub classes. 
    * Default implementation is empty.  
    * @param aCommand ID of the command to respond to. 
    */
    void HandleCommandL( TInt aCommand );
    
    /**
    * Dynamic menu initiation
    * Called by framework before menu is shown.
    * @param aResourceId Menu resource id.
	* @param aMenuPane Pointer to the menu.
    */ 
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );


    /** 
    * Handles a view activation and passes the message of type 
    * @c aCustomMessageId. This function is intended for overriding by 
    * sub classes. This function is called by @c AknViewActivatedL().
    * Views should not launch waiting or modal notes or dialogs in
    * @c DoActivateL.
    * @param aPrevViewId Specifies the view previously active.
    * @param aCustomMessageId Specifies the message type.
    * @param aCustomMessage The activation message.
    */
    void  DoActivateL ( const TVwsViewId& aPrevViewId, 
                        TUid aCustomMessageId, 
                        const TDesC8& aCustomMessage );

    /** 
    * View deactivation function intended for overriding by sub classes. 
    * This function is called by @c AknViewDeactivated().
    */
    void DoDeactivate();
 
private: // from MAknToolbarObserver 
    
    /**
    * Should be used to set the properties of some toolbar components 
    * before it is drawn.
    * @param aResourceId The resource ID for particular toolbar
    * @param aToolbar The toolbar object pointer
    */
    void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );
            
    /**
    * Handles toolbar events for a certain toolbar item.
    * @param aCommand The command ID of some toolbar item.
    */
    void OfferToolbarEventL( TInt aCommand );
    
private: // from MIAUpdateContainerObserver   
 
    /**
    * Mark list item
    * @param aMark True if marking, False if unmarking
    * @param aIndex  Index of the item
    * @return False value if marking/unmarking cancelled when dependencies/dependants not found
    */ 
    TBool MarkListItemL( TBool aMark, TInt aIndex );
  
    /**
    * Set text label to middle soft key
    * @param aVisible   True value when MSK text is visible
    * @param aSelected  True value when list item is selected( marked)
    */
    void SetMiddleSKTextL( TBool aVisible, TBool aSelected );
    
private: // constructors
     
    /** 
    * Constuctor 
    * @param aRect Rectangular of the view
    */
    void ConstructL(const TRect& aRect);

    /** 
    * C++ constuctor 
    */
    CIAUpdateMainView();

private: // new methods    
    /**
    * Get selected (marked) nodes in the list
    * @param aSelectedNodes Array of nodes 
    */  
    void GetSelectedNodesL( RPointerArray<MIAUpdateAnyNode>& aSelectedNodes ) const;
    
    /**
      * Get mandatory nodes in the list
      * @param aMandNodes Array of nodes 
      */  
    void GetMandatoryNodesL( RPointerArray<MIAUpdateAnyNode>& aMandNodes ) const;  
    /**
    * Pass selected indices to the container 
    */ 
    void SetSelectedIndicesL(); 
    
    /**
    * Get index of a node 
    *
    * @param aNode Node
    * @return Index of the node
    */  
    TInt NodeIndex( const MIAUpdateAnyNode& aNode ) const; 
    
    /**
    * Updates the status pane text
    */ 
    void UpdateStatusPaneL();
    
    /**
    * Updates selection information in navi pane 
    */ 
    void UpdateSelectionInfoInNaviPaneL();
    
    /**
    * Removes selection info in  navi pane
    */                                      
    void RemoveSelectionInfoInNaviPane();
    
    /**
    * Shows dependencies found dialog
    *
    * @param aText Text shown in a dialog
    * @return True value if marking/unmarking accepted 
    */ 
    TBool ShowDependenciesFoundDialogL( TDesC& aText ) const;
    
    /**
    * Count of selected (marked) items 
    *
    * @return Count of selected items 
    */                     
    TInt CountOfSelectedItems() const;
    
    void ShowUpdateCannotOmitDialogL() const;
    
private: // Data

    CAknNavigationControlContainer* iNaviPane; //not owned
    
    CAknNavigationDecorator* iDecorator;
        
    CIAUpdateMainContainer* iContainer;
    
    RPointerArray<MIAUpdateAnyNode> iAllNodes;
    
    TBool iShowStatusDialogAgain;
    
    TInt iRefreshError;
    
    TInt iLastCommandId;
    
    CIAUpdateFWUpdateHandler* iFwUpdateHandler;
    
    TBool iIsDMSupported;
    
    };


#endif // IAUPDATEMAINVIEW_H

// End of File
