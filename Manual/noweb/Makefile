
CMDDIR = $(LEDAROOT)/Manual/cmd
TEXDIR = $(LEDAROOT)/Manual/tex

all: manual html 

#------------------------------------------------------------------------------
# Typesetting ext.nw:
#------------------------------------------------------------------------------

Doctools.tex: ext.nw
	gawk '/<<.*_.*>>/{gsub(/_/,"\\_")}{print}' ext.nw > Doctools.nw
	noweave -delay Doctools.nw > Doctools.tex

Doctools.dvi: Doctools.tex
	latex Doctools.tex


ext.dvi : ext.nw
	gawk '/<<.*_.*>>/{gsub(/_/,"\\_")}{print}' ext.nw > ext1.nw
	noweave -delay ext1.nw > ext.tex
	latex ext.tex
	latex ext.tex

ext.ps : ext.dvi
	dvips ext.dvi

clean: 
	rm -f ext1.nw ext.tex ext.toc ext.aux ext.log ext.dvi ext.ps *~
	rm -f ../cmd/Mkhtml ../cmd/HTMLman ../cmd/extract_mtype.pl \
	../cmd/handle_refs* ../cmd/*~

#------------------------------------------------------------------------------
# Perlextractions:
#------------------------------------------------------------------------------

DEBUG = -L

extout.pm exthelp.pm extbasic.pm extHTML.pm extFman.pm extLdoc.pm extCdoc.pm : ext.nw
	notangle -R$@ $(DEBUG) ext.nw > $(CMDDIR)/$@ 

ext.pl: ext.nw extout.pm exthelp.pm extbasic.pm  
	notangle -R$@ $(DEBUG) ext.nw > $(CMDDIR)/$@
	chmod +x $(CMDDIR)/ext.pl

ldel.pl: ext.nw
	notangle -Rldel.pl $(DEBUG) ext.nw > $(CMDDIR)/ldel.pl
	chmod +x $(CMDDIR)/ldel.pl


lw2nw.pl: ext.nw  extout.pm exthelp.pm extbasic.pm  
	notangle -Rlw2nw.pl $(DEBUG) ext.nw > $(CMDDIR)/lw2nw.pl

Lman: ext.pl extLdoc.pm
	notangle -RLman ext.nw > $(CMDDIR)/Lman
	chmod +x $(CMDDIR)/Lman


lw2nw: lw2nw.pl extLdoc.pm
	notangle -Rlw2nw ext.nw > $(CMDDIR)/lw2nw
	chmod +x $(CMDDIR)/lw2nw

emptyline: ext.nw
	notangle $(DEBUG) -Remptyline.pl -filter emptydefn ext.nw > $(CMDDIR)/emptyline.pl
	notangle -Remptyline ext.nw > $(CMDDIR)/emptyline
	chmod +x $(CMDDIR)/emptyline

lw2dvi: ext.pl ext.nw
	notangle -Rlw2dvi ext.nw > $(CMDDIR)/lw2dvi
	chmod +x $(CMDDIR)/lw2dvi

lweave: ext.nw
	notangle -Rlweave ext.nw > $(CMDDIR)/lweave
	chmod +x $(CMDDIR)/lweave

Ldoc: ext.pl extLdoc.pm
	notangle -RLdoc ext.nw > $(CMDDIR)/Ldoc
	chmod +x $(CMDDIR)/Ldoc

Cdoc: ext.pl extCdoc.pm
	notangle -RCdoc ext.nw > $(CMDDIR)/Cdoc
	chmod +x $(CMDDIR)/Cdoc

Fman: ext.pl extFman.pm
	notangle -RFman ext.nw > $(CMDDIR)/Fman
	chmod +x $(CMDDIR)/Fman

Mkman: ext.pl extLdoc.pm
	notangle -RMkman ext.nw > $(CMDDIR)/Mkman
	chmod +x $(CMDDIR)/Mkman

Mkdvi: ext.nw 
	notangle -RMkdvi ext.nw > $(CMDDIR)/Mkdvi
	chmod +x $(CMDDIR)/Mkdvi

Mkdvisfrom: ext.nw
	notangle -RMkdvisfrom ext.nw > $(CMDDIR)/Mkdvisfrom
	chmod +x $(CMDDIR)/Mkdvisfrom

lextract: ext.pl extLdoc.pm
	notangle -Rlextract ext.nw > $(CMDDIR)/lextract
	chmod +x $(CMDDIR)/lextract

ldel: ext.nw ldel.pl extbasic.pm extout.pm
	notangle -Rldel ext.nw > $(CMDDIR)/ldel
	chmod +x $(CMDDIR)/ldel

ext_ldel: ext.nw ldel.pl
	notangle -Rext_ldel ext.nw > $(CMDDIR)/ext_ldel
	chmod +x $(CMDDIR)/ext_ldel

#------------------------------------------------------------------------------
# HTML-Extractions:
#------------------------------------------------------------------------------

HTMLman: ext.pl extHTML.pm
	notangle -RHTMLman ext.nw > $(CMDDIR)/HTMLman
	chmod +x $(CMDDIR)/HTMLman

HTMLdoc: ext.pl extHTML.pm
	notangle -RHTMLdoc ext.nw > $(CMDDIR)/HTMLdoc
	notangle -RHTMLLdoc ext.nw > $(CMDDIR)/HTMLLdoc
	notangle -Rcorrect_xlinks.pl ext.nw > $(CMDDIR)/correct_xlinks.pl
	chmod +x $(CMDDIR)/HTMLdoc
	chmod +x $(CMDDIR)/HTMLLdoc
	chmod +x $(CMDDIR)/correct_xlinks.pl

HTMLextract: ext.pl extHTML.pm
	notangle -RHTMLextract ext.nw > $(CMDDIR)/HTMLextract
	chmod +x $(CMDDIR)/HTMLextract

Mkhtml: ext.pl ext.nw
	notangle -RMkhtml ext.nw > $(CMDDIR)/Mkhtml
	notangle -Rextract_mtype.pl ext.nw > $(CMDDIR)/extract_mtype.pl
	chmod +x $(CMDDIR)/Mkhtml $(CMDDIR)/extract_mtype.pl

handle_refs: ext.nw
	notangle -Rhandle_refs.pl ext.nw > $(CMDDIR)/handle_refs.pl
	notangle -Rhandle_refs ext.nw > $(CMDDIR)/handle_refs
	chmod +x $(CMDDIR)/handle_refs

#------------------------------------------------------------------------------
# Tex-Extractions:
#------------------------------------------------------------------------------


noweb.sty: support.nw
	notangle -Rnoweb.sty support.nw > $(TEXDIR)/noweb.sty


MANUAL.mac: ext.nw
	notangle -RMANUAL.mac -filter emptydefn ext.nw > $(TEXDIR)/MANUAL.mac

Lweb.sty: ext.nw
	notangle -RLweb.sty -filter emptydefn ext.nw > $(TEXDIR)/Lweb.sty

Lbook.sty: ext.nw
	notangle -RLbook.sty -filter emptydefn ext.nw > $(HOME)/tex/Lbook.sty

MANUAL.pagesize: ext.nw
	notangle -RMANUAL.pagesize ext.nw > $(TEXDIR)/MANUAL.pagesize

#------------------------------------------------------------------------------
# 
#------------------------------------------------------------------------------

modules: extout.pm exthelp.pm extbasic.pm extHTML.pm extFman.pm extLdoc.pm extCdoc.pm
manualtex: MANUAL.mac MANUAL.pagesize Lweb.sty noweb.sty
manualcmd: Lman Fman Ldoc Cdoc Mkman Mkdvi Mkdvisfrom lweave lw2nw \
           lw2dvi emptyline lextract ldel ext_ldel
manual: manualtex manualcmd 

html: Mkhtml HTMLman HTMLdoc handle_refs 






