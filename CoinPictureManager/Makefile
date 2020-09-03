CC := g++
CFLAGS := -g -Wall -pedantic

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

LIBS := -lopencv_core -lopencv_photo -lopencv_highgui -lopencv_calib3d -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_gapi -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lstdc++fs -lX11
INCLD := -I/usr/local/include/opencv4

PROG := coinpicturemanager

all: $(PROG)

coinpicturemanager: $(OBJS)
	${CC} $(CFLAGS) -o $@ $^ $(LIBS) $(INCLD)

%.o: %.cpp
	${CC} $(CFLAGS) -c $< $(INCLD)

clean:
	rm -rf $(OBJS)

install:
	mv $(PROG) /usr/local/bin/$(PROG)

remove:
	rm -rf /usr/local/bin/$(PROG)
