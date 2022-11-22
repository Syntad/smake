g++ main.cpp src\*.cpp -Iinclude -Ilua -Llua -llua -std=c++2a -o smake.exe -static-libgcc -static-libstdc++
call smake
:: del smake.exe