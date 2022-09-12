#fbtest.cpp SConstruct file

env = Environment(CCFLAGS='-fpermissive -I/home/rachel/git/spectator/inc/OpenNI2/Include -I/usr/local/include/opencv4', 
	LIBS=['OpenNI2', 'opencv_core', 'opencv_features2d', 'opencv_imgcodecs', 'opencv_highgui'], LIBPATH=['/home/rachel/git/spectator', '/usr/local/lib'])
env.Program(['testBlob.cpp'])
env.Program(['cameraBlob.cpp'])
