TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    raft/nodei.cpp

HEADERS += \
    raft/common.h \
    raft/database.h \
    raft/log.h \
    raft/message.h \
    raft/node.h \
    raft/nodeproxy.h \
    raft/nodei.h

QMAKE_CXXFLAGS += -std=c++17
