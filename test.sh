#!/bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$expected" = "$actual" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 "0;"
try 42 "42;"

try 21 '5+20-4;'

try 41 " 12 + 34 - 5 ;"

try 47 "5+6*7;"
try 8  "5+6/2;"
try 14 "2+4*6/2;"

try 15 "5*(9-6);"
try 4 "(3+5)/2;"

try 3 "+3;"
try 2 "-3+5;"
try 5 "20-3*+5;"

try 1 "1==1;"
try 0 "1==2;"
try 1 "(1+3*4)==(5*2+3);"
try 0 "(1-3*4)==(-5*2+3);"

try 1 "-1*2 < 3*4;"
try 1 "-1*2 <= 3*4;"
try 0 "3*4  < -1*2;"
try 0 "3*4  <= -1*2;"
try 0 "3*4  < 3*4;"
try 1 "3*4  <= 3*4;"

try 1 "3*4  > -1*2;"
try 1 "3*4  >= -1*2;"
try 0 "-1*2 > 3*4;"
try 0 "-1*2 >= 3*4;"
try 0 "3*4  > 3*4;"
try 1 "3*4  >= 3*4;"

try 64 "1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1;"

try 10 "b=2; a=4+b*3;"
try 18 "b=2; z=(4+b)*3;"

try 10 "foo=2; bar=4+foo*3;"
try 18 "foo=2; hoge=(4+foo)*3;"

try 14 "a=3; b = 5 * 6 -8; return a + b / 2;"
try 5  "return 5; return 8;"

try 27 "a=1; b=2; c=3; d=4; e=5; f=6; g=7; h=8; i=9; j=10; k=11; l=12; m=13; n=14; o=15; p=16; q=17; r=18; s=19; t=20; u=21; v=22; w=23; x=24; y=25; z=26; hoge=27; hoge;"

try 2 "if (1) 2;"
try 4 "a=1; b=2; if ((a*2)==2) b=b*2; b;"
try 1 "a=1; if (0) a=a*2; a;"

try 2 "if (1) 2; else 3;"
try 3 "if (0) 2; else 3;"
try 3 "a=1; b=2; if ((a*2)==3) b=b*2; else b=a*3;"

echo OK
