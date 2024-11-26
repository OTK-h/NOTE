# ics
## 01_计算机系统概述
### 1.1 计算机基本工作原理
存储程序：程序和原始数据存入主存才能执行，一旦执行能在不需干预下自动诸条指令取出执行
### 1.2 程序的开发与运行
汇编器：汇编语言翻译成机器语言  
解释器：源程序翻译成机器指令并执行  
编译器：高级语言源程序翻译成汇编语言或机器语言  
### 1.3 计算机系统的层次结构
ABI: Application Binary Interface  
机器级代码接口，如，过程调用约定，系统调用约定，二进制格式和函数库使用约定，寄存器使用规定，虚拟地址空间划分等  
API: Application Programming Interface  
高层次源程序代码和库之间接口，通常硬件无关(ABI相同或兼容，编译好的代码可直接运行)
### 1.4 计算机系统性能评价
指标：吞吐率(带宽，单位时间完成的工作量)，响应时间(作业提交到作业完成的时间)  
用户CPU时间：运行用户程序代码的时间  
系统CPU时间：为了执行用户程序需要CPU运行操作系统程序的时间  
其他时间：等待I/O操作完成时间，CPU执行其他用户程序时间  
>系统性能(响应时间)，CPU性能(用户CPU时间)
计算CPU时间：
1. 时钟周期，时钟频率(CPU主频)
2. CPI(Cycles Per Instruction)
指令执行速度：MIPS (编译优化后可能CPI少的指令减少，总时间减少，MIPS减少)
Amdahl定律(增强加速部分获得的整体性能改进程度)
改进后时间 = 改进部分时间/改进倍数 + 未改进部分时间
整体改进倍数 = 1/(改进部分时间占比/改进倍数 + 未改进部分时间占比)

## 02 数据的机器级表示与处理
### 数制和编码 整数 浮点数
阶码
非零(规格化): 2^exp-127^
全零(非规格化): 2^-126^
尾数真值
规格化：1.(23位)
非规格化: 0.(23位)

significand带隐藏位的
fraction不带隐藏位

max: (1-2^-24^) * 2^127^
min: 2^-129^

正负零：符号不同，其他全零
正负无穷：符号不同，解码全1，尾数全0
无定义：阶码全1，尾数非0

有权BCD码(8421码)
无权BCD码(格雷码，余3码)

小端：低有效在低地址
LSB，MSB(least significant bit)

### 数据基本运算
浮点数加减无结合性
对阶(小阶向大阶对齐)，尾数加减，尾数规格化(右规左规到1.bbbbb……)，尾数舍入，阶码溢出判断
右移对阶时留出G(保护位)R(舍入位)S(粘位，舍入位右边有非零数粘位为1)
附加位保护右移的中间结果，左移时移入significand，也作为舍入依据
Guard(保护位)：significand右边一位
Round(舍入位)：G右边一位
```txt
// Kahan 累加算法
// 每次因累加舍入的截断误差保存起来，加入到下一次累加
float tem = 0.1f;
float sum = 0.1f;
float c = 0;
float y, t;
for(int i = 1; i < 4000000; i++) {
    y = tem - c;
    t = sum + y;
    c = (t - sum) - y;
    sum = t;
}
```
```txt
规格化 舍入
case 规格化 && sig_grs隐藏位后超过26位
    右移 exp++ 直到sig_grs >> 26 == 1
    exp上溢
case 规格化 sig_grs隐藏位后不到26位
    左移 exp-- 直到sig_grs >> 26 == 1
    exp下溢
    为配合非规格化数，额外右移一次
case 非规格化 sig_grs >> 26 == 1
    exp++
case exp < 0
    右移 exp++ 直到exp==0 && sig_grs >> 26 <= 1 && sig_grs > 0
                或exp>0 && sig_grs>>26==1
    sig_grs<=4(舍入后为0) && exp < 0 阶码下溢
```
尾数是0，需要把阶码置0
一个负数的补码等于模减该负数的绝对值
对于某一确定的模，某数减去小于模的另一数，总可以用该数加上另一数负数的补码来代替计算
1. 求阶差(Δx = x1 - x2，x1 > x2，结果阶码为x1)
2. 对阶
3. 尾数加减
4. 尾数规格化
5. 尾数舍入
6. 尾数为0，阶码置零
q1: 通过阶码差的补码计算阶差
    [ΔE]补= [Ex–Ey]~补~ = [Ex]~移~ + [–[Ey]~移~]~补~   (mod 2n)
q2: ΔE溢出时无法用q1计算
q3: IEEE 754 SP格式，|ΔE|>24时，计算结果等于大的数，小数被吃掉
```cpp
int x;
float f;
double d;
x == (int)(float) x     // 0
x == (int)(double) x    // 1
f == (float)(double) f  // 1
d == (float) d          // 0
f == -(-f);             // 1
2/3 == 2/3.0            // 0
d < 0.0 ⇒((d*2) < 0.0) //  1  
d > f ⇒ -f > -d         // 1
d * d >= 0.0            // 1
x*x>=0                  // 0
(d+f)-d == f            // 0
```
IEEE 754 加减最多只右规一次：即使是最大的尾数相加尾数和也小于4，保留隐含1后，最多一位右移到小数部分
n位带标志加法器
OF = C~n~ ^ C~n-1~
SF = F~n-1~
CF = Cout ^ Cin
A + B
AB同号，和Sum不同号，OF=1 否则OF = 0
CF = Cout ^ sub
无符号x+y溢出时：res < x && res < y
带符号x+y溢出时：x < 0 && y < 0 && res >= 0 || x >= 0 && y >= 0 && res < 0
带符号x-y溢出时：x>>31 != ~y>>31 && x>>31 != res>>31
无符号x*y溢出时：res高32位 != 0
带符号x*y溢出时：res高32位每位 = res低32位最高位
booth's algorithm
变量*常数：移位组合加减
整数除法只有-2^n-1^/-1会溢出
变量/常数：逻辑/算数右移，若不能整除，右移出的1要相应处理
```txt
     x = – 1.5 x 1038,   y = 1.5 x 1038,    z = 1.0
     (x+y)+z = (–1.5x1038+1.5x1038 ) +1.0 = 1.0
     x+(y+z) = –1.5x1038+ (1.5x1038+1.0) = 0.0
浮点数不符合加法结合律
```
```txt
位运算除法除32
int div32(int x) {
    int off = (x>>31)&0x1F;
    // 加偏移量 2^k^-1, 32 = 2^5^, k = 5
    return (x+off) >> 5;
}
```
变量常数除运算向零舍入(带符号数结果负，先纠偏(加偏移量)，再右移)
### 字符
'a' = 97
'A' = 65
'1' = 49

