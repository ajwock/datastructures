#ifndef VERTEX_COVER_HEAD
#define VERTEX_COVER_HEAD

#include <string>
#include <vector>
#include <ctime>
#include "VCGraph.h"
#include "IntAdaptableBucketHeap.h"

class VertexCover {
private:
  void maxDegreeIteration(VCGraph *g);
  void minDegreeIteration(VCGraph *g);
  void twoApproxIteration(VCGraph *g);
  void queueMaxDegreeIteration(IntAdaptableBucketHeap *queue, VCGraph *g);
  void queueMinDegreeIteration(IntAdaptableBucketHeap *queue, VCGraph *g);

  IntAdaptableBucketHeap *createVertexHeap(VCGraph *g);

  double convertToMs(clock_t start, clock_t end);

public:
  char *hueristic;
  int value;
  double runtime;
  int iterations;
  std::vector<int> solution;

  void maxDegreeApproximation(VCGraph *g);
  void minDegreeApproximation(VCGraph *g);
  void twoApproximation(VCGraph *g);

  void queueMaxDegreeApproximation(VCGraph *g);
  void queueMinDegreeApproximation(VCGraph *g);

  void report();
  void reportExtended();
  void reportFull();
};

#endif
