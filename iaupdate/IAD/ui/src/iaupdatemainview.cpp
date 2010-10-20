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
#include <QGraphicsWidget>
#include <QGraphicsLayout>
#include <QGraphicsLayoutItem>
#include <QGraphicsLinearLayout>
#include <QSizeF>

#include <hbdockwidget.h>
#include <HbListView>
#include <hblistviewitem.h>
#include <hblistwidgetitem.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbdocumentloader.h>
#include <xqconversions.h>
#include <hbmessagebox.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <HbDataForm>
#include <HbLabel>
#include <hbtranslator.h>
#include <HbStyleLoader>
#include <HbIconItem>
#include <HbIconAnimationManager>
#include <HbIconAnimator>
#include <hbparameterlengthlimiter.h>

#include "iaupdatemainview.h"
#include "iaupdateengine.h"
#include "iaupdateagreement.h"
#include "iaupdatedeputils.h"
#include "iaupdatedialogutil.h"
#include "iaupdateversion.h"
#include "iaupdateapplistmodel.h"
#include "iaupdateapplistitem.h"
#include "iaupdateuicontroller.h"

#include "iaupdatedebug.h"



const int KKiloByte = 1024;
const int KMegaByte = 1024 * 1024;
const int KMaxShownInKiloBytes = 10 * KMegaByte;
const QString KTranslationsPath = "/resource/qt/translations/";
const QString KTranslationsFile = "swupdate";

const QString ANIMATION_ICON("qtg_anim_loading");
const QString ANIMATION_FILE("qtg_anim_loading.axml");


IAUpdateMainView::IAUpdateMainView(IAUpdateEngine *engine):
mEngine(engine)        
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::IAUpdateMainView() begin");
    mFwListView = NULL;
    mApplicationUpdatesGroupBox = NULL;;
    mFwNSUGroupBox = NULL;
    mContentDataForm = NULL;
    mDialogUtil = NULL;
    mDialogState = NoDialog;
    mPrimaryAction = NULL;
    mNode = NULL;
    mSelectionUpdate = false;
    mSelectionConnect = false;
    mTranslator = NULL;
    mAnimationIconItem = NULL;
    mLabel = NULL;
    mMark = false;
    mUpdating = false;
        
    mTranslator = new HbTranslator(KTranslationsPath, KTranslationsFile);
        
    HbDocumentLoader loader;
    bool ok = false;
    loader.load(":/xml/iaupdate_mainview.docml", &ok);
        
    // Exit if the file format is invalid
    Q_ASSERT_X(ok, "Software update", "Invalid docml file");
    // Create the view from DocML
    
    HbView* loadedWidget = qobject_cast<HbView*>(loader.findWidget("view"));
    Q_ASSERT_X(loadedWidget != 0, "Software update", "View not found");
    QString viewTitle(loadedWidget->title());
    // Set the IAUpdateMainView view to be the widget that was loaded from the xml
    setWidget(loadedWidget);
    setTitle(viewTitle);
      
    HbToolBar *toolBar = qobject_cast< HbToolBar*>( loader.findWidget("viewToolbar") );
    //setToolBar(toolBar);
    
    HbMenu *menu = qobject_cast< HbMenu*>( loader.findWidget("viewMenu") );
    setMenu(menu);
    
    //Get the Action objects from the docml file
    mActionStartUpdate = qobject_cast<HbAction*>(loader.findObject("action"));
    mActionSettings = qobject_cast<HbAction*>(loader.findObject("action_1"));
    mActionDisclaimer = qobject_cast<HbAction*>(loader.findObject("action_2"));
    mActionCancelRefresh = qobject_cast<HbAction*>(loader.findObject("action_3"));
    mActionCancelUpdate = qobject_cast<HbAction*>(loader.findObject("action_4"));
    
    
    connect(mActionStartUpdate, SIGNAL(triggered()), this, SLOT(handleStartUpdate()));
    connect(mActionSettings, SIGNAL(triggered()), this, SLOT(handleSettings()));
    connect(mActionDisclaimer, SIGNAL(triggered()), this, SLOT(handleDisclaimer()));
    connect(mActionCancelRefresh, SIGNAL(triggered()), this, SLOT(handleCancelRefresh()));
    connect(mActionCancelUpdate, SIGNAL(triggered()), this, SLOT(handleCancelUpdate()));
    
    mActionStartUpdate->setDisabled(true);
    
    mActionCancelRefresh->setVisible(false);
    mActionCancelUpdate->setVisible(false);
        
    mContent = qobject_cast< HbWidget*>( loader.findWidget("content") );
           
    //mListView = qobject_cast<HbListView*>( loader.findWidget("listWidget") );
    mListView = new HbListView(mContent);
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    
    //mListView->setLayoutName("iaupdate_progress");
    HbStyleLoader::registerFilePath(":/layout/");
    
    connect( mListView, SIGNAL( longPressed( HbAbstractViewItem *, const QPointF & ) ),
            this, SLOT( handleDetails( HbAbstractViewItem *, const QPointF & ) ) ); 
    
    IAUpdateAppListModel *model = new IAUpdateAppListModel();
    
    //HbListViewItem *prototype = mListView->listItemPrototype();
    
    HbListViewItem *prototype = new IAUpdateAppListItem(mListView);
    prototype->setGraphicsSize(HbListViewItem::LargeIcon);
    prototype->setStretchingStyle(HbListViewItem::NoStretching); 
            
    mListView->setModel(model, prototype);
    
    HbDockWidget *dock = new HbDockWidget(this);
    HbWidget *dockContainer = new HbWidget(dock);
    QGraphicsLinearLayout *dockLayout = new QGraphicsLinearLayout(dockContainer);
    dockLayout->setOrientation(Qt::Vertical);
    dockContainer->setLayout(dockLayout);
    
    mSelections = new HbGroupBox(dockContainer);

    QString selectedString = 
        QString(hbTrId("txt_software_subhead_selected_1l_2l_3l_kb")).arg(0).arg(0).arg(0);
    mSelections->setHeading(selectedString); 
    
    dockLayout->addItem(mSelections);
    
    dockLayout->addItem(toolBar);
    
    dock->setWidget(dockContainer);
    
    setDockWidget(dock);


    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::IAUpdateMainView() end");
}
    
