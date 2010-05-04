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

#include "iaupdatemainview.h"
#include "iaupdateagreement.h"

#include "iaupdatedebug.h"

const int KKiloByte = 1024;
const int KMegaByte = 1024 * 1024;
const int KMaxShownInKiloBytes = 10 * KMegaByte;


IAUpdateMainView::IAUpdateMainView()
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
void IAUpdateMainView::refresh( const RPointerArray<MIAUpdateNode>& nodes,
                                const RPointerArray<MIAUpdateFwNode>& fwNodes,
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
 
            if ( node-> Base().IsSelected() )
                {
                int count = mListView->indexCount();
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

void IAUpdateMainView::handleSelectionChanged(QItemSelection)
{

}



