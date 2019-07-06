#include <stdio.h>

void testprint() {
    printf("Function Call Success\n");
}
int testret() { return 10; }
int testsum(int x, int y) { return x + y; }

int testrsp () {
    // RSP が 16の倍数なら 0, 倍数以外なら 1 を返す テスト関数

    int ret;

    __asm__ ("mov rax, rsp");
    __asm__ ("and rax, 0xf"); // RAX(=RSP) & 0xF
    __asm__ ("cmp rax, 0");
    // "mov %0, rax" : "=r"(ret) として、この時点の rax を
    // 関数の戻り値(ret)に直接代入しようとしたがコンパイルが通らない。
    //
    // 正しい記法がわからないので、とりあえず if else で対応する
    //   (mov で代入する値が即値ならコンパイルが通ったので)
    __asm__ ("je .Lelse0");
    __asm__ ("mov %0, 1"
             : "=r"(ret) ); // RSP が 16Byteの倍数ではない
    __asm__ ("jmp .Lend0");
    __asm__ (".Lelse0:");
    __asm__ ("mov %0, 0"
             : "=r"(ret) ); // RSP が 16Byteの倍数
    __asm__ (".Lend0:");

    return ret;
}
