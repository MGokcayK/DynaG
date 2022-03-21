#
# DynaG setup.
# 
# Author : @MGokcayK
# C.Date : 19/03/2022
# Update : 19/03/2022 - Initialization & Implementation // @MGokcayK

import os
import sys
import subprocess
from pathlib import Path

from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext

def findOS():
        if sys.platform == 'win32':
            return True
        else:
            return False

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])
        

class BuildCMakeExt(build_ext):

    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, ext):
        org_path = os.getcwd()
        os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'dynag'))

        shell = findOS()
    
        print("Creating build folder..")
        if os.path.exists('build'):
            path = os.path.join(os.getcwd(), 'build')
            if shell:
                os.system('rmdir /S /Q "{}"'.format(path))
            else:
                os.system('rm -r build')
        
        os.makedirs('build', exist_ok=True)
        os.chdir('build')

        print("Building APIs..")
        if shell:
            subprocess.check_call('cmake ..', shell=shell)
            subprocess.check_call('cmake --build . --config Release', shell=shell)
        else:
            subprocess.run(['cmake', '-G', 'Unix Makefiles', '-DCMAKE_BUILD_TYPE=Release', '..'])
            subprocess.run(['make'])

        os.chdir(org_path)


this_directory = Path(__file__).parent
setup(name='DynaG',
    version='0.0.0.3',
    description='Dynamic System solver for reinforcement learning environment. It solves ODE with $4^{th}$ order Runge-Kutta methods. ',
    long_description = (this_directory / "README.md").read_text(),
    long_description_content_type='text/markdown',
    url='https://github.com/MGokcayK/DynaG',
    author='Mehmet Gökçay Kabataş, Uğurcan Özalp',
    author_email='mgokcaykdev@gmail.com',
    license='MIT',
    classifiers=[
      'License :: OSI Approved :: MIT License',
      'Development Status :: 2 - Pre-Alpha',
      'Intended Audience :: Science/Research',
      'Programming Language :: Python :: 3.6',
      'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    ext_modules=[CMakeExtension('.')],
    cmdclass={
          'build_ext': BuildCMakeExt,
    },
    install_requires=['numpy', 'gym','cmake'],
    python_requires='>=3.6.5',
    packages = ['.'],
    package_dir={'.': ''},
    include_package_data=True,
    zip_safe=False
    )