## 03 程序的转换及机器级表示
RTL: Register Transfer Language
|||
|:---:|:---:|
|R[r]|寄存器r内容|
|M[addr]|存储单元addr内容|
|M[PC]|PC指向存储单元内容|
|%eax|eax寄存器|
|$0x7|立即数|
### 3.1 程序转换概述

|类型|指令|operand_1|operand_2|
|:---:|:---:|:---:|:---:|
|AT&T|movl|(src)$0x7|(dest)%eax|
|INTEL|MOV|(dest)EAX|(src)0x7|

|类型|长度|符号|
|:---:|:---:|:---:|
|Byte|8|b|
|Word|16|w|
|Double Word|32|l|

```txt
movl 0x1100(%ebx, %eax, 4)
位移(基址, 变址, 比例)
```
一个程序包括两个源文件，最终可执行文件为prog，-o指定输出文件名

### 3.2 IA-32指令系统概述
#### 3.2.1 数据类型及其格式
#### 3.2.2 寄存器组织和寻址方式
GPR: General-Purpose Register
|编号|32位寄存器||15 8 7 0|作用|64位寄存器|128位寄存器|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|000|EAX||AH AX AL|累加器|MM0/ST(0)|XMM0|
|001|EBX||BH BX BL|基址寄存器|MM1/ST(1)|XMM1|
|010|ECX||CH CX CL|计数寄存器|MM2/ST(2)|XMM2|
|011|EDX||DH DX DL|数据寄存器|MM3/ST(3)|XMM3|
|100|ESP||SP|栈指针寄存器|MM4/ST(4)|XMM4|
|101|EBP||BP|帧指针寄存器|MM5/ST(5)|XMM5|
|110|ESI||SI|源变址寄存器|MM6/ST(6)|XMM6|
|111|EDI||DI|目标变址寄存器|MM7/ST(7)|XMM7|
  
1. Special Register
|寄存器|31 16|15 0|作用|
|:---:|:---:|:---:|:---:|
|EIP||IP|指令指针寄存器|
|EFLAGS||FLAGS|标志寄存器|
条件标志: OF, SF, ZF, CF  
控制标志: DF, IF, TF  
CF是无符号数溢出标志，OF是有符号数溢出标志  
DF方向标志：确定ESI，EDI递增(0)或递减(1)
IF中断允许：允许响应中断(1)
TF陷阱标志：单步方式执行(1)

SREG：Segment Register
|15 0||
|:---:|:---:|
|CS|代码段|
|SS|堆栈段|
|DS|数据段|
|ES|附加段|
|FS|附加段|
|GS|附加段|

2. 寻址方式：实地址模式，保护模式
保护模式：多任务方式下对不同任务虚拟空间完全隔离

3. 浮点寄存器栈 多媒体扩展寄存器组

#### 3.2.3 机器指令格式
|前缀||||
|:---:|:---:|:---:|:---:|:---:|
|前缀类型|指令前缀|段前缀|操作数长度|地址长度|
|字节数|0 or 1| 0 or 1| 0 or 1| 0 or 1|

|指令||||||
|:---:|:---:|:---:|:---:|:---:|:---:|
|指令段|操作码|寻址方式(Mod/R)|SIB|位移|imm|
|字节数|1 or 2|0 or 1|0 or 1|1 or 2 or 4|imm|
操作码：opcode;w(与机器模式(32/16位)确定寄存器位数);d(操作方向)


Mod/R: 2字节
||||
|:---:|:---:|:---:|
|xx|xxx|xxx|
|MOD|REG/OPCODE|R/M|
|寻址方式|寄存器编号|寄存器编号|

SIB(scale index base): 2字节
||||
|:---:|:---:|:---:|
|xx|xxx|xxx|
|SS|INDEX|BASE|
|比例因子|变址|基址|
>disp32 + base + index * SS


### 3.3 IA-32常用指令类型操作
#### 3.3.1 传送指令
1. 通用数据传送指令：MOV
|||
|:---:|:---|
|MOV|movb movw movl|
|MOVS MOVZ|sign/zero ext, movzwl(w->l), movsbw(b->w)|
|XCHG|register exchange, xchgb|
|PUSH|R[esp]<-R[esp]-4*data_size, M[R[esp]]<-R[]|
|POP|R[]<-M[R[esp]](data_size), R[esp]<-R[esp]+4|

2. 地址传送指令：LEA(Load Effect Address)
不影响标志位
目的寄存器不是SREG，src为存储器寻址
leal (%edx, %eax), %eax
R[eax]<-R[edx]+R[eax]
3. 输入输出指令：in, out
累加器和I/O端口数据传送
4. 标志传送指令：popf(可能改变标志), pushf

#### 3.3.2 定点算数运算指令
后均可加b,w,l

