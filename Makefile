
MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_QML_DEBUG -DQT_DECLARATIVE_DEBUG -DQT_CORE_LIB
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT -fPIE $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W -D_REENTRANT -fPIE $(DEFINES)
INCPATH       = -I/opt/Qt5.2.0/5.2.0/gcc/mkspecs/linux-g++ -I../BackGroundTest -I/usr/local/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -I/opt/Qt5.2.0/5.2.0/gcc/include -I/opt/Qt5.2.0/5.2.0/gcc/include/QtCore -I. -I.
LINK          = g++
LFLAGS        = -Wl,-rpath,/opt/Qt5.2.0/5.2.0/gcc -Wl,-rpath,/opt/Qt5.2.0/5.2.0/gcc/lib
LIBS          = $(SUBLIBS) /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libcvblob.so /usr/local/lib/libopencv_video.so -L/opt/Qt5.2.0/5.2.0/gcc/lib -lQt5Core -lpthread 
AR            = ar cqs
RANLIB        = 
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = ../BackGroundTest/AvgBackground.cpp \
		../BackGroundTest/backgroundDiff.cpp \
		../BackGroundTest/BlobProcess.cpp \
		../BackGroundTest/Main.cpp \
		../BackGroundTest/Ctracker.cpp \
		../BackGroundTest/HungarianAlg.cpp \
		../BackGroundTest/Kalman.cpp 
OBJECTS       = AvgBackground.o \
		backgroundDiff.o \
		BlobProcess.o \
		Main.o \
		Ctracker.o \
		HungarianAlg.o \
		Kalman.o

#QMAKE_TARGET  = BackGroundTest
#DESTDIR       = #avoid trailing-slash linebreak
TARGET        = BackGroundTest


first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9][^0-9]*\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true






clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_header_make_all:
compiler_moc_header_clean:
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

AvgBackground.o: ../BackGroundTest/AvgBackground.cpp ../BackGroundTest/AvgBackground.h \
		/usr/local/include/opencv/cv.h \
		/usr/local/include/opencv2/core/core_c.h \
		/usr/local/include/opencv2/core/types_c.h \
		/usr/local/include/opencv2/core/core.hpp \
		/usr/local/include/opencv2/core/version.hpp \
		/usr/local/include/opencv2/core/operations.hpp \
		/usr/local/include/opencv2/core/mat.hpp \
		/usr/local/include/opencv2/imgproc/imgproc_c.h \
		/usr/local/include/opencv2/imgproc/types_c.h \
		/usr/local/include/opencv2/imgproc/imgproc.hpp \
		/usr/local/include/opencv2/video/tracking.hpp \
		/usr/local/include/opencv2/features2d/features2d.hpp \
		/usr/local/include/opencv2/flann/miniflann.hpp \
		/usr/local/include/opencv2/flann/defines.h \
		/usr/local/include/opencv2/flann/config.h \
		/usr/local/include/opencv2/flann/flann.hpp \
		/usr/local/include/opencv2/flann/flann_base.hpp \
		/usr/local/include/opencv2/flann/general.h \
		/usr/local/include/opencv2/flann/matrix.h \
		/usr/local/include/opencv2/flann/params.h \
		/usr/local/include/opencv2/flann/any.h \
		/usr/local/include/opencv2/flann/saving.h \
		/usr/local/include/opencv2/flann/nn_index.h \
		/usr/local/include/opencv2/flann/result_set.h \
		/usr/local/include/opencv2/flann/all_indices.h \
		/usr/local/include/opencv2/flann/kdtree_index.h \
		/usr/local/include/opencv2/flann/dynamic_bitset.h \
		/usr/local/include/opencv2/flann/dist.h \
		/usr/local/include/opencv2/flann/heap.h \
		/usr/local/include/opencv2/flann/allocator.h \
		/usr/local/include/opencv2/flann/random.h \
		/usr/local/include/opencv2/flann/kdtree_single_index.h \
		/usr/local/include/opencv2/flann/kmeans_index.h \
		/usr/local/include/opencv2/flann/logger.h \
		/usr/local/include/opencv2/flann/composite_index.h \
		/usr/local/include/opencv2/flann/linear_index.h \
		/usr/local/include/opencv2/flann/hierarchical_clustering_index.h \
		/usr/local/include/opencv2/flann/lsh_index.h \
		/usr/local/include/opencv2/flann/lsh_table.h \
		/usr/local/include/opencv2/flann/autotuned_index.h \
		/usr/local/include/opencv2/flann/ground_truth.h \
		/usr/local/include/opencv2/flann/index_testing.h \
		/usr/local/include/opencv2/flann/timer.h \
		/usr/local/include/opencv2/flann/sampling.h \
		/usr/local/include/opencv2/calib3d/calib3d.hpp \
		/usr/local/include/opencv2/objdetect/objdetect.hpp \
		/usr/local/include/opencv2/legacy/compat.hpp \
		/usr/local/include/opencv2/core/internal.hpp \
		/usr/local/include/opencv2/core/eigen.hpp \
		/usr/local/include/opencv/highgui.h \
		/usr/local/include/opencv2/highgui/highgui_c.h \
		/usr/local/include/opencv2/highgui/highgui.hpp \
		/usr/local/include/opencv/cxcore.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o AvgBackground.o ../BackGroundTest/AvgBackground.cpp

