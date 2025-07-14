@echo off

gcc -shared -o ..\bin\des.dll ^
  ..\src\main.c ^
  ..\src\des_block\main_encryption\des.c ^
  ..\src\des_block\f_function\f.c ^
  ..\src\des_block\subkey_genaration\subKeyGen.c ^
  ..\src\modes\*.c ^
  ..\src\key_iv\key_iv.c ^
  ..\src\pkcs7_padding\pkcs7.c ^
  -I..\src\des_block\main_encryption ^
  -I..\src\des_block\f_function ^
  -I..\src\des_block\subkey_genaration ^
  -I..\src\modes ^
  -I..\src\key_iv ^
  -I..\src\pkcs7_padding ^
  -Wl,--output-def=des.def ^
  -Wl,--out-implib=libdes.a ^
  -DBUILDING_DLL
