@echo off

gcc ^
  ..\src\main.c ^
  ..\src\des_block\main_encryption\des.c ^
  ..\src\des_block\f_function\f.c ^
  ..\src\des_block\subkey_genaration\subKeyGen.c ^
  ..\src\modes\ECB.c ^
  ..\src\pkcs7_padding\pkcs7.c ^
  -o out.exe

