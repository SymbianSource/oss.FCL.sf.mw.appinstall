/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "certificatechain.h"
using namespace std;

CertificateChain::CertificateChain()
	:iDevCertificate (false) ,
	iDevCapabilities (0) ,
	iValidationStatus (EValidationSuccessful) 
	{
	}

bool CertificateChain::ValidateChain(const Options& aOptions , const string& aFile , const SWICertStore& aStore )
	{
	X509* endEntity = NULL ;
	X509* previous = NULL ; 
	X509* current = NULL;
	ASN1_UTCTIME* from = NULL ;
	ASN1_UTCTIME* to = NULL;
	char buf[KLength];
	int count = 0;
	int verify = 0;
	string issuer ; 
	bool isValid = false ;
	const char* fileName = aFile.c_str();

	FILE *fp = fopen(fileName,"r");
	if(fp == NULL)
		{
		throw EUnableToOpenFile ; 
		}
	do
		{
		current = PEM_read_X509 (fp, NULL, NULL, NULL);
		//end entity will hold the first certificate in the chain which signs the sis file.
		if(count == 0 && current)
			{
			endEntity = current;
			}

		if(current)
			{
			X509_NAME_oneline (X509_get_issuer_name (current) , buf , KLength);
			iCertificateChain.push_back(buf);
			X509_NAME_oneline (X509_get_subject_name (current), buf , KLength);
			iCertificateChain.push_back(buf);
											
			from = X509_get_notBefore (current);			
			to = X509_get_notAfter (current);
			ExtractCertValidityPeriod(*from , *to);

			//validity period status
			int validFrom = X509_cmp_current_time(from);
			int validTo = X509_cmp_current_time(to);

			//check whether "Valid From" is less than the current time and 
			//"Valid To" is greater than current time for each certificate in the chain.
			//If check fails,current time is not within certificate's validity period.
			if(validFrom >= 0 || validTo <= 0)
				{
				iValidationStatus = iValidationStatus | EValidityCheckFailure ;
				}

			//count is greater than one when there is more than 1 certificate in the chain	
			//so that the public key of the next certificate is used to verify the signature of 
			//previous certificate in the chain.
			if(count)
				{
				EVP_PKEY* pubKey = X509_get_pubkey(current);
				if (!pubKey)
					{
#ifdef DUMPCHAINVALIDITYTOOL
					cout << "no public key in certificate" << endl;
#endif
					fclose(fp);
					return false;
					}

				//Signature validation.
				if(count == 1)
					{
					verify = X509_verify(endEntity,pubKey);
					}

				else
					{
					verify = X509_verify(previous,pubKey);
					}

				if(!verify)
					{
					iValidationStatus = iValidationStatus | EBrokenChain ;
					}
				
				if(previous)
					{
					X509_free (previous);
					}
				
				previous = current;
				}

			count++;
			}
		}while(current);

	fclose(fp);
	CheckForChainValidity(iValidationStatus);

	//check whether end entity is devcert,self signed or others.
	iDevCertificate = CheckIfDevCert(*endEntity);
	if(iDevCertificate)
		{
#ifdef DUMPCHAINVALIDITYTOOL
		cout << "SIS File is signed with a Developer Certificate with Constraints :\n" << endl;
		cout << iDevConstraints << endl;
#endif
		}

	// end entity certificate is self signed
	else if(CheckIfSelfSigned(*endEntity))
		{
#ifdef DUMPCHAINVALIDITYTOOL
		cout << ": SIS File is signed by a Self Signed Certificate " << endl;
#endif
		}
	
	//check whether the last certificate in the chain is validated against the certstore.
	//if there is only 1 certificate in the chain .
	if(count == 1)
		{
		isValid = ValidateWithCertstore(*endEntity , aStore , issuer);
		}
	else 
		{
		isValid = ValidateWithCertstore(*previous , aStore , issuer);
		}

	if(previous)
		{
		X509_free (previous);
		previous = NULL ;
		}

	if(endEntity)
		{
		X509_free (endEntity);
		endEntity = NULL ; 
		}

	if(isValid)
		{
#ifdef DUMPCHAINVALIDITYTOOL	
		cout << "Is Validated By : " << issuer << endl << endl;
#endif
		return true;
		}

	return false;
	}

void CertificateChain::ExtractCertValidityPeriod(const ASN1_UTCTIME& aFrom , const ASN1_UTCTIME& aTo)
	{
	char date [12] = "xx/xx/xx";
	date [0] = aFrom . data [4];
	date [1] = aFrom . data [5];
	date [3] = aFrom . data [2];
	date [4] = aFrom . data [3];
	date [6] = aFrom . data [0];
	date [7] = aFrom . data [1];
	iCertificateChain.push_back(date);

	date [0] = aTo . data [4];
	date [1] = aTo . data [5];
	date [3] = aTo . data [2];
	date [4] = aTo . data [3];
	date [6] = aTo . data [0];
	date [7] = aTo . data [1];
	iCertificateChain.push_back(date);
	}

