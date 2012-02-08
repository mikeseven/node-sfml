#
## This file contains proprietary software owned by Motorola Mobility, Inc. 
## No rights, expressed or implied, whatsoever to this software are provided by Motorola Mobility, Inc. hereunder. 
## 
## (c) Copyright 2011 Motorola Mobility, Inc.  All Rights Reserved.  
#

import Options
import sys
from os import unlink, symlink, popen, environ
from os.path import exists 

top='.'
srcdir = "."
blddir = "build"
VERSION = "0.1.0"

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool('node_addon')
  conf.check(lib=['sfml-system','sfml-window','sfml-graphics','sfml-audio'], uselib_store='SFML')
  #conf.check(lib='openal', uselib_store='AL')
  #conf.check(lib='sndfile', uselib_store='SNDFILE')

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "node_sfml"
  obj.source  = bld.path.ant_glob('src/*.cc')
  obj.cxxflags = ["-g"]
  obj.uselib=['SFML']
  if sys.platform.startswith('darwin'):
    framework=['OpenGL','OpenAL']
  elif sys.platform.startswith('linux'):  
    obj.ldflags = [ "-lsfml-window.2.0","-lsfml-graphics.2.0","-lsfml-system.2.0" ]

def shutdown():
  if Options.commands['clean']:
    if exists('node_sfml.node'): unlink('node_sfml.node')
  else:
    if exists('build/Release/node_sfml.node') and not exists('node_sfml.node'):
      symlink('build/Release/node_sfml.node', 'node_sfml.node')