|指令|显式操作数|标志位|操作数|tips|
|:---:|:---|:---|:---:|:---|
|ADD|2|OF, ZF, SF, CF|Integer||
|SUB|2|OF, ZF, SF, CF|Integer||
|INC|1|OF, ZF, SF|Integer||
|DEC|1|OF, ZF, SF|Integer||
|NEG|1|OF, ZF, SF, CF|Integer|按位取反加一|
|CMP|2|OF, ZF, SF, CF|Integer|sub src, dest|
|MUL|1|OF, CF|unsigned Integer|dest->AL/AX/EAX, OF=CF=!(if高n位相同)|
|IMUL|1, 2, 3|OF, CF|Interger|REG<-SRC(R/M)*IMM, 操作数>1时结果取n位|
|DIV|1||unsigned Integer|dest->AL/AX/EAX, dest_data_size = 2*src_data_size|
|IDIV|1||Integer|src_data_size=8/16/32, 余数AH/DX/EDX|
|||||商超过寄存器最大值，商和余数不确定，中断型号0|
做减法比较大小：
unsigned大于：CF = 0
signed大于：CF = SF
#### 3.3.3 按位运算指令
|指令|显式操作数|标志位|tips|
|:---|:---:|:---|:---|
|NOT|1|||
|AND|2|OF=CF=0, ZF, SF|可用于实现掩码|
|OR|2|OF=CF=0, ZF, SF|可用于特定位置1|
|XOR|2|OF=CF=0, ZF, SF||
|TEST|2|OF=CF=0, ZF, SF|a and b|
|SHL/R|2|CF|逻辑|
|SAL/R|2|CF|算数, 左移时符号位变化OF=1|
|ROL/R|2|CF|循环|
|RCL/R|2|CF|带循环, CF作为操作数一部分|
#### 3.3.4 控制转移指令
|类型|指令|tips|
|:---|:---|:---|
|无条件转移|JMP|段内/段间, 直接/间接, 短/近|
|条件转移|JCC|Jcc dst,单个标志位/无符号整数比较/带符号整数比较|
|条件设置|SET|SETcc dst, 将Jcc换成SETcc|
|条件传送|CMOVcc|CMOVcc dst, src, 符合条件就传送|
|调用|CALL|类似JMP -> 返回地址PUSH,跳转,EIP or CS:EIP入栈,装入目标EIP or CS:EIP|
|返回|RET|类似JMP -> POP返回地址装入EIP和CS,若带有立即数n,R[esp]<-R[esp]+n|
|中断||类似CALL,还要保存标志寄存器|

段间转移(远转移)：改变CS，EIP(绝对转移)
段内转移(近转移 and 短转移)：改变EIP(相对转移)
#### 3.3.5 x87 浮点处理指令

### 3.4 程序的机器级表示
#### 3.4.1 过程调用的机器级表示
模拟程序运行
1. 装载程序
>初始化模拟内存：建立空间，拷贝elf
>初始化cpu：初始化eip，esp
2. 执行程序
>读eip处1字节数据(操作码)
>解码执行，返回指令长度
>更新eip
3. 指令解码
>eip指向字节是前缀还是opcode
>INTEL格式操作数若有Gv，Ev说明opcode后有Mod/R字节
>根据 Mod + R/M 决定是否有SIB字节(内存地址disp32[--][--])
>SIB字节后有disp32(32位偏移量，小段方式，8字节)

P调用Q
```txt
1. P: 实参放到Q能访问的位置(CALL)
2. P: 保存返回地址，控制转移到Q(CALL)
3. Q: 保存P现场，非静态局部变量分配空间(准备阶段)
4. Q: 执行(处理阶段)
5. Q: 恢复P现场，释放局部变量(结束阶段)
6. Q: 取返回地址，控制转移到P(结束阶段)

-准备阶段
    形成栈底：push，mov      # 入口参数从右向左，返回地址为call的下一条指令
        movl 参数3 8(%esp)  # 按地址传参则为lea
        movl 参数2 4(%esp)  # 按值传参只改变入口参数
        movl 参数1 (%esp)
    生成栈帧：sub，and       # 不一定必要
    保存现场：push           # 如果有被调用者保存寄存器
    
    call Q
        R[esp]<-R[esp]-4
        M[R[esp]]<-返回地址
        R[eip]<-Q的起始地址
-过程体
    push %ebp
    mov %esp %ebp

    分配局部变量空间并赋值   
    执行                    # R[ebp] + 8/12/16访问入口参数
    EAX中准备返回参数
-结束阶段
    退栈：leave or pop
    取返回地址并返回：ret

    mov %ebp %esp
    pop %ebp
```
i386:
调用者保存寄存器(EAX, ECX, EDX，不用保存)
被调用者保存寄存器(EBX, ESI, EDI，需要保存)
每个过程有自己的栈区(栈帧)，EBP指示起始位置，

调用前：调用过程栈帧
调用者保存寄存器
参数n-参数1
返回地址
执行中：当前栈帧
EBP旧值
被调用者保存寄存器
非静态局部变量

过程调用参数入栈顺序从右到左  
windows中EBP旧值在其他地方  
全局变量，静态变量，连续存储在可读写数据区(不一定顺序)，编译优化把简单变量放在数据区，局部变量在栈  
地址类型 == != 之外为未定义操作  
全局变量只有0 or 初始值，局部变量不会初始化
RET返回在EAX

