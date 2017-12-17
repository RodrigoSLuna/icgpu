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

// prepare data to use for the tracking
void DataFile::prepareData(string fileName) {
	/*opening file*/
	//ofstream dataFile("dados.txt");
	ofstream log("log.txt", ios_base::app | ios_base::out);

	// ifstream ifs("0.json");
	/*open file to read*/
	ifstream ifs(fileName.c_str());
	/*reading the json file*/
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj); // reader can also read strings

	/*printing the file name*/
	log << "arquivo " << fileName << endl;
 	

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

    /*constructing the hits and putting in the vector*/
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
	//dataFile.close();
	log.close();
}

//prepare data to compare with the results of tracking
void DataFile::prepareResults(string fileName){
	/*opening file*/
	ofstream dataFile("dados.txt", ios_base::app | ios_base::out);
	// ifstream ifs("0.json");
	/*open file to read*/
	ifstream ifs(fileName.c_str());
	/*reading the json file*/
	Json::Reader reader;
	Json::Value obj;
	reader.parse(ifs, obj); // reader can also read strings

	/*getting information about the particles*/
	const Json::Value& particles = obj["montecarlo"]["particles"];
    /*getting the track of each particle*/
	for(int particle = 0; particle < particles.size(); particle++){
		vector<unsigned int> aux;
		const Json::Value& ids = particles[particle][15];
		for(int id = 0; id < ids.size(); id++){
			aux.push_back(ids[id].asUInt());
		}
		id_results.push_back(aux);
		/*information with the track is long or not*/
		isLong.push_back(particles[particle][6].asUInt());
	}

    /*printing the informations on a file*/
	for(int i = 0; i < id_results.size(); i++){
		vector<unsigned int> aux = id_results[i];
		for(int j = 0; j < aux.size(); j++)
			dataFile << id_results[i][j] << " ";
		dataFile << endl;
	}

    /*total number of tracks*/
    cout << "total tracks: " << id_results.size() << endl;
	/*closing file*/
	dataFile.close();
}

void DataFile::compareGood(vector<TrackS> tracks){
	/*opening file*/
	ofstream goodTrack("good.txt"); //good tracks
	ofstream fakeTrack("fake.txt"); //fake tracks
	ofstream cloneTrack("clone.txt"); //clone tracks
	ofstream angulosTrack("angulos.txt"); 
	ofstream log("log.txt", ios_base::app | ios_base::out);

    /*put 1 if the track if rebuilt*/
	int visitedTracks[id_results.size()];
	for(int i = 0; i < id_results.size(); i++)
		visitedTracks[i] = 0;


    int countLong = 0;
    for(int i = 0; i < isLong.size(); i++){
    	// cout << isLong[i] << " ";
        if(isLong[i]) countLong++;
    }

    /*verify if the track was used*/
    int isUsed[tracks.size()];
    for(int i = 0; i < tracks.size(); i++) isUsed[i] = 0;

    cout << "Total de tracks reconstrutívies: " <<  id_results.size() << endl;
	cout << "Total de tracks reconstrutívies long: " << countLong << endl;
	cout << "Total de tracks formadas: " <<  tracks.size() << endl;

    log << "Numeros esperados: " << endl;
	log << "Total de tracks reconstrutívies: " <<  id_results.size() << endl;
	log << "Total de tracks reconstrutívies long: " << countLong << endl;
	log << "Total de tracks formadas: " <<  tracks.size() << endl;
 
	/*variables*/
	int goodTracks = 0;
	int fakeTracks = 0;
	int cloneTracks = 0;
	int longTracks = 0;
	int entrei = 0;

	/*comparing formed tracks with original tracks*/
	for(int track = 0; track < tracks.size(); track++){
		vector<PrPixelHit> hits = tracks[track].getHits();
		for(int i = 0; i < id_results.size(); i++){
			int qtdHits = 0;
			for(int comp = 0; comp < hits.size(); comp++){
				for(int j = 0; j < id_results[i].size(); j++){
					// cout << "j: " << j << endl;
					if(hits[comp].id() == id_results[i][j]){ 
						// cout << track << " "<< i << " "<< j << endl;
						// cout << "reconstruida: " << hits[comp].id() << ", verdadeira: " << id_results[i][j] << endl;
						qtdHits++; break;
					}
				}
			}
			// if(qtdHits >= 3){ cout << "qtd de hits iguais: " << qtdHits << " qtd total: " << hits.size() << endl; exit(0);}
			if(qtdHits == 0) continue;
			else{
				float per = (float) qtdHits/hits.size();
				// cout << "percentual: " << per << endl;
				if(per >= 0.6){
					if(!visitedTracks[i]){
						visitedTracks[i]++;
						goodTracks++;
						goodTrack << goodTracks << ":";
						for(int k = hits.size()-1; k>= 0; k--) 
							goodTrack << hits[k].id() << ", ";
						goodTrack << endl;
						if(isLong[i]){
							longTracks++;
							angulosTrack << tracks[track].getLastAngle() << endl;
						}
					}
					else{
						visitedTracks[i]++;
						cloneTracks++;
						cloneTrack << cloneTracks << ":";
						for(int k = hits.size()-1; k>= 0; k--) 
							cloneTrack << hits[k].id() << ", ";
						cloneTrack << endl;
					}
				}
				else{
					fakeTracks++;
					fakeTrack << fakeTracks << ":";
					for(int m = hits.size()-1; m>= 0; m--) 
						fakeTrack << hits[m].id() << ", ";
					fakeTrack << endl;
				}
			}
		}
	}
	cout << "Total de tracks reconstrutívies e reconstruídas: " <<  goodTracks << endl;
	cout << "Total de tracks fakes: " <<  fakeTracks << endl;
	cout << "Total de tracks clones: " << cloneTracks << endl;
	cout << "Total de tracks reconstrutívies e reconstruídas long: " << longTracks << endl;
	cout << "entrei " << entrei << " vezes na comparação" << endl;

	/*
	loop sobre as tracks reconstruidas
		loop sobre as tracks verdadeiras
			enquanto tiver hits da reta reconstruida para serem comparados
				loop sobre cada hit de cada track verdadeira
			verifica se é good ou clone; se for, break
		se não for good, nem clone, é fake
	*/

}

