/*
	This is a code that process Kalman Filtering of position date from GPS.
	
*/

#include "KalmanFilter.h"

/*Kalman Filter class Structor*/
KalmanFi1ter::KalmanFi1ter()     

{
  	i = 1;
	cosLat = 0.0;  
	Rearth = 6378137;
	
}
/*Kalman Filter class Destroier*/
KalmanFi1ter::~KalmanFi1ter()
{

}
//
// public methods
//
/*This is a function to filter the GPS data from GPS
	Parameter: lat-> Position data1
			   lon-> Position data2
	filtered data->GPS_data:
*/
int64_t* KalmanFi1ter::KalmanProcessing(int64_t lat, int64_t lon){
	 time = millis();
         prevGPS_data[0][0] = GPS_data[0][0];
         prevGPS_data[0][1] = GPS_data[0][1];
         prevGPS_data[0][2] = GPS_data[0][2];
         GPS_data[0][0] = lat;
         GPS_data[0][1] = lon;
         GPS_data[0][2] = (uint64_t) time;       

          if (i == 1){ //Initialize cosLat            
            firstGPS_data[0][0] = GPS_data[0][0];
            firstGPS_data[0][1] = GPS_data[0][1];
            firstGPS_data[0][2] = GPS_data[0][2]; 
            cosLat = cos((float) firstGPS_data[0][0]/10000000*pi/180);     
          }              
          delta_T = ((float) (GPS_data[0][2] - prevGPS_data[0][2])) / 1000; //Time elapsed in seconds
          float Amatrix[4][4] = {
                 {1, 0,  delta_T, 0      },
                 {0, 1,  0,       delta_T},
                 {0, 0,  1,       0      },
                 {0, 0,  0,       1      },
          };          
          float nextXstateEstimate[4][1] = {
                {0},
                {0},
                {0},
                {0},
          };    
          Matrix.Multiply((float*) Amatrix, (float*) Xstate, 4, 4, 1, (float*) nextXstateEstimate);           
          /*************Line 78 of Matlab code*************/
          float PerrorCovarianceEstimate[4][4] = {
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
          };
          float AmatrixTranspose[4][4] = {
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
          };
          float IntermediateProductMatrix[4][4] = {
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
          };
          float IntermediateSumMatrix[4][4] = {
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
          };          
          Matrix.Transpose((float*) Amatrix, 4 ,4, (float *) AmatrixTranspose);
          Matrix.Multiply((float*) Amatrix, (float*) PerrorCovariance, 4,4,4, (float*) IntermediateProductMatrix);
          Matrix.Multiply((float*) IntermediateProductMatrix, (float*) AmatrixTranspose, 4,4,4, (float*) IntermediateSumMatrix);
          Matrix.Add((float*) IntermediateSumMatrix, (float*) QcovarianceMatrix, 4, 4, (float*) PerrorCovarianceEstimate);
          /*************Line 81 of Matlab code*************/
          float KalmanGain[4][2] = {
                {0.0, 0.0},
                {0.0, 0.0},
                {0.0, 0.0},
                {0.0, 0.0},
          };    
          float IntermediateProductMatrix2[2][4] = {
                {0.0, 0.0, 0.0, 0.0},
                {0.0, 0.0, 0.0, 0.0},
          };
          float IntermediateProductMatrix3[2][2] = {
                {0.0, 0.0},
                {0.0, 0.0}, 
          };
          float IntermediateQuotientMatrix[4][2] = {
                {0.0, 0.0},
                {0.0, 0.0},
                {0.0, 0.0},
                {0.0, 0.0},
          };
          float IntermediateSumMatrix2[2][2] = {
                {0.0, 0.0}, 
                {0.0, 0.0},
          };
          Matrix.Multiply((float*) Hmatrix, (float*) PerrorCovarianceEstimate, 2,4,4, (float*) IntermediateProductMatrix2);
          Matrix.Multiply((float*) IntermediateProductMatrix2, (float*) HmatrixTranspose, 2,4,2, (float*) IntermediateProductMatrix3);
          Matrix.Add((float*) IntermediateProductMatrix3, (float*) RcovarianceMatrix, 2, 2, (float*) IntermediateSumMatrix2);
          int flag = Matrix.Invert((float*) IntermediateSumMatrix2, 2); //The argument matrix gets inverted; no need to create temp matrix.
          if (flag == 0){
            Serial.println("CANT INVERT");
          }
          Matrix.Multiply((float*) PerrorCovarianceEstimate, (float*) HmatrixTranspose, 4,4,2, (float*) IntermediateQuotientMatrix);
          Matrix.Multiply((float*) IntermediateQuotientMatrix, (float*) IntermediateSumMatrix2, 4,2,2, (float*) KalmanGain);
          data[0] = (float) (GPS_data[0][0]-firstGPS_data[0][0])*pi/180*Rearth/10000000;
          data[1] = (float) (GPS_data[0][1]-firstGPS_data[0][1])*pi/180*Rearth*cosLat/10000000;     
          float ZkTranspose[2][1] = { //We only need the transposed version of this, so we do it right here.
                {data[0]},
                {data[1]},
                }; 
          float IntermediateProductMatrix4[2][1] = {
                {0.0}, 
                {0.0},
          };
          float IntermediateProductMatrix5[4][1] = {
                {0.0}, 
                {0.0},
                {0.0}, 
                {0.0},
          };
          float IntermediateSubtractionMatrix[2][1] = {
                {0.0}, 
                {0.0},
          };
          Matrix.Multiply((float*) Hmatrix, (float*) nextXstateEstimate, 2,4,1, (float*) IntermediateProductMatrix4);
          Matrix.Subtract((float*) ZkTranspose, (float*) IntermediateProductMatrix4, 2, 1, (float*) IntermediateSubtractionMatrix);
          Matrix.Multiply((float*) KalmanGain, (float*) IntermediateSubtractionMatrix, 4,2,1, (float*) IntermediateProductMatrix5); //Reuse intermediate matrix because it has appropriate dimensions
          Matrix.Add((float*) nextXstateEstimate, (float*) IntermediateProductMatrix5, 4,1, (float*) Xstate); //NO NEED TO TRANSPOSE X STATE. WE DID THAT IN MATLAB FOR CONVENIENCE
          /*************Line 84 of Matlab code*************/
          Matrix.Multiply((float*) KalmanGain, (float*) Hmatrix, 4,2,4, (float*) IntermediateProductMatrix); //Reuse this intermediate matrix because it's 4x4 and we need 4x4
          float IdentityMatrix[4][4] = {
                {1,0,0,0},
                {0,1,0,0},
                {0,0,1,0},
                {0,0,0,1},
          };
          Matrix.Subtract((float*) IdentityMatrix, (float*) IntermediateProductMatrix, 4,4, (float*) IntermediateQuotientMatrix); //Reuse this intermediate matrix because it's 4x4 and we need 4x4.
          Matrix.Multiply((float*) IntermediateQuotientMatrix, (float*) PerrorCovarianceEstimate, 4,4,4, (float*) PerrorCovariance);              

          i = i + 1; 
          return GPS_data[0];
        
}

