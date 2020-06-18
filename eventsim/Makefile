all: traversal
traversal: derived_events.cpp derived_events.h event.h eventsim.cpp heap.h input_reader.cpp input_reader.h streetgrid.cpp streetgrid.h vehicle.h
	g++ -g -Wall derived_events.cpp eventsim.cpp input_reader.cpp streetgrid.cpp -o eventsim

clean:
	rm -f *.o eventsim
