# 1.1 Shared Objects and Synchronization
## 找出 1-10^9 中的素数
1. 平均分成十份，每份做同样的遍历检查(无法保证每一份花费相同)
2. 给每线程分配一个数，判断完后获取下一个数(无法锁住counter)
```Java
void primePrint {
    int i = ThreadID.get(); // thread IDs are in {0..9}
    int block = power(10, 9);
    for (int j = (i * block) + 1; j <= (i + 1) block; j++)
        if (isPrime(j)) print(j);
}
```
```Java
Counter* counter = new Counter(i)
void primePrint() {
    long i = 0;
    long limit = power(10, 10);
    while( i < limit) {
        i = counter.getAndIncrement();
        if(isPrime(i)) print(i);
    }
}
```
# 1.2 A Fable
## 遛狗
```txt
A，B不相容，共用一个院子

A:
1. set flag
2. B flag reset, A begin
3. A end, A flag reset

B:
1. set flag
2. if A set, B reset, wait, B set
3. B begin
4. B end, B flag reset
```
## Properties of Mutual Exclusion
```txt
deadlock_free(避免死锁)
starvation-freedom(某线程最终不会不运行)
fault-tolerance(某线程发生意外不影响其他线程)
```
## The Moral
不可通过线程互相交流实现互斥：Transient communication需要多线程同时参与，Persistent communication可以发生在不同时间
# 1.3 The Producer-Consumer Problem
```txt
A养狗，B送食，狗咬B

A
1. wait flag reset
2. A begin
3. A end, A check food clear, if so, flag set

B
1. wait flag set
2. B begin
3. B reset flag
```
```txt
Mutual Exclusion(AB不同时进行)
Starvation-freedom(B一直准备product，A一饿就吃)
```
# 1.4 The Readers-Writers Problem
B只在A传送的信息完成时阅读信息，用于获取持续信息
# 1.5The Harsh Realities of Parallelization
Amdahl's Law
