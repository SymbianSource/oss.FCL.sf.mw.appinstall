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
* Description:
*     CNetworkStatusListener definition
*
*
*/

#include <e32std.h>
#include <e32debug.h>
#include <rconnmon.h>

#undef USE_LOGFILE
#include "networkstatuslistener.h"
#include "networkstatusobserver.h"

#ifdef USE_LOGFILE
#define LOG( aMsg ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg ); RDebug::Print( KMsg ); }
#define LOG2( aMsg, aParam1 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1 ); RDebug::Print( KMsg, aParam1 ); }
#define LOG3( aMsg, aParam1, aParam2 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1, aParam2 ); RDebug::Print( KMsg, aParam1, aParam2 ); }
#define LOG4( aMsg, aParam1, aParam2, aParam3 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1, aParam2, aParam3 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3 ); }
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1, aParam2, aParam3, aParam4 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4 ); }
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5 ); }
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 ); }
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ) { _LIT(KMsg, aMsg); iObserver.Log( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); RDebug::Print( KMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 ); }
#else
#define LOG( a )
#define LOG2( a, b )
#define LOG3( a, b, c )
#define LOG4( a, b, c, d )
#define LOG5( aMsg, aParam1, aParam2, aParam3, aParam4 )
#define LOG6( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5 )
#define LOG7( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 )
#define LOG8( aMsg, aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 )
#endif

