/* class to make the tracking
 *
 *  @author Leticia Freire
*/
#include "tracking.h"
using namespace std;
#if defined(__cplusplus)
extern "C"
#endif


/*compare if two hits are equal */
inline bool Tracking::compareHits( PrPixelHit const &one,  PrPixelHit const &two){
	//cout << "modulo de um hit : " << one.module() << endl;
	//if(one.module() == two.module()){ cout << "nao estao no mesmo modulo" << endl; exit (EXIT_FAILURE); }
        if (one.id() == two.id()) return true;
	//if ((one.x() == two.x()) && (one.y() == two.y()) && (one.z() == two.z())) return true;
	return false;
}

/*compare if the TrackSegment one and two has two equal hits*/
inline bool Tracking::compareHits( TrackSegment &one,  TrackSegment &two){
	PrPixelHit one_one =  one.getFirstHit();
	PrPixelHit one_two = one.getSecondHit();
	PrPixelHit two_one = two.getFirstHit();
	PrPixelHit two_two = two.getSecondHit();
	if (one_one.id() == two_one.id()) {
            return true;}
	if (one_one.id() == two_two.id()){ 
            return true;}
	if (one_two.id() == two_one.id()){
            return true;}
	if (one_two.id() == two_two.id()){
            return true;}
	//if ((one.x() == two.x()) && (one.y() == two.y()) && (one.z() == two.z())) return true;
	return false;
}

/*calculate the angle between two segments*/
inline double Tracking::calculateAngle(double tx_cur, double ty_cur, double tx_next, double ty_next){
	double difference_tx = (tx_next - tx_cur);
	double difference_ty = (ty_next - ty_cur);
	//return sqrt(difference_tx*difference_tx + difference_ty*difference_ty);
	// Modificado para o valor do breakingAngle
	return (difference_tx*difference_tx + difference_ty*difference_ty);
}

/*compare if the angle is smaller than the breaking angle*/
inline bool Tracking::compareBreakingAngle(double angle){
//Retirei o sqrt( da funcao getAngle, logo preciso multiplicar o breaking angle, elevando ambos os lados ao quadrado
//	printf("angle: %lf, breaking_angle^2 = %lf\n",angle,BREAKING_ANGLE*BREAKING_ANGLE);
	if(angle <= BREAKING_ANGLE*BREAKING_ANGLE) return true;
	return false;
}

/*compare if two segments can be neighbors seeing the status*/
inline bool Tracking::compareStatus(int status_one, int status_two){
	//cout << "status one: " << status_one << " e status two: "<< status_two << endl;
        if(status_two == (status_one-1)) return true;
	if(status_one == (status_two-1)) return true;
        return false;
}

/*choose the best track according the equation */
int Tracking::chooseBestTrack( vector<TrackS> &trackAux, double a, double b){
	//double angle = trackAux[0].getLastAngle();
    TrackSegment segment = trackAux[0].getLastSeg();
    PrPixelHit one_track = segment.getFirstHit();
    //PrPixelHit two_track = segment.getSecondHit();

    //equacao reta: ax+b = y
    //double y = (one_track.x() - two_track.x())/(one_track.y() - two_track.y());
    double y = b+ a*one_track.x();
    int indice = 0;
    double difference = (one_track.y() - y)*(one_track.y() - y);
	for(int iangle = 1; iangle < (int) trackAux.size(); iangle++){
		segment = trackAux[iangle].getLastSeg();
		one_track = segment.getFirstHit();
		y = b+ a*one_track.x();
		double difference_aux = (one_track.y() - y)*(one_track.y() - y);
		if(difference_aux < difference){
			//angle = trackAux[iangle].getLastAngle();
			difference = difference_aux;
			indice = iangle;
		}
	}
	return indice;
}

