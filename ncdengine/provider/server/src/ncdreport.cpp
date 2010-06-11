/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CNcdReport implementation
*
*/


#include "ncdreport.h"
#include "catalogsdebug.h"
#include "ncdpanics.h"
#include "ncdnodeidentifier.h"
#include "catalogsutils.h"
#include "ncdreportmanager.h"
#include "catalogsconnectionmethod.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CNcdReport::~CNcdReport()
    {
    DLTRACEIN((""));

    delete iMetadataId;
    iTimeStamps.Reset();
    delete iAttributes;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReport::ExternalizeL( RWriteStream& aStream )
    {
    DLTRACEIN((""));
    
    ExternalizeEnumL( iType, aStream );
    
    ExternalizeEnumL( iStatus.iStatus, aStream );
    aStream.WriteInt32L( iStatus.iErrorCode );
    
    aStream.WriteInt32L( iReportId );
    
    iMetadataId->ExternalizeL( aStream );
    
    iAccessPoint.ExternalizeL( aStream );
    
    DLTRACE(("Externalizing timestamps"));
    DASSERT( iTimeStamps.Count() == ENcdReportTimeInternal );
    for ( TInt i = 0; i < ENcdReportTimeInternal; ++i )
        {
        aStream << iTimeStamps[i];
        }
    iAttributes->ExternalizeL( aStream );
    DLTRACEOUT(("Externalization successful"));
    }
    
    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNcdReport::InternalizeL( RReadStream& aStream )
    {
    DLTRACEIN((""));
    
    // iType is internalized by the report manager    
    InternalizeEnumL( iStatus.iStatus, aStream );
    iStatus.iErrorCode = aStream.ReadInt32L();
    
    iReportId = aStream.ReadInt32L();
    
    delete iMetadataId;
    iMetadataId = NULL;
    
    iMetadataId = CNcdNodeIdentifier::NewL( aStream );
        
    iAccessPoint.InternalizeL( aStream );    
        
    ResetTimeStampsL();
    DLTRACE(("Internalizing timestamps"));
    DASSERT( iTimeStamps.Count() == ENcdReportTimeInternal );
    for ( TInt i = 0; i < ENcdReportTimeInternal; ++i )
        {
        aStream >> iTimeStamps[i];
        }
    
    delete iAttributes;
    iAttributes = NULL;
    iAttributes = CNcdAttributes::NewL( 
        aStream, 
        ENcdReportAttributeInternal );
        
    DLTRACEOUT(("Internalization successful"));
    }


// ---------------------------------------------------------------------------
// Report type getter
// ---------------------------------------------------------------------------
const TNcdReportType& CNcdReport::ReportType() const
    {
    return iType;
    }


// ---------------------------------------------------------------------------
// Report id setter
// ---------------------------------------------------------------------------
void CNcdReport::SetReportId( const TNcdReportId& aReportId )
    {
    DLTRACEIN(("aReportId: %d", aReportId ));
    iReportId = aReportId;
    }
    

// ---------------------------------------------------------------------------
// Report id getter
// ---------------------------------------------------------------------------
const TNcdReportId& CNcdReport::ReportId() const
    {
    return iReportId;    
    }


// ---------------------------------------------------------------------------
// Attribute getter
// ---------------------------------------------------------------------------
CNcdAttributes& CNcdReport::Attributes()
    {
    DLTRACEIN((""));
    return *iAttributes;    
    }


// ---------------------------------------------------------------------------
// Attribute getter
// ---------------------------------------------------------------------------
const CNcdAttributes& CNcdReport::Attributes() const
    {
    DLTRACEIN((""));
    return *iAttributes;    
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TNcdReportStatusInfo& CNcdReport::Status() const
    {
    DLTRACEIN(("iStatus: %d", iStatus.iStatus));
    return iStatus;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CNcdReport::SetStatus( const TNcdReportStatusInfo& aStatus )
    {
    DLTRACEIN(("aStatus: %d", aStatus.iStatus));
    iStatus = aStatus;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CNcdReport::Match( 
    const CNcdNodeIdentifier& aMetadataId,
    const TDesC& aGenericId,
    const TDesC& aReportUri,
    const TNcdReportType& aReportType ) const
    {
    return aMetadataId.Equals( *iMetadataId ) &&
        ( aGenericId == iAttributes->AttributeString16( 
            ENcdReportAttributeGenericId ) ) &&
        ( aReportUri == iAttributes->AttributeString16(
            ENcdReportAttributeReportUri ) &&
        ( aReportType == ReportType() ) );
    }


// ---------------------------------------------------------------------------
// Linear order for reports
// ---------------------------------------------------------------------------
TInt CNcdReport::Compare( 
    const CNcdReport& aFirst,
    const CNcdReport& aSecond )
    {
    // Ordering is done so that reports that can be bundled together are
    // as close as possible
    
    // Zero means that items equal.
    TInt comp( 0 );
    
    // First compare that nodes are of the same type
    comp = aFirst.ReportType() - aSecond.ReportType();
    
    if ( comp == 0 )
        {
        // Compare report URIs    
        comp = aFirst.Attributes().AttributeString16( 
            ENcdReportAttributeReportUri).Compare( 
                aSecond.Attributes().AttributeString16(
                        ENcdReportAttributeReportUri ) );
                
        if ( comp == 0 ) 
            {
            // Compare generic IDs
            comp = aFirst.Attributes().AttributeString16( 
                ENcdReportAttributeGenericId ).Compare( 
                    aSecond.Attributes().AttributeString16(
                        ENcdReportAttributeGenericId ) );
            if ( comp == 0 ) 
                {
                // Compare metadata's
                comp = CNcdNodeIdentifier::Compare( 
                    aFirst.MetadataId(),
                    aSecond.MetadataId() );        
                }
            }        
        }
    return comp;
    }
    

// ---------------------------------------------------------------------------
// Access point setter
// ---------------------------------------------------------------------------
void CNcdReport::SetAccessPoint( const TCatalogsConnectionMethod& aAp )
    {
    DLTRACEIN((""));    
    iAccessPoint = aAp;
    DLMETHOD( iAccessPoint );
    }

// ---------------------------------------------------------------------------
// Access point getter
// ---------------------------------------------------------------------------
const TCatalogsConnectionMethod& CNcdReport::ConnectionMethod() const
    {
    DLTRACEIN((""));   
    DLMETHOD( iAccessPoint );
    return iAccessPoint;        
    }
    

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const CNcdNodeIdentifier& CNcdReport::MetadataId() const
    {
    return *iMetadataId;
    }
    

// ---------------------------------------------------------------------------
// TimeStamp setter
// ---------------------------------------------------------------------------
void CNcdReport::SetTimeStamp( 
    const TNcdReportTimeStamps& aTimeStamp,
    const TTime& aTime )
    {
    DLTRACEIN(("aTimeStamp: %d, time: %Ld", aTimeStamp, aTime.Int64() ));    
    DASSERT( aTimeStamp < iTimeStamps.Count() );
    iTimeStamps[aTimeStamp] = aTime.Int64();
    }
    

// ---------------------------------------------------------------------------
// TimeStamp getter
// ---------------------------------------------------------------------------
const TInt64& CNcdReport::TimeStamp( 
    const TNcdReportTimeStamps& aTimeStamp ) const
    {
    DLTRACEIN(("aTimeStamp: %d", aTimeStamp));    

    DASSERT( aTimeStamp < iTimeStamps.Count() );
    DLTRACEOUT(("Time: %Ld", iTimeStamps[aTimeStamp] ));
    return iTimeStamps[aTimeStamp];
    }


// ---------------------------------------------------------------------------
// Reset the timestamp array
// ---------------------------------------------------------------------------
void CNcdReport::ResetTimeStampsL()
    {
    DLTRACEIN((""));
    iTimeStamps.Reset();
    iTimeStamps.ReserveL( ENcdReportTimeInternal );
    
    TInt64 zero = 0;
    for ( TInt i = 0; i < ENcdReportTimeInternal; ++i )
        {
        iTimeStamps.Append( zero );
        }
    DLTRACEOUT(("Time stamps reseted"));
    }

// ---------------------------------------------------------------------------
// Transaction getter
// ---------------------------------------------------------------------------
const MCatalogsHttpOperation* CNcdReport::ReportTransaction() const
    {
    return iReportTransaction;
    }


// ---------------------------------------------------------------------------
// Transaction setter
// ---------------------------------------------------------------------------
void CNcdReport::SetReportTransaction( 
    MCatalogsHttpOperation* aTransaction)
    {
    iReportTransaction = aTransaction;
    }


// ---------------------------------------------------------------------------
// Sets time stamp to current time
// ---------------------------------------------------------------------------
void CNcdReport::SetCurrentTime( const TNcdReportTimeStamps& aTime )
    {
    DLTRACEIN(("aTime: %d", aTime));
        
    TTime time;
    time.UniversalTime();
    SetTimeStamp( aTime, time );
    }


// ---------------------------------------------------------------------------
// Calculates difference between given times in seconds
// ---------------------------------------------------------------------------
TInt CNcdReport::DurationInSecondsL( 
    const TNcdReportTimeStamps& aStart,
    const TNcdReportTimeStamps& aEnd ) const
    {
    DLTRACEIN((""));
    TTime start( TimeStamp( aStart ) );
    TTime end( TimeStamp( aEnd ) );
    TTimeIntervalSeconds duration;
    
    User::LeaveIfError( end.SecondsFrom( start, duration ) );
    DLTRACEOUT(("Duration: %d", duration.Int() ));
    return duration.Int();
    }


// ---------------------------------------------------------------------------
// Default implementation for CanBundle
// ---------------------------------------------------------------------------
TBool CNcdReport::CanBundle() const
    {
    DLTRACEIN((""));
    return EFalse;
    }


// ---------------------------------------------------------------------------
// Add report data
// ---------------------------------------------------------------------------
void CNcdReport::AddReportDataL( 
    CNcdRequestInstallation& /* aRequest */ ) const
    {
    DLTRACEIN((""));
    User::Leave( KErrNotSupported );    
    }

// ---------------------------------------------------------------------------
// PRIVATE METHODS
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CNcdReport::CNcdReport( 
    TNcdReportType aType, 
    CNcdReportManager& aReportManager,
    const TNcdReportStatusInfo& aStatus )
    : iType( aType ), 
      iReportManager( aReportManager ),
      iStatus( aStatus ),
      iReportId( KNcdReportNotSupported )
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CNcdReport::BaseConstructL( 
    const CNcdNodeIdentifier& aMetadataId )
    {
    DLTRACEIN((""));
    
    iMetadataId = CNcdNodeIdentifier::NewL( aMetadataId );
    ResetTimeStampsL();
    iAttributes = CNcdAttributes::NewL( ENcdReportAttributeInternal );
        
    DLTRACEOUT((""));
    }


// ---------------------------------------------------------------------------
// ReportManager
// ---------------------------------------------------------------------------
const CNcdReportManager& CNcdReport::ReportManager() const
    {
    return iReportManager;
    }

// ---------------------------------------------------------------------------
// ReportManager
// ---------------------------------------------------------------------------
CNcdReportManager& CNcdReport::ReportManager()
    {
    return iReportManager;
    }

