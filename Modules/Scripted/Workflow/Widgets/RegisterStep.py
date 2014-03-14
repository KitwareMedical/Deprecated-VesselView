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
    cliNode = self.getCLINode(slicer.modules.expertautomatedregistration)
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
                               'reg',
                               self.get('RegisterOutputNodeComboBox') )

  def registerImageWorkflowParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.resampleimage)
    parameters['fixedImage'] = self.get('RegisterFixedNodeComboBox').currentNode()
    parameters['movingImage'] = self.get('RegisterMovingNodeComboBox').currentNode()
    parameters['resampledImage'] = self.get('RegisterOutputNodeComboBox').currentNode()

    registrationType = self.get('RegisterTypeComboBox').currentText
    if registrationType == 'Initial':
      parameters['registration'] = registrationType
    else:
      parameters['registration'] = 'Pipeline' + registrationType

    parameters['expectedOffset'] = self.get('RegisterExpectedOffsetSpinBox').value
    parameters['expectedRotation'] = self.get('RegisterExpectedRotationSpinBox').value
    parameters['expectedScale'] = self.get('RegisterExpectedScaleSpinBox').value
    parameters['expectedSkew'] = self.get('RegisterExpectedSkewSpinBox').value

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.getCLINode(slicer.modules.expertautomatedregistration)
    self.get('RegisterFixedNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('fixedImage'))
    self.get('RegisterMovingNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('movingImage'))
    self.get('RegisterOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('resampledImage'))

    index = self.get('RegisterTypeComboBox').findText(cliNode.GetParameterAsString('registration'))
    if index != -1:
      self.get('RegisterTypeComboBox').setCurrentIndex(index)

    self.get('RegisterExpectedOffsetSpinBox').setValue(float(cliNode.GetParameterAsString('expectedOffset')))
    self.get('RegisterExpectedRotationSpinBox').setValue(float(cliNode.GetParameterAsString('expectedRotation')))
    self.get('RegisterExpectedScaleSpinBox').setValue(float(cliNode.GetParameterAsString('expectedScale')))
    self.get('RegisterExpectedSkewSpinBox').setValue(float(cliNode.GetParameterAsString('expectedSkew')))

  def runRegistration( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.expertautomatedregistration)
      parameters = self.registerImageWorkflowParameters()
      self.get('RegisterApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.onRegistrationCLIModified)
      cliNode = slicer.cli.run(slicer.modules.expertautomatedregistration, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onRegistrationCLIModified)
      self.get('RegisterApplyPushButton').enabled = False
      cliNode.Cancel()

  def onRegistrationCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.setViews(self.get('RegisterFixedNodeComboBox').currentNode(),
                       self.get('RegisterOutputNodeComboBox').currentNode())
      self.validate()

    if not cliNode.IsBusy():
      self.get('RegisterApplyPushButton').setChecked(False)
      self.get('RegisterApplyPushButton').enabled = True
      print 'Expert Automated Registration %s' % cliNode.GetStatusString()
      self.removeObservers(self.onRegistrationCLIModified)

  def openRegisterImageModule( self ):
    self.openModule('ExpertAutomatedRegistration')

    cliNode = self.getCLINode(slicer.modules.expertautomatedregistration)
    parameters = self.registerImageWorkflowParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)
