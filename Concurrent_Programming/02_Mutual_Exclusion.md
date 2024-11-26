# 2.1 Time
|||
|:---:|:---:|
|a<sub>i</sub>|A线程第i个任务|
|a<sup>j</sup>|总过程第j个任务/时期|
|I<sub>A</sub> = (a<sub>i</sub>, a<sub>j</sub>)|第i个任务到第j个任务的时期|
|I<sub>A</sub> → I<sub>B</sub>|I<sub>A</sub>最后一个任务在I<sub>B</sub>第一个任务之前|
|a<sub>0</sub> → a<sub>1</sub>|第0个任务在第1个任务之前|
# 2.2 Critical Sections(CS)
1. 每个特殊部分和unique Lock交流  
2. 线程尝试进入特殊部分，上锁  
3. 线程离开特殊部分，解锁
```Java
public interface Lock {
    public void lock();
    public void unlock();
}
public class Counter {
    private long value;
    private Lock lock;
    public long getAndIncrement() {
        lock.lock();
        try {
            long tmp = value;
            value = tmp + 1;
        } finally {
            lock.unlock();
        }
        return tmp;
    }
}
```
# 2.3 2-Tread Solutions
## The LockOne Class
```Java
// not deadlock-free
// not mutual exclusive (2 threads lock the other meanwhile)
class LockOne implements Lock {
    private boolean[] flag = new boolean[2];
    //index 0 or 1
    public void lock(){
        int i = ThreadID.get();
        int j = 1-i;
        flag[i] = true;
        while(flag[j]==true) {}
        //wait j finish
    }
    public void unlock() {
        int i = ThreadID.get();
        flag[i] = false;
    }
}
```
## The LockTwo Class
```Java
// not deadlock-free
// one runs first, and the other never runs
class LockTwo implements Lock {
    private volatile int victim;
    public void lock() {
        int i = ThreadID.get();
        victim = i;
        while(victim == i) {}
    }
    public void unlock() {}
    // deadlock if one runs completely before the other
}
```
## The Peterson Lock
```Java
// mutual exclusion & starvation-free & deadlock-free
class PetersonLock implements Lock {
    private volatile boolean[] flag = new boolean[2];
    private volatile int victim;
    public void lock() {
        int i = ThreadID.get();
        int j = 1-i;
        flag[i] = true;
        victim = i;
        while(flag[j] && victim == i) {}
    }
    public void unlock() {
        int i = TreahID.get();
        flag[i] = false;
    }
}
```
# 2.4 The Filter Lock
```Java
// 2 mutual exclusive protocols work for n threads (n>2)
// thread gets through n-1 level(waiting room) to reach cs
class FilterLock implements Lock {
    int[] level;
    int[] victim;
    int n;
    public FilterLock(int n_) {
        level = new int[n_];
        victim = new int[n_];
        this.n = n_;
        for(int i = 0; i < n_; i++) {
            level[i] = 0;
        }
    }
    public void lock() {
        // for j in 0..n-1, most n-j threads in level j
        int me = ThreadID.get();
        for(int i = 1; i < n; i++) {
            // control this level
            level[me] = i;
            victim[i] = me;
            for(int k = 0; k < n; k++) {
                while(k != me && level[k] >= i && victim[i] == me) {}
                // check other threads in this or nexr level
                // check if this thread wait in cur level
            }
        }
    }
    public void unlock() {
        int me = ThradID.get();
        level[me] = 0;
    }
}
```
# 2.5 Fairness
starvation-free: each thread call lock() will reach cs eventually  
>***but*** may take a long time, cannot find which first call lock() -> split lock()
1. doorway: countable step  
2. waiting room: uncountable step
# 2.6 Lamport's Bakery Algorithm
```Java
// deadlock-free, first-come-first-serve
class BakeryLock implements Lock {
    boolean[] flag;
    Label[] label;
    int n;
    private int max(int[] arr);
    public Bakery(int n) {
        flag = new boolean[n];
        label = new Label[n];
        for(int i = 0; i < n; i++) {
            flag[i] = false;
            label[i] = 0;
        }
    }
    
    public void lock() {
        int i = ThreadID.get();
        flag[i] = true;
        label[i] = max(label)+1;
        //overflow bug
        for(int k = 0; k < n; k++) {
            while(k != i && flag[k] &&
                 (label[k] < label[i] || label[k]==label[i] && k<i)) {}
            // each thread has 2 work
            // scan other threads, set itself a bigger label
        }
    }
    public void unlock() {
        flag[ThreadID.get()] = false;
    }
}
```
# 2.7 Bounded Timestamps
label[i]的值无限增长可能溢出
# 2.8 Lower Bounds on the Number of Locations
Bakery不实用: r-w n distinct Locations  
线性下界是解决互斥问题时锁固有的: info can be overwritten without other thread before read  
>***Any deadlock-free Lock algorithm must have n distinct locations.***
