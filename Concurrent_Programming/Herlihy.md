## 3.1 Concurrency and Correctness
```java
class LockBasedQueue {
    int head, tail;
    T[] items;
    Lock lock;
    public LockBasedQueue (capacity) {
        head = tail = 0;
        items = (T[])new Object(capacity);
        lock = new ReentrantLock;
    }
    public void enq(T x) throws FullException {
        lock.lock();
        try{
            if(tail - head == item.length)
                throw FullException();
            items[tail % items.length] = x;
            tail++;
        } finaly {
            lock.unlock();
        }
    }
    public T deq() throws EmptyException {
        lock.lock();
        try {
            if(tail == head)
                throw EmptyException();
            T x = items[head % items.length];
            head++;
            return x;
        } finaly {
            lock.unlock();
        }
    }
}
```

## 3.2 Sequential Objects
fail if enqueue in the same time

## 3.3 Quiescent Consistency
method calls should appear to happen in a one-at-a-time sequential order  
method calls seperated by a period of quiescence should appear to take effect in real-time order

ab同时入队，c入队，ab无法判断先后，但ab在c前
静态一致性：任何时间，一个object变成静态的，执行结果等效于已完成的操作的某种排序(不重叠的以实时顺序生效，重叠操作可能被重新排序)
序列一致性：操作按照每个程序指定的顺序生效

principle
1. method call happen one-at-a-time sequential order
2. Method calls separated by a period of quiescence should appear to take effect in their real-time order.

## 3.4 Sequential Consistency
principle
methods call take effect in program order

3.1 + 4.1: sequential consistence
多个方法共同作用得到某个结果，若存在重叠，这些方法间的顺序可能有不同组合

静态一致性不一定按照按照程序顺序，顺序一致性不被静态时期影响

## 3.5 Linearizability
principle
methods call take effect between invocation and response
Linearization point:
lock-based: CS 
not use lock: typically a single step where effects of method call become visible to other method
顺序一致适合独立系统，如硬件内存，适合组合
线性化适合描述大型系统组件，组件需要独立和验证
实现并发对象的技术都是线性化的

线性化和顺序一致性一样非常阻塞，和静态一致性一样是组合性的

静态一致性：较弱的对象行为约束代价换取高性能
>方法调用呈顺序，瞬时发生
>静态状态前的方法调用 生效先于 静态方法后的方法调用
>非阻塞(一个完全的方法调用不需要等待另一个未决调用完成)，可复合(对正确性p，每个部分满足p，系统满足p)
顺序一致性：强约束限制，通常描述类似硬件存储接口
>方法调用呈顺序，瞬时发生
>方法调用效果等于程序顺序调用(输入输出一致)
>总顺序满足每个线程内部的顺序
>非阻塞，不可复合
可线性化特性：更强约束，描述可线性化组件构成的高层系统
>方法生效的地方通常是该方法调用的结果对其他方法调用可见时

静态一致与顺序一致无关
可线性化比静态，顺序高级，是他们的结合

## 3.6 Formal Definations
在调用和相应之间effect：更精确的，无法处理未返回的方法调用

并发系统的执行由history，finite sequence of method, response events

history H 的 subhistory是H的events的一个subsequence
把method invocation写作<x.m(a*)A>
把method response写作<>
x Object
m method name
a* sequence of arguments
a thread
t Ok or exceotion name

method call: formal name: H中的method call是H中的调用和下一个与之匹配的相应组成的一对(区分已返回，未返回的调用)

## 3.7 Progress Conditions
each object in class is wait-free, the class is wait-free
wait-free algorithm can be ineffective
wait-free method is lock-free method, 反之不然
exploit obstruction-free: introduce back-off mechanism（a thread detects conflict pauses to give an earlier thread time to finish）

