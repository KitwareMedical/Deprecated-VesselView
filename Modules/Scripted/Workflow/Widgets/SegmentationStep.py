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

import os
from __main__ import qt, ctk, slicer
from WorkflowStep import *
import EditorLib

class SegmentationStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(SegmentationStep, self).__init__()

    self.initialize( 'SegmentationStep' )
    self.setName( 'Segment Liver' )
    self.setDescription('Segment the liver from the image')

    self.segmentedOutputCreated = False

  def setupUi( self ):
    self.loadUi('SegmentationStep.ui')
    self.step('RegisterStep').get('RegisterOutputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                        self.get('SegmentInputNodeComboBox').setCurrentNode)
    self.get('SegmentOutputNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('SegmentSaveToolButton').icon = saveIcon
    self.get('SegmentSaveToolButton').connect('clicked()', self.saveSegmentedImage)

    self.get('SegmentGoToModulePushButton').connect('clicked()', self.openSegmentModule)

    # \todo Set up editor GUI

  def validate( self, desiredBranchId = None ):
    validSegmentation = (self.get('SegmentInputNodeComboBox').currentNode() != None and
                         self.get('SegmentOutputNodeComboBox').currentNode() != None)

    self.validateStep(validSegmentation, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(SegmentationStep, self).onEntry(comingFrom, transitionType)

    # Create output if necessary
    if not self.segmentedOutputCreated:
      self.get('SegmentInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createSegmentedOutput)
      self.segmentedOutputCreated = True
    self.createSegmentedOutput()

  def saveSegmentedImage( self ):
    self.saveFile('Segmented Image', 'VolumeFile', '.mha', self.get('SegmentOutputNodeComboBox'))

  def createSegmentedOutput( self ):
    self.createOutputIfNeeded( self.get('SegmentInputNodeComboBox').currentNode(),
                               'Segmented',
                               self.get('SegmentOutputNodeComboBox') )

    # Set segmented node as a labelmap
    segmentedNode = self.get('SegmentOutputNodeComboBox').currentNode()
    if segmentedNode != None:
      volumesLogic = slicer.modules.volumes.logic()
      volumesLogic.SetVolumeAsLabelMap(segmentedNode, 1)

  def openSegmentModule( self ):
    self.openModule('Editor')

    editorWidget = slicer.modules.editor.widgetRepresentation()
    masterVolumeNodeComboBox = editorWidget.findChild('qMRMLNodeComboBox')
    masterVolumeNodeComboBox.addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)
    masterVolumeNodeComboBox.setCurrentNode(self.get('SegmentOutputNodeComboBox').currentNode())
    setButton = editorWidget.findChild('QPushButton')
    setButton.click()

    # \todo Set up editor mode too ?
