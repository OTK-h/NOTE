# Concurrent Objects
## 1. Concurrency and Correctness
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

## 2. Sequential Objects
fail if enqueue in the same time

## 3. Quiescent Consistency
method calls should appear to happen in a one-at-a-time sequential order  
method calls seperated by a period of quiescence should appear to take effect in real-time order

ab同时入队，c入队，ab无法判断先后，但ab在c前
静态一致性：任何时间，一个object变成静态的，执行结果等效于已完成的操作的某种排序(不重叠的以实时顺序生效，重叠操作可能被重新排序)
序列一致性：操作按照每个程序指定的顺序生效

principle
1. method call happen one-at-a-time sequential order
2. Method calls separated by a period of quiescence should appear to take effect in their real-time order.

## 4. Sequential Consistency
principle
methods call take effect in program order

3.1 + 4.1: sequential consistence
多个方法共同作用得到某个结果，若存在重叠，这些方法间的顺序可能有不同组合

静态一致性不一定按照按照程序顺序，顺序一致性不被静态时期影响

## 5. Linearizability
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

## 6. Formal Definations
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

## 7. Progress Conditions
each object in class is wait-free, the class is wait-free
wait-free algorithm can be ineffective
wait-free method is lock-free method, 反之不然
exploit obstruction-free: introduce back-off mechanism（a thread detects conflict pauses to give an earlier thread time to finish）

## 8. The Java Memory Model
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