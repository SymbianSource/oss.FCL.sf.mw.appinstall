/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class MNcdProviderObserver declaration
*
*/


#ifndef M_NCD_PROVIDERCLOSEOBSERVER_H
#define M_NCD_PROVIDERCLOSEOBSERVER_H

class CNcdProvider;

class MNcdProviderCloseObserver
    {
public:
    
    virtual void ProviderClosed( CNcdProvider& aProvider ) = 0;
    
protected:
    
    virtual ~MNcdProviderCloseObserver() {}
    };

#endif /* M_NCD_PROVIDERCLOSEOBSERVER_H */
