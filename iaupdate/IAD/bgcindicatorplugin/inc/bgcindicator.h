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
 * Description: Message Indicator class
 *
 */

#ifndef BGCINDICATOR_H
#define BGCINDICATOR_H

#include <QObject>

#include <hbindicatorinterface.h>

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
     * Prepares the display name from stream.
     * @param dataStream data stream. 
     */
    //void prepareDisplayName(QDataStream& dataStream);
    
    //HLa
    void StartIaupdateL() const;
    
private:
    /**
     * Nr of updates
     */
    int mNrOfUpdates;
    
};

#endif // BGCINDICATOR_H

