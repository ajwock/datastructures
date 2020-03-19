#ifndef RANGED_GRAPH__
#define RANGED_BUCKETS__

#ifndef NO_CHECKS
#include <stdlib.h>
#endif

#include "RangedBuckets.h"

/**
* With the valued positional list implementation of RangedBuckets, there is
* no need for a special Vertex type. Since BucketPositionalList already has
* everything needed to be a vertex a simple renaming of the type makes
* the code more readable.
*/
template<class V>
using Vertex<V> = BucketPositionalList<V, Edge<E> *>

/**
* RangedBuckets-based Graph implementation.  Currently, graph features
* necessary for a particular use-case, Vertex Cover approximation, are the
* only supported operations.  Number of vertices is static and equal to
* the number of buckets, though a vertex can be "removed" by having all of
* its edges removed.
*/
template<class V, class E>
class RangedGraph : public RangedBuckets<V, Edge<E> *> {
  public:
  int edgeCount;

  RangedGraph(int bottom, int  top) :
      RangedBuckets(bottom, top),
      edgeCount(0) {}

  /**
  * The edge handles linkage between vertices.
  */
  template<class E>
  class Edge {
    E value;
    Position<Edge<E> *> *left;
    Position<Edge<E> *> *right;

    /**
    * Creates an edge and links the vertices to one another.
    */
    Edge(int left, int right, E value) :
        left(add(left, this),
        right(add(right, this) {
      this->value = value;
      incrementEdgeCount();
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
      decrementEdgeCount();
    }

  };

  enum Notification {
    EDGE_ADD,
    EDGE_REMOVE,
    VERTEX_REMOVE
  };

  struct Message {
    Notification notification;
    union {
      Edge<E> *edge;
      Vertex<V> *vertex;
    } data;
  };

  /**
  * There is no trivial lookup for edges available, thus to keep track of edges
  * a pointer must be stored externally.
  */
  Edge<E> *addEdge(int left, int right, E value) {
    Edge<E> *edge = new Edge<E>(left, right, value);
    Message msg = {EDGE_ADD, edge};
    notifyObservers(&Message);
    return new edge;
  }

  void removeEdge(Edge<E> *edge) {
    Message msg = {EDGE_REMOVE, edge};
    notifyObservers(&msg);
    delete edge;
  }

  void removeVertex(int vertex) {
    Vertex<V> *v = bucket(vertex);
    Message msg = {VERTEX_REMOVE, v};
    notifyObservers(&msg);

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
}
