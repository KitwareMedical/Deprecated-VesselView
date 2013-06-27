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

class ResampleStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(ResampleStep, self).__init__()

    self.initialize( 'ResampleStep' )
    self.setName( 'Resample image' )
    self.setDescription('Resample the image')

    self.createResamplingOutputConnected = False

  def setupUi( self ):
    self.loadUi('ResampleStep.ui')

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('ResampleOutputSaveToolButton').icon = saveIcon
    self.get('ResampleSaveToolButton').icon = saveIcon
    self.get('ResampleSaveToolButton').connect('clicked()', self.saveResampledImage)

    self.get('ResampleApplyPushButton').connect('clicked(bool)', self.runResampling)
    self.get('ResampleGoToModulePushButton').connect('clicked()', self.openResampleImageModule)

    self.get('ResampleHiddenOutputNodeComboBox').setVisible(False)

  def validate( self, desiredBranchId = None ):
    validResampling = self.isVolumeIsotropic(self.get('ResampleHiddenOutputNodeComboBox').currentNode())

    self.get('ResampleOutputSaveToolButton').enabled = validResampling
    self.get('ResampleSaveToolButton').enabled = validResampling

    self.validateStep(validResampling, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):

    # Create output if necessary
    if not self.createResamplingOutputConnected:
      self.get('ResampleInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createResampledOutput)
      self.createResamplingOutputConnected = True

    # Set volume OnEntry so we can actually check if it's isotropic or not.
    # Otherwise, the volume is set when the empty volume is created
    # and since it's still empty, it's not isotropic.
    node = self.step('RegisterStep').get('RegisterOutputNodeComboBox').currentNode()
    self.get('ResampleInputNodeComboBox').setCurrentNode(node)
    if self.isVolumeIsotropic(node):
      self.get('ResampleHiddenOutputNodeComboBox').setCurrentNode(node)

    # No need to call createResampledOutput by hand here.
    # It was called when setting the node of the ResampleInputNodeComboBox

    # Superclass call done last because it calls validate()
    super(ResampleStep, self).onEntry(comingFrom, transitionType)

  def saveResampledImage( self ):
    self.saveFile('Resampled Image', 'VolumeFile', '.mha', self.get('ResampleOutputNodeComboBox'))

  def createResampledOutput( self ):
    self.createOutputIfNeeded( self.get('ResampleInputNodeComboBox').currentNode(),
                              'Iso',
                              self.get('ResampleOutputNodeComboBox') )

  def resampleImageWorkflowParameters( self ):
    parameters = {}
    parameters['inputVolume'] = self.get('ResampleInputNodeComboBox').currentNode()
    parameters['outputVolume'] = self.get('ResampleOutputNodeComboBox').currentNode()
    parameters['interpolator'] = self.get('ResampleInterpolationTypeComboBox').currentText
    parameters['makeIsotropic'] = True
    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.getCLINode(slicer.modules.resampleimage)
    self.get('ResampleInputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('inputVolume'))
    self.get('ResampleOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('outputVolume'))

    index = self.get('ResampleInterpolationTypeComboBox').findText(cliNode.GetParameterAsString('interpolator'))
    if index != -1:
      self.get('ResampleInterpolationTypeComboBox').setCurrentIndex(index)

  def runResampling( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.resampleimage)
      parameters = self.resampleImageWorkflowParameters()
      self.get('ResampleApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.onResampleCLIModified)
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
      self.updateViews(self.get('ResampleInputNodeComboBox').currentNode(),
                       outputNode)
      self.validate()

    if not cliNode.IsBusy():
      self.get('ResampleApplyPushButton').setChecked(False)
      self.get('ResampleApplyPushButton').enabled = True
      print 'Resampling %s' % cliNode.GetStatusString()
      self.removeObservers(self.onResampleCLIModified)

  def openResampleImageModule( self ):
    self.openModule('ResampleImage')

    cliNode = self.getCLINode(slicer.modules.resampleimage)
    parameters = self.resampleImageWorkflowParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def isVolumeIsotropic( self, volume ):
    if volume == None:
      return False

    if volume.GetImageData() == None:
      return False

    spacing = volume.GetSpacing()
    return ((spacing[0] == spacing[1]) and (spacing[0] == spacing[2]))
