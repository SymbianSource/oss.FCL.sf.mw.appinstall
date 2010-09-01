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
* CScrXmlParser - Used to retrieve details from xml files for creating database and 
* Software environment updates.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef XMLPARSER_H
#define XMLPARSER_H
#pragma warning(disable: 4786)
#pragma warning(disable: 4291)

#include <vector>
#include <string>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOM.hpp>

#include "toolsconf.h"

/**
* This template is used to cleanup memory by calling a function as pointed 
* by the first parameter as a function parameter. It can be used only used
* with functions which take a pointer to a pointer as an argument. This is 
* currently a limitation as the xerces library takes the same to release 
* its own memory.
*/

template <typename tFnPtr, typename tPtr> class fn_auto_ptr 
	{
	public:
		explicit fn_auto_ptr(tFnPtr aFnPtr, tPtr* aPtr)
			: iFnPtr(aFnPtr),
			iPtr(aPtr)
		{}

		~fn_auto_ptr() { iFnPtr(&iPtr); }
		
		tPtr *get() const throw()
			{
			return (iPtr); 
			}

	private:
		tFnPtr iFnPtr;
		tPtr* iPtr;
	};


template <typename tFnPtr, typename tPtr> class mem_fn_auto_ptr 
	{
	public:
		
		explicit mem_fn_auto_ptr(tFnPtr aFnPtr, tPtr aPtr)
			: iFnPtr(aFnPtr),
			iPtr(aPtr)
			{}
		
		~mem_fn_auto_ptr() { (iPtr->*iFnPtr)(); }
		
		tPtr operator->() const throw()
			{
			return (get()); 
			}

		tPtr get() const throw()
			{
			return (iPtr); 
			}

	private:
		tFnPtr iFnPtr;
		tPtr iPtr;
	};

template <typename tFnPtr> class static_fn_auto_ptr 
	{
	public:
		
		explicit static_fn_auto_ptr(tFnPtr aFnPtr)
			: iFnPtr(aFnPtr)
			{}
		
		~static_fn_auto_ptr() { &iFnPtr; }

	private:
		tFnPtr iFnPtr;
	};

namespace XmlDetails
{
	struct TScrEnvironmentDetails
			{
		public:
			TScrEnvironmentDetails()
			:iSifPluginUid(0),
			iInstallerSid(0),
			iExecutionLayerSid(0)
			{}
			
			class TLocalizedSoftwareTypeName
				{
			public:
				TLocalizedSoftwareTypeName()
				:iLocale(0)
				{}
								
				int iLocale;
				std::wstring iName;
				};
			
			std::wstring iUniqueSoftwareTypeName;
			std::vector<TLocalizedSoftwareTypeName> iLocalizedSoftwareTypeNames;
			int iSifPluginUid;
			int iInstallerSid;
			int iExecutionLayerSid;
			std::vector<std::wstring> iMIMEDetails;
			};

		
	class TScrPreProvisionDetail
		{
	public:
		TScrPreProvisionDetail()
		{}

		class TComponentLocalizable
			{

		public:
			TComponentLocalizable()
			:iLocale(0)
			{}
			
			int iLocale;
			std::wstring iName;
			std::wstring iVendor;
			};
		
		class TComponentProperty
			{

		public:
			TComponentProperty()
			:iLocale(0),
			iIsIntValue(false),
			iIsStr8Bit(false)
			{}

		public:
			std::wstring iName;
			int iLocale;
			std::wstring iValue;
			bool iIsIntValue;
			int iIsStr8Bit;
			};

		class TComponentDependency
			{
		public:
			class TComponentDependencyDetail
				{
				public:
					std::wstring iSupplierId;
					std::wstring iFromVersion;
					std::wstring iToVersion;
				};

			public:
				std::wstring iDependentId;
				std::vector<TComponentDependencyDetail>	iComponentDependencyList;
			};

		class TComponentFile
			{
			public:
			TComponentFile()
			{}

