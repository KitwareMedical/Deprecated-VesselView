#============================================================================
#
# Copyright (c) Kitware Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#============================================================================

import imp, sys, os, unittest

from __main__ import vtk, qt, ctk, slicer

class InteractiveSegmentTubes:
  def __init__(self, parent):
    import string
    parent.title = "Interactive Segment Tubes"
    parent.categories = ["TubeTK"]
    parent.contributors = ["Johan Andruejol (Kitware)"]
    parent.helpText = string.Template("""TODO""")
    parent.acknowledgementText = """TODO"""
    #parent.icon = qt.QIcon('')
    self.parent = parent

from InteractiveSegmentTubesWidget import *
from InteractiveSegmentTubesLogic import *
