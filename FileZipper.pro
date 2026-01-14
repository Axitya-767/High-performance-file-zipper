QT += core concurrent
CONFIG += c++17
CONFIG -= app_bundle

SOURCES += main.cpp \
    src/BitWriter.cpp \
    src/Decompressor.cpp \
    src/FrequencyCounter.cpp \
    src/HuffmanTree.cpp \
    src/ZipperApp.cpp

HEADERS += \
    src/BitWriter.h \
    src/Decompressor.h \
    src/FrequencyCounter.h \
    src/HuffmanNode.h \
    src/HuffmanTree.h \
    src/ZipperApp.h

INCLUDEPATH += src