CC = gcc
CPP= g++
PPFLAGS2 = -g -O3
FLAGS= -D__USE_BSD -g
ExtraLFLAGS = -lboost_iostreams
SRCS_cpp = process_fastqFile.cpp process_illumina_to_fastq.cpp \
		process_sff_to_fastq.cpp generic.cpp rapifilt.cpp 
SRCS_c =sff.c
OBJS1 = $(SRCS_cpp:.cpp=.o) 
OBJS2 = $(SRCS_c:.c=.o)

MAIN = rapifilt

.PHONY: depend clean

all:    $(MAIN)
	@echo  RAPIFILT has been compiled
$(MAIN): $(OBJS1) $(OBJS2) 
	$(CPP) $(PPFLAGS2) $(PPFLAGS) $(LFLAGS) $(ExtraLFLAGS) -o $(MAIN) $(OBJS1) $(OBJS2) 

.c.o:
	$(CC) $(FLAGS) $(INCLUDES)  -c $<  -o $@

.cpp.o:
	$(CPP) $(PPFLAGS) $(PPFLAGS2) $(LFLAGS) $(INCLUDES)  -c $<  -o $@


depend: $(SRCS_cpp) $(SRCS_c)
	makedepend $(INCLUDES) $^
	
clean:
	$(RM) *.o *~ $(MAIN)