/*test with two segments can be part of a track*/
inline bool Tracking::testSegment( TrackSegment &one, TrackSegment &two, vector<vector<PrPixelHit> > &hits){
	PrPixelHit hit_one =  one.getFirstHit();
	PrPixelHit hit_two = two.getSecondHit();
	//bool  testHits = compareHits(hit_one, hit_two);
	bool  testHits = compareHits(one, two);
    if(!testHits) return false;
    //cout << "resultado do compareHits: " << testHits << endl;
	//bool testHit_one = compareHitUsed(hit_one);
	//bool testHit_two = compareHitUsed(hit_two);
	bool seg_status = compareStatus(one.getStatus(), two.getStatus());
    if(!seg_status) return false;
	//calculate angle
	double angle = calculateAngle(one.getTx(), one.getTy(), two.getTx(), two.getTy());
	bool breakingAngle = compareBreakingAngle(angle);
    if(!breakingAngle) return false;
	return (breakingAngle && testHits && seg_status);//  && testHit_one && testHit_two);
}


/*make the combination between track and segment*/
vector<TrackS> Tracking::combinationTrack( vector<vector<TrackSegment> > &tSegment, TrackS &track, int sensor_id,  vector<vector<PrPixelHit> > &hits){
	vector<TrackS> tracks;
	vector<TrackSegment> segments = tSegment[sensor_id];
	//take the last segment
	TrackSegment aux = track.getLastSeg();

	double angle;
	TrackS aux_track;
	int count = 0;
	for(int iseg = 0; iseg < (int) segments.size(); iseg++){

		if(testSegment(aux, segments[iseg], hits)){				
			angle = calculateAngle(aux.getTx(), aux.getTy(), segments[iseg].getTx(), segments[iseg].getTy());
			aux_track = track;
			aux_track.setLastSeg(segments[iseg], aux_track);
			aux_track.setLastAngle(angle);
				
			vector<PrPixelHit> points = segments[iseg].getTrackSegment();
			for(int i = 0;i<points.size();i++){
				aux_track.addHits(points[i]);			
			}
			vector<PrPixelHit> teste = aux.getTrackSegment();

			tracks.push_back(aux_track);
	
			count++;
		}


	}
	if(count == 0) tracks.push_back(track);

	return tracks;
}

/*make the track from the sensor_id sensor*/
void Tracking::makeTrack(vector<vector<TrackSegment> > &tSegment, TrackS &track, int sensor_id, vector<vector<PrPixelHit> > &hits){
	TrackSegment aux;
	int indice = 0;
	int id = sensor_id;
	
    TrackSegment track_reta = track.getLastSeg();
    PrPixelHit one_track = track_reta.getFirstHit();
    PrPixelHit two_track = track_reta.getSecondHit();
//	printf("First Hit: x:%.3f,y: %.3f, z: %.3f\n",one_track.x(),one_track.y(),one_track.z());

//	printf("Second Hit: x:%.3f,y: %.3f, z: %.3f\n",two_track.x(),two_track.y(),two_track.z());
//   	puts("\n\n");
	//equacao reta: az+b = y ^ x = cz + d
   double a = (one_track.x() - two_track.x())/(one_track.y() - two_track.y());
   double b_reta = one_track.y() - a*one_track.x();
   //cout << "a : " << a << " ,b: " << b_reta << endl;

	for(; id >= 0; id = id-2){
		/*make combination between the track and the segments*/
//		printf("->>Combinacao da track: %d %d\n",id,id-2);
		vector<TrackS> trackAux = combinationTrack(tSegment, track, id, hits);
		
		indice = chooseBestTrack(trackAux, a, b_reta);
		track = trackAux[indice];
		
		//track.setLastSeg(trackAux[indice], track.);

        /*update information about the track*/
		track_reta = track.getLastSeg();
		PrPixelHit last_hit = track_reta.getFirstHit();
		a = (last_hit.x() - one_track.x())/(last_hit.y() - one_track.y());
		b_reta = one_track.y() - a*one_track.x();
//		puts("\n\n");
	}
	

}

