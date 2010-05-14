BEGIN TRANSACTION CreateScrDatabase;
--Old Schema
--SoftwareTypes(SoftwareTypeId,SifPluginUid,InstallerSecureId,ExecutionLayerSecureId)
--New Schema
--SoftwareTypes(SoftwareTypeId,SifPluginUid,LauncherExecutable)
--CustomAccessList(AccessId,SoftwareTypeId,SecureId,AccessMode)
CREATE TABLE Temp(SoftwareTypeId INTEGER PRIMARY KEY NOT NULL,SifPluginUid INTEGER,LauncherExecutable TEXT DEFAULT 'SomeData');
INSERT INTO Temp(SoftwareTypeId,SifPluginUid) SELECT SoftwareTypeId,SifPluginUid FROM SoftwareTypes;
CREATE TABLE CustomAccessList(AccessId INTEGER PRIMARY KEY AUTOINCREMENT,SoftwareTypeId INTEGER NOT NULL,SecureId INTEGER NOT NULL,AccessMode INTEGER DEFAULT 1);
INSERT INTO CustomAccessList(SoftwareTypeId,SecureId) SELECT SoftwareTypeId,InstallerSecureId FROM SoftwareTypes;
INSERT INTO CustomAccessList(SoftwareTypeId,SecureId) SELECT SoftwareTypeId,ExecutionLayerSecureId FROM SoftwareTypes;
DROP TABLE SoftwareTypes;
ALTER TABLE Temp RENAME TO SoftwareTypes;
END TRANSACTION CreateScrDatabase;
