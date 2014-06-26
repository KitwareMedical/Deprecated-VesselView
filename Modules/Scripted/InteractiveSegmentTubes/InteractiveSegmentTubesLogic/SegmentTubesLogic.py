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

class AbstractInteractiveSegmentTubes():
  def __init__(self):
    self._Observations = []

  def removeObservers(self, method, objectType = ''):
    for o, e, m, g, t in self._Observations:
      if objectType != '' and o.GetClassName() != objectType:
        continue

      if method == m:
        o.RemoveObserver(t)
        self._Observations.remove([o, e, m, g, t])

  def addObserver(self, object, event, method, group = 'none'):
    if self.hasObserver(object, event, method):
      print('already has observer')
      return
    tag = object.AddObserver(event, method)
    self._Observations.append([object, event, method, group, tag])

  def hasObserver(self, object, event, method):
    for o, e, m, g, t in self._Observations:
      if o == object and e == event and m == method:
        return True
    return False

  def observer(self, event, method):
    for o, e, m, g, t in self._Observations:
      if e == event and m == method:
        return o
    return None

  def removeAllObservers(self):
    for o, e, m, g, t in self._Observations:
      o.RemoveObserver(t)
      self._Observations.remove([o, e, m, g, t])

#
# Segment Tubes logic
#

class SegmentTubesLogic(AbstractInteractiveSegmentTubes):
  '''Logic used to drive the Segment Tubes cli from python.'''
  def __init__(self):
    AbstractInteractiveSegmentTubes.__init__(self)

    self.currentOutputNode = None

  def addDisplayNodes( self, node ):
    '''Add the necessary display nodes for a spatial object.
    '''
    if node:
      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.AddDisplayNodes(node)

  def getFilenameFromNode(self, node):
    '''Returns a filename for a spatial object. If no storage node exists,
       one is created and the filename will point to a file in the temp directory.
    '''
    if not node:
      return ''

    storageNode = node.GetNthStorageNode(0)
    if not storageNode or not storageNode.GetFileName():
      # Save it in temp dir
      tempPath = slicer.app.temporaryPath
      nodeName = os.path.join(tempPath, node.GetName() + '.tre')
      if os.path.isfile(nodeName):
        os.remove(nodeName)

      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.SaveSpatialObject(nodeName, node)
      return nodeName

    return storageNode.GetFileName()

  def getCLINode(self, module, nodeName = None):
    '''Return the cli node correspoding to the given module if the module isn't
       a scripted module. If a name is given, this module will be renamed accordingly
    '''
    if not nodeName:
      nodeName = module.title

    cliNode = slicer.mrmlScene.GetFirstNodeByName(nodeName)
    # Also check path to make sure the CLI isn't a scripted module
    if (cliNode == None) and ('qt-scripted-modules' not in module.path):
      cliNode = slicer.cli.createNode(module)
      cliNode.SetName(nodeName)
    return cliNode

  def run( self, run, parameters, callback ):
    '''Run the segment tubes cli with the given parameters. If run is false, the
       cli is cancelled.

       If a parameter with the name 'OutputNode' exists, this node will be updated
       to the output spatial object automatically after a succesful execution.

       The callback function can be given by the user to receive feedback on the
       segment tube cli execution. It is up to the user to stop the callback from
       listening to the cli.
    '''

    if run:
      cliNode = self.getCLINode(slicer.modules.segmenttubes)
      if callback:
        self.observeCLINode(cliNode, callback)
      self.observeCLINode(cliNode, self.loadTREFileCallback)

      try:
        self.currentOutputNode = parameters['OutputNode']
      except KeyError:
        self.currentOutputNode  = None

      cliNode = slicer.cli.run(slicer.modules.segmenttubes, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.getCLINode(slicer.modules.segmenttubes)
      if cliNode:
        cliNode.Cancel()

  def loadTREFileCallback( self, cliNode, *unused ):
    ''' ** Protected **
       Callback to automatically reload the output spatial object.
    '''
    if cliNode.GetStatusString() == 'Completed' and self.currentOutputNode:
      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.SetSpatialObject(self.currentOutputNode, cliNode.GetParameterAsString('outputTubeFile'))

  def observeCLINode(self, cliNode, onCLINodeModified = None):
    '''Utility function to observe the given cli StatusModifiedEvent.
    '''
    if cliNode != None and onCLINodeModified != None:
      self.addObserver(cliNode,
                       slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
                       onCLINodeModified)
