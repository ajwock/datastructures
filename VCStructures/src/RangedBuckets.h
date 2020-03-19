#ifndef RANGED_BUCKETS__
#define RANGED_BUCKETS__

#ifndef NO_CHECKS
#include <stdlib.h>
#endif

#include "PositionalList.h"
#include <utility>

/**
* Simple extension of a normal PositionalList.
* Adds an key and value pair extension to the PositionalList, as though
* the positional list were also a mapped pair.
*
* Also allows RangedBuckets size to be controlled by the enclosed
* PositionalLists so that users can safely perform Position or PositionalList
* operations on enclosed objects.
*/
template <class V, class P>
class BucketPositionalList : public PositionalList<P> {
  public:
  int key;
  V value;
  RangedBuckets<V, P> *parent;

  /**
  * Method to allow RangedBuckets size to be controlled by the enclosed
  * PositionalLists.
  */
  void incrementSize() {
    PositionalList<P>::incrementSize();
    RangedBuckets->incrementSize();
  }

  /**
  * Method to allow RangedBuckets size to be controlled by the enclosed
  * PositionalLists.
  */
  void decrementSize() {
    PositionalList<P>::decrementSize();
    parent->decrementSize();
  }
    
};

/**
* A simple but incredibly powerful data structure under certain circumstances.
*
* It's simply an array of PositionalLists which also have a value type.
*
* When a problem is presented where there needs to be a one to many mapping
* with integer-represented keys in a reasonably small range (no more than 
* O(n^2)), this data structure excels as an optimized implementation for various
* Abstract Data Types.  Such ADTs include but are not limited to Adaptable
* Priority Queues, Graphs*, and One-to-Many Maps.
*
* When a RangedBuckets implementation is possible, many operations in said ADTs
* are reduced to constant time with significantly less overhead.
*
* This can also be used to quickly implement Counting Sort.
*
* *Graphs can be represented as a one to many mapping from a vertex to edges or,
*   in a simpler implementation, other vertices.
*/
template<class V, class P>
class RangedBuckets {
  public:
  /**
  * Pointer to key "0".  Valid memory starts at data[bottom] and ends
  * at data[top - 1].  May be negatively indexed.
  */
  BucketPositionalList<V, P> *data;

  /** Index of the top valid bucket. */
  int topBucket;
  /** Index of the bottom valid bucket. */
  int bottomBucket;
  /** Number of items in buckets. */
  std::size_t size;
  /** Number of buckets. */
  std::size_t buckets;

  /**
  * Constructor for RangedBuckets.
  *
  * Though top and bottom are respectively upper and lower bounds for the
  * position of valid data, top is initialized at bottom and the opposite
  * for bottom.  This is because adding adjusts these values to a more
  * tightly constrained position than simply the ends of the array.
  *
  * @param bottom The lowest valid bucket.
  * @param top The highest valid bucket plus one.
  */
  RangedBuckets(int bottom, int top) : 
      size(0),
      topBucket(top - 1),
      bottomBucket(bottom),
      buckets(top - bottom) {
    data = new BucketPositionalList<V, P>[buckets]();
    //Set data to point to the "0" key.
    data = data - bottom;
  }

  void incrementSize() {
    size++;
  }

  void decrementSize() {
    size--;
  }

#ifndef NO_CHECKS
#define CHECK_BOUNDS(bucket) { \
  if (bucket < bottomBucket || buckect > topBucket) error(); \
}

  void error() {
    exit(1);
  }

#else
#define CHECK_BOUNDS(bucket)
#endif

  /**
  * Simply returns the bucket at a given index.
  * Should be the primary access method as it may include bound checking.
  */
  BucketPositionalList<V, P> *bucket(int bucket) {
    CHECK_BOUNDS(bucket)
    return data[bucket];
  }

  /**
  * Adds a value at the given bucket.
  * @param bucket The given bucket.
  * @param value The given value.
  * @return The position of the new pairing- it's important that the user
  * store this externally.
  */
  Position<P> *add(int bucket, P value) {
    auto position = bucket(bucket).addLast(value);
    return position;
  }

  Position<P> *add(int bucket, P value, void (*onPos)(Position<P> *)) {
    auto position = bucket(bucket).addLast(value);
    onPos(position);
    return position;
  }

  /**
  * Removes the given position.
  *
  * @param position The position to remove from a bucket.
  */
  void remove(Position<P> *position) {
    position->remove();
  }

};

#endif