IAUpdateMainView::~IAUpdateMainView()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::~IAUpdateMainView() begin");
    delete mTranslator;
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::~IAUpdateMainView() end");
}

// -----------------------------------------------------------------------------
// IAUpdateMainView::refresh
// Refreshes update list
// -----------------------------------------------------------------------------
//    
void IAUpdateMainView::refresh(const RPointerArray<MIAUpdateNode> &nodes,
                               const RPointerArray<MIAUpdateFwNode> &fwNodes,
                               int error)
{   
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh() begin");
    removeCurrentContentLayout();
        
    refreshFirmwareUpdates(fwNodes);
    
    refreshApplicationUpdates(nodes);
    
    if (nodes.Count() == 0 && fwNodes.Count() == 0 && 
        error != KErrCancel && error != KErrAbort)
    {    
        if (!mContentDataForm)
        {
            mContentDataForm  = new HbDataForm(mContent); 
        }
        else
        {
            mContentDataForm->setVisible(true);
        }
        QGraphicsLinearLayout *linearLayout = (QGraphicsLinearLayout*) mContent->layout();
        linearLayout->addItem(mContentDataForm);
        QString formText;
        
        if (error == KErrNone)
        {
             formText = 
                hbTrId("txt_software_formlabel_applications_are_up_to_date");
        }
        else
        {
            formText = 
                hbTrId("txt_software_formlabel_refreshing_failed_try_agai");
        }
        mContentDataForm->setDescription(formText);
    }
    updateSelectionInfoInDock(); 
    
    if (!mUpdating)
    {    
        mActionStartUpdate->setEnabled(countOfSelections() > 0);
    } 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refresh() end");
}

void IAUpdateMainView::refreshProgress()
{ 
    IAUpdateAppListModel *model = NULL;
    model = (IAUpdateAppListModel*)  mListView->model();
    if (model)
    {
        model->refreshProgress();    
    }
}

void IAUpdateMainView::startRefreshingAnimation()
{
    removeCurrentContentLayout();
     
    mActionDisclaimer->setVisible(false);
    mActionSettings->setVisible(false);
    mActionCancelRefresh->setVisible(true);
   
    
    if (!mLabel)
    {    
        mLabel = new HbLabel(mContent);
    }
    else
    {    
        mLabel->setVisible(true);
    }
        
    QString formText;
    formText = hbTrId("txt_software_formlabel_refreshing_updates_list");
    mLabel->setPlainText(formText);
    mLabel->setAlignment(Qt::AlignHCenter);
          
     
    QGraphicsLinearLayout *linearLayout = (QGraphicsLinearLayout*) mContent->layout();
    linearLayout->addStretch();
    linearLayout->addItem(mLabel);
    
    
    if (!mAnimationIconItem)
    {    
        mAnimationIconItem = new HbIconItem(ANIMATION_ICON,mContent);
    }
    else
    {
        mAnimationIconItem->setVisible(true);
    }
    mAnimationIconItem->setAlignment(Qt::AlignHCenter );
    linearLayout->addItem(mAnimationIconItem);
    linearLayout->addStretch();
        
    HbIconAnimationManager::global()->addDefinitionFile(ANIMATION_FILE);
    
    mAnimationIconItem->animator().startAnimation();
}
    
