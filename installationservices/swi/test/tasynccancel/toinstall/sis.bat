@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

call createsis create -cert %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tsisfile\data\signedsis\Root5CA\cert_chain_rsa_len3\chain_rsa_len3.cert.pem -key ..\..\tsisfile\data\signedsis\Root5CA\cert_chain_rsa_len3\rsa3.key toinstall-sub3.pkg
call createsis create -cert %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tsisfile\data\signedsis\Root5CA\cert_chain_dsa_len3\chain_dsa_len3.cert.pem -key ..\..\tsisfile\data\signedsis\Root5CA\cert_chain_dsa_len3\dsa3.key toinstall-sub2.pkg
call createsis create -cert %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tsisfile\data\signedsis\Root5CA\ca.pem -key %SECURITYSOURCEDIR%\installationservices\switestfw\testcertificates\swi\test\tsisfile\data\signedsis\Root5CA\ca.key.pem toinstall.pkg

call echo Y | del /F data\*.exe

call copy toinstall.sis toinstall_ocsp.sis
call copy ..\..\trevocation\sisInteg\%1\ocsp_01.sis ocsp_01.sis
