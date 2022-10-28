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

# Define build options
BUILDOPTS = ['GameLoop', 'Uart', 'Camera', 'Projector', 'ColorPicker']

# create empty dictionary to store buildflags and arguments for env.Program()
BUILDDICT = {opt: {'defs': list(), 'args': list()} for opt in BUILDOPTS}

BUILDDICT['GameLoop'] = {
    'defs': ['TESTGAMELOOP', 'DISABLEOPENCV'],
    'args': [BUILDDIR+'GameLoop.cpp',BUILDDIR+'UartDecoder.cpp',BUILDDIR+'Projector.cpp']
    }
BUILDDICT['Uart'] = {
    'defs': ['TESTUART'],
    'args': [BUILDDIR+'UartDecoder.cpp']
    }
BUILDDICT['Projector'] = {
    'defs': ['TESTPROJECTOR'],
    'args': [BUILDDIR+'Projector.cpp',BUILDDIR+'UartDecoder.cpp']
    }
BUILDDICT['Camera'] = {
    'defs': [''],
    'args': [BUILDDIR+'cameraBlob.cpp',BUILDDIR+'CameraInterface.cpp', BUILDDIR+'ColorTracker.cpp', BUILDDIR+'ContourTracker.cpp']
    }
BUILDDICT['ColorPicker'] = {
    'defs': [''],
    'args': [BUILDDIR+'colorPicker.cpp',BUILDDIR+'CameraInterface.cpp', BUILDDIR+'Projector.cpp', BUILDDIR+'UartDecoder.cpp']
    }

# Build program depending on build option parameter
BUILDOPTION = GetOption('buildOption')
if BUILDOPTION == None:
    print('Specify a build option using --build')
    print('Options: ', end = None)
    print(BUILDOPTS, sep=', ')
    exit()
else:
    env.Replace(CPPDEFINES = BUILDDICT[BUILDOPTION]['defs'])
    env.Program(BUILDDIR+'runme', BUILDDICT[BUILDOPTION]['args'])

