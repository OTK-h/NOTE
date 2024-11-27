```cpp
const int Max_Weight = 0x3f3f3f3f;
const int Default_Vertex = 0x3f3f3f3f;

template<class T, class E>
class Graph {
protected:
    int maxVertex;
    int curVertex;
    int curEdge;
    virtual int getVertexPos(T vertex);

public:
    Graph();
    void insertVertex(const T& vertex);
    void removeVertex(int v);
    void insertEdge(int v1, int v1, int w);
    void removeEdge(int v1, int v2);
    bool empty() const { return curVretex == 0; }
    T getWeight(int v1, int v2) const;
};
```
邻接矩阵表示：
```cpp
template<class T, class E>
class adjMatrixGraph: public Graph<T, E> {
private:
    T* vertex;
    E** edge;
    int getVertexPos(T v) {
        for(inti  = 0; i < curVertex; i++) if( vertex[i] == v ) return i;
        return -1;
    }
public:
    matrixGraph(int sz = Default_Vertex);
    ~matrixGraph() { delete[] vertex; delete[] edge; }
};
```
邻接表表示:
```cpp
template<class T, class E>
struct Edge {
    E weight;
    Edge<T, E>* nextEdge;
    int toVertex;
};
template<class T, class E>
struct Vertex {
    T data;
    Edge<T, E>* edges;
};

template<class T, class E>
class adjListGraph: public Graph<T, E> {
private:
    Vertex<T, E>* vertex;
    int getVertexPos(const T v);
public:
    adjListGraph(int sz = Default_Vertex);
    ~adjListGraph() { 
        for(int i = 0; i < Max_Vertex; i++) {
            Edge<T, E>* p = vertex[i].edges;
            while(p != nullptr) {
                vertex[i].edges = p->nextEdge;
                delete p;
                p = vertex[i].edges;
            }
        }
        delete[] vertex;
    }
};
```
邻接多重表表示
```cpp
template<class T, class E>
struct Edge {
    bool flag;
    E weight;   // 是否已经被遍历过
    int v1, v2;
    Edge<T, E>* inList; // 出边表，有相同起始点的边
    Edge<T, E>* outList;// 入边表，有相同终止点的边
};
template<class T class E>
struct Vertex {
    T data;
    Edge<T, E>* firstInEdge; // 入边表的第一个边
    Edge<T, E>* firstOutEdge; // 出边表的第一个边
};
```