void CertificateChain::DisplaySigningChain(const StringVector& aSigningChain)
	{
	if(aSigningChain.size())
		{
		std::vector<std::string>::const_iterator chainIter = aSigningChain.begin();	
		do
			{
			cout << "Issued By : " << (*chainIter) << endl;
			++chainIter;
			cout << "Issued To : " << (*chainIter) << endl;
			++chainIter;
			cout << "Valid From : " << (*chainIter) << endl;
			++chainIter;
			cout << "Valid To : " << (*chainIter) << endl << endl;
			++chainIter;
			}while(chainIter != aSigningChain.end());	
		}
	}

bool CertificateChain::CheckIfDevCert(X509& aEndEntity) 
	{
	X509_EXTENSION* extension = NULL;
	ASN1_OCTET_STRING* extData = NULL;
	ASN1_OBJECT * asnObj = NULL;
	int	constraintCount = X509_get_ext_count(&aEndEntity);
	for(int k = 0 ; k < constraintCount ; k++)
		{
		extension = X509_get_ext(&aEndEntity, k);
		extData = X509_EXTENSION_get_data(extension);
		
		asnObj = X509_EXTENSION_get_object(extension);
		
		char buf[KLength];
		i2t_ASN1_OBJECT(buf, KLength, asnObj);
	
		//all the constraints in the vector are compared against the predefined ones.
		if(KDeviceIdListConstraint.compare(buf) == 0)
			{
			iDevConstraints.append("DeviceId Constraints :\n\n");
			string deviceId = ExtractDeviceConstaints(extData);
			//only if device id constraint is present in devcert,append value to iDevConstraints
			if(deviceId.length())
				{
				iDevConstraints.append(deviceId);
				}
			else
				{
				iDevConstraints.append("Empty Device Constraints List. \n");
				}
			}

		else if(KSidListConstraint.compare(buf) == 0)
			{
			iDevConstraints.append("\nSID Constraints :\n\n");
			string sid = ExtractASN1IntegerData(extData);
			if(sid.length())
				{
				iDevConstraints.append(sid);
				}
			else
				{
				iDevConstraints.append("Empty SID Constraints List. \n");
				}
			}

		else if(KVidListConstraint.compare(buf) == 0)
			{
			iDevConstraints.append("\nVID Constraints :\n\n");
			string vid = ExtractASN1IntegerData(extData);
			if(vid.length())
				{
				iDevConstraints.append(vid);
				}
			else
				{
				iDevConstraints.append("Empty VID Constraints List.\n");
				}
			}

		else if(KCapabilitiesConstraint.compare(buf) == 0)
			{
			iDevConstraints.append("\nCapabilities Constraints :\n\n");
			string capabilities = ExtractCapabilities(extData);
			if(capabilities.length())
				{
				iDevConstraints.append(capabilities);
				}
			else
				{
				iDevConstraints.append("Empty Capabilitites Constraints List. \n");
				}
			}
		}
	
	//if there are constaints with oid matching the predefined ones,they are devcerts
	if(iDevConstraints.length())
		{
		return true;
		}

	return false;
	}

bool CertificateChain::CheckIfSelfSigned(X509& aEndEntity) 
	{
	char buf[KLength] ;
	string issuer ;
	string subject ;
	X509_NAME_oneline (X509_get_issuer_name (&aEndEntity), buf, KLength);
	issuer = buf;
	X509_NAME_oneline (X509_get_subject_name (&aEndEntity), buf, KLength);
	subject = buf;

	if((issuer.compare(subject)) == 0)
		{
		EVP_PKEY* pubKey = X509_get_pubkey(&aEndEntity);
		if (!pubKey)
			{
#ifdef DUMPCHAINVALIDITYTOOL
			cout << "no public key in certificate" << endl;
#endif
			return false;
			}

		int verify = X509_verify(&aEndEntity, pubKey);
		if(verify)
			{
			return true;
			}
		}
	return false;
	}

bool CertificateChain::ValidateWithCertstore(X509& aPrevious, const SWICertStore& aStore , string& aIssuer)	
	{
	int verifyWithRoot = 0;
	const vector <X509*>& certStoreVector = aStore.GetRootCertVector();
	if(certStoreVector.size())
		{
		char buf[KLength];
		for(std::vector<X509*>::const_iterator root = certStoreVector.begin(); root != certStoreVector.end(); ++root)
			{
			EVP_PKEY* pubkey = X509_get_pubkey(*root);
			if (!pubkey)
				{
#ifdef DUMPCHAINVALIDITYTOOL
				cout << "no public key in certificate" << endl;
#endif
				return false;
				}
	
			verifyWithRoot = X509_verify(&aPrevious, pubkey);
			if(verifyWithRoot)
				{
				X509_NAME_oneline (X509_get_issuer_name (*root),buf, KLength);
				char* issued = strstr (buf, "/CN=");
				if (issued)
					{
					issued += 4;
					char* end = strchr (issued, '/');
					if (end)
						{
						*end = 0;
						}
					aIssuer=issued;
					string rootSignature((const char*)(aPrevious.signature->data) , (aPrevious.signature->length));
					iValidatedRootSignatures = rootSignature;
				}
				return true;
				}
			}
		}
	return false;
	}

	
