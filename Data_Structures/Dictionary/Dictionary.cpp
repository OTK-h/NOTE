#include <iostream>
using namespace std;

// Average Search Length (succ)
// ASL = log2(n+1)-1

template <class K, class V>
class Dictionary {
public:
    virtual bool isEmpty() const = 0;
    virtual int size() const = 0;
    virtual pair<const K, V> *find(const K &) const = 0;
    virtual void insert(const pair<K, V> &) = 0;
    virtual void erase(const K &) = 0;
};

template <class K, class V>
class sortedArray : public Dictionary<K, V> {
protected:
    // to do
public:
    bool isEmpty() const;
    int size() const;
    pair<const K, V> *find(const K &) const;
    void insert(const pair<K, V> &);
    void erase(const K &);
};

template <class K, class V>
struct pairNode {
    pair<K, V> Element;
    pairNode<K, V> *next;
    pairNode<K, V>(const std::pair<K, V> tar, pairNode<K, V> *nxt = nullptr) {
        Element = tar;
        next = nxt;
    }
};

template <class K, class V>
class sortedChain : public Dictionary<K, V> {
protected:
    pairNode<K, V> *header;
    int Size;

public:
    sortedChain() {
        Size = 0;
        header = nullptr;
    }
    bool isEmpty() const;
    int size() const;
    pair<const K, V> *find(const K &) const;
    void insert(const pair<K, V> &);
    void erase(const K &);
};

template <class K, class V>
pair<const K, V> *sortedChain<K, V>::find(const K &tar) const {
    pairNode<K, V> *p = header;
    while (p != nullptr && p->Element.first != tar)
        p = p->next;
    if (p != nullptr && p->Element.first == tar)
        return &p->Element;
    return nummptr;
}

template <class K, class V>
void sortedChain<K, V>::insert(const pair<K, V> &tar) {
    pairNode<K, V> *p1 = header;
    pairNode<K, V> *p2 = nullptr;
    while (p1 != nullptr && p1->Element.first < tar.first) {
        p2 = p1;
        p1 = p1->next;
    }
    if (p1 != nullptr && p1->Element.first == tar.first) {
        p1->Element.second = tar.second;
        return;
    }
    pairNode<K, V> *tmp = new pairNode<K, V>(tar, p2->next);
    if (p2 = nullptr)
        header = tmp;
    else
        p2->next = tmp;
    Size++
}
