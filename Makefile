################################################################################
# Makefile for the Dynamics Toolset
# Copyright (c) 2006-2008 Jordan Van Aalsburg
#
# This file is part of the Dynamics Toolset.
#
# The Dynamics Toolset is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version3 3 of the License, or (at your option) any 
# later version.
# 
# The Dynamics Toolset is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
# details.
#
# You should have received a copy of the GNU General Public License
# along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
################################################################################

# Set the Vrui installation directory
#
VRUI_MAKEDIR = $(HOME)/usr/share/Vrui-2.2-003/share

# Set the installation root directory
#
INSTALLDIR = /usr/local

ifeq ($(INSTALLDIR),/)
	BININSTALLDIR = /usr/bin
	SHAREINSTALLDIR = /usr/share/DTS
else
	BININSTALLDIR  = $(INSTALLDIR)/bin
	SHAREINSTALLDIR =  $(INSTALLDIR)/share/DTS
endif


# Program name
#
PROGRAM=FieldViewer
VERSION=1.0

ifeq ($(shell uname -s),Darwin)
  SYSTEM_NAME = Darwin
endif
ifeq ($(shell uname -s),Linux)
  SYSTEM_NAME = Linux
endif
ifndef SYSTEM_NAME
  $(error Unsupported system type)
endif

# Compiler options
#
CC     = g++
CFLAGS = -Wall
OPT    = #-mfpmath=sse -msse2 -mmmx -g0 -DNDEBUG -O3

## If using MESA and getting GL enum errors, set this flag
#OPT    += -DMESA 

## If you have a ghetto video card that claims to support shaders
## but actually displays nothing, set this flag
#OPT     += -DGHETTO

LOCAL_INCLUDE += -I. -IExternal -IExternal/VruiSupport -IToolBox
LOCAL_LINK += -lGLU -lgle

TOOLBOX = lib/libToolBox.a
TOOLBOX_SOURCES = $(shell find ToolBox -name "*.cpp")


# Search plugin directory and generate list of plugin names
#
PLUGINS = $(addprefix plugins/lib,$(subst .cpp,.so,$(subst ParameterDialog,,$(subst ./Dynamics/plugins/,,$(shell find ./Dynamics -name *ParameterDialog.cpp)))))

PLUGINS += plugins/libCartesianGrid3D.so

# Project source files
#
SOURCES = Tools/AbstractDynamicsTool.cpp              	  \
	  Tools/ParticleSprayerTool.cpp                   \
	  Tools/DotSpreaderTool.cpp                       \
	  Tools/ParticleSprayerOptionsDialog.cpp          \
	  Tools/DotSpreaderOptionsDialog.cpp              \
	  Tools/StaticSolverTool.cpp                      \
	  Tools/StaticSolverOptionsDialog.cpp             \
	  Tools/DynamicSolverTool.cpp                     \
	  Tools/DynamicSolverOptionsDialog.cpp            \
	  Dynamics/IntegrationStepSize.cpp                \
      Dynamics/Interpolator.cpp                       \
      Dynamics/TrilinearInterpolator.cpp              \
	  External/VruiSupport/VruiStreamManip.cpp        \
	  DataItem.cpp                                    \
	  FrameRateDialog.cpp                             \
	  PositionDialog.cpp                              \
	  FieldViewer_ui.cpp                              \
	  FieldViewer.cpp                                 \
	  main.cpp


# Object and dependency file locations
#
OBJECT_DIR = obj
DEPEND_DIR = dep


# define Vrui include and link settings
ifeq ($(shell uname -m), x86_64)
  LIBEXT = lib64
else
  LIBEXT = lib
endif

include $(VRUI_MAKEDIR)/Vrui.makeinclude

ifndef VERBOSE 
	QUIET = @
endif


.PHONY: all
all: $(TOOLBOX) $(PROGRAM) $(PLUGINS)


# Main program
#
$(PROGRAM): $(SOURCES:%.cpp=$(OBJECT_DIR)/%.o)
	@echo Linking executable $@...
	$(QUIET)$(CC) $(CFLAGS) -rdynamic -o $@ $^ $(LOCAL_LINK) $(TOOLBOX) $(VRUI_LINKFLAGS) -ldl


# ToolBox library (static)
#
$(TOOLBOX): $(TOOLBOX_SOURCES:%.cpp=$(OBJECT_DIR)/%.o)
	@echo Creating  $@...
	$(QUIET)mkdir -p lib
	$(QUIET)$(AR) rcs $@ $^


# Include plugin makefile fragment
#
include etc/plugin.mk

# Additional plugins
plugins/libCartesianGrid3D.so: obj/Dynamics/plugins/CartesianGrid3D.o obj/Dynamics/plugins/CartesianGrid3DParameterDialog.o
obj/Dynamics/plugins/CartesianGrid3D.o: Dynamics/plugins/CartesianGrid3D.cpp
obj/Dynamics/plugins/CartesianGrid3DParameterDialog.o: Dynamics/plugins/CartesianGrid3DParameterDialog.cpp


