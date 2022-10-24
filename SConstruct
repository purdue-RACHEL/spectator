#fbtest.cpp SConstruct file
# EXAMPLE CALL:
# To build projector you would type
# scons --build=Projector

import os

# Set up build option parameter
AddOption('--build',
        dest='buildOption',
        type='string',
        nargs=1,
        action='store',
        help='Options: GameLoop, Uart, Camera, Projector')

AddOption('--device',
        dest='deviceOption',
        type='string',
        nargs=1,
        action='store',
        help='Options: rachel, micah')

# Defined directories
BUILDDIR='/home/rachel/git/spectator/build/'
SRCDIR='/home/rachel/git/spectator/src/'

# Set up enivronment
env = Environment(CCFLAGS='-fpermissive -I/home/rachel/git/spectator/inc/OpenNI2/Include -I/usr/local/include/opencv4', 
        LIBS=['OpenNI2', 'opencv_core', 'opencv_features2d', 'opencv_imgcodecs', 'opencv_highgui', 'opencv_video', 'opencv_imgproc', 'opencv_videoio', 'opencv_optflow'], LIBPATH=['/home/rachel/git/spectator', '/usr/local/lib'],ENV = os.environ)

DEVOPTION = GetOption('deviceOption')
if DEVOPTION == "micah":
    # reset environment paths
    BUILDDIR='/home/micah/spectator/build/'
    SRCDIR='/home/micah/spectator/src/'
    env = Environment(CCFLAGS='-fpermissive', 
        LIBS=[], LIBPATH=[],ENV = os.environ)

VariantDir(BUILDDIR,SRCDIR)

# Add callback command to run and clean if build is successful
RUN = env.Command(target = 'runme', source=BUILDDIR+'runme', action=[BUILDDIR+'runme','rm -rf '+BUILDDIR,'mkdir '+BUILDDIR])
env.AlwaysBuild(RUN)

# Build program depending on build option parameter
BUILDOPTION = GetOption('buildOption')
if BUILDOPTION == None:
    print('Specify a build option using --build')
    print('Options: GameLoop, Uart, Camera, Projector')
    exit()
elif BUILDOPTION == 'Uart':
    # Add test flags that surround the main function you want to run
    env.Replace(CPPDEFINES = 'TESTUART')
    # Add all the cpp files needed. Keep the executable name 'runme' the same
    env.Program(BUILDDIR+'runme', [BUILDDIR+'UartDecoder.cpp'])
elif BUILDOPTION == 'GameLoop':
    env.Replace(CPPDEFINES = ['TESTGAMELOOP', 'DISABLEOPENCV'])
    env.Program(BUILDDIR+'runme', [BUILDDIR+'GameLoop.cpp',BUILDDIR+'UartDecoder.cpp',BUILDDIR+'Projector.cpp'])
elif BUILDOPTION == 'Projector':
    env.Replace(CPPDEFINES = 'TESTPROJECTOR')
    env.Program(BUILDDIR+'runme', [BUILDDIR+'Projector.cpp',BUILDDIR+'UartDecoder.cpp'])
elif BUILDOPTION == 'Camera':
    env.Replace(CPPDEFINES = '')
    env.Program(BUILDDIR+'runme', [BUILDDIR+'cameraBlob.cpp',BUILDDIR+'CameraInterface.cpp', BUILDDIR+'ColorTracker.cpp'])
elif BUILDOPTION == 'ColorPicker':
    env.Replace(CPPDEFINES = '')
    env.Program(BUILDDIR+'runme', [BUILDDIR+'colorPicker.cpp',BUILDDIR+'CameraInterface.cpp'])

