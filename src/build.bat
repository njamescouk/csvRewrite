yacc -b pcsv -p pcsv -ldv csv.y
flex -Ppcsv -L csv.l
sed "/extern char \*getenv();/d" pcsv.tab.c > junk
copy /y junk pcsv.tab.c > nul 
del junk

g++ -o ..\bin\csvRewrite.exe *.c *.cpp
