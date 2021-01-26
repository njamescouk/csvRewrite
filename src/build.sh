yacc -b pcsv -p pcsv -ldv csv.y
flex -Ppcsv -L csv.l

g++ -o ../bin/csvRewrite *.c *.cpp 
