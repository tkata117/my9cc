#!/bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s test/test_func_call.o
    ./tmp
    actual="$?"

    if [ "$expected" = "$actual" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

# ステップ1: 整数1個
try 0 "int main() {0;}"
try 42 "int main() {42;}"

# ステップ2: 加減算
try 21 'int main() {5+20-4;}'

# ステップ3: トークナイザを導入(空白文字に対応)
try 41 "int main() { 12 + 34 - 5 ;}"

# ステップ5: 四則演算(掛け算, 割り算)
try 47 "int main() {5+6*7;}"
try 8  "int main() {5+6/2;}"
try 14 "int main() {2+4*6/2;}"

## 括弧に対応
try 15 "int main() {5*(9-6);}"
try 4 "int main() {(3+5)/2;}"

# ステップ6: 単項プラスと単項マイナス
try 3 "int main() {+3;}"
try 2 "int main() {-3+5;}"
try 5 "int main() {20-3*+5;}"

# ステップ7: 比較演算子

## ==
try 1 "int main() {1==1;}"
try 0 "int main() {1==2;}"
try 1 "int main() {(1+3*4)==(5*2+3);}"
try 0 "int main() {(1-3*4)==(-5*2+3);}"

## <, <=
try 1 "int main() {-1*2 < 3*4;}"
try 1 "int main() {-1*2 <= 3*4;}"
try 0 "int main() {3*4  < -1*2;}"
try 0 "int main() {3*4  <= -1*2;}"
try 0 "int main() {3*4  < 3*4;}"
try 1 "int main() {3*4  <= 3*4;}"

## >, >=
try 1 "int main() {3*4  > -1*2;}"
try 1 "int main() {3*4  >= -1*2;}"
try 0 "int main() {-1*2 > 3*4;}"
try 0 "int main() {-1*2 >= 3*4;}"
try 0 "int main() {3*4  > 3*4;}"
try 1 "int main() {3*4  >= 3*4;}"

## Token配列を可変長ベクタに変更 (100個以上のトークンに対応できているかテスト)
try 64 "int main() {1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1;}"

# ステップ9: 1文字のローカル変数
try 10 "int main() {int a; int b; b=2; a=4+b*3;}"
try 18 "int main() {int b; int z; b=2; z=(4+b)*3;}"

# ステップ10: 複数文字のローカル変数
try 10 "int main() {int foo; int bar; foo=2; bar=4+foo*3;}"
try 18 "int main() {int foo; int hoge; foo=2; hoge=(4+foo)*3;}"

## ローカル変数に'_', 数字, 大文字, 小文字を使用
try 10 "int main() {int foo_A0; int Bar_; foo_A0=2; Bar_=4+foo_A0*3;}"

# ステップ11: return文
try 14 "int main() {int a; int b; a=3; b = 5 * 6 -8; return a + b / 2;}"
try 5  "int main() {return 5; return 8;}"

## 27個以上のローカル変数に対応できているかのテスト
try 27 "int main() {int a; int b; int c; int d; int e; int f; int g; int h; int i; int j; int k; int l; int m; int n; int o; int p; int q; int r; int s; int t; int u; int v; int w; int x; int y; int z; int hoge; a=1; b=2; c=3; d=4; e=5; f=6; g=7; h=8; i=9; j=10; k=11; l=12; m=13; n=14; o=15; p=16; q=17; r=18; s=19; t=20; u=21; v=22; w=23; x=24; y=25; z=26; hoge=27; hoge;}"

# ステップ12: 制御構文

## if 文
try 2 "int main() {if (1) 2;}"
try 4 "int main() {int a; int b; a=1; b=2; if ((a*2)==2) b=b*2; b;}"
try 1 "int main() {int a; a=1; if (0) a=a*2; a;}"

## if-else 文
try 2 "int main() {if (1) 2; else 3;}"
try 3 "int main() {if (0) 2; else 3;}"
try 3 "int main() {int a; int b; a=1; b=2; if ((a*2)==3) b=b*2; else b=a*3;}"

## while 文
try 0 "int main() {int a; a=0; while (a < 0) a=a+1; a;}"
try 5 "int main() {int a; a=0; while (a < 5) a=a+1; a;}"

## for 文
try 5 "int main() {int a; int i; a=0; for (i=0; i < 5; i = i+1) a = a+1; a;}"
try 5 "int main() {int a; int i; a=0; i=0; for (; i < 5; i = i+1) a = a+1; a;}"
try 0 "int main() {int a; int i; a=0; i=0; for (; i < 0; ) a = a+1; a;}"
try 0 "int main() {int a; int i; a=0; i=0; for (; i < 0; i = i+1) a = a+1; a;}"
try 0 "int main() {int a; int i; a=0; i=0; for (; i < 0; i = i+1) a = a+1; i;}"

## if-else 文の入れ子
try 1 "int main() {int a; if (1) if (1) a=1; else a=2; else a=3; a;}"
try 2 "int main() {int a; if (1) if (0) a=1; else a=2; else a=3; a;}"
try 3 "int main() {int a; if (0) if (0) a=1; else a=2; else a=3; a;}"

## while 文の入れ子
try 10 "int main() {int a; a=0; while (a < 10) while (a < 10) a = a+1; a;}"

## for 文の入れ子
try 25 "int main() {int a; int i; int j; a=0; for (i=0; i < 5; i=i+1) for (j=0; j < 5; j=j+1) a = a+1; a;}"

# ステップ13: ブロック
try 1 "int main() {{} 1;}"
try 2 "int main() {int a; a=1; {a=a+1;} a;}"
try 3 "int main() {int a; a=1; {a=a+1; a=a+1;} a;}"

# ステップ14: 関数呼び出し

## 引数なしの関数呼び出し
try 1 "int main() {testprint(); 1;}"
try 1 "int main() {int a; a=1; testprint(); a;}"
try 10 "int main() {testret();}"
try 10 "int main() {int a; a=1; testret();}"

## 引数ありの関数呼び出し
try 3 "int main() {testsum(1, 2);}"
try 3 "int main() {int a; a=1; testsum(1, 2);}"
try 11 "int main() {int a; a=1; testsum(1*3, 2*4);}"
try 3 "int main() {int a; int b; a=1; b=testsum(1, 2); b;}"

## 関数呼び出しをする前にRSPが16の倍数となっていることをテスト
##   testrsp() は RSPが16の倍数なら0, 倍数以外なら1を返す
try 0 "int main() {testrsp();}"
try 0 "int main() {int a; a=2; testrsp();}"
try 2 "int main() {int a; a=2; testrsp(); a;}"

# ステップ15: 関数の定義

## 引数なしの関数定義
try 4 "int hoge() { int a; a=2; return 1; } int main() { int a; int b; a=3; b=hoge(); return a+b; }"

## 引数ありの関数定義
try 2 "int hoge(int a) { return a+1; } int main() { return hoge(1); }"
try 3 "int hoge(int a) { a = a + 1; return a; } int main() { int a; int b; a=1; b=hoge(a); return a+b; }"
try 3 "int sum(int a, int b) { int c; c = a + b; return c; } int main() { return sum(1, 2); }"
try 10 "int sum(int a, int b) { int c; c = a + b; return c; } int main() { int a; int b; a = sum(1, 2); b = sum(3, 4); return a+b; }"

# ステップ16: 単項&と単項*
try 3 "int main() { int x; int y; x = 3; y = &x; return *y; }"
try 3 "int main() { int x; int y; int z; x = 3; y = 5; z = &y + 8; return *z; }"


# ステップ17: 暗黙の変数定義を廃止して、int というキーワードを導入
## この機能をテストするために、これより上の全テストの記述を修正
## (関数の型宣言と変数宣言を追加)
echo OK
