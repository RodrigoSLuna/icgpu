all: saida
	./saida
saida: dados.o saida.o tracking.o
	g++ -o saida tracking.o dados.o saida.o  -I/usr/include/jsoncpp -ljsoncpp
saida.o: code.cpp dados.h tracking.h
	g++ -o saida.o -c code.cpp  -I/usr/include/jsoncpp -ljsoncpp
tracking.o: tracking.cpp
	g++ -o tracking.o -c tracking.cpp -I/usr/include/jsoncpp -ljsoncpp
dados.o: dados.cpp
	g++ -o dados.o -c dados.cpp -I/usr/include/jsoncpp -ljsoncpp
	#g++ -I  /usr/include/jsoncpp/ dados.cpp -ljsoncpp




#saida: teste.o saida.o
        # O compilador faz a ligação entre os dois objetos
#	nvcc -o saida teste.o saida.o
#-----> Distancia com o botão TAB ### e não com espaços
#teste.o: teste.c
#	g++ -o teste.o -c teste.cpp
#saida.o: teste.cu teste.h
#	nvcc -o saida.o -c teste.cu