#### 3.4.2 流程控制语句的机器级表示
1. if-else
```asm
# type 1
    c = cond_expr
    if(!c) goto false_label # Jcc
    then_statement
    goto end_label          # JMP
false_label:
    else_statement
end_label:

type 2

    c = cond_expr
    if(c) goto true_label   # Jcc
    else_statement
    goto end_label          # JMP
true_label:
    then_statement
end_label:
```
2. switch-case
构造跳转表，在目标文件的.rodata，按4字节边界对齐
switch值相差较大时，还是会生成分段跳转代码，而不是构造跳转表
```
    .section .rodata
    .align 4
jump_table:
    .long .case1 1  # 后面的数为switch值
    .long .case2 2
    .long .case3 3
```
```
    mov 8(%ebp), %eax
    处理switch数据
    ja  .case5
    jmp *.case3( , %eax, 4)
.case1:
.case2:
.case3:
```
3. do-while
```
loop:
    loop_body_statement
    c = cond_expr
    if(c) goto loop
```
4. while
```
    c = cond_expr
    if(!c) goto end_label
loop:
    loop_body_statement
    c = cond_expr
    if(c) goto loop
end_label:
```
5. for
```
    begin_expr
    c = cond_expr
    if(!c) goto end_label
loop:
    loop_body_statement
    update expr
    c = cond_expr
    if(c) goto loop
end_label:
```

### 3.5 复杂数据类型的分配和访问
#### 3.5.1 数组的分配和访问
数组：静态存储型(static)，外部存储型(extern)，自动存储型(auto)，全局静态数组
假设首地址在ecx，i在edx，结果在eeax
|||
|:---|:---|
|A|leal  (%ecx), %eax|
|A+i|leal (%eax, %edx, 4), %eax|
|*(A+i)|movl (%ecx, %edx, 4), %eax|
#### 3.5.2 结构体的分配和访问
各成员地址：基址加偏移量
结构体中后声明的成员在高地址，和函数入口参数一样
作为入口参数：最好按地址传递
#### 3.5.3 联合体的分配和访问
#### 3.5.4 数据的对齐
每次只能读写某个字地址开始的四个单元中的连续1，2，3，4个字节
若不对齐，节省空间，访问大数据类型时增加访存次数
1. 结构体对齐方式和最严格成员相同
2. 满足对齐方式前提下，取最小可用位置作为偏移量(中间插空)
3. 结构体大小为对其边界长度整数倍(尾部插空)
### 3.6 越界访问和缓冲区溢出
```cpp
int main(int argc, char** argv, char** envp);
int main(int argc, char* argv[], char* envp[]);
// argc 参数列表长度
// ./hello                      argc = 1    argv = ["null"]
// ld -o test main.o test.o     argc = 5    argv = ["ld", "-o", "test", "test.o", "null"]
```
segmentaation fault跳转到位置地址(数据区 or 系统区 or 非法访问的存储区)
地址空间随机化
Address Space Layout Randomization(ASLR)


## 4 程序的链接

由高地址到低地址
内核虚存区
用户栈(动态生成，esp指向栈顶，向低地址)
共享库区域
堆(malloc动态生成，向高地址)
读写数据段(可执行文件装入)
只读代码段(可执行文件装入)
未使用

### 4.1 编译，汇编，静态链接
```shell
gcc -O1 prog1.c prog2.c -o prog

gcc -o hello hello.c
gcc -E hello.c -o hello.i
gcc -S hello.o -o hello.s
gcc -c hello.s -o hello.o
gcc hello1.o hello2.o -o hello
```
```shell
hexdump hello.o | less      # 查看
objdump -d hello.o | less   # 反汇编
```
可执行文件生成：
可重定位目标文件main.o + test.o ---连接器ld---> test

连接器：汇编语言：用助记符表示操作码，寄存器，用符号表示位置
子程序起始地址和变量起始地址：符号定义
调用子程序和使用变量：符号引用
最终合并(连接)用地址替换符号
模块化
>>一个程序可分为多个源程序文件
>>构建公共函数库(数学库，标准I/O等)
>效率高
>>时间上可分别编译，然后重新连接
>>空间上无需包含共享库所有代码

局部变量在栈中，不被外部引用，不是符号定义。
.o可重定位目标文件中有不同节，连接本质是合并相同类型的节
连接的步骤
```
1. 确定标号引用关系(符号解析)
    1.1 找到程序中有定义和引用的符号(函数，变量等)
    1.2 编译器：定义的符号放在符号表中(symbol table)(结构数组，每个表项包含符号名，长度，位置)
    1.3 连接器：每个符号的引用和确定的符号定义建立关联
2. 重定位
    2.1 合并相关.o文件的代码段和数据段
    2.2 计算每个符号在虚拟地址空间中的绝对地址
    2.3 将符号引用的地址修改为重定位后的地址
```
```c
// main.c
int mygo[2] = {1, 2};   // 定义
void swap();

int main() {            // 定义
    swap();             // 引用
    return 0;
}

//swap.c
extern int mygo;
int* x = &mygo[0];      // 定义 引用
static int* y;          // 定义

void swap() {           // 定义
    int tmp;
    y = &mugo[1];       // 引用 引用
}
```