backgroundDiff.o: ../BackGroundTest/backgroundDiff.cpp ../BackGroundTest/cv_yuv_codebook.h \
		/usr/local/include/opencv/cv.h \
		/usr/local/include/opencv2/core/core_c.h \
		/usr/local/include/opencv2/core/types_c.h \
		/usr/local/include/opencv2/core/core.hpp \
		/usr/local/include/opencv2/core/version.hpp \
		/usr/local/include/opencv2/core/operations.hpp \
		/usr/local/include/opencv2/core/mat.hpp \
		/usr/local/include/opencv2/imgproc/imgproc_c.h \
		/usr/local/include/opencv2/imgproc/types_c.h \
		/usr/local/include/opencv2/imgproc/imgproc.hpp \
		/usr/local/include/opencv2/video/tracking.hpp \
		/usr/local/include/opencv2/features2d/features2d.hpp \
		/usr/local/include/opencv2/flann/miniflann.hpp \
		/usr/local/include/opencv2/flann/defines.h \
		/usr/local/include/opencv2/flann/config.h \
		/usr/local/include/opencv2/flann/flann.hpp \
		/usr/local/include/opencv2/flann/flann_base.hpp \
		/usr/local/include/opencv2/flann/general.h \
		/usr/local/include/opencv2/flann/matrix.h \
		/usr/local/include/opencv2/flann/params.h \
		/usr/local/include/opencv2/flann/any.h \
		/usr/local/include/opencv2/flann/saving.h \
		/usr/local/include/opencv2/flann/nn_index.h \
		/usr/local/include/opencv2/flann/result_set.h \
		/usr/local/include/opencv2/flann/all_indices.h \
		/usr/local/include/opencv2/flann/kdtree_index.h \
		/usr/local/include/opencv2/flann/dynamic_bitset.h \
		/usr/local/include/opencv2/flann/dist.h \
		/usr/local/include/opencv2/flann/heap.h \
		/usr/local/include/opencv2/flann/allocator.h \
		/usr/local/include/opencv2/flann/random.h \
		/usr/local/include/opencv2/flann/kdtree_single_index.h \
		/usr/local/include/opencv2/flann/kmeans_index.h \
		/usr/local/include/opencv2/flann/logger.h \
		/usr/local/include/opencv2/flann/composite_index.h \
		/usr/local/include/opencv2/flann/linear_index.h \
		/usr/local/include/opencv2/flann/hierarchical_clustering_index.h \
		/usr/local/include/opencv2/flann/lsh_index.h \
		/usr/local/include/opencv2/flann/lsh_table.h \
		/usr/local/include/opencv2/flann/autotuned_index.h \
		/usr/local/include/opencv2/flann/ground_truth.h \
		/usr/local/include/opencv2/flann/index_testing.h \
		/usr/local/include/opencv2/flann/timer.h \
		/usr/local/include/opencv2/flann/sampling.h \
		/usr/local/include/opencv2/calib3d/calib3d.hpp \
		/usr/local/include/opencv2/objdetect/objdetect.hpp \
		/usr/local/include/opencv2/legacy/compat.hpp \
		/usr/local/include/opencv2/core/internal.hpp \
		/usr/local/include/opencv2/core/eigen.hpp \
		/usr/local/include/opencv/highgui.h \
		/usr/local/include/opencv2/highgui/highgui_c.h \
		/usr/local/include/opencv2/highgui/highgui.hpp \
		/usr/local/include/opencv/cxcore.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o backgroundDiff.o ../BackGroundTest/backgroundDiff.cpp