void IAUpdateMainView::stopRefreshingAnimation()
{
    mActionDisclaimer->setVisible(true);
    mActionSettings->setVisible(true);
    mActionCancelRefresh->setVisible(false);

    mAnimationIconItem->animator().stopAnimation();
}    

void IAUpdateMainView::updateCompleted()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateCompleted() begin");  
    mUpdating = false;
    mActionStartUpdate->setEnabled(countOfSelections() > 0);
    mActionDisclaimer->setVisible(true);
    mActionSettings->setVisible(true);
    mActionCancelUpdate->setVisible(false);
    connect( mListView, SIGNAL( longPressed( HbAbstractViewItem *, const QPointF & ) ),
            this, SLOT( handleDetails( HbAbstractViewItem *, const QPointF & ) ) ); 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateCompleted() end"); 
}

void IAUpdateMainView::handleStartUpdate()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleStartUpdate() begin");

    mActionStartUpdate->setEnabled(false);
    mActionDisclaimer->setVisible(false);
    mActionSettings->setVisible(false);
    mActionCancelUpdate->setVisible(true);
    disconnect( mListView, SIGNAL( longPressed( HbAbstractViewItem *, const QPointF & ) ),
                this, SLOT( handleDetails( HbAbstractViewItem *, const QPointF & ) ) ); 
    mUpdating = true;
    mEngine->StartUpdate(fotaSelected());
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleStartUpdate() end");
}


void IAUpdateMainView::handleSettings()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleSettings() begin");
    emit toSettingView();
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleSettings() end");
}

void IAUpdateMainView::handleDisclaimer()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleDisclaimer() begin");
    int counter = mListView->model()->rowCount();
       
    if (!mDialogUtil)
    {
        mDialogUtil = new IAUpdateDialogUtil(this);
    }
    if (mDialogUtil)
    {
        HbAction *primaryAction = new HbAction(hbTrId("txt_common_button_ok"));
        mDialogUtil->showAgreement(primaryAction);
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleDisclaimer() end");
}

void IAUpdateMainView::handleCancelRefresh()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleCancelRefresh() begin");
    mEngine->Controller()->HandleUserCancelL();
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleCancelRefresh() end");
}

void IAUpdateMainView::handleCancelUpdate()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleCancelUpdate() begin");
    mEngine->Controller()->HandleUserCancelL();
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleCancelUpdate() end");
}



void IAUpdateMainView::handleDetails(HbAbstractViewItem * item, const QPointF &)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleDetails() begin");
    int ind = item->modelIndex().row();
    if (getApplicationNode(ind) != NULL)
    {    
        showDetails(*getApplicationNode(ind));
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleDetails() end");
}

void IAUpdateMainView::handleFotaDetails(HbAbstractViewItem *, const QPointF &)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleFotaDetails begin");
    //only one FOTA item in a list, just use it
    for (int i = 0; i < mFwNodes.Count(); i++)
    {
        if (mFwNodes[i]->FwType() == MIAUpdateFwNode::EFotaDp2)
        {
            showDetails(*mFwNodes[i]);
        }
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleFotaDetails end");
}


void IAUpdateMainView::handleSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleSelectionChanged begin");
    if (!mSelectionUpdate)
    {
        bool changedItemFound = false; 
        QModelIndexList indexList = selected.indexes();
        for (int i = 0; !changedItemFound && i < indexList.count(); ++i)
        {    
            changedItemFound = true;
            markListItem(true, indexList.at(i).row());
        }
        if (!changedItemFound)
        {
            indexList = deselected.indexes();
            for (int i = 0; !changedItemFound && i < indexList.count(); ++i)
            {    
                changedItemFound = true;
                markListItem(false, indexList.at(i).row());
            }
        }
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleSelectionChanged end");
}

void IAUpdateMainView::handleFwSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleFwSelectionChanged begin");
    if (!mSelectionUpdate)
    {
        bool changedItemFound = false; 
        QModelIndexList indexList = selected.indexes();
        for (int i = 0; !changedItemFound && i < indexList.count(); ++i)
        {    
            changedItemFound = true;
            markFotaItem(true);
        }
        if (!changedItemFound)
        {
            indexList = deselected.indexes();
            for (int i = 0; !changedItemFound && i < indexList.count(); ++i)
            {    
                changedItemFound = true;
                markFotaItem(false);
            }
        }
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::handleFwSelectionChanged end");
}


