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
import Editor

class SegmentationStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(SegmentationStep, self).__init__()

    self.initialize( 'SegmentationStep' )
    self.setName( 'Segment Liver' )
    self.setDescription('Segment the liver from the image')

    self.segmentedOutputCreated = False
    self.MergeVolume = None
    self.MergeVolumeValid = False

  def setupUi( self ):
    self.loadUi('SegmentationStep.ui')
    self.get('SegmentMergeNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('SegmentSaveToolButton').icon = saveIcon
    self.get('SegmentSaveToolButton').connect('clicked()', self.saveSegmentedImage)

    self.get('SegmentGoToModulePushButton').connect('clicked()', self.openSegmentModule)

    # Set up editor GUI
    placeHolderWidget = self.get('SegmentEditorPlaceHolderWidget')
    self.EditorWidget = Editor.EditorWidget(parent=placeHolderWidget)
    self.EditorWidget.setup()
    placeHolderWidget.show()

    # Hide the editor inputs
    createAndSelectCollapsibeButton = placeHolderWidget.findChild('ctkCollapsibleButton')
    createAndSelectCollapsibeButton.setVisible(False)

    self.get('SegmentMasterNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                 self.EditorWidget.setMasterNode)

    # Overload the setMRMLScene to catch the merge volume
    self.addObserver(slicer.mrmlScene, slicer.mrmlScene.NodeAddedEvent, self.onNodeAddedEvent)
    self.get('SegmentMergeNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                  self.onMergeVolumeSelected)

  def validate( self, desiredBranchId = None ):
    validSegmentation = (self.get('SegmentMasterNodeComboBox').currentNode() != None
                         and self.MergeVolumeValid)

    self.get('SegmentSaveToolButton').enabled = validSegmentation

    self.validateStep(validSegmentation, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(SegmentationStep, self).onEntry(comingFrom, transitionType)

    # Set master volume OnEntry() so the pop up windows doesnt bother the user too much
    self.get('SegmentMasterNodeComboBox').setCurrentNode(
      self.step('ResampleStep').getResampledVolume1())

  def saveSegmentedImage( self ):
    self.saveFile('Segmented Image', 'VolumeFile', '.mha', self.get('SegmentMergeNodeComboBox'))

  def openSegmentModule( self ):
    self.openModule('Editor')

    editorWidget = slicer.modules.editor.widgetRepresentation()
    masterVolumeNodeComboBox = editorWidget.findChild('qMRMLNodeComboBox')
    masterVolumeNodeComboBox.setCurrentNode(self.get('SegmentMasterNodeComboBox').currentNode())
    #setButton = editorWidget.findChild('QPushButton')
    #setButton.click()

    # \todo Set up editor mode too ?

  def onNodeAddedEvent( self, scene, event ):
    if not scene:
      return

    masterVolume = self.get('SegmentMasterNodeComboBox').currentNode()
    if not masterVolume:
      return

    # Look for the node with potentialy the correct name in the scene.
    mergeVolumeName = '%s-label' % masterVolume.GetName()
    nodes = scene.GetNodesByClass('vtkMRMLScalarVolumeNode')
    for i in range(0, nodes.GetNumberOfItems()):
      volumeNode = nodes.GetItemAsObject(i)

      if volumeNode and self.startsWith(volumeNode.GetName(), mergeVolumeName):
        self.get('SegmentMergeNodeComboBox').setCurrentNode(volumeNode)
        break

  def startsWith( self, string, stringStart ):
    return string.find(stringStart) == 0

  def isMergeVolumeValid( self, volumeNode ):
    if not volumeNode or not volumeNode.GetImageData():
      return False

    # Segmentation is valid if it has at least 2 labels
    range = volumeNode.GetImageData().GetScalarRange()
    return range[0] != range[1]

  def onMergeVolumeSelected( self, mergeVolume ):
    if self.MergeVolume:
      self.removeObservers(self.onMergeVolumeModified)

    self.MergeVolume = mergeVolume
    if self.MergeVolume:
      self.EditorWidget.setMergeNode(self.MergeVolume)
      self.addObserver(self.MergeVolume, 'ModifiedEvent', self.onMergeVolumeModified)

    self.onMergeVolumeModified(self.MergeVolume)

  def onMergeVolumeModified( self, mergeVolume, event = 'ModifiedEvent' ):
    if not mergeVolume:
      return

    self.MergeVolumeValid = self.isMergeVolumeValid(mergeVolume)
    if self.MergeVolumeValid:
      self.removeObservers(self.onMergeVolumeModified)
    self.validate()
