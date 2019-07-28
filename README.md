# my9cc

個人勉強用のリポジトリ。
以下のサイトを参考にCコンパイラを作成中。

https://www.sigbus.info/compilerbook

---

## 進捗

- [x] 整数1個
- [x] 加減算
- [x] トークナイザを導入
- [x] 四則演算
- [x] 括弧に対応
- [x] 単項プラスと単項マイナス
- [x] 比較演算子
- [x] 1文字のローカル変数
- [x] 複数文字のローカル変数
- [x] return分
- [x] if文
- [x] if-else文
- [x] while文
- [x] for文
- [x] ブロック
- [x] 引数なしの関数呼び出し
- [x] 引数ありの関数呼び出し
- [x] 引数なしの関数定義
- [x] 引数ありの関数定義
- [x] 単項&と単項*
- [ ] 暗黙の変数定義を廃止
- [ ] ポインタ型を導入する
- [ ] ポインタの加算と減算を実装する
- [ ] sizeof演算子
- [ ] 配列を実装する
- [ ] 配列の添字を実装する
- [ ] グローバル変数を実装する
- [ ] 文字型を実装する
- [ ] 文字列リテラルを実装する
- [ ] 入力をファイルから読む
- [ ] 行コメントとブロックコメント


---

## Test

```
$ make test
```


## 文法

```
program    = func*
func       = ident "(" parameter? ")" "{" stmt* "}";
stmt       = expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "+"? term
           | "-"? term
           | "*" unary
           | "&" unary
term       = num
           | ident ("(" arguments? ")")?
           | "(" expr ")"
argument   = expr ("," expr)*
parameter  = ident ("," ident)*
```