### 4.2 目标文件格式
三类目标文件
```
1. 可重定位目标文件(.o)
    1.1 有对应.c生成
    1.2 代码和数据地址从0开始
2. 可执行目标文件(a.out)
    2.1 可以直接复制到内存执行
    2.2 地址为虚拟地址空间中的地址
3. 共享库文件(.so)
    3.1 特殊的可重定位文件
    3.2 windows中为Dynamic Link Libraries(DLLs)
```
#### 4.2.1 ELF目标文件格式
Executable and Linkable Format
```
链接视图：被链接：Relocatable object files
    ELF头
        ELF魔数，版本，大小端，操作系统，目标文件类型，机器结构类型，节头表起始位置长度
    节(ELF中具有相同特征的最小可处理单位)
    节头表
执行视图：被执行：Executable object files
    ELF头
    程序头表
    段(描述节如何映射到存储段)
        可多个节映射到一个段
        e.g. 合并.data .bss映射到可读写数据段
    节头表(可选)
```
#### 4.2.2 可重定向目标文件格式
1. ELF头
ELF文件最开始的地方，固定长度，索引程序头表，节头表
```c
#define EI_NIDENT       16
typedef struct {
    unsigned char   e_ident[EI_NIDENT]; 
    Elf32_Half     	e_type;
    Elf32_Half     	e_machine;
    Elf32_Word     	e_version;
    Elf32_Addr     	e_entry;        // Entry point address = 0
    Elf32_Off      	e_phoff;
    Elf32_Off      	e_shoff;        // 节头表距离文件起始的偏移量 516
    Elf32_Word     	e_flags;
    Elf32_Half     	e_ehsize;       // length of ELF_header = 52(32位系统)
    Elf32_Half     	e_phentsize;
    Elf32_Half     	e_phnum;
    Elf32_Half     	e_shentsize;    // 每个表项大小 40B(32位系统)
    Elf32_Half     	e_shnum;        // 表项数量
    Elf32_Half     	e_shstrndx;     // section header string table 字符串表在节头表中索引
} Elf32_Ehdr;
```
```
ELF Header
    Magic:
    // a.out魔数：01H 07H
    // Pe格式魔数：4DH 5AH
```
2. 节头表
程序末尾，索引ELF头，程序头表以外的节
说明ELF有多少节，每个节在什么地方
```C
typedef struct {
    Elf32_Word  sh_name;        // 节名在strtab中偏移
    Elf32_Word  sh_type;        // 无效，代码，数据，符号
    Elf32_Word  sh_flags;       // 在虚拟空间中的访问属性
    Elf32_Addr  sh_addr;        // 虚拟地址
    Elf32_Off   sh_offset;      // 在文件中的偏移地址，对.bss节无意义
    Elf32_Word  sh_size;        // 节在文件中长度
    Elf32_Word  sh_link;        // 连接相关的节
    Elf32_Word  sh_info;        // 连接相关的节
    Elf32_Word  sh_addralign;   // 对齐要求
    Elf32_Word  sh_entsize;	    // 节中每个表项长度，0表示不固定
} Elf32_Shdr;
```
readelf -S test.o读可重定向文件节头表
发现只有.text .data .bss .rodata分配了字节(Flg A)
.data和.bss偏移量相同
3. 节
每个节地址都为0，
```
.text       编译后代码
.rodata     只读数据(printf格式化串，switch跳转表)
.data       已初始化全局变量(auto在栈中)
.bss        未初始化全局变量(占位符，不占据实际空间，区分data为了空间效率)
.symtab     符号表
.rel.text   .text节相关可重定位信息(连接后修改)
.rel.data   .data节可重定位信息(链接后修改)
.debug      调试用符号表 gcc -g(详细调试信息)得到
.line       行号和.text映射 gcc -g得到
.strtab     字符串表，以null结尾的字符串序列
```
4. 程序头表
仅可执行文件有，说明文件那一部分(节)装载到内存哪一部分(段)
5. 符号表.symtab节，字符串表.strtab节

#### 4.2.3 可执行目标文件格式
没有扩展名或默认a.out，相当于windows的.exe  
为了可以执行，还需要将具有相同访问属性的节合并成segmentation  
所有代码位置连续，只读数据连续，可读写数据连续
```
相比于可重定向
1. ELF的e_entry给出第一条指令地址
2. 多了一个程序头表(段头表)，是结构数组
3. 多了一个.init节，定义_init函数，开始执行时的初始化工作
4. 少了两个.rel(无需重定位)
```
```
readelf -a 所有
        -h ELF头
        -l 程序头表
        -S 节头表
        -s 符号表
可装入段(必须装入内存并分配空间)
    read-only code regmentation
        ELF Header
        程序头表
        .init
        .fini
        .text
        .rodata
    read/write data segmentation
        .data
        .bss
```
```cpp
// 程序头表：描述节与虚拟空间存储段的映射关系
typedef struct {
        Elf32_Word   p_type;    //
        Elf32_Off    p_offset;
        Elf32_Addr   p_vaddr;
        Elf32_Addr   p_paddr;
        Elf32_Word   p_filesz;
        Elf32_Word   p_memsz;
        Elf32_Word   p_flags;
        Elf32_Word   p_align;
} Elf32_Phdr;
```
readelf -h main
FLG = RE 只读
FLG = RW 读写

### 4.3 符号表和符号解析
#### 4.3.1 符号表
```
Global symbols      非static函数，非static全局变量     
External symbols    外部定义引用的函数和变量
Local symbols       包括static函数和全局变量(不在栈中，在静态数据区，重名则分别分配)
局部变量在运行时动态分配，不是符号
```
```cpp
typedef  struct {
        int  name;        // 对应字符串在.strtab中的偏移量	
        int  value;       // 在对应节中的偏移量，可执行文件中是虚拟地址
        int  size;        // 对应目标所占字节数
        char type: 4,     // 符号对应目标的类型：数据、函数、源文件、节
             binding: 4;  //符号类别：全局符号、局部符号、弱符号
        char reserved;
        char section;     // 符号对应目标所在的节，或其他情况
} Elf_Symbol;
// ABS 不该被重定位
// UND 未定义
// COM 未初始化(此时value为对齐要求，size为最小大小)
```
#### 4.3.2 符号解析(符号绑定)
符号定义：指被分配了存储空间，函数名的代码所在区，变量名的所占经静态数据区。所有定义符号的值是其目标首地址
1. 全局符号的强弱性
```
强符号：函数名，已初始化的全局变量(非本地局部)
弱符号：未初始化的全局变量(非本地局部)
1. 不允许多个同名强符号
2. 同名有强弱，以强符号定义为目标
3. 多个若符号，任选一个
```
2. 符号解析的过程
构建可执行文件时指定库文件名，连接器自动到库中寻找目标模块，只拷贝目标模块
静态库：.a archive files：
将所有相关.o文件大包围单独的.a库文件
gcc -c f1.x f2.c f3.c
ar rcs mylib.a f1.o f2.o f3.o
gcc -c main.c
gcc -static -o main main.c ./mylib.a libc.a无需显式指出
E：被合并用来组成可执行文件的目标文件集合
U：未解析符号集合
D：已定义符号集合