void IAUpdateMainView::dialogFinished(HbAction *action)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::dialogFinished begin");
    DialogState dialogState = mDialogState;
    mDialogState = NoDialog;
    
    switch ( dialogState )
    {
        case Dependencies:
            if (action == mPrimaryAction)
            {
                updateSelectionsToNodeArray(*mNode,mMark);
            }
            updateSelectionsToList();
            updateSelectionInfoInDock();
            mActionStartUpdate->setEnabled(countOfSelections() > 0);
            break;
        case CannotOmit:    
            break;
        case Details:
            break; 
        default: 
            break;
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::dialogFinished end");
}

bool IAUpdateMainView::fotaSelected() const
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::fotaSelected() begin");
    bool selected = false;
    for (int i = 0; i < mFwNodes.Count() && !selected; ++i)
    {
        if ( mFwNodes[i]->FwType() == MIAUpdateFwNode::EFotaDp2 && mFwNodes[i]->Base().IsSelected() )
        {    
            selected = true;
        }
    }
    if (selected)
        {
        IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::fotaSelected() Fota item is selected");
        }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::fotaSelected() end");
    return selected;
}



void IAUpdateMainView::markListItem(bool mark, int index)
{
    IAUPDATE_TRACE_2("[IAUPDATE] IAUpdateMainView::markListItem() begin mark: %d index %d", mark, index);
    if (mark)
    {
        //It's Mark Command
        if (fotaSelected())
        {    
            //FOTA item is selected, unmark FOTA item
            //FOTA item and application update items can't be selected at the same time 
            markFotaItem(false);
        }    
    }
       
    bool accepted = false;
        
    if(index > -1)
    {
        accepted = true;
        MIAUpdateNode* node = mNodes[index];
        RPointerArray<MIAUpdateNode> mands;
        RPointerArray<MIAUpdateNode> deps;
           
        if (mark)
        {
            TRAPD(err,IAUpdateDepUtils::GetDependenciesL(*node, mNodes, deps));
            if (err != KErrNone)
            {
                deps.Close();
                updateSelectionsToList();
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markListItem() return end");
                return;
            }
            if (!getMandatoryNodes(mands))
                {
                // error when creating mandatory node list
                mands.Close();
                updateSelectionsToList();
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markListItem() return end");
                return;
                }
        }
        else
        {
            //mandatory item is not allowed to be unmarked
            if (mNodes[index]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
            {
                //show dialog  
                showUpdateCannotOmitDialog();
                updateSelectionsToList();
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markListItem() return end");
                return;
            }
            TRAPD(err,IAUpdateDepUtils::GetDependantsL(*node, mNodes, deps));  
            if (err != KErrNone)
            {
                deps.Close();
                updateSelectionsToList();
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markListItem() return end");
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
                updateSelectionsToList();
                IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markListItem() return end");
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
                    text.append(hbTrId("txt_software_info_selected_update_also_needs_updat").arg(names));
                } 
                else
                {
                    text.append(hbTrId("txt_software_info_selected_update_also_needs_1").arg(names));
                    
                }
            }
            else
            {
                if (depCount > 1) 
                {
                    HbParameterLengthLimiter limiter(hbTrId("txt_software_info_updates_1_need_deselected_upd"));  
                    text.append(limiter.arg("names")); 
                } 
                else
                {
                    HbParameterLengthLimiter limiter(hbTrId("txt_software_info_update_1_needs_deselected_upd"));  
                    text.append(limiter.arg("names")); 
                }   
            }
                
            if (mark && mNodes[index]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
            {
                // depencencies of mandatory update are also selected without showing dialog
                accepted = true;
            }
            else
            {
                mNode = node;
                mMark = mark;
                accepted = false;
                showDependenciesFoundDialog(text);
            }
        }
        if (accepted)
        {    
            updateSelectionsToNodeArray(*node, mark);   
        }
    }
    if (accepted)
    {    
        updateSelectionsToList();
        updateSelectionInfoInDock();
        if (!mUpdating)
        {    
            mActionStartUpdate->setEnabled(countOfSelections() > 0);
        }    
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markListItem() end");
}       


void IAUpdateMainView::markFotaItem(bool mark)
{
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateMainView::markFotaItem() begin mark: %d", mark);
    if (mark)
    {
        //It's Mark Command
                
        //if some of application update item(s) are selected, unmark them
        //FOTA item and application update items can't be selected at the same time 
        bool deselected = false;
        for (int i = 0; i < mNodes.Count(); ++i)
        {
            if (mNodes[i]->Base().IsSelected())
            {
                mNodes[i]->Base().SetSelected(false);
                deselected = true;
            }
        } 
        if (deselected)
        {    
            // umnmarked items to be updated also to UI
            updateSelectionsToList();
        }    
    }
    for (int j = 0; j < mFwNodes.Count(); ++j)
    {    
        if (mFwNodes[j]->FwType() == MIAUpdateFwNode::EFotaDp2)
        {
            mFwNodes[j]->Base().SetSelected(mark);
            if (mark)
            {
                mFwListView->selectionModel()->select(mFwListView->model()->index(0,0),QItemSelectionModel::Select);
            }
            else
            {
                mFwListView->selectionModel()->select(mFwListView->model()->index(0,0),QItemSelectionModel::Deselect);
            }
        }
    }
    updateSelectionInfoInDock();
    mActionStartUpdate->setEnabled(countOfSelections() > 0);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::markFotaItem() end");
}





void IAUpdateMainView::updateSelectionsToNodeArray(MIAUpdateNode &node, bool mark)   
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateSelectionsToNodeArray() begin");
    RPointerArray<MIAUpdateNode> deps;  
    TInt err = KErrNone;
    if (mark)
    {
        TRAP(err,IAUpdateDepUtils::GetDependenciesL(node, mNodes, deps));
    }
    else
    {
        TRAP(err,IAUpdateDepUtils::GetDependantsL(node, mNodes, deps)); 
    }
    if (err  == KErrNone )
    {    
        int depCount = deps.Count(); 
        for(int i = 0; i < depCount; i++)
        {
            int depNodeInd = mNodes.Find(deps[i]);
            mNodes[depNodeInd]->Base().SetSelected(mark);
        }
        deps.Close();
        int nodeInd = mNodes.Find(&node);
        mNodes[nodeInd]->Base().SetSelected(mark);
    }                
    
    //mark all of the mandatory items
    if ( mark )
    {    
        RPointerArray<MIAUpdateNode> mands;    
        if (getMandatoryNodes(mands))
        {
            int mandCount = mands.Count();
            if (mandCount > 0)
            {
                for(int i = 0; i < mandCount; i++)
                {
                    int mandNodeInd = mNodes.Find(mands[i]);
                    mNodes[mandNodeInd]->Base().SetSelected(mark);
                    if (mNodes[mandNodeInd]->NodeType() == MIAUpdateAnyNode::ENodeTypeNormal)
                    {
                        // mark also all dependencies of a mandatory item
                        MIAUpdateNode* dependencyNode = mNodes[mandNodeInd];
                        RPointerArray<MIAUpdateNode> dependencies;
                        TRAPD(err,IAUpdateDepUtils::GetDependenciesL(*dependencyNode, mNodes, dependencies));
                        if (err)
                        {
                            dependencies.Close(); 
                            mands.Close();
                            return;
                        }
                        for(int j = 0; j < dependencies.Count(); j++)
                        {
                            int depNodeInd = mNodes.Find(dependencies[j]);
                            mNodes[depNodeInd]->Base().SetSelected(true);
                        }
                        dependencies.Close();
                    }
                }
            }
        }
        mands.Close();
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateSelectionsToNodeArray() end");
}


bool IAUpdateMainView::getMandatoryNodes(RPointerArray<MIAUpdateNode> &mandNodes) const
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::getMandatoryNodes() begin");
    bool ret = true;
    for(int i = 0; i < mNodes.Count(); ++i)
    {
        if (mNodes[i]->Base().Importance() == MIAUpdateBaseNode::EMandatory)
        {
            if (mandNodes.Append(mNodes[i]) != KErrNone)
            {
                ret = false; 
            }
        }
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::getMandatoryNodes() end");
    return ret;
}

void IAUpdateMainView::showUpdateCannotOmitDialog()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::showUpdateCannotOmitDialog() begin");
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    messageBox->setText(hbTrId("txt_software_info_this_required_update_cannot_be_o"));
    int actionCount = messageBox->actions().count();
    for (int i=actionCount-1; i >= 0; i--)
    { 
        messageBox->removeAction(messageBox->actions().at(i));
    }
    HbAction *okAction = NULL;
    okAction = new HbAction(hbTrId("txt_common_button_ok"));
    messageBox->addAction(okAction);
    messageBox->setTimeout(HbPopup::StandardTimeout);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    mDialogState = CannotOmit;
    messageBox->open(this, SLOT(dialogFinished(HbAction*)));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::showUpdateCannotOmitDialog() end");
}

void IAUpdateMainView::showDependenciesFoundDialog(QString &text)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::showDependenciesFoundDialog() begin");
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    HbLabel *label = new HbLabel(messageBox);
    label->setHtml(hbTrId("txt_software_title_dependencies"));
    messageBox->setHeadingWidget(label);
    //messageBox->setIconVisible(false);
        
    messageBox->setText(text);
    int actionCount = messageBox->actions().count();
    for (int i=actionCount-1; i >= 0; i--)
    { 
        messageBox->removeAction(messageBox->actions().at(i));
    }
    mPrimaryAction = NULL;
    mPrimaryAction = new HbAction("Continue");
    HbAction *secondaryAction = NULL;
    secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"));
    messageBox->addAction(mPrimaryAction);
    messageBox->addAction(secondaryAction);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    mDialogState = Dependencies;
    messageBox->open(this, SLOT(dialogFinished(HbAction*)));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::showDependenciesFoundDialog() end");
}

