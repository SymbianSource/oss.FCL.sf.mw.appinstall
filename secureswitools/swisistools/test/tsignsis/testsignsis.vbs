Dim WshShell, oExec
Dim fso, File

Set fso = CreateObject("Scripting.FileSystemObject")
fso.CopyFile "kanji_cer.cer", "\epoc32\winscw\c\tswi\tsignsis\丁丂七丄kanji_cer.cer", true

Set WshShell = CreateObject("WScript.Shell")
Set oExec = WshShell.Exec("SignSIS  -s \epoc32\release\winscw\udeb\z\tswi\tsis\data\pkg_乕乖乗乘_kanji.SIS pkg_乕乖乗乘_kanji_signed.SIS  丁丂七丄kanji_cer.cer 丟両丣kanji_key.key 乕乖乗乘pass")

Do While oExec.Status = 0
     WScript.Sleep 100
Loop

Set fso = CreateObject("Scripting.FileSystemObject")

if (fso.FileExists("pkg_乕乖乗乘_kanji_signed.SIS")) then 
       Set File = fso.CreateTextFile("ResultSignSIS.txt", true)
       File.WriteLine ("Exist")
       File.Close()
End If

fso.DeleteFile "pkg_乕乖乗乘_kanji_signed.SIS", true
fso.DeleteFile "丁丂七丄kanji_cer.cer", true
fso.DeleteFile "丟両丣kanji_key.key", true