BlobProcess.o: ../BackGroundTest/BlobProcess.cpp ../BackGroundTest/cv_yuv_codebook.h \
		/usr/local/include/opencv/cv.h \
		/usr/local/include/opencv2/core/core_c.h \
		/usr/local/include/opencv2/core/types_c.h \
		/usr/local/include/opencv2/core/core.hpp \
		/usr/local/include/opencv2/core/version.hpp \
		/usr/local/include/opencv2/core/operations.hpp \
		/usr/local/include/opencv2/core/mat.hpp \
		/usr/local/include/opencv2/imgproc/imgproc_c.h \
		/usr/local/include/opencv2/imgproc/types_c.h \
		/usr/local/include/opencv2/imgproc/imgproc.hpp \
		/usr/local/include/opencv2/video/tracking.hpp \
		/usr/local/include/opencv2/features2d/features2d.hpp \
		/usr/local/include/opencv2/flann/miniflann.hpp \
		/usr/local/include/opencv2/flann/defines.h \
		/usr/local/include/opencv2/flann/config.h \
		/usr/local/include/opencv2/flann/flann.hpp \
		/usr/local/include/opencv2/flann/flann_base.hpp \
		/usr/local/include/opencv2/flann/general.h \
		/usr/local/include/opencv2/flann/matrix.h \
		/usr/local/include/opencv2/flann/params.h \
		/usr/local/include/opencv2/flann/any.h \
		/usr/local/include/opencv2/flann/saving.h \
		/usr/local/include/opencv2/flann/nn_index.h \
		/usr/local/include/opencv2/flann/result_set.h \
		/usr/local/include/opencv2/flann/all_indices.h \
		/usr/local/include/opencv2/flann/kdtree_index.h \
		/usr/local/include/opencv2/flann/dynamic_bitset.h \
		/usr/local/include/opencv2/flann/dist.h \
		/usr/local/include/opencv2/flann/heap.h \
		/usr/local/include/opencv2/flann/allocator.h \
		/usr/local/include/opencv2/flann/random.h \
		/usr/local/include/opencv2/flann/kdtree_single_index.h \
		/usr/local/include/opencv2/flann/kmeans_index.h \
		/usr/local/include/opencv2/flann/logger.h \
		/usr/local/include/opencv2/flann/composite_index.h \
		/usr/local/include/opencv2/flann/linear_index.h \
		/usr/local/include/opencv2/flann/hierarchical_clustering_index.h \
		/usr/local/include/opencv2/flann/lsh_index.h \
		/usr/local/include/opencv2/flann/lsh_table.h \
		/usr/local/include/opencv2/flann/autotuned_index.h \
		/usr/local/include/opencv2/flann/ground_truth.h \
		/usr/local/include/opencv2/flann/index_testing.h \
		/usr/local/include/opencv2/flann/timer.h \
		/usr/local/include/opencv2/flann/sampling.h \
		/usr/local/include/opencv2/calib3d/calib3d.hpp \
		/usr/local/include/opencv2/objdetect/objdetect.hpp \
		/usr/local/include/opencv2/legacy/compat.hpp \
		/usr/local/include/opencv2/core/internal.hpp \
		/usr/local/include/opencv2/core/eigen.hpp \
		/usr/local/include/opencv/highgui.h \
		/usr/local/include/opencv2/highgui/highgui_c.h \
		/usr/local/include/opencv2/highgui/highgui.hpp \
		/usr/local/include/opencv/cxcore.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BlobProcess.o ../BackGroundTest/BlobProcess.cpp

