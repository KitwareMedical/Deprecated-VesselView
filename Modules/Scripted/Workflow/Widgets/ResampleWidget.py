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

class ResampleWidget( slicer.qMRMLWidget ) :

  # \todo Revise tooltips in GUI

  def __init__( self, parentStep ):
    super(ResampleWidget, self).__init__()

    self.WorkflowStep = parentStep
    self.createResamplingOutputConnected = False
    self.InputNode = None
    self.ResamplingValidCallBack = None

    self.setup()

  def get(self, objectName):
    return self.WorkflowStep.findWidget(self.widget, objectName)

  def setup( self ):
    layout = qt.QVBoxLayout(self)
    layout.setContentsMargins(0,0,0,0)
    self.widget = self.WorkflowStep.Workflow.loadUi('ResampleWidget.ui')
    layout.addWidget(self.widget)
    self.setLayout(layout)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('ResampleOutputSaveToolButton').icon = saveIcon
    self.get('ResampleSaveToolButton').icon = saveIcon
    self.get('ResampleSaveToolButton').connect('clicked()', self.saveResampledImage)

    self.get('ResampleApplyPushButton').connect('clicked(bool)', self.runResampling)
    self.get('ResampleGoToModulePushButton').connect('clicked()', self.openResampleImageModule)
    self.get('ResampleGoToVolumesPushButton').connect('clicked()', self.openVolumesModule)
    self.get('ResampleInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.updateSpacing)

    self.get('ResampleHiddenOutputNodeComboBox').setVisible(False)

  def setMRMLScene( self, scene ):
    self.widget.setMRMLScene(scene)

  def setTitle( self, title ):
    self.get('ResampleCollapsibleGroupBox').setTitle(title)

  def isResamplingValid( self ):
    validResampling = self.isVolumeIsotropic(self.getOutputNode())

    self.get('ResampleOutputSaveToolButton').enabled = validResampling
    self.get('ResampleSaveToolButton').enabled = validResampling

    return validResampling

  def initialize( self ):

    # Create output if necessary
    if not self.createResamplingOutputConnected:
      self.get('ResampleInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createResampledOutput)
      self.createResamplingOutputConnected = True
    self.createResampledOutput()

    # Set volume OnEntry so we can actually check if it's isotropic or not.
    # Otherwise, the volume is set when the empty volume is created
    # and since it's still empty, it's not isotropic.
    node = self.getInputNode()
    self.get('ResampleInputNodeComboBox').setCurrentNode(node)
    if self.isVolumeIsotropic(node):
      self.get('ResampleHiddenOutputNodeComboBox').setCurrentNode(node)

  def setInputNode( self, node ):
    self.get('ResampleInputNodeComboBox').setCurrentNode(node)

  def getInputNode( self ):
    return self.get('ResampleInputNodeComboBox').currentNode()

  def getResamplingValidCallBack( self ):
    return self.ResamplingValidCallBack

  def setResamplingValidCallBack( self, callback ):
    self.ResamplingValidCallBack = callback

  def collapse( self, collapsed = True, disable = False ):
    self.get('ResampleCollapsibleGroupBox').setChecked(not collapsed)
    self.get('ResampleCollapsibleGroupBox').enabled = not disable

  def getOutputNode( self ):
    return self.get('ResampleHiddenOutputNodeComboBox').currentNode()

  def saveResampledImage( self ):
    self.WorkflowStep.saveFile('Resampled Image', 'VolumeFile', '.mha', self.get('ResampleOutputNodeComboBox'))

  def createResampledOutput( self ):
    self.WorkflowStep.createOutputIfNeeded( self.get('ResampleInputNodeComboBox').currentNode(),
                                            'iso',
                                            self.get('ResampleOutputNodeComboBox') )

  def resampleImageWorkflowParameters( self ):
    parameters = self.WorkflowStep.getJsonParameters(slicer.modules.resampleimage)
    parameters['inputVolume'] = self.get('ResampleInputNodeComboBox').currentNode()
    parameters['outputVolume'] = self.get('ResampleOutputNodeComboBox').currentNode()
    parameters['interpolator'] = self.get('ResampleInterpolationTypeComboBox').currentText
    parameters['makeIsotropic'] = self.get('ResampleMakeIsotropicCheckBox').isChecked()
    parameters['spacing'] = self.get('ResampleManualSpacingCoordinatesWidget').coordinates

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.WorkflowStep.getCLINode(slicer.modules.resampleimage)
    self.get('ResampleInputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('inputVolume'))
    self.get('ResampleOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('outputVolume'))

    index = self.get('ResampleInterpolationTypeComboBox').findText(cliNode.GetParameterAsString('interpolator'))
    if index != -1:
      self.get('ResampleInterpolationTypeComboBox').setCurrentIndex(index)

    self.get('ResampleMakeIsotropicCheckBox').setChecked(cliNode.GetParameterAsString('makeIsotropic') != '')
    self.get('ResampleManualSpacingCoordinatesWidget').coordinates = cliNode.GetParameterAsString('spacing')

  def runResampling( self, run ):
    if run:
      cliNode = self.WorkflowStep.getCLINode(slicer.modules.resampleimage)
      parameters = self.resampleImageWorkflowParameters()
      self.get('ResampleApplyPushButton').setChecked(True)
      self.WorkflowStep.observeCLINode(cliNode, self.onResampleCLIModified)
      cliNode = slicer.cli.run(slicer.modules.resampleimage, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onResampleCLIModified)
      self.get('ResampleApplyPushButton').enabled = False
      cliNode.Cancel()

  def onResampleCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      outputNode = self.get('ResampleOutputNodeComboBox').currentNode()
      self.get('ResampleHiddenOutputNodeComboBox').setCurrentNode(outputNode)
      self.WorkflowStep.updateViews(self.get('ResampleInputNodeComboBox').currentNode(),
                                    outputNode)
      validResampling = self.isResamplingValid()
      if validResampling and self.ResamplingValidCallBack:
        self.ResamplingValidCallBack()

    if not cliNode.IsBusy():
      self.get('ResampleApplyPushButton').setChecked(False)
      self.get('ResampleApplyPushButton').enabled = True
      print 'Resampling %s' % cliNode.GetStatusString()
      self.WorkflowStep.removeObservers(self.onResampleCLIModified)

  def openResampleImageModule( self ):
    self.WorkflowStep.openModule('ResampleImage')

    cliNode = self.WorkflowStep.getCLINode(slicer.modules.resampleimage)
    parameters = self.resampleImageWorkflowParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def openVolumesModule( self ):
    self.WorkflowStep.openModule('Volumes')

  def updateSpacing( self, inputNode ):
    if not inputNode:
      self.get('ResampleManualSpacingCoordinatesWidget').coordinates = '0,0,0'
    else:
      self.get('ResampleManualSpacingCoordinatesWidget').coordinates = self.tupleToString(inputNode.GetSpacing())

  def tupleToString( self, tuple ):
    return str(tuple).lstrip('(').rstrip(')')

  def isVolumeIsotropic( self, volume ):
    if volume == None:
      return False

    if volume.GetImageData() == None:
      return False

    spacing = volume.GetSpacing()
    return ((spacing[0] == spacing[1]) and (spacing[0] == spacing[2]))

  def setSpacing( self, spacing ):
    ''' Assumes input is a tuple'''
    self.get('ResampleManualSpacingCoordinatesWidget').coordinates = self.tupleToString(spacing)

  def setMakeIsotropic( self, iso ):
    ''' Assumes input is a tuple'''
    self.get('ResampleMakeIsotropicCheckBox').setChecked(iso)

  def updateNames( self, volumeName, title ):
    self.get('ResampleInputNodeLabel').setText(volumeName)
    self.get('ResampleCollapsibleGroupBox').setTitle(title)
    self.get('ResampleOutputNodeLabel').setText('Resampled %s' %volumeName.lower())
