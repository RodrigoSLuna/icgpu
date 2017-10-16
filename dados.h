/*class to process data
 *
 *  @author Leticia Freire
 */

#include <fstream>
#include <iostream>
#include <json/json.h>
#include <string>
#include "PrPixelHit.h"
#include "structs.h"
using namespace std;

class DataFile{
  public:
    void prepareData(string fileName);
    int getNoSensor();
    int getNoHit();
    vector<float> getModule();
    vector<int> getNoHitsSensor();
    vector<vector<PrPixelHit> > getHits();
    vector<PrPixelHit> getHitsSensor(int i);
    void prepareResults(string fileName);
    vector<vector<unsigned int> > getResult();
    void compareTracks(vector<TrackS> tracks);
    

  private:
    int no_sensor;
	int no_hits;
	vector<float> module_z;
	vector<int> no_hits_sensor;
	vector<vector<PrPixelHit> > hits;
    vector<vector<unsigned int> > id_results;
    vector<int> isLong;
};