void IAUpdateMainView::updateSelectionsToList()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateSelectionsToList() begin");
    mSelectionUpdate = true;
    for(int i = 0; i < mNodes.Count(); ++i)
    {
        if ( mNodes[i]->Base().IsSelected() != mListView->selectionModel()->isSelected(mListView->model()->index(i,0)))
        {    
            QItemSelectionModel::SelectionFlag selectionFlag;
            if ( mNodes[i]->Base().IsSelected())
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
    mSelectionUpdate = false;
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateSelectionsToList() end");
}


MIAUpdateNode* IAUpdateMainView::getApplicationNode(int index) const
{
    IAUPDATE_TRACE_1("[IAUPDATE] IAUpdateMainView::getApplicationNode() begin index: %d", index);
    MIAUpdateNode *currentNode = NULL;
     
    if (index >= 0 && index < mNodes.Count())
    {
        currentNode = mNodes[index];
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::getApplicationNode() end");
    return currentNode;
}
                
void IAUpdateMainView::showDetails(MIAUpdateAnyNode& node)
{  
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::showDetails() begin");
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    HbLabel *label = new HbLabel(messageBox);
    label->setHtml(hbTrId("txt_software_title_details"));
    messageBox->setHeadingWidget(label);
       
    messageBox->setIconVisible(false);
            
    QString text;
    constructDetailsText(node,text); 
    messageBox->setText(text);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    mDialogState = Details;
    messageBox->open(this, SLOT(dialogFinished(HbAction*)));    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::showDetails() end");
}                
        

void IAUpdateMainView::constructDetailsText(MIAUpdateAnyNode &node, QString &text)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::constructDetailsText() begin");
    text.append(hbTrId("txt_software_dialog_name"));
    text.append(QString("<br />"));
    if (node.NodeType() == MIAUpdateAnyNode::ENodeTypeFw)
    {    
        text.append(hbTrId("txt_swupdate_dialog_device_software")); 
    }
    else
    {
        QString name = XQConversions::s60DescToQString(node.Base().Name());
        text.append(name);
    }
        
    text.append(QString("<br />"));
    text.append(QString("<br />"));
    
    text.append(hbTrId("txt_software_dialog_description"));
    text.append(QString("<br />"));
    QString description;
    if (node.NodeType() == MIAUpdateAnyNode::ENodeTypeFw)
    {
        description = 
            hbTrId("txt_software_info_this_update_improves_your_device");
    }
    else
    {    
        description = XQConversions::s60DescToQString(node.Base().Description());
    }    
    text.append(description);
    text.append(QString("<br />"));
    text.append(QString("<br />"));
    
    
    if (node.NodeType() == MIAUpdateAnyNode::ENodeTypeNormal)
    {
        MIAUpdateNode *iaupdateNode = static_cast<MIAUpdateNode*> (&node);
        if (iaupdateNode->Type()!= MIAUpdateNode::EPackageTypeServicePack)
        {
            text.append(hbTrId("txt_software_dialog_version"));
            text.append(QString("<br />"));
            QString textVersion;
            versionText(node.Base().Version(), textVersion);
            text.append(textVersion);
            text.append(QString("<br />"));
            text.append(QString("<br />"));
        }
    }
    if (node.NodeType() == MIAUpdateAnyNode::ENodeTypeFw)
    {    
        MIAUpdateFwNode *fwNode = static_cast<MIAUpdateFwNode*> (&node);
        text.append(hbTrId("txt_software_dialog_version"));
        text.append(QString("<br />"));
        QString textVersion = XQConversions::s60DescToQString(fwNode->FwVersion1());
        text.append(textVersion);
        text.append(QString("<br />"));
        text.append(QString("<br />"));
    }
    
    int contentSize = node.Base().ContentSizeL();
    if (contentSize > 0)
    {
        text.append(hbTrId("txt_software_dialog_size"));
        text.append(QString("<br />"));
        QString textFileSize;
        fileSizeText(contentSize, textFileSize);
        text.append(textFileSize);
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::constructDetailsText() end");
}

void IAUpdateMainView::versionText(const TIAUpdateVersion &version, QString &versionText)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::versionText() begin");
    versionText.append
        (hbTrId("txt_software_info_version_1l_2l_3l").arg(version.iMajor).arg(version.iMinor).arg(version.iBuild));
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::versionText() end");
}

void IAUpdateMainView::fileSizeText(int fileSize, QString &text)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::fileSizeText() begin");
    int size = 0;
    
    if (fileSize >= KMaxShownInKiloBytes )
        {
        size = fileSize / KMegaByte;
        if ( fileSize % KMegaByte != 0 )
            {
            size++;
            }
        text.append(hbTrId("txt_software_dialog_l_kb").arg(size));
        }
    else
        {
        size = fileSize / KKiloByte;
        if ( fileSize % KKiloByte != 0 )
            {
            size++;
            } 
        text.append(hbTrId("txt_software_dialog_l_mb").arg(size));
        }
    
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::fileSizeText() end");
}