gcc -static -o main main.o ./mylib.a
```
1. U, E, D置空
2. 扫描命令行所有输入文件，若是目标文件f，加入E，根据未解析未定义符号修改U, D。处理下一个输入文件；若是库文件f，尝试把U中所有符号和f中各模块定义的符号匹配，在模块m中匹配到该符号，m加入E，符号从U移动到D。重复直到U，D不变，为解析的模块丢弃，处理下一个输入文件
3. 若出现双重定义(D中加入已存在符号) or 出现未定义(扫描结束U非空)，连接器报错并停止，否则将E中姆堡文件重定位后合并生成可执行目标文件
4. 处理完libc.a，正常时U空，D中符号唯一
```
#### 4.3.3 与静态库的链接
输入文件按照调用顺序指定，否则连接错误，因为解析第一个文件时U为空，第二个文件调用的符号没有被定义而是被丢弃，解析第二个文件时无法解析
```
func.o → libx.a 和 liby.a 中的函数
libx.a → libz.a 中的函数
libx.a 和 liby.a、liby.a 和 libz.a 相互独立
则以下几个命令行都是可行的：
gcc -static –o myfunc func.o libx.a liby.a libz.a
gcc -static –o myfunc func.o liby.a libx.a libz.a
gcc -static –o myfunc func.o libx.a libz.a liby.a
```
### 4.4 重定位
1. 合并相同节：如所有.text合并为一个.text
2. 对定义符号重定位(确定地址)：确定新节中所有符号地址
3. 对引用符号重定位(确定地址)：修改.text和.data对符号的引用(用到.rel_text .rel_data)
#### 4.4.1 重定位信息
汇编器遇到引用时生成重定位条目，数据条目在.rel_data，指令条目在.rel_text
```cpp
typedef struct {
    int offset;     /*节内偏移*/
    int symbol:24,  /*所绑定符号*/
        type: 8;    /*重定位类型*/
} Elf32_Rel;
```
#### 4.4.2 R_386_32 绝对地址

#### 4.4.3 R_386_PC32 相对地址
```
转移目标地址 = 下一条指令PC + 重定位值  
重定位值 = ADDR(r_sym) - ((ADDR(.text) + r_offset) - init)
          符号定义地址 - ((符号所在节地址 + 引用处相对符号所在节偏移) - 重定位前引用处初始值)
          符号定义地址 - 该(符号引用)地址 + 重定位前引用处初始值
```
### 4.5 动态链接
#### 4.5.1 动态链接的特性
静态库：
1. 库函数包含在每个运行的代码段，高并发造成主存浪费
2. 库函数合并在可执行目标中，多个可执行文件造成磁盘空间浪费
3. 需要关注是否有新版本，定期下载，重新编译链接

共享库文件 or 共享目标文件shared object file
1. 在可执行目标文件装入或运行时动态装入内存，自动连接
2. 从程序分离，磁盘和内存只有一个备份

类unix：.so
windows：.dll(Dynamic Link Library)
#### 4.5.2 程序加载时动态链接
第一次加载并运行时load-time linking
gcc -c main.c       # libc.so无需显式指出
gcc -o myproc main.o ./mylib.so
1. 静态连接器ld生成部分连接的可执行目标文件，链接重定位和符号表
2. 加载器发现部分连接的可执行目标文件含有动态链接器路径名，动态链接器ld-linux.so链接代码和数据
3. 生成***存储空间***中完全链接的可执行目标
程序头表中特殊的段：INTERP，记录动态连接器目录和文件名
#### 4.5.3 程序运行时动态链接
运行期间run-time linking
通过动态链接器接口提供的函数在运行时动态链接
#### 4.5.4 位置无关代码
PIC: Position Independent Code
共享库位置可以不确定，共享库代码长度变化也不影响调用，连接器无需修改代码即可加载共享库到任意位置

gcc -fPIC -c myproc1.c myproc2.c    # 生成PIC
gcc -shared -o mylib.so myproc1.o myproc2.o  # 链接生成共享库

```
1. 模块内本地数据，过程：PC相对偏移寻址
2. 模块外全局数据
    在.data节开始设置指针数组(全局偏移表GOT)，加载时对GOT各项重定位。
    缺陷：多用四条指令，多了GOT，多用一个寄存器
3. 模块外全局过程：
    1. GOT中增加一项指向目标函数首地址。缺陷：
       多用三条指令，多用一个寄存器。
    2. 延迟绑定(lazy binding)，避免加载时重定位，延迟到第一次调用时。
       Ⅰ GOT开始三项固定
        GOT[0]为.dynamic节首地址，包含动态链接器所需信息，如符号表位置，重定位表位置
        GOT[1]为动态链接器表示信息
        GOT[2]为动态链接器延迟绑定代码入口地址
        根据GOT[1]和ID确定地址填入GOT[3]，之后调用只需要多执行一个jmp而不是多执行三条指令
       Ⅱ PLT(Procedure Linkage Table)
        .text节一部分，结构体数组，每项16B，PLT[0]外每项对应一个共享库函数
```

## 6 层次结构存储系统
### 6.1 存储器概述
记忆单元(Cell)
>具有两种稳态的能够表示二进制数码0和1的物理器件
存储单元/编址单位(Addressing Unit) 
>具有相同地址的位构成一个存储单元，也称为一个编址单位
存储体/存储矩阵/存储阵列(Bank)
>所有存储单元构成一个存储阵列
编址方式(Addressing Mode) 
>字节编址、按字编址
存储器地址寄存器(Memory Address Register - MAR)
>用于存放主存单元地址的寄存器
存储器数据寄存器(Memory Data Register - MDR)
>用于存放主存单元中的数据的寄存器