## 3.8 The Java Memory Model
```Java
public static Singleton getInstance() {
    if(instance == null) {
        synchronized(Singleton.class) {
            if(instance == null)
                instance = new Singleton();
        }
    }
    return instance;
}
```
double-checked locking
getIntance() : create the instance the first time it is called
enter synchronized: observe an instance is null only
不正确：java memory model中写入字段的线程可能不会立即更新到内存
```Java
class FinalFieldExample {
    final int x; int y;
    static FinalFieldExample f;
    public FinalFieldExample() {
        x = 3;
        y = 4;
    }
    static void writer() {
        f = new FinalFieldExample();
    }
    static void reader() {
        if(f != null) {
            int i = f.x;
            int j = f.y;
        }
    }
}
public class EventListener {
    final int x;
    public EventListener(EventListener eventSource) {
        eventSource.registerListneer(this);
    }
    public onEvent(Event e) {
        // handle the event
    }
}
```
不正确：另一个线程在构造函数完成前调用onEvent(), onEvent()就看不到正确的值

# 4 Foundations of Shared Memory
由简单寄存器构造复杂寄存器
线程异步：以不同速度运行，任意时刻可以停止一个不可预测的间隔
不能用互斥实现共享存储器并发调用：1. 互斥通过寄存器实现 2. 寄存器时并发计算的基础块，应当无死锁或无饥饿
寄存器的实现应该无等待
## 4.1 The Space of Register
```java
public interface Register<T> {
    T read();
    void write(T v);
}
public class SequentialRegister<T> implements Register<T> {
    private T value;
    public T read() { return value; }
    public void write(T v) { value = v; }
}
```
register:
1. 
SRSW: single-register, single-writer
MRSW: multi-register, single-writer
MRMW: multi-register, multi-writer
2. safe | regular | atomic register
3. boolean | multi-valued register

读写重叠可能读出value范围内任意值
regular register:静态一致：MRSW：
write()正在进行时，register值在新旧之间闪烁，read()与多个write()重叠，可能读出随机一个write()的值

write order
regular register or safe register: single writer
atomic register: method call have linearization order
对single-writer，写顺序和优先顺序一样