/*make the segments*/
vector<TrackSegment> Tracking::makeSimpleSegment(vector<PrPixelHit> &nextHits, vector<PrPixelHit> &currentHits,
 int snr_cur, int snr_nxt){
	vector<TrackSegment> tSegment;
 
	for(int id_current = 0; id_current < (int) currentHits.size(); id_current++){
		double x_zero = currentHits[id_current].x();
		double y_zero = currentHits[id_current].y();
		double z_zero = currentHits[id_current].z();
		for(int id_next = 0; id_next < (int) nextHits.size(); id_next++){
			double x_one = nextHits[id_next].x();
			double y_one = nextHits[id_next].y();
			double z_one = nextHits[id_next].z();
			/*calculate tx and ty*/
			double tx = (x_one - x_zero)/(z_one - z_zero);
			double ty = (y_one - y_zero)/(z_one - z_zero);
			//see the angle between the two hits
			double ff = ACCEPTANCE_ANGLE;
			//printf("ACC: %f, ACC*ACC: %f\n",ff,ff*ff);
			//printf("tx: %.2f ty: %.2f, tx*tx+ty*ty : %f, ANGLE: %f\n",tx,ty,tx*tx + ty*ty, ff*ff);
		
			if(tx*tx+ty*ty <= (ff*ff)){ // && >= PI/2??
               			//puts("Formou Segmento");
				vector<PrPixelHit> tmp;
				//make segment object
//				printf("(snr_cur:%d, snr_nxt %d), points: cur: %d, nxt: %d\n",snr_cur,snr_nxt,id_current,id_next);	
				tmp.push_back(currentHits[id_current]);
				tmp.push_back(nextHits[id_next]);
				TrackSegment aux (tmp, INITIAL_STATUS, tx, ty);
				//count the segment
				//no_segments++;
				tSegment.push_back(aux);	
			}
		}
	}
	
  return tSegment;
}
//Modificacoes: retirada da funcao sqrt(), utilizando breaking_angle*breaking angle, e tirei sqrt() da funcao calculateAngle!
// complexidade sqrt() desconhecida <
/*make the forward process*/
	void Tracking::forwardProcess(vector<TrackSegment> &currentSeg, vector<TrackSegment> &nextSeg, 
		vector<vector<PrPixelHit> > &hits,int snr_cur, int snr_nxt){
		//printf("snr_cur: %d, snr_nxt: %d\n",snr_cur,snr_nxt);
		for(int id_current = 0; id_current < (int) currentSeg.size(); id_current++){
			//take the tx and ty of the segments
			double tx_cur = currentSeg[id_current].getTx();
			double ty_cur = currentSeg[id_current].getTy();
			//take the first hit of the segment on currentSeg
			PrPixelHit hit_cur = currentSeg[id_current].getSecondHit();
			//PrPixelHit hit_cur = takeHit(hit_cur_id, hits);
			for(int id_next = 0; id_next < (int) nextSeg.size(); id_next++){
				//take the tx and ty of the segments
				TrackSegment tmp_seg = nextSeg[id_next];
				double tx_next = tmp_seg.getTx();
				double ty_next = tmp_seg.getTy();
				//take the first hit of the segment on nextSeg
				//vector<PrPixelHit> tmpNext = nextSeg[id_next].getTrackSegment();
				PrPixelHit hit_next = nextSeg[id_next].getFirstHit();
				//PrPixelHit hit_next = takeHit(hit_next_id, hits);
	            //if (!compareHits(hit_cur, hit_next)) continue;
	            if(!compareHits(currentSeg[id_current], nextSeg[id_next])) continue;
				//calculate the breaking angle and verify if the segments has one common hit
				double angle = calculateAngle(tx_cur, ty_cur, tx_next, ty_next);
				//verify if the angle and the hit it's ok
				if(compareBreakingAngle(angle)){
					//increase the status
					//if two segments has the same continuation, the preference is for the segment with greater status
					if(currentSeg[id_current].getStatus()+1 > nextSeg[id_next].getStatus()){
						nextSeg[id_next].setStatus(currentSeg[id_current].getStatus()+1);
					}
				}
			}
		}
	}

