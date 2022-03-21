#!/usr/bin/python3
import os 
import sys

packagedir = os.path.dirname(os.path.abspath(__file__))
os.environ['DYNAG_LIBS_DIR'] = os.path.join(packagedir, 'libs')
os.environ['DYNAG_PYTHON_DIR'] = os.path.join(packagedir, 'bin')
os.environ['DYNAG_RESOURCE_DIR'] = os.path.join(packagedir, 'resources')    

if sys.platform == 'win32':
    os.environ['PATH'] += os.pathsep + os.environ['DYNAG_LIBS_DIR']
    os.environ['PATH'] += os.pathsep + os.environ['DYNAG_PYTHON_DIR']
    os.environ['PATH'] += os.pathsep + os.environ['DYNAG_RESOURCE_DIR']

elif sys.platform == 'linux':
    if 'LD_LIBRARY_PATH' not in os.environ:
        os.environ['LD_LIBRARY_PATH']  = ""
    
    if os.environ['DYNAG_LIBS_DIR'] not in os.environ["LD_LIBRARY_PATH"]:
        os.environ['LD_LIBRARY_PATH'] += os.pathsep + os.environ['DYNAG_LIBS_DIR']
    
    if os.environ['DYNAG_PYTHON_DIR'] not in os.environ["LD_LIBRARY_PATH"]:
        os.environ['LD_LIBRARY_PATH'] += os.pathsep + os.environ['DYNAG_PYTHON_DIR']
    
    if os.environ['DYNAG_RESOURCE_DIR'] not in os.environ["LD_LIBRARY_PATH"]:
        os.environ['LD_LIBRARY_PATH'] += os.pathsep + os.environ['DYNAG_RESOURCE_DIR']