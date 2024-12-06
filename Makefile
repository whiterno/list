all:
	g++ src/main.cpp src/list.cpp
	./a.out

target:
	mkdir log
	touch log/dump.dot
	touch log/dump.html

clean:
	rm log/*
	rm a.out