			struct TFileProperty
				{	
			public:
				TFileProperty()
				:iIsIntValue(false)
				{}
				std::wstring iName;
				std::wstring iValue;
				bool iIsIntValue;
				};

			std::wstring iLocation;
			std::vector<TFileProperty> iFileProperties;
			
			};
		
		class TComponentDetails
			{
			public:
			TComponentDetails()

			:iIsRemovable(1),
			iSize(0),
			iScomoState(1),
			iOriginVerified(1),
			iIsHidden(0)
			{}

			class TVersion
				{
			public:

				TVersion()
				{}

			public:
				std::wstring iMajor;
				std::wstring iMinor;
				std::wstring iBuild;
				};

			int iIsRemovable;
			__int64 iSize;
			int iScomoState;
			std::wstring iGlobalId;
			TVersion iVersion;
			int iOriginVerified;
			int iIsHidden;
			};

		class TComponent
			{
			public:
			TComponent()
			:iComponentDetails()
			{}

			std::vector<TComponentLocalizable>	iComponentLocalizables;
			std::vector<TComponentProperty>		iComponentProperties;
			std::vector<TComponentFile>			iComponentFiles;
			TComponentDependency iComponentDependency;
			TComponentDetails iComponentDetails;
			};

		std::wstring iSoftwareTypeName;
		std::vector<TComponent> iComponents;
		
		};
}

class CScrXmlParser
	{

	public:
		
		/**
		 * Initializes the xml parser and logging feature based on supplied logging parameters.
		 */
		DllExport CScrXmlParser();
		
		/**
		 * Frees allocated memory.
		 */
		DllExport ~CScrXmlParser();
		
		/**
		 * Retrieves the database schema for database creation, from the supplied database file. 
		 */
		DllExport std::vector<std::string>* ParseDbSchema(const std::string& aDbFile);
		
		/**
		 * Retrieves software environment details, from the supplied xml file. 
		 */
		DllExport std::vector<XmlDetails::TScrEnvironmentDetails>* GetEnvironmentDetails(const std::string aEnvDetailFile );
		
		XmlDetails::TScrPreProvisionDetail GetPreProvisionDetails( const std::string aPreProvisionFile );

	private:
		
		/**
		 * Called while retrieving s/w environment details. 
		 */

		XmlDetails::TScrEnvironmentDetails GetEnvironmentData( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponent GetPreProvisionData( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentLocalizable GetComponentLocalizable( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentProperty GetComponentProperty( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentFile GetComponentFile( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);

		XmlDetails::TScrPreProvisionDetail::TComponentDependency GetComponentDependency( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty GetFileProperty( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrPreProvisionDetail::TComponentDetails GetComponentDetails( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		XmlDetails::TScrEnvironmentDetails::TLocalizedSoftwareTypeName GetLocalizedSoftwareTypeName( const XERCES_CPP_NAMESPACE::DOMElement* aDOMElement);
		
		void ConfigDomParser(xercesc::XercesDOMParser& aDomParser);
	};


XERCES_CPP_NAMESPACE_BEGIN

/**
 * 
 * SchemaErrorHandler - Implementation of the error handler for xml schema validation.
 *						The parser used, implements this customized error handling interface.
 *						This has been registered with the xml parser in the constructor. The parser 
 *						reports all errors and warnings through this interface.
 */

class SchemaErrorHandler : public ErrorHandler
	{
	public:
		/**
		 * Handles all warnings received while xml parsing. 
		 */
		void warning (const SAXParseException &exc);
		
		/**
		 * Handles all errors received while xml parsing. 
		 */
		void error (const SAXParseException &exc);
		
		/**
		 * Handles all fatal errors received while xml parsing. 
		 */
		void fatalError (const SAXParseException &exc);
		
		/**
		 * Called if an error occurs while reintializing the current xml handler. 
		 */
		void resetErrors ();
	};

XERCES_CPP_NAMESPACE_END

#endif // _XMLPARSER_H