#
# Generic build rules
#

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(SOURCES:%.cpp=$(DEPEND_DIR)/%.d)
  -include $(TOOLBOX_SOURCES:%.cpp=$(DEPEND_DIR)/%.d)
endif

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM -MF $3 -MP -MT $2 $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) $1
endef


# Plugin object files
#
$(OBJECT_DIR)/Dynamics/plugins/%.o:
	$(QUIET)mkdir -p $(OBJECT_DIR)/Dynamics/plugins/
	@echo [plugin] Compiling $<...
	$(QUIET)$(CC) $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) -I./Dynamics -fPIC -c -g -o $@ $<


# Regular object files
#
$(OBJECT_DIR)/%.o: %.cpp
	$(QUIET)mkdir -p $(OBJECT_DIR)
	@echo Compiling $<...
	$(QUIET)mkdir -p $(OBJECT_DIR)/$(*D)
	$(QUIET)mkdir -p $(DEPEND_DIR)/$(*D)
	$(QUIET)$(call make-depend,$<,$@,$(@:$(OBJECT_DIR)/%.o=$(DEPEND_DIR)/%.d))
	$(QUIET)$(CC) -c -g -o $@ $(CFLAGS) $(LOCAL_INCLUDE) $(VRUI_CFLAGS) $(OPT) $<

$(OBJECT_DIR)/FieldViewer.o: CFLAGS += -DRESOURCEDIR='"$(SHAREINSTALLDIR)"'

ifeq "$(SYSTEM_NAME)" "Darwin"
define plugin-compile
	$(CC) -dynamiclib -single_module -install_name $1 -undefined dynamic_lookup -o $2 $3
endef
else
define plugin-compile
	$(CC) -shared -Wl,-soname,$1 -o $2 $3
endef
endif

# Dynamic libraries (plugins)
#
plugins/%.so:
	$(QUIET)mkdir -p plugins
	@echo "[plugin] Creating $@..."
	$(QUIET)$(call plugin-compile,$(subst plugins/,,$@),$@,$^)
	$(QUIET)ln -sf $(subst plugins/,,$@) $@.1

# Unset all default rules
#
%: ;

#
# Utility targets
#

.PHONY: clean
clean:
	@echo "removing object files and dependencies..."
	$(QUIET)rm -rdf $(OBJECT_DIR)
	$(QUIET)rm -rdf $(DEPEND_DIR)
	@echo "removing libraries and binaries..."
	$(QUIET)rm -rf lib/ $(PROGRAM)


BACKUP_FILES = $(subst ./,,$(shell find . -name "*~"))

.PHONY: squeaky
squeaky: clean
	@echo "removing plugins..."	
	$(QUIET)rm -rdf plugins
	@echo "removing backup files..."
	$(QUIET)rm -f $(BACKUP_FILES)	

.PHONY: distclean
distclean: squeaky
	@echo "removing plugin code..."
	$(QUIET)rm -rdf Dynamics/plugins
	$(QUIET)rm -f etc/plugin.mk
	@echo "removing documentation..."
	$(QUIET)rm -rdf doc


.PHONY: install
install: all
	@echo "installing program and libraries"
	$(QUIET)mkdir -p $(BININSTALLDIR) 
	$(QUIET)mkdir -p $(SHAREINSTALLDIR)
	$(QUIET)cp $(PROGRAM) $(BININSTALLDIR)
	$(QUIET)cp Vrui.cfg $(SHAREINSTALLDIR)
	$(QUIET)cp particle.png $(SHAREINSTALLDIR)
	$(QUIET)cp -r plugins $(SHAREINSTALLDIR)

# Code documentation
#
ifdef VERBOSE
  MAKEDOC = doxygen
else
  MAKEDOC = doxygen > /dev/null
endif

.PHONY: doc
doc:
	@echo "generating documentation"
	@$(MAKEDOC)


ARCHIVE_LIST = $(filter-out Dynamics/plugins/%,$(subst ./,,$(shell find . -name "*h" -o -name "*.cpp")))
ARCHIVE_LIST += ToolBox/ToolBox DTSTools Makefile Doxyfile Vrui.cfg particle.png
ARCHIVE_LIST += bin/generate_plugin_code.py etc/dynamics.xml
ARCHIVE_LIST += External

# Code archival
#
ifdef VERBOSE
	TAR = tar czvf
else
	TAR = tar czf
endif

.PHONY: archive
archive:
	@echo "creating archive..."
	$(QUIET)$(TAR) $(PROGRAM)-$(VERSION).tgz $(ARCHIVE_LIST)


.PHONY: debug
debug:
	@echo "PLUGINS:"
	@echo
	@echo $(PLUGINS)
