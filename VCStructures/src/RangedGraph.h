#ifndef RANGED_GRAPH__
#define RANGED_BUCKETS__

#ifndef NO_CHECKS
#include <stdlib.h>
#endif

#include "RangedBuckets.h"


/**
* The edge handles almost all of the concrete functionality for RangedGraph.
* The constructor not only initializes Edge fields but also establishes its
* position within the graph, while its destructor also removes it from
* the graph.
*/
template<class V, class E>
class Edge {
  E value;
  Position<Edge<E> *> *left;
  Position<Edge<E> *> *right;
  RangedGraph<V, E> *parent;

  /**
  * Creates an edge and links the vertices to one another.
  */
  Edge(RangedGraph<V, E> *container, int left, int right, E value) :
      parent(container),
      left(parent->add(left, this)),
      right(parent->add(right, this)) {
    this->value = value;
    parent->incrementEdgeCount();
  }

  Position<Edge<E> *> *neighbor(Position<Edge<E> *current) {
    if (left == current) {
      return right;
    } else if (right == current) {
      return left;
    }
    return NULL;
  }

  /**
  * Destroys an edge and unlinks the vertices.
  */
  ~Edge() {
    edge->left->remove();
    edge->right->remove();
    parent->decrementEdgeCount();
  }

};

/**
* With the valued positional list implementation of RangedBuckets, there is
* no need for a special Vertex type. Since BucketPositionalList already has
* everything needed to be a vertex a simple renaming of the type makes
* the code more readable.
*/
template<class V>
using Vertex<V> = BucketPositionalList<V, Edge<E> *>;

template<class V, class E>
class RangedGraphInterface {
  virtual Edge<E> *addEdge(int left, int right, E value);
  virtual void removeEdge(Edge<E> *edge);
  virtual void removeVertex(int vertex);
}

/**
* RangedBuckets-based Graph implementation.  Currently, graph features
* necessary for a particular use-case, Vertex Cover approximation, are the
* only supported operations.  Number of vertices is static and equal to
* the number of buckets, though a vertex can be "removed" by having all of
* its edges removed.
*/
template<class V, class E>
class RangedGraph :
    public RangedBuckets<V, Edge<E> *>,
    RangedGraphInterface<V, E> {
  public:
  int edgeCount;

  RangedGraph(int bottom, int  top) :
      RangedBuckets(bottom, top),
      edgeCount(0) {}

  Vertex<V> *getVertex(int vertex) {
    return bucket(vertex);
  }

  /**
  * There is no trivial lookup for edges available, thus to keep track of edges
  * a pointer must be stored externally.
  */
  Edge<E> *addEdge(int left, int right, E value) {
    return new Edge<E>(left, right, value);
  }

  void removeEdge(Edge<E> *edge) {
    delete edge;
  }

  void removeVertex(int vertex) {
    Vertex<V> *v = bucket(vertex);

    while (v->size != 0) {
      removeEdge(v->first()->value);
    }
  }

  void incrementEdgeCount() {
    edgeCount++;
  }

  void decrementEdgeCount() {
    edgeCount--;
  }

  void incrementVertexCount() {
    size++;
  }

  void decrementVertexCount() {
    size--;
  }

  void incrementSize() {
    incrementVertexCount();
  }

  void decrementSize() {
    decrementVertexCount();
  }

}

/**
* Entanglement interface for RangedGraph.
*
* This is my first prototype for the entanglement idea.
* Each function defaults to empty.
* "before" functions have access to all of the arguments passed into the
* function call.
* "after" functions have access to the return value of the function call.
*
* If one wants to access values associated with the arguments to the function
* call after the function call is completed, it may be stored as state within
* the Entanglement object- the subclass definition must have a field for it.
*
* Intended to be subclassed such that a relationship is defined between the
* entangled ranged graph and some other data structure.
*
* It is likely that said other data structure will need to be initially synced
* up to the ranged graph somehow.
*
* I think a personal project for me would be to write code that can be used
* to analyze a class and generate the following two classes automatically- it
* could certainly be done.
*/
template<class V, class E>
class RangedGraphEntanglement {
  Position<RangedGraphEntanglement<V, E> *> *position;
  EntangledRangedGraph<V, E> *graph;

  void entangle(EntangedRangedGraph<V, E> *graph) {
    this->graph = graph;
    position = graph->entanglements->addLast(this);
  }

  void disentangle() {
    position->remove();
    graph = nullptr;
  }

  RangedGraphEntanglement() :
      graph(nullptr),
      position(nullptr) {}

  void beforeAddEdge(int left, int right, E value) {}
  void afterAddEdge(const Edge<E> *e) {}
  void beforeRemoveEdge(const Edge<E> *edge) {}
  void afterRemoveEdge() {}
  void beforeRemoveVertex(int vertex) {}
  void afterRemoveVertex() {}
  void afterIncrementEdgeCount() {}
  void afterDecrementEdgeCount() {}
  void afterIncrementVertexCount() {}
  void afterDecrementVertexCount() {}
}

/**
* Entangled version of RangedGraph.  This interface allows a user to inject
* function calls before and after each state-changing function calls so that
* external data structures may be updated with each change to the graph.
*
*/
template<class V, class E>
class EntangledRangedGraph : public
    RangedGraph<V, E> {
  public:
  PositionalList<RangedGraphEntanglement<V, E> *> *entanglements;

#ifdef STACK_ENTANGLEMENT__
#define ENTANGLEMENT_STACK EntanglementState state[entanglements->size];
    //This stack space helps with thread safety and allows the before and
    //after functions to share common and persistent space exclusive to their
    //parent call.  Not yet implemented, but this is a reminder that this
    //is necessary to implement for a thread safe data structure.

    //until then, one can (without thread safety) store common state between
    //calls as subclass state.
#else
#define ENTANGLEMENT_STACK
#endif

  Edge<E> *addEdge(int left, int right, E value) {
    ENTANGLEMENT_STACK
    entanglements->foreachByValue([](RangedGraphEntanglement<V, E> *e) {
      e->beforeAddEdge(left, right, value); 
    });
    Edge<E> *newEdge = RangedGraph<V, E>::addEdge(left, right, value);
    entanglements->foreachByValue([=](RangedGraphEntanglement<V, E> *e)
        -> {
      e->afterAddEdge(newEdge); 
    });
    return newEdge;    
  }

  void removeEdge(Edge<E> *edge) {
    ENTANGLEMENT_STACK
    entanglements->foreachByValue([](RangedGraphEntanglement<V, E> *e) {
      e->beforeRemoveEdge(Edge<E> *); 
    });
    RangedGraph<V, E>::removeEdge(Edge<E> *);
    entanglements->foreachByValue([](RangedGraphEntanglement<V, E> *e) {
      e->afterRemoveEdge();
    }
  }

  void removeVertex(int vertex) {
    ENTANGLEMENT_STACK
    entanglements->foreachByValue([](RangedGraphEntanglement<V, E> *e) {
      e->beforeRemoveVertex(vertex); 
    });
    RangedGraph<V, E>::removeVertex(vertex);
    entanglements->foreachByValue([](RangedGraphEntanglement<V, E> *e) {
      e->afterRemoveVertex();
    }
  }

}
