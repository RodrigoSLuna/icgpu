#include <cstdio>

/*
Tutorial.
https://devblogs.nvidia.com/even-easier-introduction-cuda/
*/

int N = 64;
const int TAM_BLOCO = 3;
#define CUDA_SAFE_CALL(call) { \
cudaError_t err = call ; \
if( err != cudaSuccess ) { \
fprintf ( stderr ," Erro no arquivo ’%s ’, linha %i: %s.\n", \
__FILE__ , __LINE__ , cudaGetErrorString ( err ) ) ; \
exit ( EXIT_FAILURE ) ;\
}\
}
