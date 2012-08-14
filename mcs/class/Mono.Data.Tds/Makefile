thisdir = class/Mono.Data.Tds
SUBDIRS = 
include ../../build/rules.make

LIBRARY = Mono.Data.Tds.dll
LIB_MCS_FLAGS = /r:$(corlib) /r:System.dll /r:System.Xml.dll
NO_TEST = yes

ifneq (mobile, $(PROFILE))
LIB_MCS_FLAGS += /r:Mono.Security.dll
endif

include ../../build/library.make
