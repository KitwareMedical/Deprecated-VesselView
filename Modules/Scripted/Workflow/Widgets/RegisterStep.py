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

class RegisterStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(RegisterStep, self).__init__()

    self.initialize( 'RegisterStep' )
    self.setName( 'Register images' )
    self.setDescription('Align the scans for a perfect overlay ')

    self.createRegisterOutputConnected = False

  def setupUi( self ):
    self.loadUi('RegisterStep.ui')
    self.step('LoadData').get('Volume1NodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                             self.get('RegisterFixedNodeComboBox').setCurrentNode)
    self.step('LoadData').get('Volume2NodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                             self.get('RegisterMovingNodeComboBox').setCurrentNode)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('RegisterOutputSaveToolButton').icon = saveIcon
    self.get('RegisterSaveToolButton').icon = saveIcon
    self.get('RegisterSaveToolButton').connect('clicked()', self.saveRegisteredImage)

    self.get('RegisterApplyPushButton').connect('clicked(bool)', self.runRegistration)
    self.get('RegisterGoToModulePushButton').connect('clicked()', self.openRegisterImageModule)

  def validate( self, desiredBranchId = None ):
    cliNode = self.getCLINode(slicer.modules.brainsfit)
    validRegistration = (cliNode.GetStatusString() == 'Completed')
    self.get('RegisterOutputSaveToolButton').enabled = validRegistration
    self.get('RegisterSaveToolButton').enabled = validRegistration

    self.validateStep(validRegistration, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(RegisterStep, self).onEntry(comingFrom, transitionType)

    # Create output if necessary
    if not self.createRegisterOutputConnected:
      self.get('RegisterMovingNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createRegisterOutput)
      self.createRegisterOutputConnected = True
    self.createRegisterOutput()

  def saveRegisteredImage( self ):
    self.saveFile('Registered Image', 'VolumeFile', '.mha', self.get('RegisterOutputNodeComboBox'))

  def createRegisterOutput( self ):
    self.createOutputIfNeeded( self.get('RegisterMovingNodeComboBox').currentNode(),
                               'Registered',
                               self.get('RegisterOutputNodeComboBox') )

  def registerImageParameters( self ):
    parameters = {}
    parameters['fixedVolume'] = self.get('RegisterFixedNodeComboBox').currentNode()
    parameters['movingVolume'] = self.get('RegisterMovingNodeComboBox').currentNode()
    parameters['outputVolume'] = self.get('RegisterOutputNodeComboBox').currentNode()

    # One of these should be set to true at least
    parameters['useRigid'] = self.get('RegisterTypeComboBox').currentIndex >= 0
    parameters['useScaleVersor3D'] = self.get('RegisterTypeComboBox').currentIndex >= 1
    parameters['useScaleSkewVersor3D'] = self.get('RegisterTypeComboBox').currentIndex >= 2
    parameters['useAffine'] = self.get('RegisterTypeComboBox').currentIndex >= 3
    parameters['useBSpline'] = self.get('RegisterTypeComboBox').currentIndex >= 4

    # Get outptut image type from fixed image
    type = self.get('RegisterMovingNodeComboBox').currentNode().GetImageData().GetScalarTypeAsString()
    if type == "unsigned char":
      parameters['outputVolumePixelType'] = "uchar"
    elif type == "unsigned short":
      parameters['outputVolumePixelType'] = "ushort"
    elif type == "unsigned int":
      parameters['outputVolumePixelType'] = "uint"
    elif type == "int" or type == "short":
      parameters['outputVolumePixelType'] = type
    else:
      parameters['outputVolumePixelType'] = "float"

    return parameters

  def runRegistration( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.brainsfit)
      parameters = self.registerImageParameters()
      self.get('RegisterApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.onRegistrationCLIModified)
      cliNode = slicer.cli.run(slicer.modules.brainsfit, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onRegistrationCLIModified)
      self.get('RegisterApplyPushButton').enabled = False
      cliNode.Cancel()

  def onRegistrationCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()

    if not cliNode.IsBusy():
      self.get('RegisterApplyPushButton').setChecked(False)
      self.get('RegisterApplyPushButton').enabled = True
      print 'BRAINSFIT Registration %s' % cliNode.GetStatusString()
      self.removeObservers(self.onRegistrationCLIModified)

  def openRegisterImageModule( self ):
    self.openModule('BRAINSFit')

    cliNode = self.getCLINode(slicer.modules.brainsfit)
    parameters = self.registerImageParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)