void IAUpdateMainView::removeCurrentContentLayout()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::removeCurrentContentLayout() begin");
    int itemCount = mContent->layout()->count();
    for (int i = 0; i < itemCount; i++)    
    {
        mContent->layout()->removeAt(i);
    }
    if (mContentDataForm)
    {
        mContentDataForm->setVisible(false);
    }
    if (mLabel)
    {
        mLabel->setVisible(false);
    }
    if (mAnimationIconItem)
    {   
        delete mAnimationIconItem;
        mAnimationIconItem = NULL;
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::removeCurrentContentLayout() end");    
}

void IAUpdateMainView::refreshFirmwareUpdates(const RPointerArray<MIAUpdateFwNode> &fwNodes)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshFirmwareUpdates() begin");
    mFwNodes.Reset();
            
    for (int i = 0; i < fwNodes.Count(); i++)
    {
        MIAUpdateFwNode *fwnode = (fwNodes[i]);
        mFwNodes.Append(fwnode);
        fwnode->Base().SetImportance(MIAUpdateBaseNode::ECritical);
        if (fwnode->FwType() == MIAUpdateFwNode::EFotaDp2)
        {
            refreshFotaUpdate(*fwnode );
        }
        else if (fwNodes.Count() == 1 && fwnode->FwType() == MIAUpdateFwNode::EFotiNsu)
        {
        refreshNsuUpdate(*fwnode);
        }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshFirmwareUpdates() end");    
    }    
}