## 4.2 Register Constructinons
### 4.2.1 MRSW Safe Register
```java
public class SafeBooleanMRSWRegister implements Register<Boolean> {
    boolean[] s_table;
    public SafeBooleanMRSWRegister(int capacity) {
        s_table = new boolean[capacity];
    }
    public Boolean read() {
        return s_table[ThreadID.get()];
    }
    public void write(Boolean x) {
        for(int i = 0; i < s_table.length; i++) {
            s_table[i] = x;
        }
    }
}
// 如果read不覆盖write，返回s_table[A](即最近写入的)
// 如果read覆盖write，可能返回任何值
public class RegBooleanMRSWRegister implements Register<Boolean> {
    ThreadLocal<Boolean> last;
    boolean s_value;
    RegBooleanMRSWRegister(int capacity) {
        last = new THreadLocal<Boolean>() {
            protected Boolean initialValue() {return false;}
        }
    }
    public void write(Boolean x) {
        if(x != last.get()) {
            last.set(x);
            s_value = x;
        }
    }
    public Boolean read() {
        return s_value;
    }
}
// 不同处：若读写重叠且写入数据和旧数据一样，阻止写入
```
### 4.2.2 Regular M-Valued MRSW Register
```java
public class RegMRSWRegister implements Register {
    private static int RANGE = Byte.MAX_VALUE - Byte.Min_VALUE + 1;
    boolean[] r_bit = new boolean[RANGE];
    public RegMRSWRegister(int capacity) {
        for(int i = 1; i < RANGE; i++) {
            r_bit[i] = flase;
        }
        r_bit[0] = true;
    } 
    public void write(Byte x) {
        r_bit[x] = true;
        for(int i = x-1; i >= 0; i--) {
            r_bit[i] = false;
        }
    }
    public Byte read() {
        for(int i = 0; i < RANGE; i++) {
            if(r_bit[i]) {
                return i;
            }
        }
        return -1;
    }
}
// 写入将写入位置为true，之前为false
// 读出第一个标记为true的位置
```
### 4.2.3 An Atomic SRSW Register
```java
public class StampedValue<T> {
    public long stamp;
    public T value;
    public StampleValue (T init) {
        stamp = 0;
        value = init;
    }
    public static StampedValue max(StampedValue x, StampedValue y) {
        if(x.stamp >= y.stamp) { return x; }
        else { return y; }
    }
    public static StampedValue MIN_VALUE = new StampedValue(null);
}
// write顺序由stamp决定
// read返回一个特定数据后，后面的read不能读出他之前的数据(stamp)
```
#### 4.2.4 An Atomic MRSW Register
```java
public class AtomicSRSWRegister<T> implements Register<T> {
    ThreadLocal<Long> lastStamp;
    ThreadLocal<StampedValue> lastRead;
    StampedValue<T> r_value;
    public AtomicSRSWRegister(T init) {
        r_value = new StampedValue<T>(init);
        lastStamp = new ThreadLocal<Long>() {
            protected Long initialValue() { return 0; }
        };
        lastRead = new ThreadLocal<StampedValue>() {
            protected StampedValue<T> initialValue() { return r_value; }
        };
    }
    public T read() {
        StampedValue<T> value = r_value;
        StampedValue<T> last = lastRead.get();
        StampedValue<T> result = StampedValue.max(value, last);
        lastRead.set(result);
        return result.value;
    }
    public void write(T v) {
        long stamp = lastStamp.get() + 1;
        r_value = new StampedValue(stamp, v);
        lastStamp.set(stamp);
    }
}
```
```java
public class AtomicMRSWRegister<T> implements Register<T> {
    ThreadLocal<Long> lastStamp;
    private StampedValue<T>[][] a_table;
    public AtomicMRSWRegister(T init, int readers) {
        lastStamp = new THreadLocal<Long>() {
            protected Long initialValue() { return 0; }
        };
        a_table = (StampedValue<T>[][]) new StampedValue[readers][readers];
        StampedValue<T> value = new StampedValue<T>(init);
        for(int i = 0; i < readers; i++) {
            for(int j = 0; j < readers; j++) {
                a_table[i][j] = value;
            }
        }
    }
    public T read() {
        int me = ThreadID.get();
        StampedValue<T> value = a_table[me][me];
        for(int i = 0; i < a_table.length; i++) {
            value = StampedValue.max(value, a_table[i][me]);
        }
        for(int i = 0; i < a_table.length; i++) {
            a_table[me][i] = value;
        }
        return value;
    }
    public void write(T v) {
        long stamp = lastStamp.get() + 1;
        lastStamp.set(stamp);
        StampedValue<T> value = new StampedValue<T>(stamp, v);
        for(int i = 0; i < a_table.length; i++) {
            a_table[i][i] = value;
        }
    }
}
```
#### 4.2.5 An Atomic MRMW Register
```java
public class AtomicMRSWRegister<T> implements Register<T> {
    private StampedValue<T>[] a_table;
    public AtomicMRMWRegister(int capacity, T init) {
        a_table = (StampedValue<T>[]) new StampedValue[capacity](init);
        StampedValue<T> value = new StampedValue<T>(init);
        for(int i = 0; i < a_table.length; i++) {
            a_table[i] = value;
        }
    }
    public void write(T v) {
        int me = THreadID.get();
        StampedValue<T> max = StampedValue.MIN_VALUE;
        for(int i = 0; i < a_table.length; i++) {
            max = StampedValue.max(max, a_table[i]);
        }
        a_table[me] = new StampedValue(max.stamp + 1, value);
    }
    public T read() {
        StampedValue<T> max = StampedValue.MIN_VALUE;
        for(int i = 0; i < a_table.length; i++) {
            max = StampedValue.max(max, a_table[i]);
        }
        return max.value;
    }
}
```





















