
default: MANUAL.ps MANUAL.pdf

MANUAL.ps: MANUAL.dvi
	dvips -q MANUAL.dvi

MANUAL.pdf: MANUAL.dvi
	dvipdf MANUAL.dvi

dvi:
	../cmd/Mkdvi

MANUAL.dvi: MANUAL.tex
	../cmd/Mkman 
	cp ../tex/html.sty .
	cp ../tex/MANUAL.mac .
	cp ../tex/MANUAL.pagesize .
	latex MANUAL.tex
	makeindex -s MANUAL.ist MANUAL.idx 
	latex MANUAL.tex


pdf: MANUAL.tex
	../cmd/Mkman 
	cp ../tex/html.sty .
	cp ../tex/MANUAL.mac .
	cp ../tex/MANUAL.pagesize .
	pdflatex MANUAL.tex
	makeindex -s MANUAL.ist MANUAL.idx 
	pdflatex MANUAL.tex

all: MANUAL.ps dvi

clean:
	rm -f extract/* *.aux *.dvi *.idx *.ilg *.ind *.log *.toc \
	MANUAL.mac MANUAL.pagesize html.sty 

del: clean
	rm -f MANUAL.ps

SHELL=/bin/sh

MAN:
	cp ../tex/MANUAL.mac .
	cp ../tex/MANUAL.pagesize .
	latex MANUAL.tex
	makeindex -s MANUAL.ist MANUAL.idx 
	latex MANUAL.tex
	dvips MANUAL.dvi -o MANUAL.ps
html: del
	cp ../tex/MANUAL.pagesize .
	cp ../tex/MANUAL.mac .
	cp ../tex/html.sty .
	Mkhtml
	./finish_script
