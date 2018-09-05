#all: saida
#	./saida
saida: dados.o saida.o makesegmentparal.o tracking.o
	nvcc -O3 -std=c++11 -o saida makesegmentparal.o tracking.o dados.o saida.o  -I/usr/include/jsoncpp -ljsoncpp -lpthread
saida.o: code.cpp dados.h tracking.h
	nvcc -O3 -std=c++11 -o saida.o -c code.cpp  -I/usr/include/jsoncpp -ljsoncpp -lpthread
tracking.o: tracking.cpp
	nvcc -O3 -std=c++11 -o tracking.o -c tracking.cpp -I/usr/include/jsoncpp -ljsoncpp -lpthread
dados.o: dados.cpp
	nvcc -O3 -std=c++11 -o dados.o -c dados.cpp -I/usr/include/jsoncpp -ljsoncpp  
	#g++ -I  /usr/include/jsoncpp/ dados.cpp -ljsoncpp
makesegmentparal.o: makesegmentparal.cu
	nvcc -std=c++11 -o makesegmentparal.o -c makesegmentparal.cu

run:
	./saida

clean:
	rm -f *.o



#saida: teste.o saida.o
        # O compilador faz a ligação entre os dois objetos
#	nvcc -o saida teste.o saida.o
#-----> Distancia com o botão TAB ### e não com espaços
#teste.o: teste.c
#	g++ -o teste.o -c teste.cpp
#saida.o: teste.cu teste.h
#	nvcc -o saida.o -c teste.cu
