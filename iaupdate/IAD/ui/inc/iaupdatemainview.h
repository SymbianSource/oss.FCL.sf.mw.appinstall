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
* Description:   This file contains the header file of the IAUpdateMainView
*                class 
*
*/

#ifndef IAUPDATEMAINVIEW_H_
#define IAUPDATEMAINVIEW_H_

#include <hbview.h>
#include <QItemSelection> 

#include "iaupdatenode.h"
#include "iaupdatefwnode.h"
#include "iaupdatebasenode.h"


class HbListWidget;
class HbAction;
class HbAbstractViewItem;
class HbGroupBox;
class HbDataForm;
class IAUpdateEngine;
class IAUpdateDialogUtil;
class TIAUpdateVersion;
class HbTranslator;


class IAUpdateMainView : public HbView
{
    Q_OBJECT 
    
public:
    IAUpdateMainView(IAUpdateEngine *engine);
    virtual ~IAUpdateMainView();
        
    void refresh(const RPointerArray<MIAUpdateNode> &nodes,
                 const RPointerArray<MIAUpdateFwNode> &fwNodes,
                 int error);
signals:
    void toSettingView();
    
public slots:
    void handleStartUpdate();
 
    void handleSettings();
 
    void handleDisclaimer();
    
    void handleDetails(HbAbstractViewItem *, const QPointF &);
    
    void handleFotaDetails(HbAbstractViewItem *, const QPointF &); 
    
    void handleSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    
    void handleFwSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    
    void dialogFinished(HbAction* action);
    
private:
    
    bool fotaSelected() const;
    void markListItem(bool mark, int index);
    void markFotaItem(bool mark);
    bool getMandatoryNodes(RPointerArray<MIAUpdateNode> &mandNodes) const;
    void showUpdateCannotOmitDialog(); 
    void showDependenciesFoundDialog(QString &text);
    void updateSelectionsToNodeArray(MIAUpdateNode &node, bool mark);
    void updateSelectionsToList();
    MIAUpdateNode* getApplicationNode(int index) const;
    void showDetails(MIAUpdateAnyNode& node);
    void constructDetailsText(MIAUpdateAnyNode &node, QString &text);
    void versionText(const TIAUpdateVersion &version, QString &versionText);
    void fileSizeText(int fileSize, QString &text);
    void setImportance(MIAUpdateAnyNode *node, QString &importanceDescription);
    void removeCurrentContentLayout();
    void refreshFirmwareUpdates(const RPointerArray<MIAUpdateFwNode> &fwNodes);
    void refreshFotaUpdate(MIAUpdateFwNode& fwNode);
    void refreshNsuUpdate();
    void refreshApplicationUpdates(const RPointerArray<MIAUpdateNode> &nodes);
    void updateSelectionInfoInDock();
    
private:
    enum DialogState
        {
        NoDialog,
        Dependencies,
        CannotOmit,
        Details
        };
     
    IAUpdateEngine *mEngine;
    IAUpdateDialogUtil *mDialogUtil;
    HbWidget *mContent;
    HbListWidget *mListView;
    HbListWidget *mFwListView;
    HbGroupBox *mApplicationUpdatesGroupBox;
    HbGroupBox *mFwNSUGroupBox;
    HbDataForm *mContentDataForm;
    HbGroupBox *mSelections;
    RPointerArray<MIAUpdateNode> mNodes;
    RPointerArray<MIAUpdateFwNode> mFwNodes;
    DialogState mDialogState; 
    HbAction *mPrimaryAction;
    MIAUpdateNode *mNode; 
    bool mMark;
    bool mSelectionUpdate;
    bool mSelectionConnect;
    HbTranslator *mTranslator;

};

#endif /* IAUPDATEMAINVIEW_H_ */
