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
import EditorLib
from EditorLib import EditUtil

class SegmentationStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(SegmentationStep, self).__init__()

    self.initialize( 'SegmentationStep' )
    self.setName( 'Segment Liver' )
    self.setDescription('Segment the liver from the image')

    self.MergeVolume = None
    self.MergeVolumeValid = False
    self.EditUtil = EditUtil.EditUtil()
    self.AdditionalVolume = None

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
                                                 self.setMasterNode)

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

    # Add observer on the PDF segmenter CLI
    pdfSegmenterCLINode = self.getCLINode(slicer.modules.segmentconnectedcomponentsusingparzenpdfs, 'PDFSegmenterEditorEffect')
    self.observeCLINode(pdfSegmenterCLINode, self.onPDFSegmenterCLIModified)

    # Get the previous step's node
    self.AdditionalVolume = self.step('ResampleStep').getResampledVolume2()
    # Set it to the parameter node
    self.setParameterToPDFSegmenter('additionalInputVolumeID0', self.AdditionalVolume.GetID() if self.AdditionalVolume != None else '0')

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
    nodes.SetReferenceCount(nodes.GetReferenceCount() - 1)
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

  def onPDFSegmenterCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      # Change the background label to 0
      objectColors = cliNode.GetParameterAsString('objectId')
      backgroundColor = eval(objectColors)[1] # object colors is 'foreground, background'

      # Set the parameter node necesseray for the change label logic
      parameterNode = self.EditUtil.getParameterNode()
      parameterNode.SetParameter("ChangeLabelEffect,inputColor", str(backgroundColor))
      parameterNode.SetParameter("ChangeLabelEffect,outputColor", '0')

      # Apply change label
      changeLabelLogic = EditorLib.ChangeLabelEffectLogic(self.EditUtil.getSliceLogic())
      changeLabelLogic.changeLabel()

  def updateConfiguration( self, config ):
    organ = config['Organ'].lower()
    self.get('SegmentCollapsibleGroupBox').setTitle('Segment %s' % organ)
    self.get('SegmentMasterNodeLabel').setText('Input %s' % config['Volume1Name'].lower())
    self.get('SegmentMergeVolumeLabel').setText('Segmented %s image' % organ)

    self.setName( 'Segment %s' % organ )
    self.setDescription('Segment the %s from the image' % organ)

  def getPDFSegmenterParameterName( self, parameterName ):
    return 'InteractiveConnectedComponentsUsingParzenPDFsOptions,' + parameterName

  def getParameterFromPDFSegmenter( self, parameterName ):
    parameterNode = self.EditUtil.getParameterNode()
    return parameterNode.GetParameter(self.getPDFSegmenterParameterName(parameterName))

  def setParameterToPDFSegmenter( self, parameterName, value ):
    parameterNode = self.EditUtil.getParameterNode()
    parameterNode.SetParameter(self.getPDFSegmenterParameterName(parameterName), value)

  def getMaskImageObjectId( self ):
    objectIds = self.getParameterFromPDFSegmenter('objectId')
    if objectIds:
      return eval(objectIds)[0]
    else:
      return 0

  def getVolume2( self ):
    return self.AdditionalVolume

  def setMasterNode( self, node ):
    self.EditorWidget.setMasterNode(node)
    self.setViews(node, None, self.get('SegmentMergeNodeComboBox').currentNode())
