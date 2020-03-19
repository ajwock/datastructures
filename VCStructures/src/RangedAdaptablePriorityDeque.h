#ifndef RANGED_PDEQUE__
#define RANGED_PDEQUE__

#include "RangedBuckets.h"

/**
* Struct that can possibly take up no space.
* Hopefully the compiler will allow the unused "value" field from each
* BucketPositionalList to take up no space.
*/
struct Empty {};

/**
* RangedBuckets-based implementation of Priority Deque.
*
* The RangedBuckets implementation has a trade off:
* All Priority Deque operations are in constant time, but keys are limited to 
* integers within a range set at initialization.
*
* Implements Adaptable Priority Queue functionality: allows key changes and
* random removal of pairs- though this functionality requires external storage
* of data positions.
*
* There is no difference between this and a RangedBuckets-based implementation
* of a regular Priority Queue besides allowing the user to pop from either end;
* no performance loss is incurred by the availability of double-ended pops.
*/
template<class T>
class RangedAdaptablePriorityDeque : RangedBuckets<Empty, T> {
public:
  /** The top valid bucket plus one. */
  int top;
  /** The bottom valid bucket. */
  int bottom;
 
  void null_function() {}

  void (*notification_function)(void *);

  /**
  * Constructor for RangedAdaptablePriorityDeque.
  *
  * Though top and bottom are respectively upper and lower bounds for the
  * position of valid data, top is initialized at bottom and the opposite
  * for bottom.  This is because adding adjusts these values to a more
  * tightly constrained position than simply the ends of the array.
  *
  * @param bottom The lowest valid bucket.
  * @param top The highest valid bucket plus one.
  */
  RangedAdaptablePriorityDeque(int bottom, int top) :
      RangedBuckets<Empty, T>(bottom, top),
      top(bottom),
      bottom(top - 1),
      notification_function(null_function);
      {}

#ifndef NO_CHECKS
#define CHECK_EMPTY if (size == 0) error();
#else
#define CHECK_EMPTY
#endif

  struct DequeVoid {
    RangedAdaptablePriorityDeque<T> *deque;
    void *pointer;
  };

  void notify(void *p) {
    DequeVoid dv = {this, p};
    notifcation_function(&dv);
  }

  void setNotificationFunction(void (*notification)(void *)) {
    notification_function = notification;
  }

  P peepTop() {
    CHECK_EMPTY
    while (bucket(top).size == 0) {
      top--;
    }
    return bucket(top).first()->value;
  }

  P peepBottom() {
    CHECK_EMPTY
    while (data[bottom].size == 0) {
      bottom++;
    }
    return data[bottom].last()->value;
  }

  P popTop() {
    CHECK_EMPTY
    while (bucket(top).size == 0) {
      top--;
    }
    Position<P> *toRemove = data[top].first();
    P value = toRemove->value;
    toRemove->remove();
    return value;
  }

  P popBottom() {
    CHECK_EMPTY
    while (bucket(bottom).size == 0) {
      bottom++;
    }
    Position<P> *toRemove = bucket(bottom).last();
    P value = toRemove->value;
    toRemove->remove();
    return value;
  }

  Position<P> *adapt(Position<P> *position, int bucket) {
    P value = position->value;
    remove(position);
    return add(bucket, value);
  }
  
  Position<P> *adapt_fn(Position<P> *position, int bucket, void (*onPos)(Position<P> *)) {
    P value = position->value;
    remove(position);
    return add_fn(bucket, value, onPos);
  }

  void eliminate(Position<P> *position) {
    remove(position);
  }

}

#endif
