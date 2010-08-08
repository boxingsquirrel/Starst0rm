# This is the actual Makefile for Star St0rm. You should use ./buildAll (pull req'd sources and compile) or ./compileAll (rebuild what's already there) instead.
# Copyright 2010 boxingsquirrel. GPLv3.
# Feel free to mess with your own copy of this, but if you mod anything don't expect me to support it (I'll try to help, but this build system is held together by prayers only :P)!

CC=gcc
CP=cp
RM=rm -rf
MKDIR=mkdir
GIT=git

CFLAGS=-std=gnu99 `pkg-config --cflags gtk+-2.0`
LFLAGS=`pkg-config --libs gtk+-2.0` -lzip -lplist -lssl

SRCDIR=src
RESDIR=res

SRCFILES=$(SRCDIR)/st0rm.c $(SRCDIR)/ui.c $(SRCDIR)/jb.c $(SRCDIR)/ipsw.c
OBJFILES=$(subst .c,.o,$(SRCFILES))
#MENUFILE=ipswTool.desktop

OUTFILE=st0rm

INSTPATH=/usr/local/share/starst0rm
DATAPATH=/usr/local/share/starst0rm

XPWNSRCDIR=$(SRCDIR)/xpwn
XPWNOBJFILES=$(XPWNSRCDIR)/dfu-util/main.o $(XPWNSRCDIR)/dfu-util/dfu.o $(XPWNSRCDIR)/dfu-util/sam7dfu.o $(XPWNSRCDIR)/common/abstractfile.o $(XPWNSRCDIR)/ipsw-patch/outputstate.o $(XPWNMINIZIPOBJFILES)

XPWNMINIZIP=$(XPWNSRCDIR)/minizip/unzip.c $(XPWNSRCDIR)/minizip/ioapi.c $(XPWNSRCDIR)/minizip/zip.c
XPWNMINIZIPOBJFILES=$(subst .c,.o,$(XPWNMINIZIP))

all: prep tool

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

prep: xpwn_cp
	@echo "Configured and prepared dependencies for Starst0rm"

tool: $(OBJFILES)
	$(CC) -o $(OUTFILE) $(OBJFILES) $(LFLAGS)
	gtk-builder-convert $(SRCDIR)/jb_dlg.glade $(RESDIR)/jb_dlg.xml
	gtk-builder-convert $(SRCDIR)/main.glade $(RESDIR)/main.xml
	$(CP) $(SRCDIR)/*.png $(RESDIR)/
	@echo ""
	@echo "Run 'make install' as root to install"

dist:
	tar -cvzf Starst0rm_4.0.tar.gz bundles/ FirmwareBundles/ res/ ipsw dfu-util st0rm LICENSE README CREDITS

dfu-util: $(XPWNMINIZIPOBJFILES)
	$(CC) $(CFLAGS) -c $(XPWNSRCDIR)/dfu-util/main.c -o $(XPWNSRCDIR)/dfu-util/main.o -I./$(XPWNSRCDIR)/includes
	$(CC) $(CFLAGS) -c $(XPWNSRCDIR)/dfu-util/dfu.c -o $(XPWNSRCDIR)/dfu-util/dfu.o -I./$(XPWNSRCDIR)/includes
	$(CC) $(CFLAGS) -c $(XPWNSRCDIR)/dfu-util/sam7dfu.c -o $(XPWNSRCDIR)/dfu-util/sam7dfu.o -I./$(XPWNSRCDIR)/includes
	$(CC) $(CFLAGS) -c $(XPWNSRCDIR)/common/abstractfile.c -o $(XPWNSRCDIR)/common/abstractfile.o -I./$(XPWNSRCDIR)/includes
	$(CC) $(CFLAGS) -c $(XPWNSRCDIR)/ipsw-patch/outputstate.c -o $(XPWNSRCDIR)/ipsw-patch/outputstate.o -I./$(XPWNSRCDIR)/includes -I./$(XPWNSRCDIR)/minizip
	@echo "All right, gonna try and build dfu-util now..."
	$(CC) -o dfu-util $(XPWNOBJFILES) $(LFLAGS) -lusb
	@echo "Cleanup on aisle 7!"
	$(RM) $(XPWNOBJFILES)

xpwn_fork:
	$(RM) $(XPWNSRCDIR)
	$(RM) xpwn
	$(GIT) clone git://github.com/boxingsquirrel/xpwn.git $(XPWNSRCDIR)

xpwn_build: xpwn_fork dfu-util
	cd $(XPWNSRCDIR); $(RM) build; mkdir build && cd build; cmake ..; make

xpwn_cp: xpwn_build
	$(RM) FirmwareBundles
	$(RM) bundles
	$(MKDIR) FirmwareBundles
	$(MKDIR) bundles
	$(CP) $(XPWNSRCDIR)/build/ipsw-patch/ipsw ./ipsw
	$(CP) -r $(XPWNSRCDIR)/ipsw-patch/FirmwareBundles/* ./FirmwareBundles
	$(CP) $(XPWNSRCDIR)/ipsw-patch/bundles/* ./bundles

update_depends: update_xpwn
	@echo ""

update_xpwn:
	$(CD) $(XPWNSRCDIR); $(GIT) pull

install: tool
	@echo "Installing 1 file to "$(INSTPATH)"..."
	$(CP) ./$(OUTFILE) $(INSTPATH)/$(OUTFILE)
	@echo "Installing 4 files to "$(DATAPATH)"..."
	$(RM) $(DATAPATH)
	$(MKDIR) $(DATAPATH)
	$(MKDIR) $(DATAPATH)/res $(DATAPATH)/data
	$(CP) ./res/* $(DATAPATH)/res
	$(CP) ./data/* $(DATAPATH)/data
	@echo "Adding "$(OUTFILE)" to the menu..."
	cp $(MENUFILE) /usr/share/applications/$(MENUFILE)
	@echo "Install complete"
	@echo "To remove run 'make uninstall' as root"

uninstall:
	@echo "Removing 1 file from "$(INSTPATH)"..."
	$(RM) $(INSTPATH)/$(OUTFILE)
	@echo "Removing 4 files from "$(DATAPATH)"..."
	$(RM) $(DATAPATH)
	@echo "Removing "$(OUTFILE)" from the menu..."
	$(RM) /usr/share/applications/$(MENUFILE)
	@echo "Uninstall complete"

clean:
	$(RM) $(OUTFILE) $(OBJFILES) $(XPWNOBJFILES) ipsw dfu-util
