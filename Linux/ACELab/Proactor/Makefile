main : main.o HA_Proactive_Service.o
	cc -o main main.o HA_Proactive_Service.o
main.o : main.cpp HA_Proactive_Service.h HA_Proactive_Acceptor.h
	cc -c main.cpp
HA_Proactive_Service.o : HA_Proactive_Service.cpp
	cc -c HA_Proactive_Service.cpp
clean :
	rm main *.o 