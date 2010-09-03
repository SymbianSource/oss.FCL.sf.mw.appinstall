/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Backgroundchecker Indicator class
 *
 */

#ifndef BGCINDICATOR_H
#define BGCINDICATOR_H

#include <QObject>

#include <hbindicatorinterface.h>
#include <hbtranslator.h>

/**
 * Message indicator class. 
 * Handles client request and showing the indications. 
 */
class BgcIndicator : public HbIndicatorInterface
{
public:
    /**
     * Constructor
     */
    BgcIndicator(const QString &indicatorType);
    
    /**
     * Destructor
     */
    ~BgcIndicator();
    
    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);
    
    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;
    
protected:
    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    
private: 
    /**
     * Start the iaupdate client
     */
    void StartIaupdateL() const;
    
private:
    /**
     * Owned
     */
    int mNrOfUpdates;
    /**
      * Owned
      */
    HbTranslator *mTranslator;

    
    
};

#endif // BGCINDICATOR_H

