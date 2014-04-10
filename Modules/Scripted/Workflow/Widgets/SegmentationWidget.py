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
import EditorLib.PaintEffect
from EditorLib import EditUtil
import InteractiveConnectedComponentsUsingParzenPDFs

class SegmentationWidget( slicer.qMRMLWidget ) :

  # \todo Revise tooltips in GUI

  def __init__( self, parentStep, editor):
    super(SegmentationWidget, self).__init__()

    self.Observations = []

    self.ParameterNode = None
    self.MergeNode = None
    self.MergeVolumeValid = False
    self.MergeNodeNumber = 0
    self.PaintTool = None
    self.IsValid = False
    self.PDFSegmenterSuccess = False
    self.MergeNodeSuffix = 'label'

    self.WorkflowStep = parentStep
    self.SegmentValidCallBack = None

    self.setup(editor)

  def get(self, objectName):
    return self.WorkflowStep.findWidget(self.widget, objectName)

  def setup( self, editor ):
    layout = qt.QVBoxLayout(self)
    layout.setContentsMargins(0,0,0,0)
    self.widget = self.WorkflowStep.Workflow.loadUi('SegmentationWidget.ui')
    layout.addWidget(self.widget)
    self.setLayout(layout)

    self.PDFSegmenter = slicer.modules.editorExtensions['InteractiveConnectedComponentsUsingParzenPDFs']()

    self.EditorWidget = editor
    self.EditUtil = self.EditorWidget.editUtil
    self.EditBox = self.EditorWidget.toolsBox
    self.Helper = self.EditorWidget.helper
    self.UndoRedo = self.EditBox.undoRedo

    for label in ['Organ', 'Background', 'Barrier']:
      editLabelColorWidget = EditorLib.EditColor(self.get('Segment%sLabelWidget'%label), label)
      setattr(self, 'Segment%sLabelColorBox' %label, editLabelColorWidget)
      editLabelColorWidget.label.hide()
      editLabelColorWidget.labelName.hide()
      editLabelColorWidget.colorSpin.connect('valueChanged(int)', self.updateParameterNodeFromGUI)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('SegmentSaveToolButton').icon = saveIcon
    self.get('SegmentOutputSaveToolButton').icon = saveIcon
    self.get('SegmentSaveToolButton').connect('clicked()', self.saveSegmentedImage)

    self.get('SegmentGoToModulePushButton').connect('clicked()', self.openSegmentModule)

    self.get('SegmentMasterNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                   self._setMasterNode)
    self.get('SegmentMergeNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode','LabelMap','1')
    self.get('SegmentMergeNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                  self.onMergeNodeSelected)

    self.get('SegmentBrushSizeSliderWidget').connect('valueChanged(double)', self.updateParameterNodeFromGUI)

    self.get('SegmentPaintCheckBox').connect('toggled(bool)', self.updateParameterNodeFromGUI)
    self.get('SegmentOrganRadioButton').connect ('toggled(bool)', self.updateParameterNodeFromGUI)
    self.get('SegmentBackgroundRadioButton').connect ('toggled(bool)', self.updateParameterNodeFromGUI)
    self.get('SegmentBarrierRadioButton').connect ('toggled(bool)', self.updateParameterNodeFromGUI)

    self.get('SegmentUndoButton').connect('clicked()', self.undo)
    self.get('SegmentRedoButton').connect('clicked()', self.redo)
    self.get('SegmentApplyPDFPushButton').connect('clicked(bool)', self.runPDFSegmenter)

    self.updateGUIFromParameterNode(self.getParameterNode(), vtk.vtkCommand.ModifiedEvent)

  def setMRMLScene( self, scene ):
    self.widget.setMRMLScene(scene)

  def updateGUIFromParameterNode( self, node, event):
    disable = not node or node!= self.getParameterNode()
    self.get('SegmentCollapsibleGroupBox').enabled = not disable
    if disable:
      return

    currentLabel = int(node.GetParameter('label'))
    if currentLabel == self.SegmentOrganLabelColorBox.colorSpin.value:
      self.get('SegmentOrganRadioButton').setChecked(True)
      self.get('SegmentBrushSizeSliderWidget').enabled = True
    elif currentLabel == self.SegmentBackgroundLabelColorBox.colorSpin.value:
      self.get('SegmentBackgroundRadioButton').setChecked(True)
      self.get('SegmentBrushSizeSliderWidget').enabled = True
    elif currentLabel == self.SegmentBarrierLabelColorBox.colorSpin.value:
      self.get('SegmentBarrierRadioButton').setChecked(True)
      self.get('SegmentBrushSizeSliderWidget').enabled = True
    else:
      self.get('SegmentOrganRadioButton').setChecked(True)
      self.get('SegmentBrushSizeSliderWidget').enabled = False

    self.get('SegmentPaintCheckBox').setChecked(
      isinstance(self.EditBox.currentOption, EditorLib.PaintEffectOptions))

    try:
      radius = float(node.GetParameter('PaintEffect,radius'))
    except ValueError:
      radius = 5.0
    self.get('SegmentBrushSizeSliderWidget').value = radius

  def updateParameterNodeFromGUI( self ):
    currentLabel = 0
    if self.get('SegmentOrganRadioButton').isChecked():
      currentLabel = self.SegmentOrganLabelColorBox.colorSpin.value
    elif self.get('SegmentBackgroundRadioButton').isChecked():
      currentLabel = self.SegmentBackgroundLabelColorBox.colorSpin.value
    elif self.get('SegmentBarrierRadioButton').isChecked():
      currentLabel = self.SegmentBarrierLabelColorBox.colorSpin.value

    parameterNode = self.getParameterNode()
    wasModifying = parameterNode.StartModify()
    effect = 'DefaultEffect'
    if self.get('SegmentPaintCheckBox').isChecked():
      effect = 'PaintEffect'
    self.EditBox.selectEffect(effect)

    parameterNode.SetParameter('label', str(currentLabel))
    parameterNode.SetParameter('PaintEffect,radius', str(self.get('SegmentBrushSizeSliderWidget').value))
    parameterNode.EndModify(wasModifying)

  def getParameterNode( self ):
    newParameterNode = self.EditUtil.getParameterNode()
    if newParameterNode != self.ParameterNode:
      self.removeObservers(self.updateGUIFromParameterNode)
      self.ParameterNode = newParameterNode
      self.addObserver(self.ParameterNode, vtk.vtkCommand.ModifiedEvent, self.updateGUIFromParameterNode)

    return self.ParameterNode

  def validate( self, desiredBranchId = None ):
    self.IsValid = (self.getMasterNode() and self.MergeVolumeValid)
    self.IsValid = self.IsValid and self.PDFSegmenterSuccess

    self.get('SegmentSaveToolButton').enabled = self.IsValid
    self.get('SegmentOutputSaveToolButton').enabled = self.IsValid

    if self.IsValid and self.SegmentValidCallBack:
      self.SegmentValidCallBack()

  def saveSegmentedImage( self ):
    self.saveFile('Segmented Image', 'VolumeFile', '.mha', self.get('SegmentMergeNodeComboBox'))

  def openSegmentModule( self ):
    self.openModule('Editor')

    editorWidget = slicer.modules.editor.widgetRepresentation()
    masterVolumeNodeComboBox = editorWidget.findChild('qMRMLNodeComboBox')
    masterVolumeNodeComboBox.setCurrentNode(self.getMasterNode())
    #setButton = editorWidget.findChild('QPushButton')
    #setButton.click()

    # \todo Set up editor mode too ?

  def isMergeVolumeValid( self, volumeNode ):
    if not volumeNode or not volumeNode.GetImageData():
      return False

    # Segmentation is valid if it has at least 2 labels
    range = volumeNode.GetImageData().GetScalarRange()
    return range[0] != range[1]

  def onMergeNodeSelected( self, node ):
    if self.MergeNode is not None:
      self.removeObservers(self.onMergeNodeModified)

    self.MergeNode = node
    if self.MergeNode is not None:
      self.addObserver(self.MergeNode, 'ModifiedEvent', self.onMergeNodeModified)

    self.EditorWidget.setMergeNode(self.MergeNode)
    self.updateLabelSelectors()
    self.WorkflowStep.setViews(self.getMasterNode(), None, self.getMergeNode())
    self.onMergeNodeModified(self.MergeNode)

  def onMergeNodeModified( self, MergeNode, event = 'ModifiedEvent' ):
    self.MergeVolumeValid = self.isMergeVolumeValid(MergeNode)
    if self.MergeVolumeValid:
      self.removeObservers(self.onMergeNodeModified)
    self.updateUndoRedoEnabled()
    self.validate()

  def onPDFSegmenterCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      # Change the background label to 0
      objectColors = cliNode.GetParameterAsString('objectId')
      backgroundColor = eval(objectColors)[1] # object colors is 'foreground, background'

      # Set the parameter node necesseray for the change label logic
      parameterNode = self.getParameterNode()
      parameterNode.SetParameter("ChangeLabelEffect,inputColor", str(backgroundColor))
      parameterNode.SetParameter("ChangeLabelEffect,outputColor", '0')

      # Apply change label
      changeLabelLogic = EditorLib.ChangeLabelEffectLogic(self.EditUtil.getSliceLogic())
      changeLabelLogic.changeLabel()

      self.PDFSegmenterSuccess = True
      self.validate()

    if not cliNode.IsBusy():
      self.get('SegmentApplyPDFPushButton').setChecked(False)
      self.get('SegmentApplyPDFPushButton').enabled = True
      print 'PDF Segmenter %s' % cliNode.GetStatusString()
      self.removeObservers(self.onPDFSegmenterCLIModified)

  def updateConfiguration( self, config ):
    organ = config['Organ']
    organLower = organ.lower()

    groupboxTitle = 'Segment %s' % organLower
    if self.MergeNodeSuffix != 'label':
      groupboxTitle = '%s %s' % (groupboxTitle, self.MergeNodeSuffix)
    self.get('SegmentCollapsibleGroupBox').setTitle(groupboxTitle)

    self.get('SegmentMasterNodeLabel').setText('Input %s' % config['Volume1Name'].lower())
    self.get('SegmentMergeVolumesLabel').setText('Segmented %s image' % organLower)
    self.get('SegmentOrganRadioButton').setText('%s (foreground)' %organ)

  def getPDFSegmenterParameterName( self, parameterName ):
    return 'InteractiveConnectedComponentsUsingParzenPDFsOptions,' + parameterName

  def getParameterFromPDFSegmenter( self, parameterName ):
    parameterNode = self.getParameterNode()
    return parameterNode.GetParameter(self.getPDFSegmenterParameterName(parameterName))

  def setParameterToPDFSegmenter( self, parameterName, value ):
    parameterNode = self.getParameterNode()
    parameterNode.SetParameter(self.getPDFSegmenterParameterName(parameterName), value)

  def setMasterNode( self, node ):
    if self.getMasterNode() != node:
      self.get('SegmentMasterNodeComboBox').setCurrentNodeID(node.GetID() if node else None)
      return

  def _setMasterNode( self, node ):
    self.EditorWidget.setMasterNode(node)
    if self.Helper.colorSelect:
      self.Helper.colorSelect.hide()
    self.setMergeNode(self.createMergeNodeIfNeeded(node))
    self.WorkflowStep.setViews(self.getMasterNode(), None, self.getMergeNode())

  def getMasterNode( self ):
    return self.get('SegmentMasterNodeComboBox').currentNode()

  def setMergeNode( self, node ):
    self.get('SegmentMergeNodeComboBox').setCurrentNodeID(node.GetID() if node else None)

  def getMergeNode( self ):
    return self.get('SegmentMergeNodeComboBox').currentNode()

  def createMergeNodeIfNeeded( self, masterNode ):
    if not masterNode:
      return None

    nodeName = '%s-%s' % (masterNode.GetName(), self.MergeNodeSuffix)
    newMergeNode = self.WorkflowStep.getFirstNodeByNameAndClass(nodeName, 'vtkMRMLScalarVolumeNode')
    if newMergeNode is None:
      volumesLogic = slicer.modules.volumes.logic()
      newMergeNode = volumesLogic.CreateAndAddLabelVolume(slicer.mrmlScene, masterNode, nodeName)
      volumesLogic.SetVolumeAsLabelMap(newMergeNode, True)
      newMergeNode.GetDisplayNode().SetAndObserveColorNodeID('vtkMRMLColorTableNodeFileGenericAnatomyColors.txt')

    return newMergeNode

  def runPDFSegmenter( self, run ):
    if run:
      # Add observer on the PDF segmenter CLI
      pdfSegmenterCLINode = self.WorkflowStep.getCLINode(slicer.modules.segmentconnectedcomponentsusingparzenpdfs, 'PDFSegmenterEditorEffect')
      self.observeCLINode(pdfSegmenterCLINode, self.onPDFSegmenterCLIModified)

      self.get('SegmentApplyPDFPushButton').setChecked(True)
      self.UndoRedo.saveState()
      self.updatePDFSegmenterParameters()
      self.PDFSegmenter.logic(None).applyPDFSegmenter()
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onPDFSegmenterCLIModified)
      self.get('SegmentApplyPDFPushButton').enabled = False
      cliNode.Cancel()

  def updatePDFSegmenterParameters( self ):
    options = self.PDFSegmenter.options(self.EditBox.optionsFrame)
    options.create()
    options.frame.hide()
    options.setMRMLDefaults()

    parameterNode = self.getParameterNode()
    self.setParameterToPDFSegmenter("objectId",
      parameterNode.GetParameter('Organ')
      + ',' + parameterNode.GetParameter('Background')
      + ',' + parameterNode.GetParameter('Barrier')
      )

    # Add resampled volume as additionnal volumes
    for i in range(1, 3):
      additionalVolume = self.WorkflowStep.step('RegisterStep').getRegisteredNode(i)
      additionalVolumeID = additionalVolume.GetID() if additionalVolume is not None else ''
      self.setParameterToPDFSegmenter('additionalInputVolumeID%i' %(i-1), additionalVolumeID)

  def undo( self ):
    self.UndoRedo.undo()
    self.updateUndoRedoEnabled()

  def redo( self ):
    self.UndoRedo.redo()
    self.updateUndoRedoEnabled()

  def updateUndoRedoEnabled( self ):
    self.get('SegmentUndoButton').enabled = self.UndoRedo.undoEnabled()
    self.get('SegmentRedoButton').enabled = self.UndoRedo.redoEnabled()

  def updateLabelSelectors( self, startIndex=1 ):
    self.get('SegmentPaintToolsFrame').enabled = self.getMergeNode() is not None
    parameterNode = self.getParameterNode()
    parameterNode.SetParameter('Organ', str(startIndex))
    parameterNode.SetParameter('Background', str(startIndex+1))
    parameterNode.SetParameter('Barrier', str(startIndex+2))

  def getOrganColor( self ):
    return int(self.getParameterNode().GetParameter('Organ'))

  def IsSegmentationValid( self ):
    return self.IsValid

  def getSegmentValidCallBack( self ):
    return self.SegmentValidCallBack

  def setSegmentValidCallBack( self, callback ):
    self.SegmentValidCallBack = callback

  def collapse( self, collapsed = True, enable = True ):
    self.get('SegmentCollapsibleGroupBox').setChecked(not collapsed)
    self.get('SegmentCollapsibleGroupBox').enabled = enable

  def observeCLINode(self, cliNode, onCLINodeModified = None):
    if cliNode != None and onCLINodeModified != None:
      self.WorkflowStep.addObserver(cliNode,
                                    slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
                                    onCLINodeModified)
    self.WorkflowStep.Workflow.getProgressBar().setCommandLineModuleNode(cliNode)

  def removeObservers(self, method):
    for o, e, m, g, t in self.Observations:
      if method == m:
        o.RemoveObserver(t)
        self.Observations.remove([o, e, m, g, t])

  def addObserver(self, object, event, method, group = 'none'):
    if self.hasObserver(object, event, method):
      print 'already has observer'
      return
    tag = object.AddObserver(event, method)
    self.Observations.append([object, event, method, group, tag])

  def hasObserver(self, object, event, method):
    for o, e, m, g, t in self.Observations:
      if o == object and e == event and m == method:
        return True
    return False

  def observer(self, event, method):
    for o, e, m, g, t in self.Observations:
      if e == event and m == method:
        return o
    return None