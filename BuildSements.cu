#include <cstdio>


/*
Tutorial.
https://devblogs.nvidia.com/even-easier-introduction-cuda/
*/

int N = 3;
const int TAM_BLOCO = 3;
#define CUDA_SAFE_CALL(call) { \
cudaError_t err = call ; \
if( err != cudaSuccess ) { \
fprintf ( stderr ," Erro no arquivo ’%s ’, linha %i: %s.\n", \
__FILE__ , __LINE__ , cudaGetErrorString ( err ) ) ; \
exit ( EXIT_FAILURE ) ;\
}\
}

//Mapping
//Flat[x + WIDTH * (y + DEPTH * z)] = Original[x, y, z]
/*
Flat 3D -> 1D
z = idx / (xMax * yMax);
idx -= (z * xMax * yMax);
int y = idx / xMax;
int x = idx % xMax;
*/

__global__ void Build(const double *X, const double *Y, const double *Z,double *SEG, int *N  ,double *acc_angle,int teste){
	printf("->%d teste:%d\n",*N,teste);
	/*
	unsigned int i = blockIdx.x *blockDim.x + threadIdx.x; //id sensor
	unsigned int j = blockIdx.y *blockDim.y + threadIdx.y; //position particle j 
	unsigned int k = blockIdx.z *blockDim.z + threadIdx.z; //position particle k 
	int pos = i* (*N) + j;

	if( i+2 >= *N or j> (*N) or k> (*N))
		return;
	double x = X[ (i+2)* (*N) +k  ] - X[ pos ];
	double y = Y[ (i+2)* (*N) +k  ] - Y[ pos ];
	double z = Z[ (i+2)* (*N) +k  ] - Z[ pos ];
	double tx = x/z;
	double ty = y/z;

	unsigned int idx = k* (*N)* (*N) + j* (*N) + i;
	if(tx*tx + ty*ty <= *acc_angle * (*acc_angle)){	
		SEG[idx] = 1;
	}
	*/
}


int main(){
	
	int D2_bytes = N*N;
	int D3_bytes = N*N*N;
	double h_x[D2_bytes],h_y[D2_bytes],h_z[D2_bytes],h_seg[D3_bytes], h_angle = 2; 
	double *d_x, *d_y, *d_z, *d_seg, *d_angle;
	int    *d_N,h_N;



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
	/*
		Chamada do kernel
	*/
	Build<<<blocosGrade,threadsBloco >>>(d_x,d_y,d_z,d_seg,d_N, d_angle,h_N);
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
