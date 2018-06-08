/*class that define the main method
 *
 *  @author Leticia Freire
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <locale>
//#include "dados.h"
#include "tracking.h"
#include <iomanip>
using namespace std;

void show_tracks(vector<TrackS> &tracks){
	ofstream track("TRACK.txt",ios_base::app | ios_base::out);
	track << "Quantidade de tracks: " << tracks.size() << endl;


	for(int j = 0;j<tracks.size();j++){
		track << "Track : " << j+1 << endl;
		track << "All Hits " << endl << endl; 
		set<PrPixelHit> points = tracks[j].getSetHits();
		for(auto v : points){
			track << "ID: "<< v.id() << endl;
			track << "X: " << setprecision(4) << v.x();
			track << " Y: " << setprecision(4) << v.y();
			track << " Z: " << setprecision(4) << v.z() << endl<<endl;
		}
	}
}


int main(){
	/*including date*/
	
//	ofstream log("log.txt", ios_base::app | ios_base::out);
/*
	time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    log << (now->tm_year + 1900) << '-' 
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday
         << endl;
*/
    /*adding the file names*/
    vector<string> s;
	s.push_back("events/4.json");/* s.push_back("events/1.json"); s.push_back("events/2.json");
	 s.push_back("events/3.json"); s.push_back("events/4.json");  s.push_back("events/5.json"); */ 
	for(int file = 0; file < s.size(); file++){
		DataFile data;
//		cout << "Rodando codigo: prepareData()" << endl;
		
		data.prepareData(s[file]);
//		cout << data.getNoHit() << endl;

//		cout << "testando os resultados" << endl;
	 	data.prepareResults(s[file]);
	 	// exit(0);

		Tracking teste;
//		cout << "Rodando codigo: makeTracking()" << endl;
		teste.makeTracking(data);
		
		
		
		vector<TrackS> tracks = teste.getTracks();
		
		
//		cout << "Rodando codigo: compareTracks(tracks)" << endl;
		data.compareGoodNewVersion(tracks);
		//data.compareGood(tracks);

		// Comentado ************** Falta da funcao getHits() que retorna os hits
		//ofstream trackFile("tracks.txt");
		//for(int i = 0; i < tracks.size(); i++){
		//	vector<PrPixelHit> hits = tracks[i].getHits();
		//	trackFile << "(";
		//	for(int j = 0; j < hits.size(); j++){
		//		trackFile  << hits[j].id() << ", ";
		//	}
		//	trackFile << ")" << endl;
		//}
//		show_tracks(tracks);
	}
	
	//close the log file
//	log.close();
	return 0;
}
