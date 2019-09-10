
OS := $(shell uname)

ifeq ($(OS), Linux)
	GIMP=gimp
else
	GIMP=/Applications/GIMP.app/Contents/MacOS/gimp
endif


../dist/king-death.png ../dist/king-death.atlas : king-death.xcf king-death.list trade.xcf trade-font.list extract-sprites.py pack-sprites
	rm -rf king-death
	./extract-sprites.py king-death.list king-death --gimp='$(GIMP)'
	./extract-sprites.py trade-font.list king-death --gimp='$(GIMP)'
	./pack-sprites ../dist/king-death king-death/*
