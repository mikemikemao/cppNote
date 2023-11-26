#ifndef I_GRAPH_H
#define I_GRAPH_H

template<typename V,typename E>
class IGraph
{
public:
    virtual void addVertex(V v) = 0;
    virtual void addEdge(V from, V to) = 0;
    virtual void addEdge(V from, V to, E weight) = 0;

};

#endif