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
    parent.helpText = """
    <p>This module is an interactive gui wrapping around the <i>Segment Tubes</i>
    module. It automatically processes the seeds points, allowing the user to 
    click its way through an image without having to worry about running the 
    <i>Segment Tubes</i> CLI by hand.

    <p>To use, simply specify the <b>Input image</b>, the <b>Output tube</b> and 
    the <b>Seed points</b> list. The seed list is the list that will contains 
    all the seeds to process. Click the <b>Start</b> button to start 
    processing seeds as they come.\n
    <p>Each new markup added to the list will be queued ('Queued' status) for 
    processing. Once the <i>Segment Tubes</i> CLI processed the queued seeds, 
    their status will change to 'Processed'. The new vessels will be merged to 
    the <b>Output tube</b>."""
    parent.acknowledgementText = """"""
    #parent.icon = qt.QIcon('')
    self.parent = parent

from InteractiveSegmentTubesWidget import *
from InteractiveSegmentTubesLogic import *
