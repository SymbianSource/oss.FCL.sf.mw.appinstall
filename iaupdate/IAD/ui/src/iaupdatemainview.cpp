/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module contains the implementation of IAUpdateMainView
*                class member functions.
*
*/

#include <hbaction.h>
#include <qgraphicswidget>
//#include <hblistview.h>
#include <hblistwidget.h>
#include <hblistviewitem.h>
#include <hblistwidgetitem.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbdocumentloader.h>
#include <xqconversions.h>
#include <hbmessagebox.h>

#include "iaupdatemainview.h"
#include "iaupdateengine.h"
#include "iaupdateagreement.h"
#include "iaupdatedeputils.h"

#include "iaupdatedebug.h"

const int KKiloByte = 1024;
const int KMegaByte = 1024 * 1024;
const int KMaxShownInKiloBytes = 10 * KMegaByte;


IAUpdateMainView::IAUpdateMainView(IAUpdateEngine *engine):
mEngine( engine )        
{
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/iaupdate_mainview.docml", &ok);
        
    // Exit if the file format is invalid
    Q_ASSERT_X(ok, "Software update", "Invalid docml file");
    // Create the view from DocML
    
    //QGraphicsWidget* widget = loader.findWidget("view");
    HbView* widget = qobject_cast<HbView*>(loader.findWidget("view"));
    Q_ASSERT_X(widget != 0, "Software update", "View not found");
    QString viewTitle(widget->title());
    // Set the IAUpdateMainView view to be the widget that was loaded from the xml
    setWidget(widget);
    setTitle(viewTitle);
      
    HbToolBar *toolBar = qobject_cast< HbToolBar*>( loader.findWidget("viewToolbar") );
    setToolBar(toolBar);
    
    HbMenu *menu = qobject_cast< HbMenu*>( loader.findWidget("viewMenu") );
    setMenu(menu);
    
    //Get the Action objects from the docml file
    HbAction *action = qobject_cast<HbAction*>(loader.findObject("action"));
    HbAction *action_1 = qobject_cast<HbAction*>(loader.findObject("action_1"));
    HbAction *action_2 = qobject_cast<HbAction*>(loader.findObject("action_2"));
    
    connect(action, SIGNAL(triggered()), this, SLOT(handleStartUpdate()));
    connect(action_1, SIGNAL(triggered()), this, SLOT(handleSettings()));
    connect(action_2, SIGNAL(triggered()), this, SLOT(handleDisclaimer()));

    //mListView = qobject_cast<HbListView*>( loader.findWidget("listView") ); 
    mListView = qobject_cast<HbListWidget*>( loader.findWidget("listWidget") );
    //mListView = loader.findWidget("listWidget");
    mListView->setSelectionMode( HbAbstractItemView::MultiSelection );
    
    HbListViewItem *prototype = mListView->listItemPrototype();

    prototype->setGraphicsSize(HbListViewItem::LargeIcon);
    //prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
    //prototype->setSecondaryTextRowCount(1,1);

    
        
    /*HbListWidgetItem* item = new HbListWidgetItem();
    QStringList data;
    data << "Test application" << "Critical";
    item->setData(QVariant(data), Qt::DisplayRole);
    mListWidget->addItem(item);*/
}
    
IAUpdateMainView::~IAUpdateMainView()
{
}

