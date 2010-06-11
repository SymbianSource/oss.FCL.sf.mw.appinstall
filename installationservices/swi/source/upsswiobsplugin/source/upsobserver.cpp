/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements base UPS SWI Observer ECOM plug-in interface.
*
*/


/**
 @file
 @internalComponent
 @prototype
*/
 
#include "upsobserver.h"


static const TUint KUpsSwiObserverImplementationId = 0x10283726;

static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUpsSwiObserverImplementationId, CUpsObserver::CreateUpsObserverL)
	};
	
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
	
//
//CUpsObserver
//

CUpsObserver::CUpsObserver()
/**
	Constructor.
 */
:	CSwiObserverPlugin()
 	{
 	//empty
 	}
 	

CUpsObserver::~CUpsObserver()
/**
	Destructor
 */
	{
	DEBUG_PRINTF(_L("UPS Observer Destroyed"));
	iSidList.Close();
	iUpsManager.Close();
	}
	

CSwiObserverPlugin* CUpsObserver::CreateUpsObserverL()
/**
	Factory method that instantiates a new SWI Observer ECOM plug-in.

	@return A pointer to the new UPS SWI Observer object.
*/
	{
	DEBUG_PRINTF(_L("UPS Observer Created"));
	CUpsObserver *uo = new(ELeave) CUpsObserver();
	return uo;
	}
	

void CUpsObserver::DoCancel()
// From CActive
	{
	DEBUG_PRINTF(_L("UPS Observer Cancelled"));
	if(iNotifyFlag & EPolicyChanged)
		{
		iUpsManager.CancelNotifyPolicyFilesChanged();
		}
	}
		
		
void CUpsObserver::RunL()
// From CActive
	{
	User::RequestComplete(iClientStatus,iStatus.Int());
	}
	
		
TInt CUpsObserver::RunError(TInt aError)
// From CActive
	{
	DEBUG_PRINTF2(_L8("Received error number is %d"),aError);
	User::RequestComplete(iClientStatus,aError);
	return KErrNone;
	}
	

void CUpsObserver::StartL(RFs& aFs)
/**
	Opens a management session to the User Prompt Service.
	This function is called when the SWI Observer processor starts processing
	an observation log file.
	
	@param aFs File server session handle.
 */
	{
	DEBUG_PRINTF(_L("UPS Observer Started"));
	iFs = &aFs;
	User::LeaveIfError(iUpsManager.Connect());
	iNotifyFlag = 0;
	}
 	
 		
void CUpsObserver::NextObservedHeaderL(const CObservationHeader& aHeader, TRequestStatus& aStatus)
/**
	Since UPS does not care the (un)installation of packages, this function does nothing. 
	Simply completes the caller.
	
	@param aHeader Object containing all observation data related with a specific package.
	@param aStatus The request status object.
 */
	{
	(void)aHeader;
	
	iClientStatus = &aStatus;
	User::RequestComplete(iClientStatus,KErrNone);
	}
 
 
void CUpsObserver::NextObservedDataL(const CObservationData& aData, TInt aFilterIndex, TRequestStatus& aStatus)
/**
	Saves the required information from each notification.
	This function is called for each actual observation data (i.e. adding/deleting file).
	Please note that the UPS manager is not notified here.
	
	@param aData   		Object for a single observation data.
	@param aFilterIndex The index of the filter which matches with that notification.
	@param aStatus 		The request status object.
 */
	{
	TFilterIndex idx = static_cast<TFilterIndex>(aFilterIndex);
	
	switch(idx)
		{
		case EFilterSysBin:
			{
			TInt ret = 0;
			if(aData.iFileFlag & Swi::EFileDeleted)
				{
				//If exe/dll has been deleted, add its SID into the removing decision list
				ret = iSidList.InsertInUnsignedKeyOrder(aData.iSid);
				if(KErrNone != ret && KErrAlreadyExists != ret)
					{
					User::Leave(ret);
					}
				}
			else if(aData.iFileFlag & Swi::EFileAdded)
				{
				//If exe/dll has been added ot the system, check the delete list to see
				//whether an exe/dll with the same SID had already been deleted.
				//If it is found in the delete list, it means that that exe/dll is being
				//upgraded. Since we keep decisions of upgraded exe's, the found
				//SID is removed from the list containing the SIDs whose decisions
				//will be deleted.
				ret = iSidList.FindInUnsignedKeyOrder(aData.iSid);
				if(KErrNotFound != ret)
					{
					iSidList.Remove(ret);
					}
				}
			break;
			}
		case EFilterPlugins:
			{
			iNotifyFlag |= EPluginChanged;
			break;
			}
		case EFilterPolicies:
			{
			iNotifyFlag |= EPolicyChanged;
			break;
			}
		default:
			{
			DEBUG_PRINTF(_L("Received an unexpected notification."));
			break;
			}
		}
	
	iClientStatus = &aStatus;
	User::RequestComplete(iClientStatus,KErrNone);
	}
	
	
void CUpsObserver::FinalizeL(TRequestStatus& aStatus)
/**
	Sends the required notifications to the UPS Manager.
	
	This function is called when the SWI Observer processor is finished with
	the observation log file.
	
	@param aStatus 		The request status object.
 */
 	{
 	DEBUG_PRINTF(_L("UPS Observer Finalized"));
 	iClientStatus = &aStatus;
 	
 	TInt count;
 	count = iSidList.Count();
 	//Delete decisions matching exe/dll SID
 	for(TInt i=0; i<count; ++i)
 		{
 		iUpsManager.DeleteDecisionsForExeL(iSidList[i]);
 		}
 	
 	if(iNotifyFlag & EPluginChanged)
 		{
 		//Notify UPS that a DLL (which might be an ECOM plugin for the UPS) has been 
 		//added/deleted/changed on the system drive.
 		iUpsManager.NotifyPluginsMayHaveChangedL();
 		}
 	
 	if(iNotifyFlag & EPolicyChanged)
 		{
 		//Notify UPS that a policy file has been added/deleted/changed
 		iUpsManager.NotifyPolicyFilesChanged(iStatus);
 		*iClientStatus = KRequestPending;
 		SetActive();
 		}
 	else
 		{
		User::RequestComplete(iClientStatus,KErrNone);
 		}	
  	}
 
 
CObservationFilter* CUpsObserver::GetFilterL() const
/**
	Returns a list of filters which representing in what folders the ups observer
	plugin is interested. 
	
	The Ups observer plugin needs to know any change in the Sys\Bin and \Resource\Plugins
	directories of the system drive. In addition, the upgrade notifications of ups policy 
	files are required by this plugin.

	@return A list of filters showing the interest of this plugin.
 */
 	{
 	CObservationFilter *filterList = CObservationFilter::NewLC();
 	AddFilterL(*filterList, KFilterSysBin);
 	AddFilterL(*filterList, KFilterPlugins);
 	AddFilterL(*filterList, KFilterPolicies);
 	CleanupStack::Pop(filterList);
 	return filterList;
 	}
	

void CUpsObserver::AddFilterL(CObservationFilter& aFilterList, const TDesC& aFilter) const
/**
	Inserts a new filter into the filter list.
	Sets the first character of the new filter as the current system drive.
	
	@param aFilterList A filter list object.
	@param aFilter	   A filter data which will be inserted.
 */
	{
	HBufC* filterData = aFilter.AllocLC();
	TPtr ptr(filterData->Des());
	ptr[0] = iFs->GetSystemDriveChar();
	aFilterList.AddFilterL(filterData);
	CleanupStack::Pop(filterData);
	}

