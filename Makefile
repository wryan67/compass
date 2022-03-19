
compass:  main.cpp
	g++ -llog4pi -lpthread -lwiringPi main.cpp -o compass


clean:
	rm compass