Main.o: ../BackGroundTest/Main.cpp /usr/local/include/opencv2/core/core.hpp \
		/usr/local/include/opencv2/core/types_c.h \
		/usr/local/include/opencv2/core/version.hpp \
		/usr/local/include/opencv2/core/operations.hpp \
		/usr/local/include/opencv2/core/mat.hpp \
		/usr/local/include/opencv2/highgui/highgui.hpp \
		/usr/local/include/opencv2/highgui/highgui_c.h \
		/usr/local/include/opencv2/core/core_c.h \
		/usr/local/include/opencv2/imgproc/imgproc.hpp \
		/usr/local/include/opencv2/imgproc/types_c.h \
		../BackGroundTest/AvgBackground.h \
		/usr/local/include/opencv/cv.h \
		/usr/local/include/opencv2/imgproc/imgproc_c.h \
		/usr/local/include/opencv2/video/tracking.hpp \
		/usr/local/include/opencv2/features2d/features2d.hpp \
		/usr/local/include/opencv2/flann/miniflann.hpp \
		/usr/local/include/opencv2/flann/defines.h \
		/usr/local/include/opencv2/flann/config.h \
		/usr/local/include/opencv2/flann/flann.hpp \
		/usr/local/include/opencv2/flann/flann_base.hpp \
		/usr/local/include/opencv2/flann/general.h \
		/usr/local/include/opencv2/flann/matrix.h \
		/usr/local/include/opencv2/flann/params.h \
		/usr/local/include/opencv2/flann/any.h \
		/usr/local/include/opencv2/flann/saving.h \
		/usr/local/include/opencv2/flann/nn_index.h \
		/usr/local/include/opencv2/flann/result_set.h \
		/usr/local/include/opencv2/flann/all_indices.h \
		/usr/local/include/opencv2/flann/kdtree_index.h \
		/usr/local/include/opencv2/flann/dynamic_bitset.h \
		/usr/local/include/opencv2/flann/dist.h \
		/usr/local/include/opencv2/flann/heap.h \
		/usr/local/include/opencv2/flann/allocator.h \
		/usr/local/include/opencv2/flann/random.h \
		/usr/local/include/opencv2/flann/kdtree_single_index.h \
		/usr/local/include/opencv2/flann/kmeans_index.h \
		/usr/local/include/opencv2/flann/logger.h \
		/usr/local/include/opencv2/flann/composite_index.h \
		/usr/local/include/opencv2/flann/linear_index.h \
		/usr/local/include/opencv2/flann/hierarchical_clustering_index.h \
		/usr/local/include/opencv2/flann/lsh_index.h \
		/usr/local/include/opencv2/flann/lsh_table.h \
		/usr/local/include/opencv2/flann/autotuned_index.h \
		/usr/local/include/opencv2/flann/ground_truth.h \
		/usr/local/include/opencv2/flann/index_testing.h \
		/usr/local/include/opencv2/flann/timer.h \
		/usr/local/include/opencv2/flann/sampling.h \
		/usr/local/include/opencv2/calib3d/calib3d.hpp \
		/usr/local/include/opencv2/objdetect/objdetect.hpp \
		/usr/local/include/opencv2/legacy/compat.hpp \
		/usr/local/include/opencv2/core/internal.hpp \
		/usr/local/include/opencv2/core/eigen.hpp \
		/usr/local/include/opencv/highgui.h \
		/usr/local/include/opencv/cxcore.h \
		../BackGroundTest/cv_yuv_codebook.h \
		../BackGroundTest/datastructure.h \
		/usr/local/include/cvblob.h \
		/usr/local/include/opencv2/opencv.hpp \
		/usr/local/include/opencv2/photo/photo.hpp \
		/usr/local/include/opencv2/photo/photo_c.h \
		/usr/local/include/opencv2/video/video.hpp \
		/usr/local/include/opencv2/video/background_segm.hpp \
		/usr/local/include/opencv2/ml/ml.hpp \
		/usr/local/include/opencv2/contrib/contrib.hpp \
		/usr/local/include/opencv2/contrib/retina.hpp \
		/usr/local/include/opencv2/contrib/openfabmap.hpp \
		../BackGroundTest/BackgroundSubtract.h \
		../BackGroundTest/OpencvInclude.h \
		../BackGroundTest/VIBE.h \
		../BackGroundTest/Detector.h \
		../BackGroundTest/Ctracker.h \
		../BackGroundTest/Kalman.h \
		../BackGroundTest/HungarianAlg.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Main.o ../BackGroundTest/Main.cpp

