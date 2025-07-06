@echo off

type *.c > dump.txt
cd ./include
type *.h >> ../dump.txt