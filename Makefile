all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

# makefiles:
# 	  cd src && opp_makemake -f --deep

makefiles:
	cd src && opp_makemake -f --deep -O out -KINET_PROJ=../../inet -KVEINS_VEINS_5_2_PROJ=../../veins-veins-5.2 -KVEINS_INET_PROJ=../../veins-veins-5.2/subprojects/veins_inet -DINET_IMPORT -DVEINS_IMPORT -DVEINS_INET_IMPORT -I. -I$$\(INET_PROJ\)/src -I$$\(VEINS_VEINS_5_2_PROJ\)/src -I$$\(VEINS_INET_PROJ\)/src -Isrc -L$$\(INET_PROJ\)/src -L$$\(VEINS_VEINS_5_2_PROJ\)/src -L$$\(VEINS_INET_PROJ\)/src -lINET$$\(D\) -lveins$$\(D\) -lveins_inet$$\(D\)

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