Ctracker.o: ../BackGroundTest/Ctracker.cpp ../BackGroundTest/Ctracker.h \
		../BackGroundTest/Kalman.h \
		/usr/local/include/opencv2/opencv.hpp \
		/usr/local/include/opencv2/core/core_c.h \
		/usr/local/include/opencv2/core/types_c.h \
		/usr/local/include/opencv2/core/core.hpp \
		/usr/local/include/opencv2/core/version.hpp \
		/usr/local/include/opencv2/core/operations.hpp \
		/usr/local/include/opencv2/core/mat.hpp \
		/usr/local/include/opencv2/flann/miniflann.hpp \
		/usr/local/include/opencv2/flann/defines.h \
		/usr/local/include/opencv2/flann/config.h \
		/usr/local/include/opencv2/imgproc/imgproc_c.h \
		/usr/local/include/opencv2/imgproc/types_c.h \
		/usr/local/include/opencv2/imgproc/imgproc.hpp \
		/usr/local/include/opencv2/photo/photo.hpp \
		/usr/local/include/opencv2/photo/photo_c.h \
		/usr/local/include/opencv2/video/video.hpp \
		/usr/local/include/opencv2/video/tracking.hpp \
		/usr/local/include/opencv2/video/background_segm.hpp \
		/usr/local/include/opencv2/features2d/features2d.hpp \
		/usr/local/include/opencv2/objdetect/objdetect.hpp \
		/usr/local/include/opencv2/calib3d/calib3d.hpp \
		/usr/local/include/opencv2/ml/ml.hpp \
		/usr/local/include/opencv2/highgui/highgui_c.h \
		/usr/local/include/opencv2/highgui/highgui.hpp \
		/usr/local/include/opencv2/contrib/contrib.hpp \
		/usr/local/include/opencv2/contrib/retina.hpp \
		/usr/local/include/opencv2/contrib/openfabmap.hpp \
		/usr/local/include/opencv/cv.h \
		/usr/local/include/opencv2/flann/flann.hpp \
		/usr/local/include/opencv2/flann/flann_base.hpp \
		/usr/local/include/opencv2/flann/general.h \
		/usr/local/include/opencv2/flann/matrix.h \
		/usr/local/include/opencv2/flann/params.h \
		/usr/local/include/opencv2/flann/any.h \
		/usr/local/include/opencv2/flann/saving.h \
		/usr/local/include/opencv2/flann/nn_index.h \
		/usr/local/include/opencv2/flann/result_set.h \
		/usr/local/include/opencv2/flann/all_indices.h \
		/usr/local/include/opencv2/flann/kdtree_index.h \
		/usr/local/include/opencv2/flann/dynamic_bitset.h \
		/usr/local/include/opencv2/flann/dist.h \
		/usr/local/include/opencv2/flann/heap.h \
		/usr/local/include/opencv2/flann/allocator.h \
		/usr/local/include/opencv2/flann/random.h \
		/usr/local/include/opencv2/flann/kdtree_single_index.h \
		/usr/local/include/opencv2/flann/kmeans_index.h \
		/usr/local/include/opencv2/flann/logger.h \
		/usr/local/include/opencv2/flann/composite_index.h \
		/usr/local/include/opencv2/flann/linear_index.h \
		/usr/local/include/opencv2/flann/hierarchical_clustering_index.h \
		/usr/local/include/opencv2/flann/lsh_index.h \
		/usr/local/include/opencv2/flann/lsh_table.h \
		/usr/local/include/opencv2/flann/autotuned_index.h \
		/usr/local/include/opencv2/flann/ground_truth.h \
		/usr/local/include/opencv2/flann/index_testing.h \
		/usr/local/include/opencv2/flann/timer.h \
		/usr/local/include/opencv2/flann/sampling.h \
		/usr/local/include/opencv2/legacy/compat.hpp \
		/usr/local/include/opencv2/core/internal.hpp \
		/usr/local/include/opencv2/core/eigen.hpp \
		../BackGroundTest/HungarianAlg.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Ctracker.o ../BackGroundTest/Ctracker.cpp

