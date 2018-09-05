#include "MakeSegmentParal.h"


//Mapping
//Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
/*
Flat 3D -> 1D
z = idx / (xMax * yMax);
idx -= (z * xMax * yMax);
int y = idx / xMax;
int x = idx % xMax;
*/

// Confirmar cubo angulo


//TODO
//Colocar parametros de entrada, nro de threads, dimensao..

// TODO OK
// Conferir mapeamento das threads, proxima localidade.
// particle i
// particle j
// sensor k
// Ter 3 cubos, para cubo, tx, ty.
 



__global__ void Build(double *TX, double *TY ,const double *X, const double *Y, const double *Z,double *SEG, int N  ,double acc_angle){
	unsigned int i = blockIdx.x *blockDim.x + threadIdx.x; //particle i
	unsigned int j = blockIdx.y *blockDim.y + threadIdx.y; //particle j 
	unsigned int k = blockIdx.z *blockDim.z + threadIdx.z; //sensor k 
	int pos = k* (N) + i;

	double x = X[ (k+2)* (N) +j  ] - X[ pos ];
	double y = Y[ (k+2)* (N) +j  ] - Y[ pos ];
	double z = Z[ (k+2)* (N) +j  ] - Z[ pos ];
	double tx = x/z;
	double ty = y/z;

	unsigned int idx = k* (N * N) + i*(N) + j;
	double b_angle = tx*tx + ty*ty;
	if( b_angle <= (acc_angle) * (acc_angle)){	
		SEG[idx] = b_angle;
		TX[idx] = tx;
		TY[idx] = ty;
	}
	else{
		TX[idx]  = -1;
		TY[idx]  = -1;
		SEG[idx] = -1; // -1 ou 0?
	}
}


double HostBuild(double M_x[64][64], double M_y[64][64],double M_z[64][64],double M_seg[64][64][64],double d_angle){

	int D2_bytes = 64*64;
	int D3_bytes = 64*64*64;
	double h_x[D2_bytes],h_y[D2_bytes],h_z[D2_bytes],h_seg[D3_bytes]; 
	double *d_x, *d_y, *d_z, *d_seg,*d_TX,*d_TY;
	int     d_N = 64;



	//leitura dos dados que estao no host
	
	//inicializo d_seg com -1
	memset(h_seg,0,D3_bytes);
		
	for(int i = 0;i<64;i++){
		for(int j = 0;j<64;j++){
			int pos = 64*i+j;
			h_x[pos] = M_x[i][j];
			h_y[pos] = M_y[i][j];
			h_z[pos] = M_z[i][j];
		}
	}
	
	CUDA_SAFE_CALL(	cudaMalloc( (void**) &d_x 		, D2_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_y 		, D2_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_z 		, D2_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_seg 	, D3_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_TX 		, D3_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_TY 		, D3_bytes*sizeof(double)  ) );


	CUDA_SAFE_CALL(	cudaMemcpy(d_x,&h_x,  D2_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_y,&h_y,  D2_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_z,&h_z,  D2_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_seg,&h_seg,D3_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));

	// Crio os eventos para poder contabilizar o tempo gasto para o processamento na GPU
	cudaEvent_t start, stop;
	CUDA_SAFE_CALL(cudaEventCreate(&start));
	CUDA_SAFE_CALL(cudaEventCreate(&stop));

	//max 16.
	int n_threads_dim = 8;
	CUDA_SAFE_CALL(cudaEventRecord(start));
	

	dim3 threadsBloco(n_threads_dim,n_threads_dim,n_threads_dim);
	dim3 blocosGrade( N/threadsBloco.x, N/threadsBloco.y,N/threadsBloco.z ); // quantidade de blocos
	//
	//	Chamada do kernel
	//
	Build<<<blocosGrade,threadsBloco >>>(d_TX,d_TY,d_x,d_y,d_z,d_seg,d_N, d_angle);
	CUDA_SAFE_CALL ( cudaGetLastError () ) ;
	//
	CUDA_SAFE_CALL ( cudaEventRecord(stop)    );
	CUDA_SAFE_CALL ( cudaEventSynchronize ( stop ) );

	float delta_eventos = 0;


	CUDA_SAFE_CALL(cudaEventElapsedTime(&delta_eventos,start,stop));
//	printf("Tempo em seg: %lf \n",delta_eventos/1000 );
	CUDA_SAFE_CALL(	cudaMemcpy(&h_seg,d_seg,D3_bytes*sizeof(double),cudaMemcpyDeviceToHost ));
	
	CUDA_SAFE_CALL(	cudaMemcpy(&h_seg,d_seg,D3_bytes*sizeof(double)	,cudaMemcpyDeviceToHost ));

	for(int i = 0;i<64;i++){
		for(int j = 0;j<64;j++){
			for(int k = 0;k<64;k++){
				int pos = k*64*64 + j*64 +i;
				M_seg[i][j][k] = h_seg[pos];
			}
		}
	}



	CUDA_SAFE_CALL ( cudaFree ( d_x ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_y ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_z ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_seg ) ) ;
	return delta_eventos/1000;
}