void IAUpdateMainView::refreshFotaUpdate(MIAUpdateFwNode& fwNode)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshFotaUpdate() begin");
    if (!mFwListView)
    {
        mFwListView  = new HbListView(mContent);
        mFwListView->setSelectionMode( HbAbstractItemView::MultiSelection );
        connect( mFwListView, SIGNAL( longPressed( HbAbstractViewItem *, const QPointF & ) ),
        this, SLOT( handleFotaDetails( HbAbstractViewItem *, const QPointF & ) ) ); 
        
        IAUpdateAppListModel *model = new IAUpdateAppListModel();
        HbListViewItem *prototype = new IAUpdateAppListItem(mListView);
        prototype->setGraphicsSize(HbListViewItem::LargeIcon);
        prototype->setStretchingStyle(HbListViewItem::NoStretching);            
        mListView->setModel(model, prototype);
         
        mFwListView->listItemPrototype()->setGraphicsSize(HbListViewItem::LargeIcon);   
        mFwListView->listItemPrototype()->setStretchingStyle(HbListViewItem::NoStretching);
        connect(mFwListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(handleFwSelectionChanged(QItemSelection,QItemSelection)));     
    }
    QGraphicsLinearLayout *linearLayout = (QGraphicsLinearLayout*) mContent->layout();
    linearLayout->addItem(mFwListView);
    IAUpdateAppListModel *model = (IAUpdateAppListModel*)  mListView->model();
    QItemSelectionModel *selectionModel = mFwListView->selectionModel();  
    mSelectionUpdate = true;
    selectionModel->clear();
    mSelectionUpdate = false;
    model->setFwNode(&fwNode);
 
    if (fwNode.Base().IsSelected())
    {
        QModelIndex modelIndex = mFwListView->model()->index(0,0);
        selectionModel->select(modelIndex, QItemSelectionModel::Select);
    }
    model->refresh();
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshFotaUpdate() end");
}

void IAUpdateMainView::refreshNsuUpdate(MIAUpdateFwNode& fwNode)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshNsuUpdate() begin");
    if (!mFwNSUGroupBox)
    {
        mFwNSUGroupBox = new HbGroupBox(mContent);
        mFwNSUGroupBox->setHeading(hbTrId("txt_software_subhead_device_software_available"));
        HbDataForm *dataForm  = new HbDataForm(mFwNSUGroupBox); 
        mFwNSUGroupBox->setContentWidget(dataForm);
        QString textVersion = XQConversions::s60DescToQString(fwNode.FwVersion1());
        dataForm->setDescription
            (hbTrId("txt_software_info_please_use_your_pc_to_update_the").arg(textVersion));
    }
    QGraphicsLinearLayout *linearLayout = (QGraphicsLinearLayout*) mContent->layout();
    linearLayout->addItem(mFwNSUGroupBox);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshNsuUpdate() end");
}

