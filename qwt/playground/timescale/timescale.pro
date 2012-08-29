################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

include( $${PWD}/../playground.pri )

TARGET       = timescale

HEADERS = \
    timedate.h \
    timeinterval.h \
    timescaleengine.h \
    timescaledraw.h \
    panel.h \
    plot.h \
    mainwindow.h

SOURCES = \
    timedate.cpp \
    timeinterval.cpp \
    timescaleengine.cpp \
    timescaledraw.cpp \
    panel.cpp \
    plot.cpp \
    mainwindow.cpp \
    main.cpp

