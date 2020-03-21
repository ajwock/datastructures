#ifndef POSITIONAL_LIST__
#define POSITIONAL_LIST__

#ifndef NO_CHECKS
#include <stdlib.h>
#endif

#include <utility>

template <typename T>
class PositionalList;

template <typename T>
class Position {
  public:
  PositionalList<T> *container;
  T value;
  Position<T> *next;
  Position<T> *previous;

  Position() {}
  Position(PositionalList<T> *container) : container(container) {}
  Position(T value) { this->value = value; }
  Position(PositionalList<T> *container, T value) : container(container) {
    this->value = value;
  }

  Position<T> *addAfter(T value) {
    Position<T> *newNode = new Position<T>(container, value);
    newNode->previous = this;
    newNode->next = next;
    next->previous = newNode;
    next = newNode;
    container->incrementSize();
    return newNode;
  }

  Position<T> *addBefore(T value) {
    Position<T> *newNode = new Position<T>(container, value);
    newNode->next = this;
    newNode->previous = previous;
    previous->next = newNode;
    previous = newNode;
    container->incrementSize();
    return newNode;
  }

  void remove() {
    return container->remove(this);
  }
};

template <typename T>
class PositionalList {
  public:
  int size;
  Position<T> *head;
  Position<T> *tail;

  //Uses sentinels
  PositionalList() {
    head = new Position<T>(this);
    tail = new Position<T>(this);
    head->previous = nullptr;
    head->next = tail;
    tail->previous = head;
    tail->next = nullptr;
    size = 0;
  }

  //Hooks for subclasses, particularly so that the data structure can be made
  //observable.
  void incrementSize() { size++; }
  void decrementSize() { size--; }

  Position<T> *first() {
    return head->next;
  }

  Position<T> *last() {
    return tail->previous;
  }

  /**
  * Applies a function that takes an argument of a value of type T to a copy
  * of each value in the list.  Useful for working with primitive types
  * and pointers.  State contained within the list remains unchanged.
  */
  void foreachByValue(void (*apply)(T const value)) {
    for (Position<t> *curr = first(); curr != tail; curr = curr->next) {
      apply(curr->value);
    }
  }

  /**
  * Applies a function that takes an argument of a pointer to T to a pointer to
  * each value in the list.  State within the list may be modified.  Good to use
  * when the list contains complex objects.
  */
  void foreach(void (*apply)(T *const reference)) {
    for (Position<T> *curr = first(); curr != tail; curr = curr->next) {
      apply(&curr->value);
    }
  }

#ifndef NO_CHECKS
  void error() {
    exit(1);
  }

#define CHECK_CONTAINER(position) { if (position->container != this) error(); }
#else
#define CHECK_CONTAINER(position)
#endif

  void remove(Position<T> *position) {
    CHECK_CONTAINER(position)
    position->previous->next = position->next;
    position->next->previous = position->previous;
    delete position;
    decrementSize();
  }

  void removeFirst() {
    remove(head->next);
  }

  void removeLast() {
    remove(tail->previous);
  }

  Position<T> *addAfter(Position<T> *position, T value) {
    CHECK_CONTAINER(position)
    return position->addAfter(value);
  }

  Position<T> *addBefore(Position<T> *position, T value) {
    CHECK_CONTAINER(position)
    return position->addBefore(value);
  }

  Position<T> *addFirst(T value) {
    return addAfter(head, value);
  }

  Position<T> *addLast(T value) {
    return addBefore(tail, value);
  }

};


#endif