/*make the backward process*/
void Tracking::backwardProcess(vector<vector<TrackSegment> > &tSegment, vector<TrackS> &tracks, vector<vector<PrPixelHit> > &hits){
	int i = 1;
	for(int isen =  tSegment.size()-1; isen >= 0; isen--){
		
		vector<TrackSegment> currentSensor = tSegment[isen];
		for(int iseg = 0; iseg < (int) currentSensor.size(); iseg++){
			// Cria-se uma track com aquele segmento, e adiciona
			// os pontos na propria track necessario?Checando.
			TrackS track(currentSensor[iseg]);
			vector<PrPixelHit> pontos_seg = currentSensor[iseg].getTrackSegment();
			for(auto p : pontos_seg){
				track.addHits(p);
			}
//			printf("isen:%d, status: %d\n",iseg,currentSensor[iseg].getStatus());
			//make track
			// cout << "sensor: " << isen-2 << endl;
			makeTrack(tSegment, track, isen-2, hits);


			//setUsedTrack(tSegment, auxTrack[id], id_sensor);
			set<PrPixelHit> aux = track.getSetHits();
			
			/*if the track has less than 2 segments, continue*/
			if(aux.size() <= 2) continue;
			//printTrack(track, i); i++;
			tracks.push_back(track);
			//return;
		}
	
	}

}




/*make the backward process with threads*/
void *Tracking::backwardProcessParallel(void *arg){
	Tracking::thread_data *my_data;
	/*getting the data*/
	my_data = ( Tracking::thread_data *) arg;
	TrackSegment aux;
	int indice = 0;
	int id = my_data->sensor_id;
	vector<vector<TrackSegment> > tSegment = my_data->tSegment;
	TrackS &track = my_data->track;
	vector<vector<PrPixelHit> > hits = my_data ->hits;
	TrackSegment track_reta = track.getLastSeg();
	PrPixelHit one_track = track_reta.getFirstHit();
	PrPixelHit two_track = track_reta.getSecondHit();


	//equacao reta: ax+b = y
	double a = (one_track.x() - two_track.x())/(one_track.y() - two_track.y());
	double b_reta = one_track.y() - a*one_track.x();
	//cout << "a : " << a << " ,b: " << b_reta << endl;

	/*making the combination track and choosing the best combination*/
	Tracking obj;
	for(; id >= 0; id = id-2){
		vector<TrackS> trackAux = obj.combinationTrack(tSegment, track, id, hits);
		indice = obj.chooseBestTrack(trackAux, a, b_reta);
		track = trackAux[indice];

		track_reta = track.getLastSeg();
		PrPixelHit last_hit = track_reta.getFirstHit();
		a = (last_hit.x() - one_track.x())/(last_hit.y() - one_track.y());
		b_reta = one_track.y() - a*one_track.x();
		// vector<PrPixelHit> Thits = track.getHits();
		// cout << "track: " << Thits.size() << endl;
		// track.setLastSeg(trackAux[indice], track);
	}
	my_data->track = track;
	return (void *) my_data;
	
}

/*make the parallel tracking*/
void Tracking::parallelTracking(vector<vector<TrackSegment> > &tSegment, vector<TrackS> &tracks, vector<vector<PrPixelHit> > hits){
	int i = 1;
	int rc;
    /**/
	for(int isen =  tSegment.size()-1; isen >= 0; isen--){
		vector<TrackSegment> currentSensor = tSegment[isen];
		int NUM_THREADS = (int) currentSensor.size();
		//int NUMCONS = (int) currentSensor.size();
		//int NUMPROD = (int) currentSensor.size();
		pthread_t threads[NUM_THREADS];
		pthread_attr_t attr;
		void *status;

        /*creating thread*/
		pthread_attr_init(&attr);
   		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		thread_data td[NUM_THREADS];
		
		/*making the backward process in each thread*/
		for(int iseg = 0; iseg < NUM_THREADS; iseg++){
			TrackS track(currentSensor[iseg]);
			td[iseg].thread_id = iseg;
			td[iseg].tSegment = tSegment;
			td[iseg].track = track;
			td[iseg].sensor_id = isen-2;
			td[iseg].hits = hits;
			pthread_create(&(threads[iseg]), NULL, backwardProcessParallel, (void *) &td[iseg]);
      	}
   
        /*taking the results of each thread*/
		for(i=0; i< NUM_THREADS; i++){
			// void *returnValue;
			pthread_join(threads[i], &status);
			// Comentando **********************************
			if (status != 0){
			//	vector<PrPixelHit> aux = td[i].track.getHits();
				// cout << "TAMANHO: " << aux.size() << endl;
			//	if(aux.size() <= 2){
					//cout << "entrei aqui" << endl;
			//		continue;
			//	} 
				// printTrack(track, i); i++;
			//	tracks.push_back(td[i].track);
			}
			else
				cout << "thread failed" << endl;
   		}	 
		
	}
}

