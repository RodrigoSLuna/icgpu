/*class to process data
 *
 *  @author Leticia Freire
 */

//includes
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <string>
#include "PrPixelHit.h"
#include "structs.h"
using namespace std;


class DataFile{
  public:
    void prepareData(string fileName); // method to prepare data to use for the tracking
    int getNoSensor(); // get number of sensors
    int getNoHit(); // get number of hits
    vector<float> getModule(); // get the Z modules
    vector<int> getNoHitsSensor(); // get the number of hits by sensor
    vector<vector<PrPixelHit> > getHits(); // get all hits of all sensors
    vector<PrPixelHit> getHitsSensor(int i); // get hits of i sensor
    void prepareResults(string fileName); // prepare data to compare with the results of tracking
    vector<vector<unsigned int> > getResult(); // get the tracks of the event
    void compareTracks(vector<TrackS> tracks); // compare the rebuilt tracks with the tracks of the event
    void compareGood(vector<TrackS> tracks);

  private:
    int no_sensor; //number of sensors
	int no_hits; //number of hits
	vector<float> module_z; //vector with all Z modules
	vector<int> no_hits_sensor; //vector with the number of hits by sensor
	vector<vector<PrPixelHit> > hits; //all the hits of the event
    vector<vector<unsigned int> > id_results; //struct with all the tracks of the event
    vector<int> isLong; //information about the tracks: 0 if the track is not long, 1 if the track is long
};