//-----------------------------------------------------------------------------
// CNetworkStatusListener::NewLC
//-----------------------------------------------------------------------------
//
CNetworkStatusListener* CNetworkStatusListener::NewLC(MNetworkStatusObserver& aObserver, const TUint aIapId)
	{
	CNetworkStatusListener* self = new (ELeave) CNetworkStatusListener(aObserver, aIapId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//-----------------------------------------------------------------------------
// CNetworkStatusListener::NewL
//-----------------------------------------------------------------------------
//
CNetworkStatusListener* CNetworkStatusListener::NewL(MNetworkStatusObserver& aObserver, const TUint aIapId)
	{
	CNetworkStatusListener* self = CNetworkStatusListener::NewLC(aObserver, aIapId);
	CleanupStack::Pop(self);
	return self;
	}

//-----------------------------------------------------------------------------
// CNetworkStatusListener::CNetworkStatusListener
//-----------------------------------------------------------------------------
//
CNetworkStatusListener::CNetworkStatusListener(MNetworkStatusObserver& aObserver, const TUint aIapId) :
		iObserver(aObserver),
		iIapId(aIapId)
	{
	LOG2( "CNetworkStatusListener::CNetworkStatusListener(): id=%d", iIapId );
	}

//-----------------------------------------------------------------------------
// CNetworkStatusListener::~CNetworkStatusListener()
//-----------------------------------------------------------------------------
//
CNetworkStatusListener::~CNetworkStatusListener()
	{
	LOG( "CNetworkStatusListener::~CNetworkStatusListener()" );
	iAvailableIAPs.Close();
	iConnMon.CancelNotifications();
	iConnMon.Close();
	}

//-----------------------------------------------------------------------------
// CNetworkStatusListener::ConstructL
//-----------------------------------------------------------------------------
//
void CNetworkStatusListener::ConstructL()
	{
	User::LeaveIfError( iConnMon.ConnectL() );
/*
	// Set KBearerAvailabilityThreshold to 1 so we can get
	// EConnMonBearerAvailabilityChange events.
	// If this fails, we cannot get EConnMonBearerAvailabilityChange events
	iConnMon.SetUintAttribute( EBearerWLAN, 0,
			KBearerAvailabilityThreshold, 1 );
*/
	iConnMon.NotifyEventL( *this );
	}

//-----------------------------------------------------------------------------
// CNetworkStatusListener::EventL
//-----------------------------------------------------------------------------
//
void CNetworkStatusListener::EventL( const CConnMonEventBase& aEvent )
	{
	LOG3( "+ CNetworkStatusListener::EventL(): type=%d, connId=%d", aEvent.EventType(), aEvent.ConnectionId() );

	switch( aEvent.EventType() )
		{
		case EConnMonConnectionStatusChange:
			{
			// check what WLAN connection available and act accordingly
			LOG( "CNetworkStatusListener::EventL(): EConnMonConnectionStatusChange" );

			const CConnMonNetworkStatusChange* event =
					static_cast<const CConnMonNetworkStatusChange*>(&aEvent);
			const TUint connectionId = event->ConnectionId();
			TUint iap = 0;
			const TInt ret = GetIapId(connectionId, iap);

			LOG5( "CNetworkStatusListener::EventL(): IAP=%d (%d), connId=%d, status=%d", iap, ret, connectionId, event->NetworkStatus() );
			break;
			}
		case EConnMonIapAvailabilityChange:
			{
			LOG( "CNetworkStatusListener::EventL(): EConnMonIapAvailabilityChange" );
			const CConnMonIapAvailabilityChange* event =
				static_cast<const CConnMonIapAvailabilityChange*>(&aEvent);

			UpdateIapArray( event->IapAvailability() );
			}
			break;
#if 0
		case EConnMonDownlinkDataThreshold:
		case EConnMonNetworkRegistrationChange: // roaming, not roaming
		case EConnMonBearerAvailabilityChange:
		case EConnMonOldWLANNetworkLost:
			{
			LOG2( "CNetworkStatusListener::EventL(): Unprocessed event %d", aEvent.EventType() );
			// not interested
			break;
			}
#endif
		default:
			// unrecognized events go here, not interested in those, either
			break;
		} // switch

	// Send the event only if we have a valid IAP set here.
	// If the IAP is invalid, the user IAP selection has not yet been
	// send to CNetworkStatusListener.
	if ( iIapId != KInvalidIapId && !IsIapAvailable(iIapId) )
		{
		LOG2( "Monitored IAP %d not available", iIapId );
		iObserver.HandleNetworkStatusChangedL(EConnMonStatusNotAvailable);
		}

	}

// ---------------------------------------------------------------------------
// Updates the IAP ID array
// ---------------------------------------------------------------------------
//
void CNetworkStatusListener::UpdateIapArray(TConnMonIapInfo aIaps)
	{
	LOG3( "+ UpdateIapArray(): now=%d, new=%d", iAvailableIAPs.Count(), aIaps.iCount );

	iAvailableIAPs.Reset();

	for (TUint count = 0; count < aIaps.iCount; count++)
		{
		LOG2( "UpdateIapArray: ID %d", aIaps.iIap[ count ].iIapId );
		iAvailableIAPs.Append( aIaps.iIap[ count ].iIapId );
		}
	}

// ---------------------------------------------------------------------------
// Updates the list of IAPs and checks, if requested IAP is available.
// @param aIapId IAP ID which availability is checked.
// ---------------------------------------------------------------------------
//
TBool CNetworkStatusListener::IsIapAvailable(const TUint aIapId)
	{
	TRequestStatus status = KRequestPending;
	TConnMonIapInfoBuf iapsBuf;
	iConnMon.GetPckgAttribute( EBearerIdAll,
			0,
			KIapAvailability,
			iapsBuf,
			status );
	User::WaitForRequest( status );
	if ( status.Int() == KErrNone )
		{
		UpdateIapArray( iapsBuf() );
		}

	return ( KErrNotFound == iAvailableIAPs.Find( aIapId ) )
		? EFalse : ETrue;
	}

//-----------------------------------------------------------------------------
// CNetworkStatusListener::SetIapIdToMonitor
//
// Sets the IAP ID whose connections this object monitors
//-----------------------------------------------------------------------------
//
void CNetworkStatusListener::SetIapIdToMonitor(const TInt aIAP)
	{
	LOG2( "CNetworkStatusListener: Monitoring IAP %d", aIAP );
	iIapId = aIAP;
	//iConnections.Reset();
	//GetConnectionsForIap(iConnMon, iIapId, iConnections);
	// Update the IAP availability list
	IsIapAvailable(aIAP);
	}

//-----------------------------------------------------------------------------
// Gets the IAP ID for specific connection. Synchronized version.
//
// @param aConnId Connection ID
// @param aIapId IAP ID for specified connection (out).
// @return Standard Symbian error code
//-----------------------------------------------------------------------------
//
TInt CNetworkStatusListener::GetIapId(const TUint aConnId, TUint& aIapId)
	{
	TRequestStatus status = KRequestPending;
	iConnMon.GetUintAttribute( aConnId, 0, KIAPId, aIapId, status );
	User::WaitForRequest( status );
	return status.Int();
	}