|||||
|:---:|:---:|:---:|:---:|
|TAS/TTAS||||
|BackOffLock||||
|ALock||||
|CLHLock|空间消耗|||
|MCSLock|适用于NUMA系统|释放锁也要自旋|O(L+n)|
|TOLock||||
|CompositeLock|适合集群处理器|||
|LevelLock||||


阻塞：某线程意外延迟阻止其他线程继续
无等待：方法每次调用执行在有限步完成
有界无等待：上方基础上，步数存在界限
无锁：某方法无限次调用至少有一个调用在有限步完成
无干扰：某方法在有限步完成，任意点开始的执行都是隔离的
无死锁：有能进入临界区
无饥饿：任何能进入临界区

lockone: 并发死锁，一个完全先于一个无死锁
locktwo: 并发无死锁，一个完全先于一个死锁



并发a++
线程不安全
乐观锁：给出一个预期值，若相等则a++。if和a++不是原子的
CAS：比较和交换，三个参数：地址，旧值，新值。当且仅当地址的值为旧值使交换。用于乐观锁，适用于竞争不激烈


原子类
原子性：对于一组操作，只有全成功和全失败
相对于锁：粒度细(竞争范围减小到变量级别)，效率高(底层利用CAS不阻塞线程)

自旋锁

lock：对锁反复测试(自旋|忙等待，预期锁延迟小时) or 挂起自己，调度另外一个线程(代价大，预期锁延迟大)

TASlock
test-and-set
```java
public class TASLock implements Lock {
    AtomicBoolean state = new AtomicBoolean(false);

    public void lock() {
    while (state.getAndSet(true)) {}
    }
    public void unlock() {
        state.set(false);
    }
}
```
```java
TTASlock
test-test-and-set
public class TTASLock inplements Lock {
    AtomicBoolean state = new AtomicBoolean(false);

    public void lock() {
        while(true) {
            while(state.get()) {}
            if(!state.getAndSet(true)) { return; }
        }
    }
    public void unlock() {
        state.set(false);
    }
}
```

TAS比TTAS性能差
CPU在总线上广播消息，同一时刻只有一个CPU占用总线
cache从memory读取data，另一个CPU通知总线自己要同一个data，第一个cache已有data，不访问memory，一个CPU修改data，不能直接操作，因为存在多份拷贝，需要通知其他CPU使他们的拷贝失效(从cache移除)
TAS每一步在总线广播一个消息，让其他cache的data失效，总线被一个线程占用后会延迟其他线程(包括没有等待锁的)，cache删除后再次使用该数据要再通过总线获取，某线程试图获取空闲锁时会因总线占用被延迟
TTAS第一步只读(从cache)减少总线占用，缺陷：释放时设置为false，导致自旋的cache失效，需要执行一次TAS

后退策略改进 TTASLock
争用：指多个线程试图获取同一个锁。
高争用：意味着存在大量正在争用的线程。
低争用：与高争用相反

TTAS的lock()
1一直检查锁是否释放2看似空闲时执行TAS
其他某线程在第一二步的之间获取锁，该锁大可能高征用
解决：introduce delay
if looks free, but fail to get: better back off than collide again

````java
public class Backoff {
    final int minDelay, maxDelay;
    int limit;
    final Random random;

    public Backoff(int min, int max) {
        minDelay = min;
        maxDelay = max;
        limit = minDelay;
        random = new Random();
    }

    public void backoff() throw Exception {
        int delay = random.nextInt(limit);
        limit = Math.min(maxDelay, 2*limit);
        Thread.sleep(delay);
    }
}
```
```java
public class BackoffLock implements Lock {
    AtomicBoolean state = new AtomicBoolean(false);
    private static final int MIN_DELAY;
    private static final int MAX_DELAY;

    public void lock() {
        Backoff backoff = new Backoff(MIN_DELAY, MAX_DELAY);
        while(true) {
            while(state.get()) {}
            if(!state.getAndSet(true)) { return; }
            else { backoff.backoff(); }
        }
    }

