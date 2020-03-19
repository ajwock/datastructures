#include "AdaptablePriorityQueue.h"

//Large base capacity because we're gonna see some big files.
AdaptablePriorityQueue::AdaptablePriorityQueue() {
  capacity = 4096;
  size = 0;
  data = new PQEntry<Vertex *>[capacity];
  data = data - 1;
}

PriorityQueue::~PriorityQueue() {
  delete [] (data + 1);
}

void PriorityQueue::swap(int left, int right) {
  PQEntry<Vertex *> temp = data[left];
  data[left] = data[right];
  data[right] = temp;
}

void PriorityQueue::enqueue(/**/) {
  size++;
  data[size] = NEW_ENTRY;
  int index = size;
  while (PQEntry::compare(data[index], data[index / 2]) > 0 && (index / 2) != 0)) {
    swap(index, index / 2);
    index = index / 2;
  }
  
}

void PriorityQueue::dequeue() {
  if (size == 0) {
    return;
  }
  data[1] = data[size];
  size--;
  int index = 1;
  int greater;

  while ((2 * index) < size && (PQEntry::compare(data[index], data[2 * index]) < 0 ||
                        PQEntry::compare(data[index], data[2 * index + 1]) < 0)) {
    if (PQEntry::compare(data[2 * index], data[2 * index + 1]) > 0) {
      greater = 2 * index;
    } else {
      greater = 2 * index + 1;
    }
    swap(index, greater);
    index = greater;
  }
}
