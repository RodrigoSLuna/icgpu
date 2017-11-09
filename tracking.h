/* class to make the tracking
 *
 *  @author Leticia Freire
 */


#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "PrPixelHit.h"
// #include "structs.h"
#include "dados.h"
#include "clock_timer.h"
using namespace std;


#include <unistd.h>
#include <cstdlib>
// # define M_PI           3.14159265358979323846  /* pi */

const int INITIAL_STATUS = 1;
const int DIFFERENCE_SENSOR = 3;
const float BREAKING_ANGLE = 0.01;
//const float BREAKING_ANGLE = M_PI;
const float ACCEPTANCE_ANGLE = 0.2;
//const float ACCEPTANCE_ANGLE = M_PI;


class Tracking{
  public:
    void makeTracking(DataFile data);
    vector<TrackSegment> makeSimpleSegment(vector<PrPixelHit> nextHits, vector<PrPixelHit> currentHits);
    void forwardProcess(vector<TrackSegment>& currentSeg, vector<TrackSegment>& nextSeg, vector<vector<PrPixelHit> > hits);
    void backwardProcess(vector<vector<TrackSegment> > &tSegment, vector<TrackS> &tracks, vector<vector<PrPixelHit> > hits);
    bool compareHits(PrPixelHit one, PrPixelHit two);
    bool compareHits(TrackSegment one, TrackSegment two);
    float calculateAngle(float tx_cur, float ty_cur, float tx_next, float ty_next);
    bool compareBreakingAngle(float angle);
    void makeTrack(vector<vector<TrackSegment> > tSegment, TrackS &track, int sensor_id, vector<vector<PrPixelHit> > hits);
    vector<TrackS> combinationTrack(vector<vector<TrackSegment> > tSegment, TrackS &track, int sensor_id, vector<vector<PrPixelHit> > hits);
    bool testSegment(TrackSegment one, TrackSegment two, vector<vector<PrPixelHit> > hits);
    bool compareStatus(int status_one, int status_two);
    int chooseBestAngle(vector<TrackS> trackAux, float a, float b);
    vector<TrackS> getTracks();
    void parallelTracking(vector<vector<TrackSegment> > &tSegment, vector<TrackS> &tracks, vector<vector<PrPixelHit> > hits);
    static void *backwardProcessParallel(void *arg);



  private:
    vector<vector<TrackSegment> > tSegment;
    vector<TrackS> tracks;
    struct thread_data {
       int  thread_id;
       vector<vector<TrackSegment> > tSegment;
       TrackS track;
       int sensor_id;
       vector<vector<PrPixelHit> > hits;
    };
};

