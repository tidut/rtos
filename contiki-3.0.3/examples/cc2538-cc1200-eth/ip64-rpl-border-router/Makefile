CONTIKI_PROJECT = border-router
all: $(CONTIKI_PROJECT) 

### Specify where CONTIKI resides
CONTIKI = ../../..

### Define our target platform
TARGET = cc2538-cc1200-eth

### IP64 specific stuff
# This tells the platform's makefile that we build with IP64 support
WITH_IP64 = 1

### Build with code size optimisations
SMALL = 1

### Include project specific configuration file
DEFINES += PROJECT_CONF_H=\"project-conf.h\"

### Built-in webserver configuration
# Simple built-in webserver is the default.
# Override with make WITH_WEBSERVER=0 for no webserver.
# WITH_WEBSERVER=webserver-name will use /apps/webserver-name if it can be
# found in the /apps, /platform/$(TARGET)/apps/, or current directory 
# (in that order).
# WITH_WEBSERVER=webserver for /apps/webserver
# WITH_WEBSERVER=raven-webserver for /platform/avr-raven/apps/raven-webserver/
# make clean before changing webservers!
# Note /apps/webserver contains a 2500 byte style sheet which is a severe test
# of the slip connection. Large MSS together with low baud rates without flow
# control will overrun the transmit buffer when the style sheet is requested.
WITH_WEBSERVER = 1
ifeq ($(WITH_WEBSERVER),1)
  DEFINES += UIP_CONF_TCP=1
  DEFINES += WEBSERVER=1
  PROJECT_SOURCEFILES += httpd-simple.c
else ifneq ($(WITH_WEBSERVER),0)
  APPS += $(WITH_WEBSERVER)
  DEFINES += UIP_CONF_TCP=1
  DEFINES += WEBSERVER=2
endif

### Use IPv6 stack
CONTIKI_WITH_IPV6 = 1


### Include Contiki's global makefile
include $(CONTIKI)/Makefile.include
