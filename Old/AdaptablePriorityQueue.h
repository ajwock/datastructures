
template<class V>
class PQEntry {
public:
  int key;
  V value;

  PQEntry() {}
  PQEntry(K key, V value) : key(key) : value(value) {}

  static int compare(PQEntry left, PQEntry right) {
    return left.key - right.key;
  }
}

/**
* For time's sake, this is implemented with an integer key and a 
*
*/
template<class V>
class AdaptablePriorityQueue {
public:
  // Array representation of the heap.
  PQEntry *data;

  int capacity;
  int size;


}
