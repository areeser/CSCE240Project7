GPP = g++ -O3 -Wall -std=c++11

UTILS = ../../Utilities

A = main.o
Q = queue.o
O = onevoter.o
R = myrandom.o
S = scanner.o
SL = scanline.o
U = utils.o

Aprog: $A $Q $O $R $S $(SL) $U
	$(GPP) -o Aprog $A $Q $R $O $S $(SL) $U

main.o: main.h main.cc
	$(GPP) -c main.cc

queue.o: queue.h queue.cc
	$(GPP) -c queue.cc

onevoter.o: onevoter.h onevoter.cc
	$(GPP) -c onevoter.cc

myrandom.o: myrandom.h myrandom.cc
	$(GPP) -c myrandom.cc

scanner.o: $(UTILS)/scanner.h $(UTILS)/scanner.cc
	$(GPP) -c $(UTILS)/scanner.cc

scanline.o: $(UTILS)/scanline.h $(UTILS)/scanline.cc
	$(GPP) -c $(UTILS)/scanline.cc

utils.o: $(UTILS)/utils.h $(UTILS)/utils.cc
	$(GPP) -c $(UTILS)/utils.cc
