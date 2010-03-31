/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of CIAUpdateMainView class 
*                member functions.
*
*/


 
#include <eikmenup.h>
#include <aknViewAppUi.h> 
#include <akntitle.h> 
#include <aknnavi.h> // CAknNavigationControlContainer
#include <aknnavide.h> // CAknNavigationDecorator
#include <AknUtils.h>
#include <aknmessagequerydialog.h>      // CAknMessageQueryDialog
#include <akntoolbar.h> 
#include <StringLoader.h>
#include <featmgr.h>  
#include <iaupdate.rsg>

#include "iaupdate.hrh"
#include "iaupdatemainview.h"
#include "iaupdatemaincontainer.h"
#include "iaupdatenode.h"
#include "iaupdatefwnode.h"
#include "iaupdatebasenode.h"
#include "iaupdatedialogutil.h"
#include "iaupdatestatusdialog.h"
#include "iaupdateappui.h"
#include "iaupdateuicontroller.h"
#include "iaupdatedeputils.h"
#include "iaupdatedebug.h"
#include "iaupdatefwupdatehandler.h"

//CONSTANTS
const TInt KKiloByte = 1024;
const TInt KMegaByte = 1024 * 1024;
const TInt KMaxShownInKiloBytes = 10 * KMegaByte;
const TInt KSelInfoArrayGranularity = 3;

//MACROS
_LIT( KSpace, " " );

// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CIAUpdateMainView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIAUpdateMainView* CIAUpdateMainView::NewL( const TRect& aRect )
    {
    CIAUpdateMainView* self = CIAUpdateMainView::NewLC( aRect );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::NewLC
// 
// -----------------------------------------------------------------------------
//
CIAUpdateMainView* CIAUpdateMainView::NewLC( const TRect& aRect )
    {
    CIAUpdateMainView* self = new (ELeave) CIAUpdateMainView();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::CIAUpdateMainView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIAUpdateMainView::CIAUpdateMainView()
    {
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::~CIAUpdateMainView
// Destructor
// -----------------------------------------------------------------------------
//
CIAUpdateMainView::~CIAUpdateMainView()
    {
    delete iDecorator;
    iAllNodes.Close();
    if ( iContainer )
        {
    	AppUi()->RemoveFromStack( iContainer );
    	delete iContainer;
        }
    delete iFwUpdateHandler;
    }
// -----------------------------------------------------------------------------
// CIAUpdateMainView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIAUpdateMainView::ConstructL( const TRect& /*aRect*/ )
    {
    BaseConstructL( R_IAUPDATE_MAIN_VIEW );
    Toolbar()->SetItemDimmed( EIAUpdateCmdStartUpdate, ETrue, ETrue );
    Toolbar()->SetItemDimmed( EIAUpdateCmdMarkedUpdateDetails, ETrue, ETrue );
    Toolbar()->SetItemDimmed( EIAUpdateCmdUpdateHistory, ETrue, ETrue );
    Toolbar()->SetToolbarObserver( this ); 
    iFwUpdateHandler = CIAUpdateFWUpdateHandler::NewL();
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::RefreshL
// Refreshes update list
// -----------------------------------------------------------------------------
//    
void CIAUpdateMainView::RefreshL( const RPointerArray<MIAUpdateNode>& aNodes,
                                  const RPointerArray<MIAUpdateFwNode>& aFwNodes,
                                  TInt aError  )
    {   
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateMainView::RefreshL begin");
    iRefreshError = aError;    
    iAllNodes.Reset();
        
    if ( aFwNodes.Count() > 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] CIAUpdateMainView::RefreshL hard code importance");
        //hardcode the importance of firmware as Critical
        for ( TInt i = 0; i < aFwNodes.Count(); i++ )
             {
             aFwNodes[i]->Base().SetImportance( MIAUpdateBaseNode::ECritical );
             }
         
        //either NSU or FOTA available
        if ( aFwNodes.Count() == 1 )
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateMainView::RefreshL either NSU or FOTA available");
            MIAUpdateAnyNode* node = aFwNodes[0];
            User::LeaveIfError( iAllNodes.Append( node ) );
            }
        
        //both NSU and FOTA available, show only FOTA node
        if ( aFwNodes.Count() == 2 )
            {
            IAUPDATE_TRACE("[IAUPDATE] CIAUpdateMainView::RefreshL both NSU and FOTA available");
            MIAUpdateAnyNode* node1 = aFwNodes[0];
            MIAUpdateFwNode* fwnode = static_cast<MIAUpdateFwNode*>( node1 );
            if ( fwnode->FwType() == MIAUpdateFwNode::EFotaDp2  )
                {
                User::LeaveIfError( iAllNodes.Append( node1 ) );
                }
            else
                {
                MIAUpdateAnyNode* node2 = aFwNodes[1];
                User::LeaveIfError( iAllNodes.Append( node2 ) );
                }
            }
        }
    
   
    for( TInt i = 0; i < aNodes.Count(); ++i ) 
        {
        MIAUpdateAnyNode* node = aNodes[i];
        User::LeaveIfError( iAllNodes.Append( node ) );
        }
        
    if( iContainer )
        {        
        iContainer->RefreshL( iAllNodes, iRefreshError );  
        SetSelectedIndicesL();     
        DynInitToolbarL( R_IAUPDATE_MAIN_TOOLBAR, Toolbar() );
        }    
     Toolbar()->SetItemDimmed( EIAUpdateCmdUpdateHistory, EFalse, ETrue );
     }
    

// -----------------------------------------------------------------------------
// CIAUpdateMainView::GetCurrentNode
// 
// -----------------------------------------------------------------------------
//    
MIAUpdateAnyNode* CIAUpdateMainView::GetCurrentNode()
   {
   if ( iContainer )
      {
   	  return iContainer->GetCurrentNode( iAllNodes ); 	 
      }
   return NULL;   
   }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::GetSelectedNode
// 
// -----------------------------------------------------------------------------
//    
MIAUpdateAnyNode* CIAUpdateMainView::GetSelectedNode()
   {
   MIAUpdateAnyNode* selectedNode = NULL;
   for( TInt i = 0; i < iAllNodes.Count() && !selectedNode ; ++i )
       {
       if ( iAllNodes[i]->Base().IsSelected() )
           {
           selectedNode = iAllNodes[i];
           }
       }
    return selectedNode;   
   }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::Id
// Get Uid of this view
// -----------------------------------------------------------------------------
//       
TUid CIAUpdateMainView::Id() const
    {
    return TUid::Uid( EIAUpdateMainViewId );
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::HandleCommandL
// 
// -----------------------------------------------------------------------------
// 
void CIAUpdateMainView::HandleCommandL( TInt aCommand )
    {
    switch( aCommand ) 
        {  
        case EIAUpdateCmdStartUpdate:
            {
            RPointerArray<MIAUpdateAnyNode> selectedNodes;
            CleanupClosePushL( selectedNodes );
            GetSelectedNodesL( selectedNodes ); 
            if ( selectedNodes[0]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                {
                //the marking logic will make sure firmware won't be marked with normal sis updates
                //at the same time.
                iFwUpdateHandler->FirmWareUpdatewithFOTA();
                }
            else
                {
                AppUi()->HandleCommandL( aCommand );
                }
            CleanupStack::PopAndDestroy( &selectedNodes );
            break;    
            }
        case EIAUpdateCmdUpdateWithPC:
            {
            iFwUpdateHandler->FirmWareUpdatewithNSU();
            break;
            }
        case EIAUpdateCmdSettings:
            {
            Toolbar()->SetToolbarVisibility( EFalse );
            AppUi()->HandleCommandL( aCommand );
            Toolbar()->SetToolbarVisibility( ETrue );
            break;
            }
        case EAknSoftkeyMark:
            {
            iContainer->HandleMarkCommandL( EAknCmdMark );
            break;	
            }
        case EAknSoftkeyUnmark:
            {
            iContainer->HandleMarkCommandL( EAknCmdUnmark );
            break;	
            }
        case EAknCmdMark:        
        case EAknCmdUnmark:
            {
            iContainer->HandleMarkCommandL( aCommand );
            break;	
            }
        default:
            { 
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::DynInitMenuPaneL
// Dim options which are not availble in empty list.
// Set Mark/Unmark option
// -----------------------------------------------------------------------------
//    
void CIAUpdateMainView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if( aResourceId == R_IAUPDATE_MAIN_MENU ) 
        {
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        RPointerArray<MIAUpdateAnyNode> selectedNodes;
        CleanupClosePushL( selectedNodes );
        GetSelectedNodesL( selectedNodes ); 
                    
        if ( selectedNodes.Count() )
            {
            if ( selectedNodes[0]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                {
                //firmware is selected
                MIAUpdateFwNode* node = static_cast<MIAUpdateFwNode*>( selectedNodes[0] );
            
                if ( node->FwType() == MIAUpdateFwNode::EFotaDp2 )
                    {
                    aMenuPane->SetItemDimmed( EIAUpdateCmdUpdateWithPC, ETrue );
                    }
                    
                if ( node->FwType() == MIAUpdateFwNode::EFotiNsu )
                    {
                    aMenuPane->SetItemDimmed( EIAUpdateCmdStartUpdate, ETrue );
                    }
                }
            else
                {
                //firmware is not selected, only sis
                aMenuPane->SetItemDimmed( EIAUpdateCmdUpdateWithPC, ETrue );
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( EIAUpdateCmdStartUpdate, ETrue );
            aMenuPane->SetItemDimmed( EIAUpdateCmdUpdateWithPC, ETrue );
            }
        CleanupStack::PopAndDestroy( &selectedNodes );
        }
    }
    
// -----------------------------------------------------------------------------
// CIAUpdateMainView::DoActivateL
// Activate view
// -----------------------------------------------------------------------------
// 
void  CIAUpdateMainView::DoActivateL( const TVwsViewId& /*aPrevViewId*/, 
                                      TUid /*aCustomMessageId*/, 
                                      const TDesC8& /*aCustomMessage*/ )
    {
    UpdateStatusPaneL();
    if( !iContainer )
        {    
        iContainer = CIAUpdateMainContainer::NewL( ClientRect(), *this );
   
        if( iAllNodes.Count() > 0 )
            {            
            iContainer->RefreshL( iAllNodes, iRefreshError );
            }
  
        SetSelectedIndicesL();  
        DynInitToolbarL( R_IAUPDATE_MAIN_TOOLBAR, Toolbar() );
        AppUi()->AddToStackL( iContainer ); 
        }
    else
        {
        UpdateSelectionInfoInNaviPaneL();	
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::DoDeactivate()
// Container is deleted in deactivation
// -----------------------------------------------------------------------------
//
void CIAUpdateMainView::DoDeactivate()
    {
    RemoveSelectionInfoInNaviPane();
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::DynInitToolbarL()
//
// -----------------------------------------------------------------------------
//
void CIAUpdateMainView::DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar )
    {
    if ( aResourceId == R_IAUPDATE_MAIN_TOOLBAR )
        {
        if ( iContainer )
            {
            if ( CountOfSelectedItems() > 0 )
                {
                aToolbar->SetItemDimmed( EIAUpdateCmdStartUpdate, EFalse, ETrue );
                }
            else
                {
                aToolbar->SetItemDimmed( EIAUpdateCmdStartUpdate, ETrue, ETrue );
                }
            if ( CountOfSelectedItems() == 1 )
                {
                aToolbar->SetItemDimmed( EIAUpdateCmdMarkedUpdateDetails, EFalse, ETrue );
                }
            else
                {
                aToolbar->SetItemDimmed( EIAUpdateCmdMarkedUpdateDetails, ETrue, ETrue );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::OfferToolbarEventL()
//
// -----------------------------------------------------------------------------
//            
void CIAUpdateMainView::OfferToolbarEventL( TInt aCommand )
    {
    if ( aCommand == EIAUpdateCmdStartUpdate )
        {
        RPointerArray<MIAUpdateAnyNode> selectedNodes;
        CleanupClosePushL( selectedNodes );
        GetSelectedNodesL( selectedNodes ); 
        
        if ( selectedNodes.Count() )
            {
            if ( selectedNodes[0]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                {
                //firmware is selected
                MIAUpdateFwNode* node = static_cast<MIAUpdateFwNode*>( selectedNodes[0] );
            
                if ( node->FwType() == MIAUpdateFwNode::EFotaDp2 )
                    {
                    aCommand = EIAUpdateCmdStartUpdate;
                    }
                    
                if ( node->FwType() == MIAUpdateFwNode::EFotiNsu )
                    {
                    aCommand = EIAUpdateCmdUpdateWithPC;
                    }
                }
            else
                {
                //firmware is not selected, only sis
                aCommand = EIAUpdateCmdStartUpdate;
                }
            }
        else
            {
             MIAUpdateAnyNode* currentNode = GetCurrentNode(); 
             if ( currentNode->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                 {
                 MIAUpdateFwNode* node = static_cast<MIAUpdateFwNode*>( currentNode );
                 if ( node->FwType() == MIAUpdateFwNode::EFotiNsu )
                     {
                     aCommand = EIAUpdateCmdUpdateWithPC;
                     }
                 } 
            }

        CleanupStack::PopAndDestroy( &selectedNodes );   
        }
    HandleCommandL( aCommand );
    }



// -----------------------------------------------------------------------------
// CIAUpdateMainView::MarkListItemL
// Mark list item
// -----------------------------------------------------------------------------
//    
TBool CIAUpdateMainView::MarkListItemL( TBool aMark, TInt aIndex )
    {
    if ( aMark )
        {
        //It's Mark Command
        RPointerArray<MIAUpdateAnyNode> selectedNodes;
        CleanupClosePushL( selectedNodes );
        GetSelectedNodesL( selectedNodes ); 
        
        //There are selected items already and type are different with the current one
        if ( selectedNodes.Count() > 0 && (iAllNodes[aIndex]->NodeType() != selectedNodes[0]->NodeType() ) )
            {       
            /*HBufC* msgText = StringLoader::LoadLC(R_IAUPDATE_FIRMWARE_WITH_OTHERS);

            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
       
            dlg->ExecuteLD( R_IAUPDATE_INFORMATION_QUERY, *msgText );
            
            CleanupStack::PopAndDestroy( msgText );*/
            
            //clean all of the current selection(s)
            iContainer->CleanAllSelection();
            
            // firmware item and normal sis items can't be selected at the same time
            // unmark the selected nodes.
            for ( TInt i = 0; i < selectedNodes.Count(); i++ )
                {                
                TInt index = iAllNodes.Find( selectedNodes[i]);
                iAllNodes[index]->Base().SetSelected( EFalse );
                }
            }
        
        CleanupStack::PopAndDestroy( &selectedNodes );
        }
    
    //there is no selected items or the type is the same with the current one
    
    if ( iAllNodes[aIndex]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
        {
        iAllNodes[aIndex]->Base().SetSelected( aMark );       
        
        if ( aMark )
            {
            SetMiddleSKTextL( ETrue, ETrue );
            }
        else
            {
            SetMiddleSKTextL( ETrue, EFalse );  
            }  
        UpdateSelectionInfoInNaviPaneL();
        DynInitToolbarL( R_IAUPDATE_MAIN_TOOLBAR, Toolbar() );
        return ETrue;
        }
    
    if ( iAllNodes[aIndex]->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
        {
        TBool accepted = EFalse;
        
        if( aIndex > -1 )
            {
            accepted = ETrue;
            
            MIAUpdateNode* node = static_cast<MIAUpdateNode*>( iAllNodes[aIndex] );
            RPointerArray<MIAUpdateAnyNode> mands;
            RPointerArray<MIAUpdateNode> deps;
           
            if ( aMark )
                {
                CleanupClosePushL( mands );
                CleanupClosePushL( deps );
                IAUpdateDepUtils::GetDependenciesL( *node, iAllNodes, deps );
                GetMandatoryNodesL( mands );
                }
            else
                {
                //mandatory item is not allowed to be unmarked
                if ( iAllNodes[aIndex]->Base().Importance() == MIAUpdateBaseNode::EMandatory )
                    {
                    //show dialog  
                    ShowUpdateCannotOmitDialogL();
                    return EFalse;
                    }
                
                CleanupClosePushL( deps );
                IAUpdateDepUtils::GetDependantsL( *node, iAllNodes, deps );  
                // item is not allowed to be unmarked if its dependant is mandatory
                TBool mandatoryDependantFound = EFalse;
                for( TInt i = 0; i < deps.Count() && !mandatoryDependantFound; i++ )
                     {
                     if ( deps[i]->Base().Importance() == MIAUpdateBaseNode::EMandatory )
                         {
                         mandatoryDependantFound = ETrue;
                         }
                     }
                if ( mandatoryDependantFound )
                    {
                    ShowUpdateCannotOmitDialogL();
                    CleanupStack::PopAndDestroy( &deps );
                    return EFalse;
                    }
                }
             
            TInt depCount = deps.Count();
           
            if ( depCount > 0 )
                {
                HBufC* text = NULL;
                HBufC* names = NULL;
                MIAUpdateNode* depNode = NULL;
                HBufC* separator = NULL;
                
                if ( depCount > 1 )
                    {                                 
                    separator = StringLoader::LoadLC( R_IAUPDATE_SEPARATOR );
                    }
                
                names = HBufC::NewLC( 0 );
                       
                for( TInt i = 0; i < depCount; i++ )  
                    {
                    depNode = deps[i];
                    HBufC* temp = NULL;
                    if ( separator )
                        {
                        temp = names->ReAllocL( names->Length() + 
                                                depNode->Base().Name().Length() +
                                                separator->Length() +
                                                KSpace.iTypeLength );
                        }
                    else
                        {
                        temp = names->ReAllocL( names->Length() + 
                                                depNode->Base().Name().Length() +
                                                KSpace.iTypeLength );
                        }
                   
                    CleanupStack::Pop( names );
                    names = temp; 
                    CleanupStack::PushL( names ); 
                    
                    if (  i > 0 )
                        {
                        names->Des() += *separator;
                        names->Des() += KSpace; 
                        }
                    names->Des() += depNode->Base().Name();
                    }
                
                TInt resourceId = 0;    
                if ( aMark )
                    {
                    if ( depCount > 1 ) 
                        {
                        resourceId = R_IAUPDATE_DEPENDENCY_MARK_MANY;
                        } 
                    else
                        {
                        resourceId = R_IAUPDATE_DEPENDENCY_MARK_ONE;    
                        }
                    }
                else
                    {
                    if ( depCount > 1 ) 
                        {
                        resourceId = R_IAUPDATE_DEPENDENCY_UNMARK_MANY;
                        } 
                    else
                        {
                        resourceId = R_IAUPDATE_DEPENDENCY_UNMARK_ONE;  
                        }   
                    }
                text = StringLoader::LoadLC( resourceId, *names );
                
                if ( aMark && iAllNodes[aIndex]->Base().Importance() == MIAUpdateBaseNode::EMandatory )
                    {
                    // depencencies of mandatory update are also selected without showing dialog
                    accepted = ETrue;
                    }
                else
                    {
                    accepted = ShowDependenciesFoundDialogL( *text );
                    }
                CleanupStack::PopAndDestroy( text );
                CleanupStack::PopAndDestroy( names );
                
                if ( separator )
                    {
                    CleanupStack::PopAndDestroy( separator );
                    }
                }
            
            if ( accepted )
                {
                for( TInt j = 0; j < depCount; j++ )
                    {
                    TInt depNodeInd = iAllNodes.Find( deps[j] );
                    iAllNodes[depNodeInd]->Base().SetSelected( aMark );
                    }
                CleanupStack::PopAndDestroy( &deps );
                
                TInt nodeInd = iAllNodes.Find( node );
                iAllNodes[nodeInd]->Base().SetSelected( aMark );
                }
            else
                {
                //user rejects the dependency dialog
                CleanupStack::PopAndDestroy( &deps );
                if ( aMark )
                    {
                    CleanupStack::PopAndDestroy( &mands );
                    }
                return EFalse;
                }
                
            //mark all of the mandatory items
            TInt mandCount = mands.Count();
            if ( mandCount > 0 && aMark )
                {
                for( TInt j = 0; j < mandCount; j++ )
                    {
                    TInt mandNodeInd = iAllNodes.Find( mands[j] );
                    iAllNodes[mandNodeInd]->Base().SetSelected( aMark );
                    if ( iAllNodes[mandNodeInd]->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
                        {
                        // mark also all dependencies of a mandatory item
                        MIAUpdateNode* dependencyNode = static_cast<MIAUpdateNode*>( iAllNodes[mandNodeInd] );
                        RPointerArray<MIAUpdateNode> dependencies;
                        CleanupClosePushL( dependencies );
                        IAUpdateDepUtils::GetDependenciesL( *dependencyNode, iAllNodes, dependencies );
                        for( TInt k = 0; k < dependencies.Count(); k++ )
                            {
                            TInt depNodeInd = iAllNodes.Find( dependencies[k] );
                            iAllNodes[depNodeInd]->Base().SetSelected( ETrue );
                            }
                        CleanupStack::PopAndDestroy( &dependencies );
                        }
                    }
                }
            if ( aMark )
                {
                CleanupStack::PopAndDestroy( &mands );
                }
            
            if ( depCount > 0 || mandCount > 0 )
                {
                SetSelectedIndicesL();
                }
            else
                {
                UpdateSelectionInfoInNaviPaneL();   
                }
               
            if ( aMark )
                {
                SetMiddleSKTextL( ETrue, ETrue );
                }
            else
                {
                SetMiddleSKTextL( ETrue, EFalse );  
                }        
            }  
        DynInitToolbarL( R_IAUPDATE_MAIN_TOOLBAR, Toolbar() );
        return ETrue;
        }
    return EFalse;
    }
    
    
   
    
// -----------------------------------------------------------------------------
// CIAUpdateMainView::SetMiddleSKTextL
// Middle soft key label to be set 
// -----------------------------------------------------------------------------
//    
void CIAUpdateMainView::SetMiddleSKTextL( TBool aVisible, TBool aSelected )
    {
	CEikButtonGroupContainer* cbaGroup = Cba();
	if ( cbaGroup )
        {
        cbaGroup->RemoveCommandFromStack( CEikButtonGroupContainer::EMiddleSoftkeyPosition, iLastCommandId );
        if ( aVisible )
            {
        	HBufC* middleSKText = NULL;
    	    if ( aSelected )
        	    {
        	    middleSKText = StringLoader::LoadLC( R_IAUPDATE_MSK_UNMARK );
                TPtr mskPtr = middleSKText->Des();
    	        cbaGroup->AddCommandToStackL(  
    	                    CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
    	                    EAknSoftkeyUnmark, 
    	                    mskPtr );
    	        iLastCommandId = EAknSoftkeyUnmark;            
                }
            else
                {
                middleSKText = StringLoader::LoadLC( R_IAUPDATE_MSK_MARK );
        	    TPtr mskPtr = middleSKText->Des();
    	        cbaGroup->AddCommandToStackL( 
    	                  CEikButtonGroupContainer::EMiddleSoftkeyPosition, 
    	                  EAknSoftkeyMark, 
    	                  mskPtr );
    	        iLastCommandId = EAknSoftkeyMark;
        	    }
        	CleanupStack::PopAndDestroy( middleSKText );
            }
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::GetSelectedNodesL
// Get selected (marked) nodes
// -----------------------------------------------------------------------------
//
void CIAUpdateMainView::GetSelectedNodesL( RPointerArray<MIAUpdateAnyNode>& aSelectedNodes ) const
    {
    for( TInt i = 0; i < iAllNodes.Count(); ++i )
        {
        if ( iAllNodes[i]->Base().IsSelected() )
            {
        	User::LeaveIfError( aSelectedNodes.Append( iAllNodes[i] ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::GetMandatoryItemsL
// Get selected (marked) nodes
// -----------------------------------------------------------------------------
//
void CIAUpdateMainView::GetMandatoryNodesL( RPointerArray<MIAUpdateAnyNode>& aMandNodes ) const
    {
    for( TInt i = 0; i < iAllNodes.Count(); ++i )
        {
        if ( iAllNodes[i]->Base().Importance() == MIAUpdateBaseNode::EMandatory )
            {
            User::LeaveIfError( aMandNodes.Append( iAllNodes[i] ) );
            }
        }
    }



// -----------------------------------------------------------------------------
// CIAUpdateMainView::SetSelectedIndicesL
// Indices of selected items are passed to container 
// -----------------------------------------------------------------------------
//  
void CIAUpdateMainView::SetSelectedIndicesL()
    {
    RArray<TInt> indices;
        
    CleanupClosePushL( indices );

    for( TInt i = 0; i < iAllNodes.Count(); ++i )
        {
    	if ( iAllNodes[i]->Base().IsSelected() )
    	    {
    		User::LeaveIfError( indices.Append( i ) );
    	    }
        }
        
    iContainer->SetSelectedIndicesL( indices );
    UpdateSelectionInfoInNaviPaneL();
 
    CleanupStack::PopAndDestroy( &indices );
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::NodeIndex
// Get index of node 
// -----------------------------------------------------------------------------
//  
TInt CIAUpdateMainView::NodeIndex( const MIAUpdateAnyNode& aNode ) const
    {
    TInt result = -1;
    
    for( TInt i = 0; i < iAllNodes.Count(); ++i )
        {
        if( &aNode == iAllNodes[i] )
            {
            result = i;
            break;
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::UpdateStatusPaneL
// Update title in status pane
// -----------------------------------------------------------------------------
// 
    
void CIAUpdateMainView::UpdateStatusPaneL()
    {
    CAknTitlePane* titlePane = 
            static_cast< CAknTitlePane* >( StatusPane()->ControlL(
                                           TUid::Uid( EEikStatusPaneUidTitle ) ) );
    HBufC* text = StringLoader::LoadLC( R_IAUPDATE_TEXT_TITLE_MAIN_VIEW );
    titlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy( text );        
    }


// -----------------------------------------------------------------------------
// CIAUpdateMainView::UpdateSelectionInfoInNaviPaneL
// Update selection information in status pane
// -----------------------------------------------------------------------------
// 
//
void CIAUpdateMainView::UpdateSelectionInfoInNaviPaneL()
    {
    RemoveSelectionInfoInNaviPane();
    
    if ( iAllNodes.Count() > 0 )
        {
        //show empty navi pane when firmware is selected but no size info or it is NSU
        if ( (iAllNodes[0]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw) && iAllNodes[0]->Base().IsSelected() )
            {
            MIAUpdateFwNode* fwnode = static_cast<MIAUpdateFwNode*>( iAllNodes[0] );
            if ( (iAllNodes[0]->Base().ContentSizeL() == 0) || (fwnode->FwType() == MIAUpdateFwNode::EFotiNsu)  )
                {
                return;
                }
            }
        
        if ( !iNaviPane )
            {
    	    iNaviPane = static_cast<CAknNavigationControlContainer*>
               ( StatusPane()->ControlL( TUid::Uid(EEikStatusPaneUidNavi ) ) );
            }
        TInt selectedSizeInBytes = 0;
        for( TInt i = 0; i < iAllNodes.Count(); ++i )
            {
            if ( iAllNodes[i]->Base().IsSelected() )
                {
            	selectedSizeInBytes += iAllNodes[i]->Base().ContentSizeL();
                }
            }
                
        
        TInt resourceId = 0;
        TInt selectedSize;
        if ( selectedSizeInBytes >= KMaxShownInKiloBytes )
            {
    	    resourceId = R_IAUPDATE_NAVIPANE_MEGABYTE;
    	    selectedSize = selectedSizeInBytes / KMegaByte;
    	    if ( selectedSizeInBytes % KMegaByte != 0 )
    	        {
    	    	selectedSize++;
    	        }
            }
        else 
            {
    	    resourceId = R_IAUPDATE_NAVIPANE_KILOBYTE;
    	    selectedSize = selectedSizeInBytes / KKiloByte;
    	    if ( selectedSizeInBytes % KKiloByte != 0 )
    	        {
    	    	selectedSize++;
    	        }
            }
     
        CArrayFix<TInt>* numberArray = 
                 new ( ELeave ) CArrayFixFlat<TInt>( KSelInfoArrayGranularity );
        CleanupStack::PushL( numberArray );
        numberArray->AppendL( CountOfSelectedItems() ); 
        numberArray->AppendL( iAllNodes.Count() );
        numberArray->AppendL( selectedSize );
    
        HBufC* selectionInfo = StringLoader::LoadLC( resourceId, 
                                                     *numberArray );
        TPtr ptr = selectionInfo->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr ); 
            
        iDecorator = iNaviPane->CreateNavigationLabelL( *selectionInfo );

        iNaviPane->PushL( *iDecorator );
        
        CleanupStack::PopAndDestroy( selectionInfo ); 
        CleanupStack::PopAndDestroy( numberArray );	
        }
    }

// -----------------------------------------------------------------------------
// CIAUpdateMainView::RemoveSelectionInfoInNaviPane
// Remove existing selection info (created by main view) from navi pane
// -----------------------------------------------------------------------------
// 

void CIAUpdateMainView::RemoveSelectionInfoInNaviPane()
    {
    if ( iNaviPane && iDecorator )
        {
    	iNaviPane->Pop( iDecorator );
    	delete iDecorator;
    	iDecorator = NULL;
        }
    }
 
// -----------------------------------------------------------------------------
// CIAUpdateMainView::ShowDependenciesFoundDialogL
// 
// -----------------------------------------------------------------------------
//  
TBool CIAUpdateMainView::ShowDependenciesFoundDialogL( TDesC& aText ) const
    {
	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( aText );
    dlg->PrepareLC( R_IAUPDATE_MESSAGE_QUERY );
    CAknPopupHeadingPane* headingPane = dlg->Heading();
    HBufC* title = StringLoader::LoadLC( R_IAUPDATE_DEPENDENCY_TITLE );
    headingPane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
  
    dlg->ButtonGroupContainer().SetCommandSetL( 
                                  R_IAUPDATE_SOFTKEYS_CONTINUE_CANCEL__CONTINUE );

    return( dlg->RunLD() == EAknSoftkeyOk );
    }
    
// ---------------------------------------------------------------------------
// CIAUpdateMainView::CountOfSelectedItems
// 
// ---------------------------------------------------------------------------
//     
TInt CIAUpdateMainView::CountOfSelectedItems() const
    {
    TInt countOfSelectedItems = 0; 
    for( TInt i = 0; i < iAllNodes.Count(); ++i )
        {
        if ( iAllNodes[i]->Base().IsSelected() )
            {
        	countOfSelectedItems++;
            }
        }
    return countOfSelectedItems;   	
    }

// ---------------------------------------------------------------------------
// CIAUpdateMainView::ShowUpdateCannotOmitDialogL
// 
// ---------------------------------------------------------------------------
//
void CIAUpdateMainView::ShowUpdateCannotOmitDialogL() const
    {
    HBufC* msgText = StringLoader::LoadLC( R_IAUPDATE_CANNOT_OMIT );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    dlg->ExecuteLD( R_IAUPDATE_INFORMATION_QUERY, *msgText  );         
    CleanupStack::PopAndDestroy( msgText );
    }
    
// End of File
    
