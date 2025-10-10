#!/bin/sh

F1 		  = Quicksort
DVIFLAG = -D600 

$(F1):
	@if [ ! -f $(F1).tex ]; then \
		exit; \
	else \
	  latex $(F1).tex; \
	fi;
	@if [ -f $(F1).dvi ]; then rm $(F1).aux $(F1).log; fi;


clean:
	@if [ -f $(F1).dvi ]; then rm $(F1).dvi; fi;
	@if [ -f $(F1).ps ]; then rm $(F1).ps; fi;
	
ps:
	@if [ -f $(F1).dvi ]; then	\
		dvips $(DVIFLAG) $(F1).dvi;	\
	else  \
		echo ""; \
		echo "Type 'make all' or type 'make' and "; \
		echo "then 'make ps' for create a ps-file!"; \
		echo ""; \
	fi;

all:
	$(MAKE) 
	$(MAKE) ps
