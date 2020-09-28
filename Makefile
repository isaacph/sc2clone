# may have to: export DISPLAY=localhost:0
#
# required packages:
#	libglew-dev
#	libglfw3-dev
#	libfreetype6-dev
#
# to get -lfreetype to work:
# 	cd /usr/include/
#	ln -s freetype2/freetype/ freetype
#	sudo ln -s freetype2/ft2build.h ft2build.h

game:
	g++ -c -o obj/main.o src/main.cpp
	g++ -c -o obj/resources/import.o src/resources/import.cpp
	g++ obj/main.o obj/resources/import.o -lGLEW -lglfw3 -lGL -lpthread -lX11 -ldl -o ./bin/sc2

resources:
	g++ ./src/resources/export.cpp -ldl -lfreetype -lpng -o ./bin/export

all: game resources