all:
	touch log/placeholder
	rm log/*
	g++ src/main.cpp src/list.cpp
	./a.out
