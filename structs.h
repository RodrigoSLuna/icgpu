/* structs to be used in the code
 *
 *  @author Leticia Freire
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <iostream>
#include "PrPixelHit.h"
#include <set>
using namespace std;



/*class to store the segments*/
class TrackSegment{
	public:
		/*one segments is made with two hits*/
		void setTrackSegment(PrPixelHit one, PrPixelHit two){
			trackSegment.push_back(one); trackSegment.push_back(two);
		}

        //colocar: tx, ty, estado
		vector<PrPixelHit> getTrackSegment() {return trackSegment;}
		PrPixelHit getFirstHit(){return trackSegment[0];}
		PrPixelHit getSecondHit(){return trackSegment[1];}
		
		
		//trackSegment.size()-1
		void setTrackSegment(PrPixelHit hit) {trackSegment.push_back(hit);}
		/*get the status of the segment*/
		int getStatus(){return status;}
		/*set the segment status*/
		void setStatus(int _status){status = _status;}

	/*	void printTrackSegment(){
			for(int i = 0; i < trackSegment.size:q(); i++)
				cout << "x: " << trackSegment[i].x() << " y: " << trackSegment[i].y() << " z: " << trackSegment[i].z() << endl;
		} */

		double getTx(){ return tx;}
		void setTx(double _tx){tx = _tx;}
		double getTy(){return ty;}
		void setTy(double _ty){ty = _ty;}

		TrackSegment(){}
		TrackSegment(vector<PrPixelHit> _trackSegment, int _status, double _tx, double _ty){
			trackSegment = _trackSegment;
			status = _status;
			tx = _tx;
			ty = _ty;
		}


	private:
		vector<PrPixelHit> trackSegment;
		int status;
		double tx;
		double ty;
};

/*class to store the tracks*/
/*a track is made with 2 or more segments*/
class TrackS{
	public:
		
		TrackSegment getLastSeg() {return m_lastSeg;}
		//void setLastSeg(TrackSegment trackSeg) {m_lastSeg = trackSeg;}
		void addHits(PrPixelHit hit) {
			cnj_hits.insert(hit);	
		}
		vector<PrPixelHit> getHits(){ return m_hits;}
		set<PrPixelHit> getSetHits() {return cnj_hits;}
		int NumberHits() {return m_hits.size(); }
		// double getSum() {return sum;}
		// void setSum(double _sum) {sum = _sum;}

		double getLastAngle() {return lastAngle;}
		void setLastAngle(double angle) {lastAngle = angle;}

		void setLastSeg(TrackSegment trackSeg, TrackS &track){
			TrackSegment segment = m_lastSeg;
			vector<PrPixelHit> aux = segment.getTrackSegment();
			m_hits.push_back(aux[0]); //m_hits.push_back(aux[1]);
			m_lastSeg = trackSeg;
		}

		//constructor
		TrackS(){}

		TrackS(vector<PrPixelHit> _track, double _sum){
			m_hits = _track;
			sum = _sum;
		}

		TrackS(vector<PrPixelHit> _track, TrackSegment lastSeg, double _sum){
			m_hits = _track;
			m_lastSeg = lastSeg;
			sum = _sum;
		}

		TrackS(TrackSegment lastSeg){
			m_lastSeg = lastSeg;
		}

	private:
		set<PrPixelHit> cnj_hits;
		TrackSegment m_lastSeg;
		vector<PrPixelHit> m_hits;
		double sum;
		double lastAngle;

};

#endif  /* STRUCTS_H */