// -----------------------------------------------------------------------------
// IAUpdateMainView::refresh
// Refreshes update list
// -----------------------------------------------------------------------------
//    
void IAUpdateMainView::refresh( const RPointerArray<MIAUpdateNode> &nodes,
                                const RPointerArray<MIAUpdateFwNode> &fwNodes,
                                int /*error*/ )
    {   
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh begin");
       
    mAllNodes.Reset();
        
    if ( fwNodes.Count() > 0 )
        {
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh hard code importance");
        //hardcode the importance of firmware as Critical
        for ( TInt i = 0; i < fwNodes.Count(); i++ )
             {
             fwNodes[i]->Base().SetImportance( MIAUpdateBaseNode::ECritical );
             }
         
        //either NSU or FOTA available
        if ( fwNodes.Count() == 1 )
            {
            IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh either NSU or FOTA available");
            MIAUpdateAnyNode* node = fwNodes[0];
            mAllNodes.Append( node );
            }
        
        //both NSU and FOTA available, show only FOTA node
        if ( fwNodes.Count() == 2 )
            {
            IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh both NSU and FOTA available");
            MIAUpdateAnyNode* node1 = fwNodes[0];
            MIAUpdateFwNode* fwnode = static_cast<MIAUpdateFwNode*>( node1 );
            if ( fwnode->FwType() == MIAUpdateFwNode::EFotaDp2  )
                {
                mAllNodes.Append( node1 );
                }
            else
                {
                MIAUpdateAnyNode* node2 = fwNodes[1];
                mAllNodes.Append( node2 );
                }
            }
        }
    
   
    for( int i = 0; i < nodes.Count(); ++i ) 
        {
        MIAUpdateAnyNode* node = nodes[i];
        mAllNodes.Append( node );
        }
        
    mListView->clear();
    QItemSelectionModel *selectionModel = mListView->selectionModel();  
    selectionModel->clear();
    HbIcon icon(QString(":/icons/qgn_menu_swupdate"));
    
    for( int j = 0; j < mAllNodes.Count(); ++j ) 
        {
        MIAUpdateAnyNode* node = mAllNodes[j];
        int sizeInBytes = node->Base().ContentSizeL();

        int size = 0;
        TBool shownInMegabytes = EFalse;        
        if ( sizeInBytes >= KMaxShownInKiloBytes )
            {
            shownInMegabytes = ETrue;
            size = sizeInBytes / KMegaByte;
            if ( sizeInBytes % KMegaByte != 0 )
                {
                size++;
                }
            }
        else 
            {
            size = sizeInBytes / KKiloByte;
            if ( sizeInBytes % KKiloByte != 0 )
                {
                size++;
                }  
            }
        QString sizeString;
        sizeString.setNum(size);        
        QString importanceDescription;
        switch( node->Base().Importance() )
            {        
            case MIAUpdateBaseNode::EMandatory:
                {
                importanceDescription = "Required ";
                importanceDescription.append(sizeString);
                if ( shownInMegabytes )
                    {
                    importanceDescription.append(" MB" );
                    }
                else 
                    {
                    importanceDescription.append(" kB" );
                    }  
                break;
                }
            
            
            case MIAUpdateBaseNode::ECritical:
                {
                bool isNSU = false;
                if( node->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
                    {
                    MIAUpdateFwNode* fwnode = static_cast<MIAUpdateFwNode*>( node );          
                    if ( fwnode->FwType() == MIAUpdateFwNode::EFotiNsu )
                        {
                        isNSU = true;
                        }
                    }
                
                importanceDescription = "Important ";
                if ( !size || isNSU )
                    {
                    //for firmware when size info is not provided by server
                    }
                else
                    {
                    importanceDescription.append(sizeString);
                    if ( shownInMegabytes )
                        {
                        importanceDescription.append(" MB" );
                        }
                    else 
                        {
                        importanceDescription.append(" kB" );
                        } 
                    }
     
                break;
                }
        
            case MIAUpdateBaseNode::ERecommended:
                {
                importanceDescription = "Recommended ";
                importanceDescription.append(sizeString);
                if ( shownInMegabytes )
                    {
                    importanceDescription.append(" MB" );
                    }
                else 
                    {
                    importanceDescription.append(" kB" );
                    }  
                break;
                }
        
            case MIAUpdateBaseNode::ENormal:
                {
                importanceDescription = "Optional ";
                importanceDescription.append(sizeString);
                if ( shownInMegabytes )
                    {
                     importanceDescription.append(" MB" );
                    }
                else 
                    {
                    importanceDescription.append(" kB" );
                    }  
                break;
                }

            default:
                {
                break;
                }
            }
            
            //AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );    
            
            HbListWidgetItem *item = new HbListWidgetItem();    
            QString name = XQConversions::s60DescToQString(node->Base().Name());
            if ( node->NodeType() == MIAUpdateAnyNode::ENodeTypeFw ) 
                {
                name.append(" DEVICE SOFTWARE");
                }
            item->setText(name);
            item->setSecondaryText(importanceDescription);
            item->setIcon(icon);
             mListView->addItem(item); 
 
            if ( node->Base().IsSelected() )
                {
                int count = mListView->count();
                QModelIndex modelIndex = mListView->model()->index(count-1,0);
                selectionModel->select(modelIndex, QItemSelectionModel::Select);
                }

               
            
        /*if ( nodes.Count() == 0 )
            {
            HBufC* emptyText = NULL;
            if ( aError )
                {
                if ( aError == KErrCancel || aError == KErrAbort )
                    {
                    emptyText = KNullDesC().AllocLC();
                    }
                else
                    {
                    emptyText = StringLoader::LoadLC( R_IAUPDATE_REFRESH_FAILED );  
                    }
                }
            else
                {
                emptyText = StringLoader::LoadLC( R_IAUPDATE_TEXT_NO_UPDATES ); 
                }
            
            iListBox->View()->SetListEmptyTextL( *emptyText );
            CleanupStack::PopAndDestroy( emptyText );
            iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                    CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );  
            }
        else
            {
            if ( iListBox->CurrentItemIndex() == KErrNotFound )
                {
                iListBox->SetCurrentItemIndex( aNodes.Count() - 1 );
                }
            }*/
            
        //iListBox->HandleItemAdditionL();
        }
     connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                    this, SLOT(handleSelectionChanged(QItemSelection)));     
    
     IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh end");
     }




