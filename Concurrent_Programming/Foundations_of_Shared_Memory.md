# Foundations of Shared Memory
由简单寄存器构造复杂寄存器
线程异步：以不同速度运行，任意时刻可以停止一个不可预测的间隔
不能用互斥实现共享存储器并发调用：1. 互斥通过寄存器实现 2. 寄存器时并发计算的基础块，应当无死锁或无饥饿
寄存器的实现应该无等待
## 1. The Space of Register
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
1. SRSW: single-register, single-writer
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

## 2. Register Constructinons
### 2.1 MRSW Safe Register
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
### 2.2 Regular M-Valued MRSW Register
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
### 2.3 An Atomic SRSW Register
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
### 2.4 An Atomic MRSW Register
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
### 2.5 An Atomic MRMW Register
```java
public class AtomicMRMWRegister<T> implements Register<T> {
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
P107