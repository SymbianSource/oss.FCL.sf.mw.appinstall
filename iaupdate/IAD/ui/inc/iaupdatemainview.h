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
class IAUpdateEngine;
class IAUpdateDialogUtil;
class TIAUpdateVersion;


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
    
    void handleDetails( HbAbstractViewItem *, const QPointF & );
    
    void handleSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    
    void dialogFinished(HbAction* action);
    
private:
    
    void getSelectedNodes(RPointerArray<MIAUpdateAnyNode> &selectedNodes) const;
    void markListItem(bool mark, int index);
    bool getMandatoryNodes(RPointerArray<MIAUpdateAnyNode> &mandNodes) const;
    void showUpdateCannotOmitDialog(); 
    void showDependenciesFoundDialog(QString &text);
    void updateSelectionsToNodeArray(MIAUpdateNode &node, bool mark);
    void updateSelectionsToList();
    MIAUpdateAnyNode* getNode(int index) const;
    void showDetails(MIAUpdateAnyNode& node);
    void constructDetailsText(MIAUpdateAnyNode &node, QString &text);
    void versionText(const TIAUpdateVersion &version, QString &versionText);
    void fileSizeText(int fileSize, QString &text);
    
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
    HbListWidget *mListView;
    RPointerArray<MIAUpdateAnyNode> mAllNodes;
    DialogState mDialogState; 
    HbAction *mPrimaryAction;
    MIAUpdateNode *mNode; 
    bool mMark;
    bool mSelectionUpdate;

};

#endif /* IAUPDATEMAINVIEW_H_ */