void IAUpdateMainView::handleStartUpdate()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleStartUpdate() begin");
    bool firmwareUpdate = false; 
    RPointerArray<MIAUpdateAnyNode> selectedNodes;
    getSelectedNodes(selectedNodes);
    if (selectedNodes.Count() > 0)
        {
        if ( selectedNodes[0]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
            {
            //the marking logic will make sure firmware won't be marked with normal sis updates
            //at the same time.
            firmwareUpdate = true;
            }
        }
    selectedNodes.Close();
    //mEngine->StartUpdate(firmwareUpdate);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleStartUpdate() end");
}


void IAUpdateMainView::handleSettings()
{
}

void IAUpdateMainView::handleDisclaimer()
{
    CIAUpdateAgreement* agreement = CIAUpdateAgreement::NewLC();
    agreement->ShowAgreementL();
    CleanupStack::PopAndDestroy( agreement);
}

void IAUpdateMainView::handleSelectionChanged(QItemSelection itemSelection)
{
    QModelIndexList indexList = itemSelection.indexes(); 
    bool changedItemFound = false; 
    for (int i = 0; !changedItemFound && i < mAllNodes.Count(); ++i)
    {
        bool currentlySelected = false; 
        for (int j = 0; !currentlySelected && j < indexList.count(); ++j)
        {
            //QModelIndex modelIndex = indexList.at(j);
            //modelIndex.row();
            if (i == indexList.at(j).row())
            {
                currentlySelected = true;
            }
        }
        if (currentlySelected !=  mAllNodes[i]->Base().IsSelected())
        {
            changedItemFound = true;
            markListItem(currentlySelected,i);
            updateSelectionsToList();
        }
    }

}

void IAUpdateMainView::getSelectedNodes(RPointerArray<MIAUpdateAnyNode> &selectedNodes) const
{
    for (int i = 0; i < mAllNodes.Count(); ++i)
    {
        if (mAllNodes[i]->Base().IsSelected())
        {
            selectedNodes.Append(mAllNodes[i]);
        }
    }
}