/*compare the rebuilt tracks with the tracks of the event*/
void DataFile::compareTracks(vector<TrackS> tracks){
	/*opening file*/
	ofstream goodTrack("good.txt"); //good tracks
	ofstream fakeTrack("fake.txt"); //fake tracks
	ofstream cloneTrack("clone.txt"); //clone tracks
	ofstream angulosTrack("angulos.txt"); 
	ofstream log("log.txt", ios_base::app | ios_base::out);


	int visitedTracks[id_results.size()];
	for(int i = 0; i < id_results.size(); i++)
		visitedTracks[i] = 0;


    int countLong = 0;
    for(int i = 0; i < isLong.size(); i++){
    	// cout << isLong[i] << " ";
        if(isLong[i]) countLong++;
    }

	cout << "Total de tracks reconstrutívies: " <<  id_results.size() << endl;
	cout << "Total de tracks reconstrutívies long: " << countLong << endl;
	cout << "Total de tracks formadas: " <<  tracks.size() << endl;

    log << "Numeros esperados: " << endl;
	log << "Total de tracks reconstrutívies: " <<  id_results.size() << endl;
	log << "Total de tracks reconstrutívies long: " << countLong << endl;
	log << "Total de tracks formadas: " <<  tracks.size() << endl;
 
	/*variables*/
	int goodTracks = 0;
	int fakeTracks = 0;
	int cloneTracks = 0;
	int longTracks = 0;


	/*comparing formed tracks with original tracks*/
	for(int track = 0; track < tracks.size(); track++){
		vector<PrPixelHit> hits = tracks[track].getHits();
		// cout << "ultimo angulo: " << tracks[track].getLastAngle() << endl;
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
									angulosTrack << tracks[track].getLastAngle() << endl;
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

	/*printing on file*/
        log << "Numeros encontrados: " << endl;
	log << "Total de tracks reconstrutívies e reconstruídas: " <<  goodTracks << endl;
	log << "Total de tracks fakes: " <<  fakeTracks << endl;
	log << "Total de tracks clones: " << cloneTracks << endl;
	log << "Total de tracks reconstrutívies e reconstruídas long: " << longTracks << endl;

	/*closing file*/
	goodTrack.close();
	fakeTrack.close();
	cloneTrack.close();
	angulosTrack.close();
	log.close();

}

int DataFile::getNoSensor() {return no_sensor;} /*get number of sensors*/
int DataFile::getNoHit() {return no_hits;} /*get number of hits*/
vector<float> DataFile::getModule() {return module_z;} /*get the Z modules*/
vector<int> DataFile::getNoHitsSensor() {return no_hits_sensor;} /*get the number of hits by sensor*/
vector<vector<PrPixelHit> > DataFile::getHits() {return hits;} /*get all hits of all sensors*/
vector<PrPixelHit> DataFile::getHitsSensor(int i) {return hits[i];} /*get hits of i sensor*/
vector<vector<unsigned int> > DataFile::getResult() {return id_results;} /*get the tracks of the event*/
