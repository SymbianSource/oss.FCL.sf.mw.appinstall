Dim WshShell, oExec
Dim fso
 
Set WshShell = CreateObject("WScript.Shell")
Set oExec = WshShell.Exec("SignSIS  -s embededunicodesisfile.SIS  embededunicodesisfile.SIS  default.cer default.key pass")

Do While oExec.Status = 0
     WScript.Sleep 100
Loop