/*make the tracks with the information of the event*/
void Tracking::makeTracking(DataFile &data){
	/*time*/
 	double start, finish, elapsed;
	int no_sensors = data.getNoSensor();
	int contHits = 0;
	vector<vector<PrPixelHit>> hits = data.getHits();
	for(int i = 0; i < hits.size(); i++){
		contHits = contHits+hits[i].size();
	}
	/*start counting time*/
//	GET_TIME(start);
	/*make segments*/
	
	GET_TIME(start);
	for(unsigned isen = 0; isen < no_sensors-2; isen++){
		vector<TrackSegment> tmpSeg(  makeSimpleSegment(hits[isen+2], hits[isen],isen,isen+2) );
		tSegment.push_back(tmpSeg);
	}
	/*finish counting time*/

	GET_TIME(finish);
	elapsed = finish - start;
	//ofstream log_1("makeSegmentSeq.txt", ios_base::app | ios_base::out);
	//log_1 << elapsed << " " << contHits <<  endl;
	//log_1.close();
	//printf("\t \t TEMPO DA MAKE makeSimpleSegment: %lf\n",elapsed);	

	double M_x[64][64],M_y[64][64],M_z[64][64],M_seg[64][64][64];
	for(int i = 0;i < no_sensors ;i++ ){
		for(int j = 0;j<  hits[i].size() ;j++){
			M_x[i][j] = hits[i][j].x();
			M_y[i][j] = hits[i][j].y();
			M_z[i][j] = hits[i][j].z();

		}
	} 

	elapsed = HostBuild(M_x,M_y,M_z,M_seg,ACCEPTANCE_ANGLE);
	

	ofstream log_2("makeSegmentParalel.txt", ios_base::app | ios_base::out);
	log_2 << elapsed << " " << contHits <<  endl;
	log_2.close();
	//count the tothal of segments
//	ofstream log("makeSimplesSegment.txt", ios_base::app | ios_base::out);
//	log << elapsed << " " << contHits <<  endl;
//	log.close();
//	cout << "Quantidade de segmentos: " << contSeg << endl;

//	int i = 0;
	/*start counting time*/
//	GET_TIME(start);
    /*increase the status*/



	for(int isen = 0; isen < (int) tSegment.size()-2; isen++){
		//std::cout << tSegment[isen].size() << std::endl;
		forwardProcess(tSegment[isen], tSegment[isen+2], hits,isen,isen+2);
	}
	/*finish counting time*/
  //  GET_TIME(finish);
    //elapsed = finish - start;    
//	ofstream log("fowardProcess.txt", ios_base::app | ios_base::out);
//	log << elapsed << " " << contHits <<  endl;
//	log.close();

	/*start counting time*/
	/*backward process*/
//    parallelTracking(tSegment, tracks, hits);
      backwardProcess(tSegment, tracks, hits);
	/*finish counting time*/
    GET_TIME(finish);
    elapsed = finish - start;
	// for(int i = 0; i < hits.size(); i++){
	// 	contHits = contHits+hits[i].size();
	// }
	// ofstream log("TotalAlgorithm.txt", ios_base::app | ios_base::out);
	// log << elapsed << " " << contHits <<  endl;
	// log.close();
 
}



vector<TrackS> Tracking::getTracks() {return tracks;} /*get all rebuilt tracks*/