    public void unlock() {
        state.set(false);
    }
}
```

Queue Spin-Lock
BackoffLock缺陷：
cache一致性流量：所有线程在共享的存储单元自旋，每次上锁产生cache一致性流量；延迟时间长，临界区利用率低

Anderson Queue Lock
```java
// starvation-free first come first serve
// 最大线程数为确定的n，每个所分配一个n数组
public class ALock implements Lock {
    ThreadLocal<Integer> mySlotIndex = new ThreadLocal() {
        protected Interger initiaValue() { return 0; }
    };
    AtomicInteger tail;
    boolean[] flag;
    int size;

    public ALock(int capacity) {
        size = capacity;
        tail = new AtomicInterger(0);
        flag = new boolean[capacity];
        flag[0] = true;
    }
    public void lock() {
        int slot = tail.getAndIncrement() % size;
        mySlotIndex.set(slot);
        while(!flag[slot]) { }
        flag[slot] = false;
    }
    public void unlock() {
        Integer slot = mySlotIndex.get();
        flag[(slot+1) % size] = true;
    }
}
```
线程共享flag[]，任意时刻线程在本地缓存的副本自旋
可能争用：假共享_相邻数据项在同一个cache line，写一个数据项导致该cache line无效，对未改变的数据项产生无效流量——要确保数据项处在单独cache line

```java
class QNode {
    public locked = false;
}

public class CLHLock implements Lock {
    AtomicReference<QNode> tail = new AtomicReference<QNode>(new QNode());
    ThreadLocal<QNode> myNode,myPred;
    // 线程局部变量，每个线程有该变量的副本，用于存储每个线程的私有数据
    public CLHLock() {
        myNode = new ThreadLocal<QNode>() {
            protected QNode initialValue() { return new QNode(); }
        };
        myPred = new ThreadLocal<QNode>() {
            protected QNode initialValue() { return null; }
        };
    }
    public void lock() {
        QNode qnode = myNode.get();
        qnode.locked = true;
        QNode pred = tail.getAndSet(qnode);
        myPred.set(pred);
        // spin until predecessor release
        while(pred.locked) { }
    }
    public void unlock() {
        QNode qnode = myNdoe.get();
        qnode.locked = false;
        myNode.set(myPred.get());
    }
}
```
QNode的locked记录每个线程状态，true已经获得|正在等待，false已释放，链表隐式，每个线程通过局部变量pred指向前驱。要获得锁：将自己的locked设置true，对tail调用getAngSet(使自己成为tail，并获取前驱引用)，在前驱locked域等待直到前驱释放
1.存储空间少于ALock 2.不需要知道可能用锁的线程数 3.FCFS

MCS lock
```java
class QNode {
    public locked = false;
    public QNode next;
}
public class MCSLock implements Lock {
    AtomicReference<QNode> tail = new AtomicReference<QNode>(null);
    ThreadLocal<QNode> myNode;

    public MCSLock() {
        myNode = new ThreadLocal<QNode>() {
            protected QNode initialValue() { return new QNode; }
        };
    }

    public void lock() {
        QNode qnode = myNode.get();
        QNode pred = tail.getAnsSet(qNode);
        if (pred != null ) {
            qnode.locked = true;
            pred.next = qnode;
            while (pred.locked) {}
        }
    }

    public void unlock() {
        QNode qnode = myNode.get();
        if(qnode.next == null) {
            if(tail.compareAndSet(qnode, null)) { return; }
            while (qnode.next == null) {}
        }
        qnode.next.locked = false;
        qnode.next = null;
    }
}
```

要获得锁：自己的QNode添加到尾部，非空则pred指向自己，自己QNode的locked域自旋，直到pred将pred的next的locked设置false
释放锁：若next空，1.不存在争用 2.存在争用执行较慢。compare返回true说明没有线程，tail设为null，返回false，等待next设置

时限队列锁