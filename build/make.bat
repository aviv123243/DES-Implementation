@echo off

gcc ^
  ..\src\main.c ^
  ..\src\des_block\main_encryption\des.c ^
  ..\src\des_block\f_function\f.c ^
  ..\src\des_block\subkey_genaration\subKeyGen.c ^
  ..\src\key_iv\key_iv.c ^
  ..\src\modes\ECB.c ^
  ..\src\modes\CBC.c ^
  ..\src\modes\PCBC.c ^
  ..\src\modes\CFB.c ^
  ..\src\modes\OFB.c ^
  ..\src\modes\CTR.c ^
  ..\src\pkcs7_padding\pkcs7.c ^
  -o ..\bin\out