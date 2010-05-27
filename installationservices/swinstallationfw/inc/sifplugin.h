/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file defines a SIF plug-in interface.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef SIFPLUGIN_H
#define SIFPLUGIN_H

#include <e32cmn.h>
#include <f32file.h>
#include <usif/sif/sifcommon.h>

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

namespace Usif
	{
	/** ECOM SIF plug-in Uid */
	const TUid PluginEcomUid = {0x10285BC2};

	/**
	An abstract SIF plug-in interface. It implements the SIF software management
	services interfaces for a particular software type. 

	The SIF Server uses a SIF plug-in when a package of a type it supports
	is to be processed.

	Each method in this interface provides a security context. An installer may
	use it in order to verify client's identity and capabilities. For example, a Java
	installer may require the ECapabilityWriteUserData capability to activate or
	deactivate a MIDlet.

	Complex installers that need to be implemented as stand-alone servers may use
	the SIF Transport library in order to simplify sending/receiving SIF requests
	across a process boundary.
	*/
	class CSifPlugin : public CBase
		{
	public:

		/**
		A derived class must provide an implementation that returns details of a component
		to be installed by file name.
		
		@param aFileName Component's file name
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aComponentInfo On return, contains component's details
		@param aStatus Asynchronous completion status
		*/
		virtual void GetComponentInfo(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
									CComponentInfo& aComponentInfo, TRequestStatus& aStatus) = 0;
		
		/**
		A derived class must provide an implementation that returns details of a component
		to be installed by file handle.
		
		@param aFileHandle Component's file name
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aComponentInfo On return, contains component's details
		@param aStatus Asynchronous completion status
		*/
		virtual void GetComponentInfo(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
									CComponentInfo& aComponentInfo, TRequestStatus& aStatus) = 0;

		/**
		A derived class must provide an implementation that installs a component by file name.
		
		@param aFileName Component's file name
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aArguments The array of opaque params for the plug-in. An empty
		array may be passed.  The following param is defined for the "SCOMO Install
		Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		If a plug-in receives this param, it must install a component normally but the SCOMO
		State should remain EDeactivated.
		@param aResults The array of opaque params returned from the plug-in.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		Each plug-in must add this param to aResults after a successful installation.
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus Asynchronous completion status
		*/
		virtual void Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aArguments, COpaqueNamedParams& aResults,
							TRequestStatus& aStatus) = 0;

		/**
		A derived class must provide an implementation that installs a component by file handle.
		
		@param aFileHandle Component's file handle
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aArguments The array of opaque params for the plug-in. An empty
		array may be passed.  The following param is defined for the "SCOMO Install
		Inactive" operation:
		Name: InstallInactive, Type: Int, Value: ETrue
		If a plug-in receives this param, it must install a component normally but the SCOMO
		State should remain EDeactivated.
		@param aResults The array of opaque params returned from the plug-in.
		The following return param is defined for the id of an installed component:
		Name: ComponentId, Type: Int
		Each plug-in must add this param to aResults after a successful installation.
		For packages containing embedded components, the returned param contains
		the id of the last/most embedded component.
		@param aStatus Asynchronous completion status
		*/
		virtual void Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
							const COpaqueNamedParams& aArguments, COpaqueNamedParams& aResults,
							TRequestStatus& aStatus) = 0;

		/**
		A derived class must provide an implementation that uninstalls a component.
		
		@param aComponentId The id of a component to be uninstalled
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aArguments The array of opaque params for the plug-in. An empty
		array may be passed.
		@param aResults The array of opaque results returned from the plug-in.
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		virtual void Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aArguments, COpaqueNamedParams& aResults,
								TRequestStatus& aStatus) = 0;

		/**
		A derived class must provide an implementation that activates a component.

		@param aComponentId The id of a component to be activated
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		virtual void Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								TRequestStatus& aStatus) = 0;

		/**
		A derived class must provide an implementation that deactivates a component.

		@param aComponentId The id of a component to be deactivated
		@param aSecurityContext this param allows the plug-in to verify client's identity
		@param aStatus The request status. KErrNone, if successful, otherwise
		one of the other system-wide or SIF error codes defined in usiferror.h.
		*/
		virtual void Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								TRequestStatus& aStatus) = 0;

		/**
		A derived class must provide an implementation that cancels an ongoing asynchronous request.
		
		This is a synchronous call. When it returns the original asynchronous call is completed.
		*/
		virtual void CancelOperation() = 0;

		virtual ~CSifPlugin() {}
		};
	} // namespace Usif

#endif // SIFPLUGIN_H