void IAUpdateMainView::refreshApplicationUpdates(const RPointerArray<MIAUpdateNode> &nodes)
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshApplicationUpdates() begin");
    if (nodes.Count() > 0)
    {
        if (!mApplicationUpdatesGroupBox)
        {
            mApplicationUpdatesGroupBox = new HbGroupBox(mContent);
            mApplicationUpdatesGroupBox->setHeading(hbTrId("txt_software_subhead_application_updates"));
        }
        QGraphicsLinearLayout *linearLayout = (QGraphicsLinearLayout*) mContent->layout();
        linearLayout->addItem(mApplicationUpdatesGroupBox);
        linearLayout->addItem(mListView);
    }
    IAUpdateAppListModel *model = (IAUpdateAppListModel*)  mListView->model();
    QItemSelectionModel *selectionModel = mListView->selectionModel();  
    mSelectionUpdate = true;
    selectionModel->clear();
    mSelectionUpdate = false;
    
    //HbIcon icon(QString(":/icons/qgn_menu_swupdate"));
    mNodes.Reset();

    for(int i = 0; i < nodes.Count(); ++i) 
    {
        MIAUpdateNode *node = nodes[i];
        mNodes.Append(node);
    }
    model->setNodeArray(mNodes);
    for(int j = 0; j < mNodes.Count(); ++j) 
    {
        if (mNodes[j]->Base().IsSelected())
        {
            QModelIndex modelIndex = mListView->model()->index(j,0);
            selectionModel->select(modelIndex, QItemSelectionModel::Select);
        }
    }
    if (!mSelectionConnect)
    {    
        mSelectionConnect = connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                        this, SLOT(handleSelectionChanged(QItemSelection,QItemSelection)));     
    }
    model->refresh();

    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::refreshApplicationUpdates() end");
}

void IAUpdateMainView::updateSelectionInfoInDock()
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateSelectionInfoInDock() begin");
    int countOfSelectedItems = 0;
    int countOfAllItems = 0;
    int selectedSizeInBytes = 0;
    for (int i = 0; i < mFwNodes.Count(); ++i)
    {
        if (mFwNodes[i]->FwType() == MIAUpdateFwNode::EFotaDp2)
        {
            countOfAllItems++;
            if (mFwNodes[i]->Base().IsSelected())
            {
                countOfSelectedItems++;
                selectedSizeInBytes += mFwNodes[i]->Base().ContentSizeL();
            }
        }
    } 
    countOfAllItems += mNodes.Count();
    for (int j = 0; j < mNodes.Count(); ++j)
    {    
        if (mNodes[j]->Base().IsSelected())
        {
            countOfSelectedItems++;
            selectedSizeInBytes += mNodes[j]->Base().ContentSizeL();
        }
    }    
    
    int selectedSize = 0;
    QString unit;
    if (selectedSizeInBytes >= KMaxShownInKiloBytes)
    {
        unit = "MB";
        selectedSize = selectedSizeInBytes / KMegaByte;
        if (selectedSizeInBytes % KMegaByte != 0)
        {
            selectedSize++;
        }
    }
    else 
    {
        unit = "kB";
        selectedSize = selectedSizeInBytes / KKiloByte;
        if (selectedSizeInBytes % KKiloByte != 0)
        {
            selectedSize++;
        }
    }
    QString selectionString; 
    selectionString.append("Selected ");
    QString numText;
    numText.setNum(countOfSelectedItems);
    selectionString.append(numText);
    selectionString.append("/");
    numText.setNum(countOfAllItems);
    selectionString.append(numText);
    if (selectedSize > 0)
    {
        selectionString.append(" (");
        numText.setNum(selectedSize);
        selectionString.append(numText);
        selectionString.append(" ");
        selectionString.append(unit);
        selectionString.append(")");
    }
    mSelections->setHeading(selectionString);
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::updateSelectionInfoInDock() end");
}

int IAUpdateMainView::countOfSelections() const
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::countOfSelections() begin");
    int countOfSelectedItems = 0;
    for (int i = 0; i < mFwNodes.Count(); ++i)
    {
        if (mFwNodes[i]->FwType() == MIAUpdateFwNode::EFotaDp2)
        {
            if (mFwNodes[i]->Base().IsSelected())
            {
                countOfSelectedItems++;
            }
        }
    } 
    for (int j = 0; j < mNodes.Count(); ++j)
    {    
        if (mNodes[j]->Base().IsSelected())
        {
            countOfSelectedItems++;
        }
    } 
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::countOfSelections() end");
    return countOfSelectedItems;
}    
