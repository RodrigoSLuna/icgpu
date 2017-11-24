/* structs to be used in the code
 *
 *  @author Leticia Freire
 */

#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <iostream>
#include "PrPixelHit.h"

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
			for(int i = 0; i < trackSegment.size(); i++)
				cout << "x: " << trackSegment[i].x() << " y: " << trackSegment[i].y() << " z: " << trackSegment[i].z() << endl;
		} */

		float getTx(){ return tx;}
		void setTx(float _tx){tx = _tx;}
		float getTy(){return ty;}
		void setTy(float _ty){ty = _ty;}

		TrackSegment(){}
		TrackSegment(vector<PrPixelHit> _trackSegment, int _status, float _tx, float _ty){
			trackSegment = _trackSegment;
			status = _status;
			tx = _tx;
			ty = _ty;
		}


	private:
		vector<PrPixelHit> trackSegment;
		int status;
		float tx;
		float ty;
};

/*class to store the tracks*/
/*a track is made with 2 or more segments*/
class TrackS{
	public:
		TrackSegment getLastSeg() {return m_lastSeg;}
		void setLastSeg(TrackSegment trackSeg) {m_lastSeg = trackSeg;}
		void addHits(PrPixelHit hit) {m_hits.push_back(hit);}
		vector<PrPixelHit> getHits() {return m_hits;}
		// float getSum() {return sum;}
		// void setSum(float _sum) {sum = _sum;}

		float getLastAngle() {return lastAngle;}
		void setLastAngle(float angle) {lastAngle = angle;}

		void setLastSeg(TrackSegment trackSeg, TrackS &track){
			TrackSegment segment = m_lastSeg;
			vector<PrPixelHit> aux = segment.getTrackSegment();
			m_hits.push_back(aux[0]); //m_hits.push_back(aux[1]);
			m_lastSeg = trackSeg;
		}

		//constructor
		TrackS(){}

		TrackS(vector<PrPixelHit> _track, float _sum){
			m_hits = _track;
			sum = _sum;
		}

		TrackS(vector<PrPixelHit> _track, TrackSegment lastSeg, float _sum){
			m_hits = _track;
			m_lastSeg = lastSeg;
			sum = _sum;
		}

		TrackS(TrackSegment lastSeg){
			m_lastSeg = lastSeg;
		}

	private:
		TrackSegment m_lastSeg;
		vector<PrPixelHit> m_hits;
		float sum;
		float lastAngle;

};

#endif  /* STRUCTS_H */
