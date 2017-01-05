#------------------------------------------------
#Copyright (C) 2016-2017 by Jorge C. Valverde-Rebaza
#Email: jvalverr@icmc.usp.br
#This file is part of Geo-LPsource

#Geo-LPsource is free software based on LPsource and LPmade softwares: you can redistribute it and/or modify it under 
#the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, 
#or (at your option) any later version. Geo-LPsource is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
#without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
#for more details. You should have received a copy of the GNU General Public License along with Geo-LPsource. If not, see <http://www.gnu.org/licenses/>.

CONFIG := BIN
ifeq ($(CONFIG),DEBUG) 
	CFLAGS := -Wall -g
	BINDIR := ./debug
endif
ifeq ($(CONFIG),BIN)
	CFLAGS := -Wall -O3 -D NDEBUG
	BINDIR := ./bin
endif
CC := g++
NETWORK_BINS := $(patsubst %.cpp,%,$(notdir $(wildcard ./mains/*.cpp)))

all: network predictors	
	cp ./Predictors/bin/predict $(BINDIR);

network: $(addprefix $(BINDIR)/,$(NETWORK_BINS))
predictors: ./Predictors/bin/predict
test: $(patsubst %,./test/%.TEST,$(NETWORK_BINS) $(PREDICTOR_BINS))

$(BINDIR)/%: ./mains/%.o Network.o Structures.o Evaluation.o
	$(CC) $(CFLAGS) $^ -o $@;

./Predictors/bin/predict: ./Predictors/mains/predict.o $(patsubst %.cpp,%.o,$(wildcard ./Predictors/*.cpp)) Network.o Evaluation.o
	$(CC) $(CFLAGS) $^ -o $@;

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@;


.PHONY: clean
	
clean:
	-rm -f *.o mains/*.o Predictors/mains/*.o $(BINDIR)/* Predictors/bin/* Predictors/*.o 
