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

  # Re-implementation of Segment Tubes Logic
  #
  def getCLINode( self, *unused ):
    ''' Re-implement getCLINode so it returns a segment tubes cli setup with
        the correct autorun.
    '''
    if not self.segmentTubesCLI:
      self.segmentTubesCLI = SegmentTubesLogic.getCLINode(self,
        slicer.modules.segmenttubes, 'Interactive Segment Tubes CLI')

    self.segmentTubesCLI.SetAutoRunMode(self.segmentTubesCLI.AutoRunEnabledMask)
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
  def createToDoMarkup( self ):
    todoMarkup = slicer.mrmlScene.AddNode(slicer.vtkMRMLMarkupsFiducialNode())
    todoMarkup.SetHideFromEditors(1)
    todoMarkup.SetName('Interactive Segment Tubes To Do list')
    todoMarkup.SetDisplayVisibility(0)
    # Set None as an active list otherwise the user might add fiducial to todoMarkup
    self.setActivePlaceNodeID(None)
    return todoMarkup

  def getToDoMarkup( self ):
    if not self.todoMarkup:
      self.todoMarkup = self.createToDoMarkup()
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

    self.removeObservers(self.updateToDoMarkup)
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

    self.addObserver(self.seedNode, self.seedNode.PointModifiedEvent, self.updateToDoMarkup)
    self.addObserver(self.seedNode, self.seedNode.NthMarkupModifiedEvent, self.updateToDoMarkup)
    self.updateToDoMarkup()
    self.updateCLINode()

  def updateCLINode( self ):
    self.run(False)
    slicer.cli.setNodeParameters(self.getCLINode(), self.segmentTubesParameters())

  def run( self, run, *unused ):
    '''Re-implement run method here to allow to use autorun correctly.
    '''

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
      self.sendToDoModifiedEventIfNecessary()

  def segmentTubesParameters( self ):
    parameters = {}
    parameters['inputVolume'] = self.inputNode
    parameters['OutputNode'] = self.currentOutputNode
    parameters['outputTubeFile'] = self.getFilenameFromNode(parameters['OutputNode'])
    parameters['seedP'] = self.getToDoMarkup()

    return parameters

  def onSegmentTubesUpdated( self, cliNode, event ):
    if not cliNode:
      return

    self.status = cliNode.GetStatusString()

    if not self.isRunning():
      self.updateSeedNode()

    if self.status == 'Cancelled':
      self.removeObservers(self.onSegmentTubesUpdated)
      self.removeObservers(self.loadTREFileCallback)

    if self.guiCallback:
      self.guiCallback(cliNode, event)

  def sendToDoModifiedEventIfNecessary( self ):
    if self.getToDoMarkup().GetNumberOfMarkups() > 0:
      self.getToDoMarkup().Modified()

  def isRunning( self ):
    return (self.status == 'Scheduled'
      or self.status == 'Running'
      or self.status == 'Cancelling'
      or self.status == 'Completing')

  def isInTransitionState( self ):
    return (self.status == 'Scheduled'
      or self.status == 'Cancelling'
      or self.status == 'Completing')

  def updateMarkup( self, node, index, processed ):
    if not node:
      return

    if processed:
      node.SetNthMarkupLabel(index, 'Processed')
    else:
      node.SetNthMarkupLabel(index, 'Queued')
    self.seedNode.SetNthMarkupSelected(index, not processed)

  def updateToDoMarkup( self, *unused ):
    if not self.seedNode:
      return

    for i in range(self.seedNode.GetNumberOfMarkups()):
      label = self.seedNode.GetNthMarkupLabel(i)
      if label == 'Processed' or label == 'Queued':
        continue

      self.updateMarkup(self.seedNode, i, False)

      point = [0.0, 0.0, 0.0, 0.0]
      self.seedNode.GetNthFiducialWorldCoordinates(i, point)

      self.getToDoMarkup().AddFiducial(0.0, 0.0, 0.0)
      self.getToDoMarkup().SetNthFiducialWorldCoordinates(
        self.getToDoMarkup().GetNumberOfMarkups() - 1, point)

  def updateSeedNode( self ):
    if not self.seedNode:
      return

    for i in range(self.seedNode.GetNumberOfMarkups()):
      if self.seedNode.GetNthMarkupLabel(i) != 'Queued':
        continue

      point = [0.0, 0.0, 0.0, 0.0]
      self.seedNode.GetNthFiducialWorldCoordinates(i, point)

      for j in range(self.getToDoMarkup().GetNumberOfMarkups()):
        todoPoint = [0.0, 0.0, 0.0, 0.0]
        self.getToDoMarkup().GetNthFiducialWorldCoordinates(j, todoPoint)

        if self.comparePoint(todoPoint, point):
          self.updateMarkup(self.seedNode, i, True)

          self.getToDoMarkup().DisableModifiedEventOn()
          self.getToDoMarkup().RemoveMarkup(j)
          self.getToDoMarkup().DisableModifiedEventOff()

    self.sendToDoModifiedEventIfNecessary()

  def comparePoint( self, p1, p2 ):
    if (len(p1) != len(p2)):
      return False

    samePoint = True
    for k in range(len(p1)):
      samePoint = samePoint and (abs(p1[k] - p2[k]) < 1e-6)
    return samePoint
