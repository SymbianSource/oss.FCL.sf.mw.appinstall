/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* xmlparser.h
* CScrXmlParser - Used to retrieve details from xml files for creating database and 
* Software environment updates.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef XMLGENERATOR_H
#define XMLGENERATOR_H
#pragma warning(disable: 4786)
#pragma warning(disable: 4291)

#include <vector>
#include <string>

#include "xmlparser.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/dom/DOM.hpp>

#include "parameterlist.h"

using namespace std;

#define AppRegistrationInfo XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo
#define ApplicationAttribute XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppAttribute 
#define AppDataType XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TDataType 
#define AppServiceInfo XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppServiceInfo 
#define AppLocalizableInfo XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo
#define AppLocalizableAttribute XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TLocalizableAttribute 
#define AppViewData XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData 
#define AppViewDataAttributes XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppLocalizableInfo::TViewData::TViewDataAttributes 
#define AppProperty XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TAppProperty 
#define AppOpaqueDataType XmlDetails::TScrPreProvisionDetail::TApplicationRegistrationInfo::TOpaqueDataType
#define ComponentLocalizable XmlDetails::TScrPreProvisionDetail::TComponentLocalizable
#define ComponentProperty XmlDetails::TScrPreProvisionDetail::TComponentProperty
#define ComponentFile XmlDetails::TScrPreProvisionDetail::TComponentFile
#define FileProperty XmlDetails::TScrPreProvisionDetail::TComponentFile::TFileProperty
#define ComponentDependency XmlDetails::TScrPreProvisionDetail::TComponentDependency
#define ComponentDependencyDetail XmlDetails::TScrPreProvisionDetail::TComponentDependency::TComponentDependencyDetail


class CXmlGenerator
	{

	public:
		
		/**
		 * Initializes the xml parser and logging feature based on supplied logging parameters.
		 */
		CXmlGenerator();
		
		/**
		 * Frees allocated memory.
		 */
		~CXmlGenerator();
		
		/**
		 * Retrieves software environment details, from the supplied xml file. 
		 */
		void WritePreProvisionDetails(const std::wstring aXmlFileName, 
											const XmlDetails::TScrPreProvisionDetail& aPreProvisionDetail,
											int& aRomApplication
										);
		
		
	private:	

		void WriteComponent(	XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
								XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
								const XmlDetails::TScrPreProvisionDetail::TComponent& aComponent,
								int& aRomApplication
							);

		void WriteComponentVersion	(	
										XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
										XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
										XmlDetails::TScrPreProvisionDetail::TComponentDetails::TVersion aVersion
									);
		
		void WriteComponentLocalizables	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<ComponentLocalizable>& aComponentLocalizable 
					);

		void WriteComponentProperties	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<ComponentProperty>& aComponentProperties 
					);

		void WriteAppRegInfo	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppRegistrationInfo>& aAppRegInfo 
					);


		void WriteAppAttributes	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<ApplicationAttribute>& aAppAttributes 
					);

		void WriteAppProperty	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppProperty>& aAppProperty 
					);

		void WriteAppServiceInfo 
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppServiceInfo>& aAppServiceInfo,
						const std::vector<AppOpaqueDataType>& aAppOpaqueData
					);

		void WriteAppDataType 
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppDataType>& aAppDataType 
					);

		void WriteAppLocalizableInfo 
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppLocalizableInfo>& aAppLocalizableInfo 
					);

		void WriteAppLocalizableAttribute 
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppLocalizableAttribute>& aAppLocalizableAttribute 
					);

		void WriteAppLocalizableViewData 
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppViewData>& aAppViewData 
					);

		void WriteAppLocalizableViewDataAttributes 
					 ( 
 						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<AppViewDataAttributes>& aAppViewDataAttributes 
					 );

		void WriteComponentFiles	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<ComponentFile>& aComponentFiles 
					);

		void WriteComponentDependencies	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const ComponentDependency& aComponentDependency 
					);

		void WriteFileProperties	
					( 
						XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
						XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
						const std::vector<FileProperty>& aFileProperties 
					);

		void AddChildElement(	
								XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
								XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
								const XMLCh* aElementName, 
								const XMLCh* aTextValue 
							);

		XERCES_CPP_NAMESPACE::DOMElement* AddTag	
							(	
								XERCES_CPP_NAMESPACE::DOMElement* aRootElement, 
								XERCES_CPP_NAMESPACE::DOMDocument* aDocument, 
								const XMLCh* aTagName
							);

		void SetWriterFeatures(XERCES_CPP_NAMESPACE::DOMWriter* aDomWriter);
	};


XERCES_CPP_NAMESPACE_BEGIN

class SchemaDomErrorHandler : public DOMErrorHandler
	{
	public:
		bool handleError(const DOMError& domError);
	};

XERCES_CPP_NAMESPACE_END


#endif // XMLGENERATOR_H