void IAUpdateMainView::markListItem(bool mark, int index)
{
    if (mark)
    {
        //It's Mark Command
        RPointerArray<MIAUpdateAnyNode> selectedNodes;
        getSelectedNodes(selectedNodes); 
        
        //There are selected items already and type are different with the current one
        if (selectedNodes.Count() > 0 && (mAllNodes[index]->NodeType() != selectedNodes[0]->NodeType()))
        {       
            // firmware item and normal sis items can't be selected at the same time
            // unmark the selected nodes.
            for (int i = 0; i < selectedNodes.Count(); i++)
            {                
                int index = mAllNodes.Find(selectedNodes[i]);
                mAllNodes[index]->Base().SetSelected(false);
            }
        }
        selectedNodes.Close();
    }
    
    //there is no selected items or the type is the same with the current one
    
    if (mAllNodes[index]->NodeType() == MIAUpdateAnyNode::ENodeTypeFw )
    {
        mAllNodes[index]->Base().SetSelected(mark);       
        return;
    }
    
    if (mAllNodes[index]->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal )
    {
        bool accepted = false;
        
        if(index > -1)
        {
            accepted = true;
            
            MIAUpdateNode* node = static_cast<MIAUpdateNode*>(mAllNodes[index]);
            RPointerArray<MIAUpdateAnyNode> mands;
            RPointerArray<MIAUpdateNode> deps;
           
            if (mark)
            {
                TRAPD(err,IAUpdateDepUtils::GetDependenciesL(*node, mAllNodes, deps));
                if (err != KErrNone)
                {
                    deps.Close();
                    return;
                }
                if (!getMandatoryNodes(mands))
                {
                    // error when creating mandatory node list
                    mands.Close();
                    return;
                }
            }
            else
            {
                //mandatory item is not allowed to be unmarked
                if (mAllNodes[index]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
                {
                    //show dialog  
                    showUpdateCannotOmitDialog();
                    return;
                }
                
                //CleanupClosePushL( deps );
                TRAPD(err,IAUpdateDepUtils::GetDependantsL(*node, mAllNodes, deps));  
                if (err != KErrNone)
                {
                    deps.Close();
                    return;
                }
                // item is not allowed to be unmarked if its dependant is mandatory
                bool mandatoryDependantFound = false;
                for(int i = 0; i < deps.Count() && !mandatoryDependantFound; i++)
                {
                    if (deps[i]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
                    {
                        mandatoryDependantFound = true;
                    }
                }
                if (mandatoryDependantFound)
                {
                    showUpdateCannotOmitDialog();
                    deps.Close();
                    return;
                }
            }
             
            int depCount = deps.Count();
           
            if (depCount > 0)
            {
                QString text;
                QString names;
                MIAUpdateNode* depNode = NULL;
                QString separator(",");
                       
                for(int i = 0; i < depCount; i++)  
                {
                    depNode = deps[i];
                    if (i > 0)
                    {
                        names.append(separator);
                        names.append(QString(" "));
                    }
                    names.append(XQConversions::s60DescToQString(depNode->Base().Name()));
                }
                

                if (mark)
                {
                    if (depCount > 1) 
                    {
                        text.append("This update needs also updates "); 
                        text.append(names);
                        text.append(" for working");
                        //resourceId = R_IAUPDATE_DEPENDENCY_MARK_MANY;
                    } 
                    else
                    {
                        text.append("This update needs also \""); 
                        text.append(names);
                        text.append("\" for working");
                        //resourceId = R_IAUPDATE_DEPENDENCY_MARK_ONE;    
                    }
                }
                else
                {
                    if (depCount > 1) 
                    {
                        text.append("Updates "); 
                        text.append(names);
                        text.append(" need this update for working");
                        //resourceId = R_IAUPDATE_DEPENDENCY_UNMARK_MANY;
                    } 
                    else
                    {
                        text.append("Update \""); 
                        text.append(names);
                        text.append("\" needs this update for working");
                        //resourceId = R_IAUPDATE_DEPENDENCY_UNMARK_ONE;  
                    }   
                }
                
                if (mark && mAllNodes[index]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
                {
                    // depencencies of mandatory update are also selected without showing dialog
                    accepted = true;
                }
                else
                {
                    accepted = showDependenciesFoundDialog(text);
                }
            }
            
            if (accepted)
            {
                for(int j = 0; j < depCount; j++)
                {
                    int depNodeInd = mAllNodes.Find(deps[j]);
                    mAllNodes[depNodeInd]->Base().SetSelected(mark);
                }
                deps.Close();
                int nodeInd = mAllNodes.Find(node);
                mAllNodes[nodeInd]->Base().SetSelected(mark);
            }
            else
            {
                //user rejects the dependency dialog
                deps.Close();
                if (mark)
                {
                    mands.Close();
                }
                return;
            }
                
            //mark all of the mandatory items
            int mandCount = mands.Count();
            if (mandCount > 0 && mark)
            {
                for(int j = 0; j < mandCount; j++)
                {
                    int mandNodeInd = mAllNodes.Find(mands[j]);
                    mAllNodes[mandNodeInd]->Base().SetSelected(mark);
                    if (mAllNodes[mandNodeInd]->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal)
                    {
                        // mark also all dependencies of a mandatory item
                        MIAUpdateNode* dependencyNode = static_cast<MIAUpdateNode*>(mAllNodes[mandNodeInd]);
                        RPointerArray<MIAUpdateNode> dependencies;
                        TRAPD(err,IAUpdateDepUtils::GetDependenciesL(*dependencyNode, mAllNodes, dependencies));
                        if (err)
                        {
                            dependencies.Close(); 
                            if (mark)
                            {
                                mands.Close();
                            }
                            return;
                        }
                        for(int k = 0; k < dependencies.Count(); k++)
                        {
                            int depNodeInd = mAllNodes.Find(dependencies[k]);
                            mAllNodes[depNodeInd]->Base().SetSelected(true);
                        }
                        dependencies.Close();
                    }
                }
            }
            if (mark)
            {
                mands.Close();
            }
        }  
        return;
        }
    return;
    }


bool IAUpdateMainView::getMandatoryNodes(RPointerArray<MIAUpdateAnyNode> &mandNodes) const
{
    bool ret = true;
    for(int i = 0; i < mAllNodes.Count(); ++i)
    {
        if (mAllNodes[i]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
        {
            if (mandNodes.Append(mAllNodes[i]) != KErrNone)
            {
                ret = false; 
            }
        }
    }
    return ret;
}

void IAUpdateMainView::showUpdateCannotOmitDialog() const
{
    HbMessageBox messageBox(HbMessageBox::MessageTypeInformation); 
    messageBox.setText(QString("This required update cannot be omitted"));
    HbAction okAction("Ok");
    messageBox.setPrimaryAction(&okAction);
    messageBox.setTimeout(HbPopup::StandardTimeout);
    messageBox.show();
    //messageBox.exec();
}

bool IAUpdateMainView::showDependenciesFoundDialog(QString &text) const
{
    bool accepted = false;
    HbMessageBox messageBox(HbMessageBox::MessageTypeQuestion);
    messageBox.setText(text);
    HbAction continueAction("Continue");
    messageBox.setPrimaryAction(&continueAction);
    HbAction cancelAction("Cancel");
    messageBox.setSecondaryAction(&cancelAction);
    messageBox.setTimeout(HbPopup::NoTimeout);
    messageBox.show();
    //HbAction *selectedAction = messageBox.exec();
    //if (selectedAction == messageBox.primaryAction())
    {
        accepted = true;
    }
    return accepted;
}

void IAUpdateMainView::updateSelectionsToList()
{
    for(int i = 0; i < mAllNodes.Count(); ++i)
    {
        if ( mAllNodes[i]->Base().IsSelected() != mListView->selectionModel()->isSelected(mListView->model()->index(i,0)))
        {    
            QItemSelectionModel::SelectionFlag selectionFlag;
            if ( mAllNodes[i]->Base().IsSelected())
            {
                selectionFlag = QItemSelectionModel::Select;
            }
            else
            {
                selectionFlag = QItemSelectionModel::Deselect;
            }
            mListView->selectionModel()->select(mListView->model()->index(i,0),selectionFlag);   
        }
    }
}


                
                
                
                
                
