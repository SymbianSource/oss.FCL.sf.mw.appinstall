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
* Description: Dummy implementation for details provider interface.
*
*/

#ifndef C_DUMMYDETAILSPROVIDER_H
#define C_DUMMYDETAILSPROVIDER_H

#include <e32base.h>            // CBase
#include <sifuidetailsif.h>     // MSifUiDetailsIf, MSifUiListIterator, MSifUiDetailsIterator


class CDummyDetailsProvider : public CBase, public MSifUiDetailsIf,
        public MSifUiListIterator, public MSifUiDetailsIterator
    {
public:
    CDummyDetailsProvider( RFs& aFs );
    ~CDummyDetailsProvider();

public:
    // from MSifUiDetailsIterator
    void SetToFirstFieldL();
    TBool HasNextField();
    void GetNextFieldL( TDes& aFieldName, TDes& aFieldValue );

    // from MSifUiListIterator
    void SetToFirstItemL();
    TBool HasNextItem();
    void GetNextItemL( TDes& aItemName );
    MSifUiDetailsIterator* DetailsIterator( const TDesC& aItemName );

    // from MSifUiDetailsIf
    MSifUiDetailsIterator* AppDetailsIterator();
    MSifUiListIterator* CertificatesListL();
    MSifUiListIterator* DrmFilesListL();
    void GetDriveListL( RArray<TDriveUnit>& aDriveList );
    void GetFolderListL( const TDesC& aParent, CDesCArray& aFolderList );
    TBool HasFolderSubfolders( const TDesC& aParent, const TDesC& aFolder );

private:    // data
    RFs& iFs;
    TInt iCurrentField;
    TInt iCurrentItem;
    };

#endif // C_DUMMYDETAILSPROVIDER_H

