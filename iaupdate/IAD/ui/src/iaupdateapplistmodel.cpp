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
* Description:   This module contains the implementation of IAUpdateAppListModel
*                class member functions.
*
*/

#include <QStringList>
#include <hbtranslator.h>
#include <xqconversions.h>

#include "iaupdateapplistmodel.h"
#include "iaupdatefwnode.h"
#include "iaupdatebasenode.h"
#include "iaupdatedebug.h"


const int KKiloByte = 1024;
const int KMegaByte = 1024 * 1024;
const int KMaxShownInKiloBytes = 10 * KMegaByte;

IAUpdateAppListModel::IAUpdateAppListModel() :
QAbstractListModel()
{
    mRefreshedCount = 0;
    mFwNode = NULL;    
}


QVariant IAUpdateAppListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < rowCount())
    {    
        if (role == Qt::DisplayRole) 
        {
            QString name;
            QString importanceDescription;
            if (mFwNode)
            {
                MIAUpdateFwNode::TUIUpdateState fwUiState = mFwNode->UiState();
                if (fwUiState == MIAUpdateFwNode::EPreparing)
                {
                    name = QString("Preparing ");
                }
                else
                {
                    name = XQConversions::s60DescToQString(mFwNode->Base().Name());
                    name.append(" DEVICE SOFTWARE");
                }
                setImportance(mFwNode,importanceDescription);
            }
            else
            {     
                MIAUpdateNode *node = mNodes[index.row()];
     
                MIAUpdateNode::TUIUpdateState uiState = node->UiState();
   
            
                if (uiState == MIAUpdateNode::EDownloading)
                {
                    name = QString("Downloading ");
                }
                else if (uiState == MIAUpdateNode::EInstalling)
                {
                    name = QString("Installing ");
                }
                name.append(XQConversions::s60DescToQString(node->Base().Name()));
                        
            
                if (uiState == MIAUpdateNode::ENormal)
                {
                    setImportance(node,importanceDescription);
                }
                else if (uiState == MIAUpdateNode::EUpdated)
                {
                    importanceDescription = 
                            hbTrId("txt_software_dblist_1_val_updated");
                }
                else if (uiState == MIAUpdateNode::EFailed)
                {
                    importanceDescription = 
                            hbTrId("txt_software_dblist_1_val_failed");
                }
                else if (uiState == MIAUpdateNode::EDownloaded)
                {
                    importanceDescription = 
                            hbTrId("txt_software_dblist_1_val_downloaded");
                }
            }            
            QStringList stringList;
            stringList << name << importanceDescription;
            return stringList;
        }
        else if (role == ProgressRole)
        {
            if (mFwNode)
            {
                return (mFwNode->UiState() == MIAUpdateFwNode::EPreparing);
            }
            else
            {    
                return (mNodes[index.row()]->UiState() == MIAUpdateNode::EDownloading || 
                        mNodes[index.row()]->UiState() == MIAUpdateNode::EInstalling);
            }
        }
        else if (role == ProgressValueRole)
        {
            QList<QVariant> list;
            if (mFwNode)
            {
                list << 0 << 0;   
            }
            else
            {    
                list << mNodes[index.row()]->Progress() << mNodes[index.row()]->Total();
            }
            return list;
        }
    }    
    return QVariant();
}


int IAUpdateAppListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    int nodeCount = mNodes.Count();
    if (nodeCount == 0 && mFwNode)
    {
        nodeCount = 1;
    }        
    return nodeCount;
}

void IAUpdateAppListModel::setNodeArray(const RPointerArray<MIAUpdateNode> &nodes)
{
    mNodes = nodes;    
}



void IAUpdateAppListModel::setFwNode(MIAUpdateFwNode *fwNode)
{
    mFwNode = fwNode;
}

bool IAUpdateAppListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    int lastRow = row + count - 1;
       
    for (int i = lastRow; i >= row; --i) 
    {
        beginRemoveRows(parent, i, i);
        endRemoveRows();
    }
    return true;
}

void IAUpdateAppListModel::refresh()
{
    if (!mFwNode)
    {
        if (mRefreshedCount > mNodes.Count())
        {
            removeRows(mNodes.Count(), mRefreshedCount - mNodes.Count());   
        }
    }
    emit dataChanged(index(0,0),index(rowCount() -1, 0));
    mRefreshedCount = mNodes.Count();
}

void IAUpdateAppListModel::refreshProgress()
{
    emit layoutChanged();
    emit dataChanged(index(0,0), index(0,0));    
}


void IAUpdateAppListModel::setImportance(MIAUpdateAnyNode *node, QString &importanceDescription) const
{
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateAppListModel::setImportance() begin");
    int sizeInBytes = node->Base().ContentSizeL();
    int size = 0;
    bool shownInMegabytes = false;        
    if (sizeInBytes >= KMaxShownInKiloBytes)
    {
        shownInMegabytes = true;
        size = sizeInBytes / KMegaByte;
        if (sizeInBytes % KMegaByte != 0)
        {
           size++;
        }
    }
    else 
    {
        size = sizeInBytes / KKiloByte;
        if (sizeInBytes % KKiloByte != 0)
        {
            size++;
        }  
    }
    QString sizeString;
    sizeString.setNum(size);     
    switch(node->Base().Importance())
    {        
        case MIAUpdateBaseNode::EMandatory:
        {
            if (shownInMegabytes)
            {
                importanceDescription = 
                    hbTrId("txt_software_dblist_update_1_val_required_mb").arg(size);
            }
            else 
            {
                importanceDescription = 
                    hbTrId("txt_software_dblist_update_1_val_required_kb").arg(size);
            }  
            break;
        }
                
        case MIAUpdateBaseNode::ECritical:
        {
            bool isNSU = false;
            if(node->NodeType() == MIAUpdateAnyNode::ENodeTypeFw)
            {
                MIAUpdateFwNode *fwnode = static_cast<MIAUpdateFwNode*>(node);          
                if (fwnode->FwType() == MIAUpdateFwNode::EFotiNsu)
                {
                   isNSU = true;
                }
            }
            if (!size || isNSU)
            {
                //for firmware when size info is not provided by server
            }
            else
            {
                if (shownInMegabytes)
                {
                    
                    importanceDescription = 
                       hbTrId("txt_software_dblist_update_1_val_important_mb").arg(size);
                }
                else 
                {
                    importanceDescription =
                        hbTrId("txt_software_dblist_update_1_val_important_kb").arg(size);
                } 
            }
            break;
        }
            
        case MIAUpdateBaseNode::ERecommended:
        {
            if (shownInMegabytes)
            {
                importanceDescription = 
                    hbTrId("txt_software_dblist_update_1_val_recommended_mb").arg(size);
            }
            else 
            {
                importanceDescription = 
                   hbTrId("txt_software_dblist_update_1_val_recommended_kb").arg(size);
            }  
            break;
        }
            
        case MIAUpdateBaseNode::ENormal:
        {
            if (shownInMegabytes)
            {
                importanceDescription = 
                   hbTrId("txt_software_dblist_update_1_val_optional_mb").arg(size);
            }
            else 
            {
                importanceDescription = 
                    hbTrId("txt_software_dblist_update_1_val_optional_kb").arg(size);
            }  
            break;
        }

        default:
        {
            break;
        }
    }
    IAUPDATE_TRACE("[IAUPDATE] IAUpdateMainView::setImportance() end");
}    
