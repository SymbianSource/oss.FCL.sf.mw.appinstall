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
* Description:   Inline functions defined in CAppMngr2LogDatabaseEntry
*
*/


// -----------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::Name()
// -----------------------------------------------------------------------------
//
inline const TDesC& CAppMngr2LogDatabaseEntry::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::Time()
// -----------------------------------------------------------------------------
//
inline const TTime CAppMngr2LogDatabaseEntry::Time() const
    {
    return iTime;
    }

// -----------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::Action()
// -----------------------------------------------------------------------------
//
inline const SwiUI::TLogTaskAction CAppMngr2LogDatabaseEntry::Action() const
    {
    return iAction;
    }

// -----------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::Version()
// -----------------------------------------------------------------------------
//
inline const TDesC& CAppMngr2LogDatabaseEntry::Version() const
    {
    if( iVersion )
        {
        return *iVersion;
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CAppMngr2LogDatabaseEntry::Vendor()
// -----------------------------------------------------------------------------
//
inline const TDesC& CAppMngr2LogDatabaseEntry::Vendor() const
    {
    if( iVendor )
        {
        return *iVendor;
        }
    return KNullDesC;
    }

