/* class to make the tracking
 *
 *  @author Leticia Freire
 */

// includes
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
#include <unistd.h>
#include <cstdlib>
// # define M_PI           3.14159265358979323846  /* pi */
using namespace std;

// defining constant variables
const int INITIAL_STATUS = 1;
const int DIFFERENCE_SENSOR = 3;
// const double BREAKING_ANGLE = 0.01; // aumentar para 0.02
const double BREAKING_ANGLE = 0.002;
const double ACCEPTANCE_ANGLE = 0.45;

//const double ACCEPTANCE_ANGLE = M_PI;


class Tracking{
  public:
    void makeTracking(DataFile &data); //make the tracks with the information of the event
    vector<TrackSegment> makeSimpleSegment(vector<PrPixelHit> &nextHits, vector<PrPixelHit> &currentHits,int snr_cur,int snr_nxt); //make the segments
    void forwardProcess(vector<TrackSegment>& currentSeg, vector<TrackSegment>& nextSeg, vector<vector<PrPixelHit> > &hits,int snr_cur, int snr_nxt); //make the forward process
    void backwardProcess(vector<vector<TrackSegment> > &tSegment, vector<TrackS> &tracks, vector<vector<PrPixelHit> > &hits); //make the backward process
    bool compareHits( PrPixelHit const &one, PrPixelHit const &two); //compare if two hits are equal 
    bool compareHits( TrackSegment &one, TrackSegment &two); //compare if the TrackSegment one and two has two equal hits
    double calculateAngle(double tx_cur, double ty_cur, double tx_next, double ty_next); //calculate the angle between two segments
    bool compareBreakingAngle(double angle); //compare if the angle is smaller than the breaking angle
    void makeTrack(vector<vector<TrackSegment> > &tSegment, TrackS &track, int sensor_id, vector<vector<PrPixelHit> > &hits); //make the track from the sensor_id sensor
    vector<TrackS> combinationTrack( vector<vector<TrackSegment> > &tSegment,  TrackS &track, int sensor_id, vector<vector<PrPixelHit> > &hits); //make the combination between track and segment
    bool testSegment( TrackSegment &one, TrackSegment &two, vector<vector<PrPixelHit> > &hits); //test with two segments can be part of a track
    bool compareStatus(int status_one, int status_two); //compare if two segments can be neighbors seeing the status
    int chooseBestTrack(vector<TrackS> &trackAux, double a, double b); //choose the best track according the equation
    vector<TrackS> getTracks(); //get all rebuilt tracks
    void parallelTracking(vector<vector<TrackSegment> > &tSegment, vector<TrackS> &tracks, vector<vector<PrPixelHit> > hits); //make the parallel tracking
    static void *backwardProcessParallel(void *arg); //make the backward process with threads



  private:
    vector<vector<TrackSegment> > tSegment; //struct with all segments
    vector<TrackS> tracks; //struct with all tracks
    //struct to be used with the threads
    struct thread_data {
       int  thread_id;
       vector<vector<TrackSegment> > tSegment;
       TrackS track;
       int sensor_id;
       vector<vector<PrPixelHit> > hits;
    };
};

