原子性：要么全执行不被打断，要么全不执行
可见性：一个线程一旦修改某数据，其他线程立刻可见
有序性：本线程内观察，所有操作有序。一个线程观察另一个线程，所有操作无序

volatile保证可见性，不保证原子性，禁止指令重排
重排序原则：不对存在数据依赖的操作重排，重拍前后单线程下执行结果相同
执行到volatile读写操作：前面的操作必须全部进行且可见，指令优化不改变volatile相对位置

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
可能争用：假共享_相邻数据项在同一个cache line，写一个数据项导致该cache line无效，对未改变的数据项产生无效流量。要确保数据项处在单独cache line

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
QNode的locked记录每个线程状态，true已经获得|正在等待，false已释放，链表隐式，每个线程通过局部变量pred指向前驱。要获得锁：将自己的locked设置true，对tail调用getAngSet()使自己成为tail，并获取前驱引用，在前驱locked域等待直到前驱释放
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
```java
class QNode {
    public Qnode pred;
}
public class TOLock implements Lock {
    static QNode AVAILABLE = new QNode();
    AtomicReference<QNode> tail = new AtomicReference<QNode>(null);
    ThreadLoal<QNode> myNode;

    public TOLock() {
        myNode = new ThreadLocal<QNode>() {
            protected QNode initialValue() { return new QNode(); }
        };
    }
    public void unlock() {
        QNode qnode = myNode.get();
        if(!qnode.compareAndSet(qnode, null)) {
            qnode.pred = AVAILABLE
        }
    }
    public void lock() {
        try {
            tryLock(Long.MAX_VALUE, TimeUnit.NANOSECONDS);
        } catch(InterruptedException ex) {
            ex.printStackTrace();
        }
    }

    public boolean tryLock(long time, TimeUnit unit) throws InterruptedException {
        long startTime = System.nanoTime();
        long patience = TimeUnit.NANOSECONDS.convert(time, unit);
        QNode qnode = new QNode();
        myNode.set(qnode);
        qnode.pred = null;
        QNode pred = tail.getAndSet(qnode);
        if(pred == null || pred.pred == AVAILABLE) {
            // free
            return true;
        }
        while(System.nanoTime() - startTime < patience) {
            QNode predOfPred = pred.pred;
            if(predOfPred == AVAILABLE) {
                return true;
            }
            else if(predOfPred != null) {
                pred = predOfPred;
            }
        }
        // time out
        if(!tail.compareAndSet(qnode, pred)){
            qnode.pred = pred;
        }
        return false;
    }

}
```
线程在前驱节点自旋。若线程超时，不能简单抛弃队列节点(否则后继节点无法注意)，要让节点从列表删除而不影响并发锁释放。
惰性方法：线程超时，标记为放弃，后继节点注意到自己在放弃节点自旋，转而在放弃节点的前驱自旋(可以重用被放弃节点)
pred为null，对应线程为获得锁或以释放锁
pred为AVALIABLE，对应线程已释放
pred为某个QNode类型tmp，对应线程已放弃，后继在tmp自旋

tryLock()：





















Syncrhonized用于线程数据共享，THreadLocal用于线程数据隔离
Synchronized利用锁，某时刻只被一个线程访问。ThreadLOcal给每个线程提供副本

ThreadLocal的set()获取当前线程，获取当前线程ThreadLocalMap，为空则创建值为v，不为空则直接改变值