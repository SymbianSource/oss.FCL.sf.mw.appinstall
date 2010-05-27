/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface for MJavaUiHandler
*
*/




/**
 @file
 @publishedPartner
 @released
*/

class MJavaUiHandler
/**
 * This represents the User Interface handler for the installation of Java files.
 *
 * @publishedPartner
 * @released
 */
	{
	public:
		// Not OK, Incompatible
		/**
		 * Displays a dialog at the beginning of an installation with details about the SISX package
		 * being installed. This dialog must be shown in TrustedUI mode.
		 *
		 * @param aMIDletName		The MIDlet name.
		 * @param aMIDletVendor		The MIDlet vendor.
		 * @param aMIDletVersion	The MIDlet version.
		 * @param aCertificate		The end entity certificates used to sign the MIDlet.
		 * @return					ETrue if the user pressed the Yes button to continue the install,
		 *							EFalse if the user pressed the No button, aborting the installation.
		 */
		virtual TBool DisplayInstallDialogL(const TDesC& aMIDletName, const TDesC& aMIDletVendor,
											const TVersion& aMIDletVersion,
											const CSisCertificateInfo& aCertificate)=0; // **TRUSTED UI**

		// OK, Compatible
		/**
		 * Displays "This application cannot be trusted" dialog and asks user 
		 * to install it anyway or to cancel installation.
		 *
		 * @param aAppName The name of the application.
		 * @return ETrue to install anyway, EFalse to cancel installation.
		 */
		virtual TBool DisplayUntrustedQueryDialogL(const TDesC& aAppName) = 0;

		// OK, Compatible
		/**
		 * Informs user that a file is in use and so therefore cannot be 
		 * deleted. Return ETrue if the user wishes to continue anyway
		 *
		 * @param aAppName	The name of the application.
		 * @param aFileName	The name of the file in use.
		 * @return			ETrue if the user wishes to continue.
		 */
		virtual TBool FileInUseL(const TDesC& aAppName, const TDesC& aFileName) = 0;
	};