/*
int main(){
	
	int D2_bytes = N*N;
	int D3_bytes = N*N*N;
	double h_x[D2_bytes],h_y[D2_bytes],h_z[D2_bytes],h_seg[D3_bytes], h_angle = 2; 
	double *d_x, *d_y, *d_z, *d_seg, d_angle;
	int    *d_N, h_N;



	//leitura dos dados que estao no host
	
	//inicializo d_seg com -1
	memset(h_seg,0,D3_bytes);
	
	scanf("%d",&h_N);
	
	for(int i = 0;i<N;i++){
		for(int j = 0;j<N;j++){
			int pos = N*i+j;
			scanf("%lf",&h_x[pos]);
		}
	}
	for(int i = 0;i<N;i++){
		for(int j = 0;j<N;j++){
			int pos = N*i+j;
			scanf("%lf",&h_y[pos]);
		}
	}
	for(int i = 0;i<N;i++){
		for(int j = 0;j<N;j++){
			int pos = N*i+j;
			scanf("%lf",&h_z[pos]);
		}
	}

	CUDA_SAFE_CALL(	cudaMalloc( (void**) &d_N, sizeof(int)  ) );
	CUDA_SAFE_CALL(	cudaMalloc( (void**) &d_x, D2_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_y, D2_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_z, D2_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_seg, D3_bytes*sizeof(double)  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_angle, sizeof(double)  ) );

	CUDA_SAFE_CALL(	cudaMemcpy(d_N,&h_N  , sizeof(int)	,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_x,&h_y,   D2_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_y,&h_y,D2_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_z,&h_z,D2_bytes*sizeof(double)		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_seg,&h_seg,D3_bytes*sizeof(double)	,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_angle,&h_angle, sizeof( double ),cudaMemcpyHostToDevice ));

	// Crio os eventos para poder contabilizar o tempo gasto para o processamento na GPU
	cudaEvent_t start, stop;
	CUDA_SAFE_CALL(cudaEventCreate(&start));
	CUDA_SAFE_CALL(cudaEventCreate(&stop));


	int n_threads = 1024;
	int n_blocos = (N+n_threads-1)/n_threads;


	CUDA_SAFE_CALL(cudaEventRecord(start));
	

	dim3 threadsBloco(TAM_BLOCO,TAM_BLOCO,TAM_BLOCO); // 4 threads nesse bloco
	dim3 blocosGrade(N/threadsBloco.x, N/threadsBloco.y,N/threadsBloco.z ); // quantidade de blocos
	//
		Chamada do kernel
	//
	Build<<<blocosGrade,threadsBloco >>>(d_x,d_y,d_z,d_seg,d_N, d_angle);
	CUDA_SAFE_CALL ( cudaGetLastError () ) ;
	//
	CUDA_SAFE_CALL(cudaEventRecord(stop));
	CUDA_SAFE_CALL ( cudaEventSynchronize ( stop ) );

	float delta_eventos = 0;


	CUDA_SAFE_CALL(cudaEventElapsedTime(&delta_eventos,start,stop));
	printf("Tempo: %lf \n",delta_eventos );
	CUDA_SAFE_CALL(	cudaMemcpy(&h_seg,d_seg,D3_bytes*sizeof(double),cudaMemcpyDeviceToHost ));
	
	for(int i =0 ;i<D3_bytes;i++)
		printf("%lf\n",h_seg[i]);
	
	CUDA_SAFE_CALL ( cudaFree ( d_x ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_y ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_z ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_seg ) ) ;




	return 0;
}
*/