按工作性质/存取方式分类
>随机存取存储器 Random Access Memory (RAM) 
>>每个单元读写时间一样，且与各单元所在位置无关。如：内存。
>顺序存取存储器 Sequential Access Memory (SAM)
>>数据按顺序从存储载体的始端读出或写入，因而存取时间的长短与信息所在位置有关。例如：磁带。
>直接存取存储器 Direct Access Memory(DAM)
>>直接定位到读写数据块，在读写数据块时按顺序进行。如磁盘。
>相联存储器 Associate Memory（AM）/ Content Addressed Memory (CAM)
>>按内容检索到存储位置进行读写。例如：快表。

按功能/速度/容量/位置分类
>寄存器(Register)
>>封装在CPU内，用于存放当前正在执行的指令和使用的数据
>>用触发器实现，速度快，容量小（几~几十个）
>高速缓存(Cache)
>>位于CPU内部或附近，用来存放当前要执行的局部程序段和数据
>>用SRAM实现，速度可与CPU匹配，容量小（几MB）
>内存储器MM（主存储器Main (Primary) Memory）
>>位于CPU之外，用来存放已被启动的程序及所用的数据
>>用DRAM实现，速度较快，容量较大（几GB）
>>主存地址空间大小不等于主存容量
>外存储器AM (辅助存储器Auxiliary / Secondary  Storage)
>>位于主机之外，用来存放暂不运行的程序、数据或存档文件
>>用磁盘、SSD等实现，容量大而速度慢

主存性能指标：存储容量，存取时间T~A~(写入时间，读取时间)，存储周期T~MC~(两个连续访存最短间隔，大于T~A~)

半导体存储器
>RAM
>>SRAM(static 用于Cache)
>>DRAM(dynamic 用于主存)
>ROM
>>不可在线改写的ROM
>>Flash ROM(闪存，用于BIOS)

### 6.2 主存与CPU的连接及其读写操作
总线 - 数据线+地址线+控制线
#### 6.2.1 主存芯片技术
1. DRAM芯片技术
双译码：地址译码器分X和Y两个方向
>16M bit = 4Mb * 4 = 2048 * 2048 * 4 = 2^11^ * 2^11^ * 4
>地址线十一根线在行列方向分时复用，由RAS，CAS控制时序
>需要四个位平面，行列交叉点的四位一起读写
>>新一代DRAM容量至少四倍：行列分时复用，各乘二总乘四

2. 基本SDRAM芯片技术
总线宽度：总线中数据线条数
支持突发传输(burst)，第一次存取给出首地址，以后按地址顺序读取
CPU访问一块连续区域(行地址相同)可直接从行缓冲读取(SRAM实现)
突发长度：传送数据的长度，BL
CAS潜伏期：收到读命令到开始传送的延迟，CL，又叫读取潜伏期，只有读命令有

3. DDR SDRAM芯片技术
利用芯片内部I/O缓冲数据两位预取功能，利用上升沿下降沿进行两次传输

4. DDR2 SDRAM芯片技术
在3的基础上利用四位预取

5. DDR3 SDRAM芯片技术
在3的基础上利用八位预取
##### 6.2.2 主存与CPU的连接及其读写
128MB的DRAM：4096 * 4096 * 8行列各十二位
芯片内地址不连续，交叉编址，同时读写所有芯片
主存地址27位，片内地址24位，主存地址第三位用来选片
12 12 3
行号 列号 片号
行缓冲中数据地址连续(行列相同，片号递增)
若片内地址连续，则高三位为片号

若一个2^n^ * b位DRAM芯片的存储阵列是r行×c列，则该芯片容量为2^n^ * b位且 2^n^=r*c
>16K×8位DRAM，则r=c=128
芯片内的地址位数为n，其中行地址位数为log2r，列地址位数为log2c
>16K×8位DRAM，则n=14，行、列地址各占7位
通常r <= c且 |r-c|最小
>按行刷新，且减少地址引脚
#### 6.2.3 Load Store指令过程
load
>CPU把addr和读命令送总线
>主存经过取数时间，把数据送总线
>CPU经确定时间，从总线取数

store
>CPU把addr和写命令送总线
>CPU把数据送总线(信号线不复用可和上一步同时)
>等待写入时间，主存写入

### 6.3 磁盘存储器
#### 6.3.1 磁盘存储器结构
写：将不同磁化状态记录在表面
读：磁头固定载体转动，在线圈两端得到感应电压

每个面一个磁头
磁道：磁头和盘面相对运动构成的圆
柱面：多个盘片上相同磁道
扇区：每个磁道划分(512B -> 4096B)

第i个扇面：
间隙1
ID域i：同步字节，道号，头号，段号，CRC
间隙2
数据域：同步字节，数据，CRC
间隙3

数据地址
10位   4位   11位
柱面号 磁头号 扇区号

并行数据来自总线，并串转换电路，穿行读写数据，串并转换电路，并行数据送总线

1. 寻道：地址送磁盘寄存器，移动到指定柱面，返回寻到结束信号给磁盘控制器，转入旋转等待操作
2. 旋转等待：扇区计数器清零，每过一个扇区加一，直到等于扇区地址
3. 读写：
#### 6.3.2 磁盘存储器性能指标
低密度：总容量 = 面数 * 柱面数 * 内圆周长 * 内道密度
>面数 = (外径 - 内径) / 2 * 道密度
>实际容量：2 * 盘片数 * 磁道数/面数 * 扇区数/磁道数 * 512B/扇区

响应时间 = 排队延迟 + 控制器时间 + 存取时间(寻道时间 + 旋转等待时间 + 数据传输时间)

