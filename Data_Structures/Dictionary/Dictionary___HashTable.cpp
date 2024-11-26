// 1. Hash(key) = a * key + b
// 2. Hash(key) = key % p
// 3. mid-square: Hash(key) = key*key 's mid k digit, total addr = 8^k
// 4. folding: split key to k parts, Hash(key) = part1 + part2 + ...
// 5. digit analysis: 数码分布较均匀的若干位作为addr

// 闭散列
enum KindOdStatus { Active, Empty, Deleted };
struct HashNode {
    int val;
    KindOfEntry info;
};
class HashTable {
private:
    int divitor;
    int curSize, maxSize;
    HashNode *elements;
    int FindPos(const int& e) const;
public:
    HashTable(int sz = DefaultSize);
    bool search(const int& e) const;
    bool insert(const int& e);
    bool remove(const int& e);
    void makeEmpty();
};

int HashTable::FindPos(const int &e) const {
    // linear probing
    int index = e % divitor;
    int nextEmpty = index;
    do {
        if(elements[nextEmpty].info == empty || elements[nextEmpty].info = Active && elements[nextEmpty].val == e) return nextEmpty;
        nextEmpty = (nextEmpty + 1) % divitor;
    } while (nextEmpty != index);
    return nextEmpty;

    // quadratic probing
    int index = e % divitor;
    int k = 0;
    while(elements[index].info == Active && elements[index].val != e) {
        index = (index + (++k * 2 - 1)) % divitor; // i*i = (i-1)*(i-1) + 2*i + 1
    }
    return index;
}

bool Insert(const int &e) {
    // linear probing
    int pos = FindPos(e);
    if (elements[pos].info != Active) {
        elements[pos].Info = Active;
        elements[pos].val = e;
        curSize++;
        return true;
    }
    return false;

    // quadratic probing
    int pos = FindPos(e);
    if(elements[pos].info == Active) return false;
    elements[pos].info = Active;
    elements[pos].val = e;
    curSize++;
    if(curSize < maxSize / 2) return true;
    HashNode *old = elements;
    maxSize = nextPrime(2 * maxSize);
    divitor = maxSize;
    elements = new HashNode[maxSize];
    for (int i = 0; i < maxSize; i++) {
        elements[i].info = empty;
        if(old[i].info == Active)
            insert(old[i].val);
    }
    delete[] old;
    return true;
}

bool remove(const int& e) {
    int pos = FindPos(e);
    if(elements[pos].info == Active) {
        elements[pos].info = Deleted;;
        curSize--;
        return true;
    }
    return false;
}

// 双散列
// H0 = Hash(key), Hi = (H0 + i * ReHash(key)) %

// 开散列
struct Node {
    int val;
    Node *next;
};

class HashTable {
private: 
    int divitor;
    int maxSize;
    Node **elements;
    Node *FindPos(const int *e) const;
public:
    bool search(const int &e) const;
    bool insert(const int &e);
    bool remove(const int &e);
};

Node* FindPos(const int *e) const {
    int index = e % divitor;
    Node *p = elements[index];
    while(p != nullptr && p->val != e) p = p->next;
    return p;
}

// ASL succ
// closeHash, linear: (1 + 1/(a-1))/2
// closeHash, other : (-ln(1-a))/a
// openHash         : 1 + a/2

// ASL fail
// closeHash, linear: (1 + 1/(a-1)^2)/2
// closeHash, other : 1/(1-a)
// openHash         : a
