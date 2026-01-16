QT       += core gui widgets concurrent
CONFIG   += c++17

TARGET = FileZipper
TEMPLATE = app

# Tell the compiler where to look for your headers
INCLUDEPATH += src

# -------------------------------------------------
# YOUR SOURCE FILES (Make sure these exist in 'src')
# -------------------------------------------------
SOURCES += \
    main.cpp \
    src/ZipperApp.cpp \
    src/MainWindow.cpp \
    src/FrequencyCounter.cpp \
    src/HuffmanTree.cpp \
    src/BitWriter.cpp \
    src/Decompressor.cpp

# -------------------------------------------------
# YOUR HEADER FILES
# -------------------------------------------------
HEADERS += \
    src/ZipperApp.h \
    src/MainWindow.h \
    src/FrequencyCounter.h \
    src/HuffmanNode.h \
    src/HuffmanTree.h \
    src/BitWriter.h \
    src/Decompressor.h