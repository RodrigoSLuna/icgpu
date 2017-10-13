/*class to process data
 *
 *  @author Leticia Freire
 */

#include <fstream>
#include <iostream>
#include "dados.h"
// #include "structs.h"
// #include "PrPixelHit.h"
#include <json/json.h>

//para compilar: g++ -I /usr/include/jsoncpp/ dados.cpp -ljsoncpp

using namespace std;

void DataFile::prepareData() {
	/*opening file*/
	ofstream dataFile("dados.txt");
	ifstream ifs("0.json");
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj); // reader can also read strings

	/*number of sensors from 0.json */
	no_sensor = obj["event"]["number_of_sensors"].asInt();
	/*number of hits from 0.json */
	no_hits = obj["event"]["number_of_hits"].asInt();
	/*modules z*/
	const Json::Value& mod_z = obj["event"]["sensor_module_z"];
	for(int z = 0; z < mod_z.size(); z++) module_z.push_back(mod_z[z].asFloat());
	/*number of hits per sensor*/
	for(int n_hits = 0; n_hits < no_sensor; n_hits++) no_hits_sensor.push_back(obj["event"]["sensor_number_of_hits"][n_hits].asInt());

	/*building hits*/
	/*hit id*/
	const Json::Value& id = obj["event"]["hit_id"];
	/*hit x*/
	const Json::Value& x = obj["event"]["hit_x"];
	/*hit y*/
	const Json::Value& y = obj["event"]["hit_y"];
	/*hit z*/
	const Json::Value& z = obj["event"]["hit_z"];

	int j = 0;
	int number_hits = 0;
	for(int n_hits = 0; n_hits < no_sensor; n_hits++){
		number_hits += no_hits_sensor[n_hits];
		vector<PrPixelHit> aux;
		for(; j < number_hits; j++){
			PrPixelHit hit;
			hit.setHit(id[j].asUInt(),
		             x[j].asFloat(), y[j].asFloat(), z[j].asFloat(),
		             0.0, 0.0,
		             module_z[n_hits]);
			aux.push_back(hit);
		}
		hits.push_back(aux);
	}

	/*closing file*/
	dataFile.close();
}

void DataFile::prepareResults(){
	/*opening file*/
	ofstream dataFile("dados.txt");
	ifstream ifs("0.json");
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj); // reader can also read strings

	
	const Json::Value& particles = obj["montecarlo"]["particles"];

	// cout << particles[0][15] << endl;

	for(int particle = 0; particle < particles.size(); particle++){
		vector<unsigned int> aux;
		const Json::Value& ids = particles[particle][15];
		for(int id = 0; id < ids.size(); id++){
			aux.push_back(ids[id].asUInt());
		}
		id_results.push_back(aux);
		isLong.push_back(particles[particle][6].asUInt());
	}

	for(int i = 0; i < id_results.size(); i++){
		vector<unsigned int> aux = id_results[i];
		for(int j = 0; j < aux.size(); j++)
			dataFile << id_results[i][j] << " ";
		dataFile << endl;
	}

    cout << "total tracks: " << id_results.size() << endl;
	/*closing file*/
	dataFile.close();
}

void DataFile::compareTracks(vector<TrackS> tracks){
	/*opening file*/
	ofstream goodTrack("good.txt"); //good tracks
	ofstream fakeTrack("fake.txt"); //fake tracks
	ofstream cloneTrack("clone.txt"); //clone tracks


	int visitedTracks[id_results.size()];
	for(int i = 0; i < id_results.size(); i++)
		visitedTracks[i] = 0;


    int countLong = 0;
    for(int i = 0; i < isLong.size(); i++){
    	cout << isLong[i] << " ";
        if(isLong[i]) countLong++;
    }

	cout << "Total de tracks reconstrutívies: " <<  id_results.size() << endl;
	cout << "Total de tracks reconstrutívies long: " << countLong << endl;
	cout << "Total de tracks formadas: " <<  tracks.size() << endl;

	/*variables*/
	int goodTracks = 0;
	int fakeTracks = 0;
	int cloneTracks = 0;
	int longTracks = 0;


	/*comparing formed tracks with original tracks*/
	for(int track = 0; track < tracks.size(); track++){
		vector<PrPixelHit> hits = tracks[track].getHits();
		/*comecar de tras pra frente*/
		int hit = hits.size()-1;
		unsigned int id = hits[hit].id();
		int trackUsada = 0;
		while(hit >= 0){
			// int good = 0;
			for(int i = 0; i < id_results.size(); i++){
				int good = 0;
				int j = 0;
				while(j < id_results[i].size()){
					while(id == id_results[i][j]){
						good++;
						j++; hit--;
						id = hits[hit].id();
					}
					// cout << good << endl;
					if(good > 0){
						// cout << good << endl;
						trackUsada = 1;
						float goodPer = (float) good/id_results[i].size();
						// cout << goodPer << endl;
						if(goodPer >= 0.6){
							if(!visitedTracks[i]){
								visitedTracks[i]++;
								// cout << "real: indice: " << i << " contagem: " << visitedTracks[i] << endl;
								goodTracks++;
								// cout << goodPer << endl;
								goodTrack << goodTracks << ":";
								for(int k = hits.size()-1; k>= 0; k--) 
									goodTrack << hits[k].id() << ", ";
								goodTrack << endl;
								if(isLong[i]){
									longTracks++;
									// cout << "long track: " << i << endl; 
								}
							}
							else if(visitedTracks[i] > 0){
								visitedTracks[i]++;
								// cout << "clone: indice: " << i << " contagem: " << visitedTracks[i] << endl;
								cloneTracks++;
								// cout << goodPer << endl;
								cloneTrack << cloneTracks << ":";
								for(int k = hits.size()-1; k>= 0; k--) 
									cloneTrack << hits[k].id() << ", ";
								cloneTrack << endl;
							}
						}  
						else{
							// cout << goodPer << ", " << fakeTracks << endl;
							fakeTracks++;
							fakeTrack << fakeTracks << ":";
							for(int m = hits.size()-1; m>= 0; m--) 
								fakeTrack << hits[m].id() << ", ";
							fakeTrack << endl;
						}
						break;
					}
					
					j++;
				}
				if(trackUsada) break;
			}
			hit--;
		}
	}


	cout << "Total de tracks reconstrutívies e reconstruídas: " <<  goodTracks << endl;
	cout << "Total de tracks fakes: " <<  fakeTracks << endl;
	cout << "Total de tracks clones: " << cloneTracks << endl;
	cout << "Total de tracks reconstrutívies e reconstruídas long: " << longTracks << endl;

	/*closing file*/
	goodTrack.close();
	fakeTrack.close();

}

int DataFile::getNoSensor() {return no_sensor;}
int DataFile::getNoHit() {return no_hits;}
vector<float> DataFile::getModule() {return module_z;}
vector<int> DataFile::getNoHitsSensor() {return no_hits_sensor;}
vector<vector<PrPixelHit> > DataFile::getHits() {return hits;}
vector<PrPixelHit> DataFile::getHitsSensor(int i) {return hits[i];}
vector<vector<unsigned int> > DataFile::getResult() {return id_results;}
