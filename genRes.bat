@echo off
windres windows.rc -O res -F pe-i386 -o windows.o
echo Generated Windows Resource!