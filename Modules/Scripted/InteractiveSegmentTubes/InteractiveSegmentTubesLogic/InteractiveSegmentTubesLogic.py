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

from SegmentTubesLogic import *

#
# Interactive Segment Tubes logic
#

class InteractiveSegmentTubesLogic(SegmentTubesLogic):
  """TODO"""
  def __init__(self):
    SegmentTubesLogic.__init__(self)

    self.todoMarkup = None
    self.segmentTubesCLI = None
    self.seedNode = None
    self.inputNode = None
    # No output node, use superclass currentOutputNode
    self.guiCallback = None
    self.status = 'Idle'

    self.started = False
    self.noProcessing = False
    self.queue = []
    self.processing = []

  # Re-implementation of Segment Tubes Logic
  #
  def getCLINode( self, *unused ):
    ''' Re-implement getCLINode so it returns a segment tubes cli setup with
        the correct autorun.
    '''
    if not self.segmentTubesCLI:
      self.segmentTubesCLI = SegmentTubesLogic.getCLINode(self,
        slicer.modules.segmenttubes, 'Interactive Segment Tubes CLI')

    self.segmentTubesCLI.SetAutoRunMode(self.segmentTubesCLI.AutoRunOnAnyInputEvent)
    self.status = self.segmentTubesCLI.GetStatusString()
    return self.segmentTubesCLI

  def loadTREFileCallback( self, cliNode, *unused ):
    ''' Re-implement loadTREFileCallback to use the logic merge function instead
        of replacing the spatial object.
    '''
    if cliNode.GetStatusString() == 'Completed' and self.currentOutputNode:
      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.MergeSpatialObjectFromFilename(
        self.currentOutputNode, cliNode.GetParameterAsString('outputTubeFile'))

  # Original methods
  #

  def createTodoMarkup( self ):
    todoMarkup = slicer.mrmlScene.AddNode(slicer.vtkMRMLMarkupsFiducialNode())
    todoMarkup.SetHideFromEditors(1)
    todoMarkup.SetName('Interactive Segment Tubes To Do list')
    todoMarkup.SetDisplayVisibility(0)
    # Set None as an active list otherwise the user might add fiducial to todoMarkup
    self.setActivePlaceNodeID(None)

    return todoMarkup

  def getTodoMarkup( self ):
    if not self.todoMarkup:
      self.todoMarkup = self.createTodoMarkup()
    return self.todoMarkup

  def setActivePlaceNodeID( self, node ):
    appLogic = slicer.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    selectionNode.SetActivePlaceNodeID(node.GetID() if node else None)

  def setGUICallback( self, callback ):
    self.run(False)
    self.guiCallback = callback

  def setInputNode( self, node ):
    self.inputNode = node
    self.updateCLINode()

  def setOutputNode( self, node ):
    self.currentOutputNode = node
    self.updateCLINode()

  def setSeedNode( self, node ):
    if not node or node == self.seedNode:
      return

    self.removeObservers(self.queueSeeds)
    self.seedNode = node

    # Select it
    self.setActivePlaceNodeID(self.seedNode)

    # Lock it
    self.seedNode.SetLocked(1)

    # Setup selected / unselect colors
    displayNode = self.seedNode.GetDisplayNode()
    if displayNode:
      displayNode.SetColor(0, 1, 0) # Processed is green (unselected)
      displayNode.SetSelectedColor(1, 0, 0) # To Do is red

    self.addObserver(self.seedNode, self.seedNode.PointModifiedEvent, self.queueSeeds)
    self.addObserver(self.seedNode, self.seedNode.NthMarkupModifiedEvent, self.queueSeeds)
    self.queueSeeds()
    self.updateCLINode()

  def updateCLINode( self ):
    self.run(False)
    slicer.cli.setNodeParameters(self.getCLINode(), self.segmentTubesParameters())

  def run( self, run, *unused ):
    '''Re-implement run method here to allow to use autorun correctly.
    '''

    self.started = run
    cliNode = self.getCLINode()
    if run:
      self.observeCLINode(cliNode, self.onSegmentTubesUpdated)
      self.observeCLINode(cliNode, self.loadTREFileCallback)
    else:
      cliNode.Cancel()

    wasRunning = cliNode.GetAutoRun()
    cliNode.SetAutoRun(run)
    # Kickstart the cli if it's the first time it's started
    if not wasRunning and run:
      self.addQueuedSeedToTodoMarkup()

  def segmentTubesParameters( self ):
    parameters = {}
    parameters['inputVolume'] = self.inputNode
    parameters['OutputNode'] = self.currentOutputNode
    parameters['outputTubeFile'] = self.getFilenameFromNode(parameters['OutputNode'])
    parameters['seedP'] = self.getTodoMarkup()

    return parameters

  def onSegmentTubesUpdated( self, cliNode, event ):
    if not cliNode:
      return

    self.status = cliNode.GetStatusString()

    if not self.isCLIRunning():
      self.reportSeeds()

    if self.status == 'Cancelled':
      self.removeObservers(self.onSegmentTubesUpdated)
      self.removeObservers(self.loadTREFileCallback)

    if self.guiCallback:
      self.guiCallback(cliNode, event)

  def isCLIRunning( self ):
    return (self.status == 'Scheduled'
      or self.status == 'Running'
      or self.status == 'Cancelling'
      or self.status == 'Completing')

  def isInTransitionState( self ):
    return (self.status == 'Scheduled'
      or self.status == 'Cancelling'
      or self.status == 'Completing')

  def isSeedMarked( self, label ):
    return label == 'Processed' or label == 'Queued'

  def updateMarkup( self, node, index, processed ):
    if not node:
      return

    if processed:
      node.SetNthMarkupLabel(index, 'Processed')
    else:
      node.SetNthMarkupLabel(index, 'Queued')
    self.seedNode.SetNthMarkupSelected(index, not processed)

  def queueSeeds( self, *unused ):
    if self.noProcessing:
      return

    if not self.seedNode:
      self.queue = []
      return

    self.noProcessing = True
    for i in range(self.seedNode.GetNumberOfMarkups()):
      label = self.seedNode.GetNthMarkupLabel(i)
      if self.isSeedMarked(label):
        continue

      point = [0.0, 0.0, 0.0, 0.0]
      self.seedNode.GetNthFiducialWorldCoordinates(i, point)
      self.updateMarkup(self.seedNode, i, False)
      self.queue.append(point)
    self.noProcessing = False

    self.addQueuedSeedToTodoMarkup()

  def canAddSeedForProcessing( self ):
    return self.started and (not self.isCLIRunning()) and (not self.noProcessing)

  def addQueuedSeedToTodoMarkup( self ):
    if not self.canAddSeedForProcessing():
      return

    wasModifying = self.getTodoMarkup().StartModify()
    for seed in self.queue:
      self.getTodoMarkup().AddFiducial(0.0, 0.0, 0.0)
      self.getTodoMarkup().SetNthFiducialWorldCoordinates(
          self.getTodoMarkup().GetNumberOfMarkups() - 1, seed)

      self.processing.append(seed)
    self.queue = []

    self.getTodoMarkup().EndModify(wasModifying)

  def reportSeeds( self ):
    if not self.seedNode:
      return

    self.noProcessing = True

    # Clear to do markup. Ignore the modified event it triggers to prevent from
    # re-running the CLI.
    wasRunning = self.getCLINode().GetAutoRun()
    self.getCLINode().SetAutoRun(False)
    self.getTodoMarkup().RemoveAllMarkups()
    self.getCLINode().SetAutoRun(wasRunning)

    for i in range(self.seedNode.GetNumberOfMarkups()):
      label = self.seedNode.GetNthMarkupLabel(i)
      if label != 'Queued':
        continue

      point = [0.0, 0.0, 0.0, 0.0]
      self.seedNode.GetNthFiducialWorldCoordinates(i, point)

      for seed in self.processing:
        if self.comparePoint(point, seed):
          self.updateMarkup(self.seedNode, i, True)

    self.processing = []
    self.noProcessing = False

    self.addQueuedSeedToTodoMarkup()

  def comparePoint( self, p1, p2 ):
    if (len(p1) != len(p2)):
      return False

    samePoint = True
    for k in range(len(p1)):
      samePoint = samePoint and (abs(p1[k] - p2[k]) < 1e-6)
    return samePoint