512B扇区，转速5400RPM，寻道时间(最大寻道时间的一半)12ms，数据传输率4MB/s，控制器时间1ms，排队时间0s，求响应时间
>0 + 1ms + 12ms + 0.5/5400RPM + 0.5KB/4MB/s
>0 + 1ms + 12ms + 0.5/90RPS + 0.128/1000s
>1ms + 12ms + 5.5ms + 0.1ms
>18.6ms

每道扇区数：(4MB * 60/5400) / 512B = 87
#### 6.3.3 磁盘存储器连接
最小单位是扇区；成批数据交换
Direct Memory Access(DMA)：直接存储器存取，数据不通过CPU，DMA控制器控制总线进行DMA传送
>CPU对DMA控制器初始化(命令，磁盘逻辑块号，主存起始地址，数据块大小)，启动磁盘驱动器
>磁盘控制器读扇区，DMA方式送主存
>DMA传送结束，DMA控制器向CPU发送DMA结束中断请求
#### 6.3.4 固态硬盘
闪存(读块写慢)
>写入
>>快擦：全1
>>编程：写0
>读出：控制栅正电压，状态0检测不到电流

### 6.4 高速缓冲存储器cache
#### 6.4.1 程序访问的局部性
层次化存储器结构：空间局部性，时间局部性
>程序的空间/时间局部性：数组按行优先顺序存放，双层循环中内层放列
#### 6.4.2 Cache基本工作原理
Cache行和主存块映射
主存和Cache一个单元为2^b^字节，主存块 | Cache行(槽)
内存中第M个块在Cache中第 (M mod Cache行数) 行

e.g.
以2^b^字节大小划分块
主存有2^m^个块
Cache提供2^c个行
某数据物理地址为32位
>块号：addr >> b：地址高m位
>行号：块号 % 2^c^：地址高m位中的低c位
>标记：地址高32位除去行号(高m-c位)
>块内地址：剩余32-m=b位
Cache每一行
有效位 | 标记 | 2^b^字节块内数据
||||
|:---|:---|:---|
|标记|Cache行号|块内地址|
|高m-c位|中间c位|低b位|

1. 找到对应行号
2. 比较Cache标记和地址标记，若不想等，读取内存到Cache
3. 看有效位，false读取内存到cache
#### 6.4.3 Cache行和主存块的映射
直接映射：块到固定行
>容易实现，命中时间短，无需考虑替换，空间利用不足，命中率低
>>直接映射，块大小16B，Cache数据区容量64KB，主存地址32位按字节，求Cache多少行，容量大小，数据占比
>>行数 = 64KB/16B = 4K
>>容量 = 4K * (16+1)b + 64KB * 8 = 580kb = 72,5KB
>>占比 = 64/72.5 = 0.883
全相联映射：块到任意行
>无需Cache索引，没有冲突缺失(只要有空)，比较器个数和位数多(每行一个)
组相联映射：块到固定组的任意行
>Cache组号 = 主存块号 % Cache组数

#### 6.4.4 Cache中主存块替换算法
Hit Rate：在Cache中的概率
Hit Time：在Cache中访问时间 = 判断时间 + 访问时间
Miss Rate = 1 - Hit Rate
Miss Penalty：缺失损失，访问一个主存块所需时间
1. FIFO
>命中率不随组增大而增大
2. LRU：最近最少用
>命中率遂组增大而增大最近少用算法，增加计数值(LRU位数 = log2(每组行数))
>被访问的置零，其余加一，
>未命中且未满，新行置零，其余加一
>未命中且已满，最大的替换，同上
3. LFU：最不常用算法
>替换引用最少的行，类似LRU
4. 随机替换算法：性能稍低，代价低
#### 6.4.5 Cache一致性问题
Write Hit/Miss
读比写容易(指令Cache比数据Cache易设计)
```
Write Hit
    Write Through
        同时写主存和Cache
        使用Write Buffer
    Write Back
        只写Cache，Miss时一次写回
        每行有dirty-bit
Write Miss
    Write Allocate
        主存装入，更新
        利用空间局部性，但每次要从主存读
    Not Write Allocate
        更新，不装入
```
### 6.5 虚拟存储器
### 6.6 IA-32/Linux中的地址转换
















用户程序可以修改自己的数据，不应该可以修改系统和自身代码
分段机制，同一种功能和访问控制类型的数据放在一起，统一提供保护
段表(全局描述符表GDT)
段名称 起始地址(Base) 界限(Limit) 权限要求(DPL)

程序访存时提供三个信息：
确定段基址：段号
检查越界情况：数据相对基址偏移量
检查权限：程序的权限等级

GDT由OS初始化，保存在内存中
GDTR寄存器保存GDT首地址和界限(长度减一)

operand_read() -> vaddr_read() -> laddr_read() -> paddr_read() -> cache_read() ->hw_mem_read() -> hw_mem[]

```
分段机制x86
    启动
        进入实模式
        加载OS
        OS初始化GDT
        进入保护模式(开启分段机制)
    启动后
        程序给出48位逻辑地址(虚拟地址)(16段选择符，32有效地址/段内偏移量)
        根据段选择符查GDT
        段级地址转换，得到线性(物理)地址
```

CR0寄存器中PE位：0实地址，1保护地址

80836保护模式地址转换：
段选择符在16位段寄存器，指向64位段描述符
CS(代码段寄存器), SS(栈段寄存器), DS(数据段寄存器), ES, FS, GS其他三个任意数据段
16位段选择符
13位index：段描述符表索引
1位TI(Table Indicator)：0表示GDT，1表示LDT
2位RPL(Requestor's Privilege Level)：00内核态，11用户态

Selector指向段描述符，Offset表示段内偏移量
由段描述符的Base，Addr和虚拟地址的Offset组成线性地址
Linear Addr：Dir | Page | Offset

32位线性地址 = 32位Base + 32位Offset

可访问：段描述符DPL >= 段选择符RPL
