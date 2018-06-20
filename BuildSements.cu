#include <cstdio>


/*
Tutorial.
https://devblogs.nvidia.com/even-easier-introduction-cuda/
*/

int N = 4;
const int TAM_BLOCO = 2;
#define CUDA_SAFE_CALL(call) { \
cudaError_t err = call ; \
if( err != cudaSuccess ) { \
fprintf ( stderr ," Erro no arquivo ’%s ’, linha %i: %s.\n", \
__FILE__ , __LINE__ , cudaGetErrorString ( err ) ) ; \
exit ( EXIT_FAILURE ) ;\
}\
}



//TODO
//estou usando muitas instruções 
__global__ void Build(const double *X, const double *Y, const double *Z,double *SEG, int N  ,double acc_angle){
	unsigned int i = blockIdx.x *blockDim.x + threadIdx.x; //id sensor
	unsigned int j = blockIdx.y *blockDim.y + threadIdx.y; //positions particle  
	unsigned int k = blockIdx.z *blockDim.z + threadIdx.z;

	int pos = i*N + j;
	if(i>N or j>N or k>N)
		return;

	double x = X[ (2*N) + pos ] - X[ pos ];
	double y = Y[ (2*N) + pos ] - Y[ pos ];
	double z = Z[ (2*N) + pos ] - Z[ pos ];
	double tx = x/z;
	double ty = y/z;

	unsigned int idx = k*N*N + j*N + i; 	 // levar pra funcao N^2 e N.

	if(tx*tx + ty*ty <= acc_angle*acc_angle) //levar pra funcao, acc_angle ao quadrado!
		SEG[idx] = 1;

}


int main(){
	double *h_x, *h_y,*h_z, *h_seg, *h_angle;
	double *d_x, *d_y, *d_z, *d_seg, *d_angle;
	int    *d_N,*h_N;

	int D2_bytes = N*N;
	int D3_bytes = N*N*N*sizeof(double);


	h_N = (int*) malloc(sizeof(int));
	h_x = (double*)malloc(D2_bytes);
	h_x = (double*)malloc(D2_bytes);
	h_y = (double*)malloc(D2_bytes);
	h_z = (double*)malloc(D3_bytes);
	h_angle = (double*)malloc( sizeof(double) );
	h_seg = (double*)malloc(D3_bytes);

	if(h_x == NULL || h_y == NULL || h_z == NULL || h_seg) exit(EXIT_FAILURE);

	//leitura dos dados que estao no host
	//inicializo d_seg com -1
	memset(h_seg,0,D3_bytes);
	
	scanf("%d",&h_N);
	for(int i = 0;i<N;i++){
		for(int j = 0;j<N;j++){
			int pos = N*i+j;
			scanf("%lf %lf %lf",&h_x[pos],&h_y[pos],&h_z[pos]);
		}
	}

	CUDA_SAFE_CALL(	cudaMalloc( (void**) &d_N, sizeof(int)  ) );
	CUDA_SAFE_CALL(	cudaMalloc( (void**) &d_x, D2_bytes  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_y, D2_bytes  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_z, D2_bytes  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_seg, D3_bytes  ) );
	CUDA_SAFE_CALL( cudaMalloc( (void**) &d_angle, sizeof(double)  ) );

	CUDA_SAFE_CALL(	cudaMemcpy(d_N,h_N  , sizeof(int)	,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_x,h_y,D2_bytes		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_y,h_y,D2_bytes		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_z,h_z,D2_bytes		,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_seg,h_seg,D3_bytes	,cudaMemcpyHostToDevice ));
	CUDA_SAFE_CALL(	cudaMemcpy(d_angle,h_angle, sizeof( double ),cudaMemcpyHostToDevice ));


	// Crio os eventos para poder contabilizar o tempo gasto para o processamento na GPU
	cudaEvent_t start, stop;
	CUDA_SAFE_CALL(cudaEventCreate(&start));
	CUDA_SAFE_CALL(cudaEventCreate(&stop));

	/*
	int n_threads = 1024;
	int n_blocos = (N+n_threads-1)/n_threads;
	*/

	CUDA_SAFE_CALL(cudaEventRecord(start));
	

	dim3 threadsBloco(TAM_BLOCO,TAM_BLOCO); // 4 threads nesse bloco
	dim3 blocosGrade(N/threadsBloco.x, N/threadsBloco.y ); // quantidade de blocos
	/*
		Chamada do kernel
	*/
	Build<<<blocosGrade,threadsBloco >>>(d_x,d_y,d_z,d_seg,d_N[0], d_angle);
	CUDA_SAFE_CALL ( cudaGetLastError () ) ;
	CUDA_SAFE_CALL(cudaEventRecord(start));

	float delta_eventos = 0;
	CUDA_SAFE_CALL(cudaEventElapsedTime(&delta_eventos,start,stop));

	CUDA_SAFE_CALL(	cudaMemcpy(d_seg,h_seg,D3_bytes,cudaMemcpyDeviceToHost ));

	CUDA_SAFE_CALL ( cudaFree ( d_x ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_y ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_z ) ) ;
	CUDA_SAFE_CALL ( cudaFree ( d_seg ) ) ;




	return 0;
}