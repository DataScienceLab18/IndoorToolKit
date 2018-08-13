all: main
CFLAG = -g -Wall 
CC = g++
.cc.o:
	$(CC) $(CFLAG) -c  $<
OBJ_T = graph.o node.o antenna.o conf.o  Particle.o ParticleFilter.o aggregateRaw.o converter.o rawDataSimulator.o object.o room.o rawDataProc.o queryProc.o huaRtree.o gtQueryProc.o queryWindow.o ShortestPath.o edge.o kNN_s.o kNN_truth.o kNN_Hua.o kNN_qp_reader.o
main: $(OBJ_T)
	$(CC) $(CFLAG) -o main $(OBJ_T) 




clean:
	rm -rf *o main
