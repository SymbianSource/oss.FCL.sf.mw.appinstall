/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the header file of the IAUpdateAppListModel
*                class 
*
*/

#ifndef IAUPDATEAPPLISTMODEL_H_
#define IAUPDATEAPPLISTMODEL_H_


#include <QAbstractListModel>
#include <QObject>

#include "iaupdatenode.h"
class MIAUpdateFwNode;

class IAUpdateAppListModel : public QAbstractListModel
{
    Q_OBJECT 
    
public:
    
    enum ItemRole
    {
        ProgressRole = Qt::UserRole + 1,
        ProgressValueRole
    };
    
    IAUpdateAppListModel();
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole ) const;
    int  rowCount (const QModelIndex &parent=QModelIndex()) const;
    void setNodeArray(const RPointerArray<MIAUpdateNode> &nodes);   
    //void setFwNodeArray(const RPointerArray<MIAUpdateFwNode> &fwNodes);
    void setFwNode(MIAUpdateFwNode *fwNode);
    void refresh();
    void refreshProgress();
    
private: 
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void setImportance(MIAUpdateAnyNode *node, QString &importanceDescription) const;
   
private:
    RPointerArray<MIAUpdateNode> mNodes;
    //RPointerArray<MIAUpdateFwNode> mFwNodes;
    MIAUpdateFwNode *mFwNode;
    
    int mRefreshedCount;
};

#endif /* IAUPDATEAPPLISTMODEL_H_ */
