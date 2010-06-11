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

@ECHO ON
attrib -R \epoc32\data\z\tusif\tsts\data\*.* /S

if exist \epoc32\winscw\c\sys\install\integrityservices rd \epoc32\winscw\c\sys\install\integrityservices /s /q
if exist \epoc32\drive_e\sys\install\integrityservices rd \epoc32\drive_e\sys\install\integrityservices /s /q
md \epoc32\winscw\c\sys\install\integrityservices
md \epoc32\drive_e\sys\install\integrityservices

if exist \epoc32\winscw\c\sts_test_dir rd \epoc32\winscw\c\sts_test_dir /s /q
if exist \epoc32\drive_e\sts_test_dir rd \epoc32\drive_e\sts_test_dir /s /q
md \epoc32\winscw\c\sts_test_dir
md \epoc32\drive_e\sts_test_dir


xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase1\c_drive \epoc32\winscw\c\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase2\c_drive \epoc32\winscw\c\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase3\c_drive \epoc32\winscw\c\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase4\c_drive \epoc32\winscw\c\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase5\c_drive \epoc32\winscw\c\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase6\c_drive \epoc32\winscw\c\sys\install\integrityservices /y /e


xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase1\e_drive \epoc32\drive_e\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase2\e_drive \epoc32\drive_e\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase3\e_drive \epoc32\drive_e\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase4\e_drive \epoc32\drive_e\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase5\e_drive \epoc32\drive_e\sys\install\integrityservices /y /e
xcopy \epoc32\data\z\tusif\tsts\data\rollbackall\testcase6\e_drive \epoc32\drive_e\sys\install\integrityservices /y /e

copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase1_register_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase1_create_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase1_register_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase1_create_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase1_overwrite_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase1_register_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase1_create_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase1_register_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase1_create_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase1_overwrite_file_drive2.txt

copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase2_register_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase2_create_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase2_register_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase2_create_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase2_overwrite_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase2_register_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase2_create_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase2_register_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase2_create_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase2_overwrite_file_drive2.txt

copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase3_register_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase3_create_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase3_register_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase3_create_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase3_overwrite_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase3_register_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase3_create_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase3_register_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase3_create_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase3_overwrite_file_drive2.txt

copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase4_register_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase4_create_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase4_register_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase4_create_temp_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase4_overwrite_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase4_register_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase4_create_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase4_register_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase4_create_temp_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase4_overwrite_file_drive2.txt

copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase5_register_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase5_create_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase5_overwrite_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase5_register_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase5_create_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase5_overwrite_file_drive2.txt

copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase6_register_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase6_create_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\winscw\c\sts_test_dir\RollbackAllTestCase6_overwrite_file_drive1.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase6_register_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase6_create_file_drive2.txt
copy \epoc32\data\z\tusif\tsts\data\tstsdummy.txt \epoc32\drive_e\sts_test_dir\RollbackAllTestCase6_overwrite_file_drive2.txt