void CertificateChain::CheckForChainValidity(const int& aValidationStatus)
	{
#ifdef DUMPCHAINVALIDITYTOOL
	cout << "Certificate Chain :" << endl << endl;

	switch(aValidationStatus)
		{
		case 0:
			{
			DisplaySigningChain(iCertificateChain);
			break;
			}

		case 1:
			{
			DisplaySigningChain(iCertificateChain);
			cout << "Invalid Certificate Chain : Validity Period Check Failed" << endl;
			break;
			}

		case 2:
			{
			DisplaySigningChain(iCertificateChain);
			cout << "Broken Chain : Signature Verification Failed" << endl;
			break;
			}

		case 3:
			{
			DisplaySigningChain(iCertificateChain);
			cout << "Signature Verification and Validity Period Check Failed " << endl;
			break;
			}
		default:
			{
			cout << "Error" << endl;
			break;
			}
		}
#endif
	}

string CertificateChain ::ExtractCapabilities(const ASN1_BIT_STRING* aString)
	{
	string capabilities;
	unsigned char* buf = aString->data;
	int dataLen = strlen((const char*)buf);
	if(dataLen)
		{
		// check the type is "bitstring",where 3 represents bitstring.
		if (*(buf++) != KDerBitStringTag)
			{
			throw ENotADerBitString;
			}
	
		// Find the length of the bitstring contained in this buffer
		unsigned long len = GetBitStringLength(&buf);

		// skip the padding length field
		buf++;

		int capability = 0;
		while (len--)
			{
			char c = *(buf++);		
			for (int j = KByteLength-1 ; j >= 0; --j)
				{
				if (c & (1 << j))
					{
					if (capability < KNumberOfCaps)
						{
						capabilities.append(CapabilityList[capability]);
						capabilities.append("\n"); 
						}
					else
						{
						capabilities.append("Unknown"+capability);
						}
				
					iDevCapabilities += pow((double)2,capability);
					}
				capability++;
				}
			}
		}
	cout<<endl;
	return capabilities;
	}

string CertificateChain ::ExtractASN1IntegerData(const ASN1_BIT_STRING* aString)	
	{
	string sid;
	char buf[KByteLength];
	unsigned char* buffer = aString->data;
	int dataLen = strlen((const char*)buffer);
	if(dataLen)
		{
		// check the type is "sequence".
		if (*(buffer++) != KDerSequenceTag)
			{
			throw ENotADerSequence;
			}

		// Find the length of the bitstring contained in this buffer
		unsigned long len = GetBitStringLength(&buffer);
	
		//each value(sid or vid) has length is 6,so divide total length by 6,to get the no of (sid or vid)values.
		int dataLen = len/(KByteLength-2);
		int data = 0;
		while (dataLen--)
			{
			//check whether it is a DER Integer tag
			if(*(buffer++) != KDerIntegerTag)
				{
				throw ENotADerInteger;
				}

			//skip the integer header
			buffer++;

			//read the 4 bytes containing the actual data(sid or vid)
			for (int j = (KByteLength-5); j >= 0; --j)
				{
				char c =  *(buffer++);
				sprintf(buf,"%2.2x",c);
				sid.append(buf);
				}

			sid.append("\n");	
			}
		}
	return sid;
	}

string CertificateChain::ExtractDeviceConstaints(const ASN1_BIT_STRING* aString)
	{
	string deviceId;
	unsigned char* buffer = aString->data;
	int dataLength = strlen((const char*)buffer);
	if(dataLength)
		{
		// check whether the type is "sequence".
		if (*(buffer++) != KDerSequenceTag)
			{
			throw ENotADerSequence;
			}

		// Find the length of the bitstring contained in this buffer
		unsigned long len = GetBitStringLength(&buffer);
	
		//each device id length is 22,so divide total length of the utf8 string by 22,to get the number of device id's.
		int noOfDeviceId = len/KDeviceIdLength;
		int data = 0;
		while (noOfDeviceId--)
			{
			if(*(buffer++) != KDerUtf8StringTag)
				{
				throw  ENotADerUtf8String;
				}
			//skip the data length.
			buffer++;
		
			//read the 20 bytes containing the actual data(device id)
			char buf[KDeviceIdLength-2];
			for (int j = 0; j < (KDeviceIdLength-2); j++)
				{
				buf[j] = *(buffer++);
				}

			deviceId.assign(buf,(KDeviceIdLength-2));
			deviceId.append("\n");
			}
		}
	return deviceId;
	}

unsigned long CertificateChain::GetBitStringLength(unsigned char** aASN1BitStringData)
	{
	unsigned long len = 0;
	if (**aASN1BitStringData & 0x80)
		{
		// long form of the length field
		int lenlen = *((*aASN1BitStringData)++) & 0x0F; // bottom 4 bits
		len = *((*aASN1BitStringData)++);
		while (--lenlen)
			{
			len = len << KByteLength;
			len += *((*aASN1BitStringData)++);
			}
		}
	else
		{
		// short form
		len = *((*aASN1BitStringData)++);
		}
	return len;
	}

CertificateChain::~CertificateChain()
	{
	}