HungarianAlg.o: ../BackGroundTest/HungarianAlg.cpp ../BackGroundTest/HungarianAlg.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o HungarianAlg.o ../BackGroundTest/HungarianAlg.cpp

Kalman.o: ../BackGroundTest/Kalman.cpp ../BackGroundTest/Kalman.h \
		/usr/local/include/opencv2/opencv.hpp \
		/usr/local/include/opencv2/core/core_c.h \
		/usr/local/include/opencv2/core/types_c.h \
		/usr/local/include/opencv2/core/core.hpp \
		/usr/local/include/opencv2/core/version.hpp \
		/usr/local/include/opencv2/core/operations.hpp \
		/usr/local/include/opencv2/core/mat.hpp \
		/usr/local/include/opencv2/flann/miniflann.hpp \
		/usr/local/include/opencv2/flann/defines.h \
		/usr/local/include/opencv2/flann/config.h \
		/usr/local/include/opencv2/imgproc/imgproc_c.h \
		/usr/local/include/opencv2/imgproc/types_c.h \
		/usr/local/include/opencv2/imgproc/imgproc.hpp \
		/usr/local/include/opencv2/photo/photo.hpp \
		/usr/local/include/opencv2/photo/photo_c.h \
		/usr/local/include/opencv2/video/video.hpp \
		/usr/local/include/opencv2/video/tracking.hpp \
		/usr/local/include/opencv2/video/background_segm.hpp \
		/usr/local/include/opencv2/features2d/features2d.hpp \
		/usr/local/include/opencv2/objdetect/objdetect.hpp \
		/usr/local/include/opencv2/calib3d/calib3d.hpp \
		/usr/local/include/opencv2/ml/ml.hpp \
		/usr/local/include/opencv2/highgui/highgui_c.h \
		/usr/local/include/opencv2/highgui/highgui.hpp \
		/usr/local/include/opencv2/contrib/contrib.hpp \
		/usr/local/include/opencv2/contrib/retina.hpp \
		/usr/local/include/opencv2/contrib/openfabmap.hpp \
		/usr/local/include/opencv/cv.h \
		/usr/local/include/opencv2/flann/flann.hpp \
		/usr/local/include/opencv2/flann/flann_base.hpp \
		/usr/local/include/opencv2/flann/general.h \
		/usr/local/include/opencv2/flann/matrix.h \
		/usr/local/include/opencv2/flann/params.h \
		/usr/local/include/opencv2/flann/any.h \
		/usr/local/include/opencv2/flann/saving.h \
		/usr/local/include/opencv2/flann/nn_index.h \
		/usr/local/include/opencv2/flann/result_set.h \
		/usr/local/include/opencv2/flann/all_indices.h \
		/usr/local/include/opencv2/flann/kdtree_index.h \
		/usr/local/include/opencv2/flann/dynamic_bitset.h \
		/usr/local/include/opencv2/flann/dist.h \
		/usr/local/include/opencv2/flann/heap.h \
		/usr/local/include/opencv2/flann/allocator.h \
		/usr/local/include/opencv2/flann/random.h \
		/usr/local/include/opencv2/flann/kdtree_single_index.h \
		/usr/local/include/opencv2/flann/kmeans_index.h \
		/usr/local/include/opencv2/flann/logger.h \
		/usr/local/include/opencv2/flann/composite_index.h \
		/usr/local/include/opencv2/flann/linear_index.h \
		/usr/local/include/opencv2/flann/hierarchical_clustering_index.h \
		/usr/local/include/opencv2/flann/lsh_index.h \
		/usr/local/include/opencv2/flann/lsh_table.h \
		/usr/local/include/opencv2/flann/autotuned_index.h \
		/usr/local/include/opencv2/flann/ground_truth.h \
		/usr/local/include/opencv2/flann/index_testing.h \
		/usr/local/include/opencv2/flann/timer.h \
		/usr/local/include/opencv2/flann/sampling.h \
		/usr/local/include/opencv2/legacy/compat.hpp \
		/usr/local/include/opencv2/core/internal.hpp \
		/usr/local/include/opencv2/core/eigen.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Kalman.o ../BackGroundTest/Kalman.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

