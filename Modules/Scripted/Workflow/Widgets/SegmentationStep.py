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
import SegmentationWidget

class SegmentationStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(SegmentationStep, self).__init__()

    self.initialize( 'SegmentationStep' )
    self.setName( 'Segment Liver' )
    self.setDescription('Segment the liver from the image')

    self.SegmentWidgets = []
    self.createMergeAllOutputConnected = False

  def setupUi( self ):
    self.loadUi('SegmentationStep.ui')

    placeHolderWidget = self.get('SegmentEditorPlaceHolderWidget')
    self.EditorWidget = Editor.EditorWidget(parent=placeHolderWidget)
    self.EditorWidget.setup()
    placeHolderWidget.hide()

    for i in range(2):
      newSegmentWidget = SegmentationWidget.SegmentationWidget(self, self.EditorWidget)
      self.get('SegmentPlaceHolderWidget').layout().addWidget(newSegmentWidget)
      newSegmentWidget.setMRMLScene(slicer.mrmlScene)
      newSegmentWidget.setSegmentValidCallBack(getattr(self, 'onSegment%iValid' %(i+1)))
      self.SegmentWidgets.append(newSegmentWidget)

      if i > 0:
        self.SegmentWidgets[i].collapse(True)
        self.SegmentWidgets[i].visible = False
        self.SegmentWidgets[i].MergeNodeSuffix = 'tumor'

    self.get('SegmentRemoveSegmentWidgetToolButton').connect('clicked()', self.removeSegmentWidget)
    self.get('SegmentAddSegmentWidgetToolButton').connect('clicked()', self.addSegmentWidget)  

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('MergeAllOutputSaveToolButton').icon = saveIcon
    self.get('MergeAllSaveToolButton').icon = saveIcon
    self.get('MergeAllSaveToolButton').connect('clicked()', self.saveMergedImage)

    self.get('MergeAllGoToButton').connect('clicked()', self.openImageLabelCombineModule)
    self.get('MergeAllApplyButton').connect('clicked(bool)', self.runMergeAll)
    self.get('MergeAllOutputNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode','LabelMap','1')
    
    self.get('MergeAllCollapsibleGroupBox').visible = False
    self.get('MergeAllCollapsibleGroupBox').setChecked(False)

  def onSegment1Valid( self ):
    if not self.SegmentWidgets[1].visible:
      self.validate()
    else:
      self.SegmentWidgets[0].collapse(True)
      self.SegmentWidgets[1].collapse(False)
      self.setViews(
        self.SegmentWidgets[1].getMasterNode(),
        None,
        self.SegmentWidgets[1].getMergeNode())

  def onSegment2Valid( self ):
    self.SegmentWidgets[1].collapse(True)
    self.get('MergeAllCollapsibleGroupBox').setChecked(True)
    self.createMergeAllOutput(self.SegmentWidgets[1].getMergeNode())
    self.setViews(
      self.SegmentWidgets[0].getMergeNode(), self.SegmentWidgets[1].getMergeNode())

  def validate( self, desiredBranchId = None ):
    validSegmentation = True
    for widget in self.SegmentWidgets:
      if widget.visible:
        validSegmentation = validSegmentation and widget.IsSegmentationValid()

    if self.get('MergeAllCollapsibleGroupBox').visible:
      cliNode = self.getCLINode(slicer.modules.imagelabelcombine)
      validSegmentation = validSegmentation and (cliNode.GetStatusString() == 'Completed')

      self.get('MergeAllOutputSaveToolButton').enabled = validSegmentation
      self.get('MergeAllSaveToolButton').enabled = validSegmentation

    self.validateStep(validSegmentation, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(SegmentationStep, self).onEntry(comingFrom, transitionType)

    # Set master volume OnEntry() so the pop up windows doesnt bother the user too much
    self.SegmentWidgets[0].setMasterNode(self.step('ResampleStep').getResampledNode(0))
    self.setViews(
      self.SegmentWidgets[0].getMasterNode(),
      None,
      self.SegmentWidgets[0].getMergeNode())

    for widget in self.SegmentWidgets:
      widget.updateUndoRedoEnabled()
      widget.updateParameterNodeFromGUI()

  def saveMergedImage( self ):
    self.saveFile('Merged Image', 'VolumeFile', '.mha', self.get('MergeAllOutputNodeComboBox'))

  def createMergeAllOutput( self, node ):
    self.createOutputIfNeeded(node, 'merged', self.get('MergeAllOutputNodeComboBox'))

  def openImageLabelCombineModule( self ):
    self.openModule('ImageLabelCombine')

    cliNode = self.getCLINode(slicer.modules.imagelabelcombine)
    parameters = self.imageLabelCombineParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def imageLabelCombineParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.imagelabelcombine)
    parameters['InputLabelMap_A'] = self.SegmentWidgets[0].getMergeNode()
    parameters['InputLabelMap_B'] = self.SegmentWidgets[1].getMergeNode()
    parameters['OutputLabelMap'] = self.get('MergeAllOutputNodeComboBox').currentNode()
    parameters['FirstOverwrites'] = False
    return parameters

  def onImageLabelCombineModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.setViews(
        self.SegmentWidgets[0].getMasterNode(), self.getMergeNode())
      self.validate()

    if not cliNode.IsBusy():
      self.get('MergeAllApplyButton').setChecked(False)
      self.get('MergeAllApplyButton').enabled = True
      print 'Merge all %s' % cliNode.GetStatusString()
      self.removeObservers(self.onImageLabelCombineModified)

  def updateConfiguration( self, config ):
    for widget in self.SegmentWidgets:
      widget.updateConfiguration(config)

    organLower = config['Organ'].lower()
    self.setName( 'Segment %s' % organLower )
    self.setDescription('Segment the %s from the image' % organLower)

  def addSegmentWidget( self ):
    self.SegmentWidgets[1].visible = True
    self.SegmentWidgets[1].setMasterNode(self.step('ResampleStep').getResampledNode(0))
    shouldCollapseFirstWidget = self.SegmentWidgets[0].IsSegmentationValid()
    self.SegmentWidgets[0].collapse(shouldCollapseFirstWidget)
    self.SegmentWidgets[1].collapse(not shouldCollapseFirstWidget)
    self.get('MergeAllCollapsibleGroupBox').visible = True

    visibleWidget = self.SegmentWidgets[0]
    if shouldCollapseFirstWidget:
      visibleWidget = self.SegmentWidgets[1]

    self.setViews(
      visibleWidget.getMasterNode(),
      None,
      visibleWidget.getMergeNode())

    for widget in self.SegmentWidgets:
      widget.updateParameterNodeFromGUI()
    self.validate()

  def removeSegmentWidget( self ):
    self.SegmentWidgets[1].visible = False
    self.get('MergeAllCollapsibleGroupBox').visible = False
    self.validate()

  def runMergeAll( self, run, currentMerge = 0 ):
    if run:
      # Add observer on the PDF segmenter CLI
      cliNode = self.getCLINode(slicer.modules.imagelabelcombine)
      parameters = self.imageLabelCombineParameters()
      self.observeCLINode(cliNode, self.onImageLabelCombineModified)

      self.get('MergeAllApplyButton').setChecked(True)
      cliNode = slicer.cli.run(slicer.modules.imagelabelcombine, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onImageLabelCombineModified)
      self.get('SegmentApplyPDFPushButton').enabled = False
      cliNode.Cancel()

  def getMergeNode( self ):
    if self.get('MergeAllCollapsibleGroupBox').visible:
      return self.get('MergeAllOutputNodeComboBox').currentNode()
    else:
      return self.SegmentWidgets[0].getMergeNode()
