#ifndef LIST_GRAPH_H
#define LIST_GRAPH_H
#include "IGraph.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
using namespace std;

template <typename V, typename E>
class Edge;

template <typename V, typename E>
class Vertex;

template <typename V, typename E>
class Vertex
{
    friend bool operator==(const Vertex &lv, const Vertex &rv)
    {
        cout << "call ==" << endl;
        return lv.value == rv.value;
    }
    friend class std::hash<Vertex>;

public:
    V value;
    unordered_set<Edge<V, E>> inEdges;
    unordered_set<Edge<V, E>> outEdges;

public:
    Vertex() {}
    Vertex(V v)
    {
        this->value = v;
    }
};

template <typename V, typename E>
bool operator==(const Vertex<V, E> &lv, const Vertex<V, E> &rv)
{
    cout << "call ==" << endl;
    return lv.value == rv.value;
}

namespace std
{
    template <typename V, typename E> // we're defining a specialization with
    struct hash<Vertex<V, E>>         // the template parameter of Sales_data
    {
        // the type used to hash an unordered container must define these types
        typedef size_t result_type;
        typedef Vertex<V, E> argument_type; // by default, this type needs ==

        size_t operator()(const Vertex<V, E> &v) const
        {
            cout << "call vertex hash" << endl;
            return v.value == NULL ? 0 : hash<V>()(v.value);
        }

        // our class uses synthesized copy control and default constructor
        // other members as before
    };
} // close the std namespace; note: no semicolon after the close curly

template <typename V, typename E>
class Edge
{
    friend bool operator==(const Edge &le, const Edge &re)
    {
        cout << "call Edge ==" << endl;
        return le.from->value == re.from->value && le.to->value == re.to->value;
    }
    friend class std::hash<Edge>;

public:
    Vertex<V, E> *from;
    Vertex<V, E> *to;
    E weight;

public:
    Edge(){};
    Edge(Vertex<V, E> &from, Vertex<V, E> &to)
    {
        this->from = std::addressof(from);
        this->to = std::addressof(to);
    }
    ostream &print(ostream &os, const Edge &edge)
    {
        os << "Edge [from=" << edge.from->value << "to=" << edge.to->value << " weight"
           << weight ;
        return os;
    }
};

template <typename V, typename E>
bool operator==(const Edge<V, E> &lv, const Edge<V, E> &rv)
{
    cout << "call Edge ==" << endl;
    return lv.from.value == rv.from.value && lv.to.value == rv.to.value;
}

namespace std
{
    template <typename V, typename E> // we're defining a specialization with
    struct hash<Edge<V, E>>           // the template parameter of Sales_data
    {
        // the type used to hash an unordered container must define these types
        typedef size_t result_type;
        typedef Edge<V, E> argument_type; // by default, this type needs ==

        size_t operator()(const Edge<V, E> &e) const
        {
            cout << "call Edge hash" << endl;
            return hash<V>()(e.from->value) * 31 + hash<V>()(e.to->value);
        }

        // our class uses synthesized copy control and default constructor
        // other members as before
    };
} // close the std namespace; note: no semicolon after the close curly

template <typename V, typename E>
class ListGraph : public IGraph<V, E>
{
private:
    unordered_map<V, Vertex<V, E>> m_vertices;
    unordered_set<Edge<V, E>> m_edges;
public:
    void addVertex(V v);
    void addEdge(V from, V to);
    void addEdge(V from, V to, E weight);

public:
    void print()
    {
        cout << "[顶点]-------------------" << endl;
        for (auto Vertex : m_vertices)
        {
            cout << Vertex.first << endl;
            cout << "out-----------" << endl;
            // cout << Vertex.second.outEdges << endl;
            cout << "in-----------" << endl;
            // cout << Vertex.second.inEdges << endl;
        }
    }
};

template <typename V, typename E>
void ListGraph<V, E>::addVertex(V v)
{
    cout << "addVertex" << endl;
}

template <typename V, typename E>
void ListGraph<V, E>::addEdge(V from, V to)
{
    addEdge(from, to, NULL);
}

template <typename V, typename E>
void ListGraph<V, E>::addEdge(V from, V to, E weight)
{
    Vertex<V, E> fromVertex = Vertex<V, E>(from);
    if (m_vertices.find(from) == m_vertices.end())
    {
        m_vertices.insert(make_pair<V, Vertex<V, E>>(std::move(from), std::move(fromVertex)));
    }
    Vertex<V, E> toVertex = Vertex<V, E>(to);
    if (m_vertices.find(to) == m_vertices.end())
    {
        m_vertices.insert(make_pair<V, Vertex<V, E>>(std::move(to), std::move(toVertex)));
    }
    Edge<V, E> edge = Edge<V, E>(fromVertex, toVertex);
    edge.weight = weight;
    if (fromVertex.outEdges.erase(edge))
    {
        toVertex.inEdges.erase(edge);
        m_edges.erase(edge);
    }
    fromVertex.outEdges.insert(edge);
    toVertex.inEdges.insert(edge);
    m_edges.insert(edge);
}

#endif