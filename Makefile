
all: main.o trace.o unp.o sig_alarm.o traceutil.o
	g++ $^ -o traceroute

%: %.cc unp.o
	g++ $^ -o $@

unp.o: unp.cc
	g++ -c $<

clean: 
	rm -rf *.o a.out

