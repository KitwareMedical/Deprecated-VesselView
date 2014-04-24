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

class RegisterWidget( slicer.qMRMLWidget ) :

  # \todo Revise tooltips in GUI

  def __init__( self, parentStep ):
    super(RegisterWidget, self).__init__()

    self.WorkflowStep = parentStep
    self.createRegisterOutputConnected = False
    self.RegisterValidCallBack = None

    self.setup()

  def get(self, objectName):
    return self.WorkflowStep.findWidget(self.widget, objectName)

  def setup( self ):
    layout = qt.QVBoxLayout(self)
    layout.setContentsMargins(0,0,0,0)
    self.widget = self.WorkflowStep.Workflow.loadUi('RegisterWidget.ui')
    layout.addWidget(self.widget)
    self.setLayout(layout)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('RegisterOutputSaveToolButton').icon = saveIcon
    self.get('RegisterSaveToolButton').icon = saveIcon
    self.get('RegisterSaveToolButton').connect('clicked()', self.saveRegisteredImage)

    self.get('RegisterApplyPushButton').connect('clicked(bool)', self.runRegistration)
    self.get('RegisterGoToModulePushButton').connect('clicked()', self.openRegisterImageModule)

  def setMRMLScene( self, scene ):
    self.widget.setMRMLScene(scene)

  def setTitle( self, title ):
    self.get('RegisterCollapsibleGroupBox').setTitle(title)

  def isRegistrationValid( self ):
    cliNode = self.WorkflowStep.getCLINode(slicer.modules.expertautomatedregistration)
    validRegistration = (cliNode.GetStatusString() == 'Completed')
    validRegistration = validRegistration and self.getOutputNode()

    self.get('RegisterOutputSaveToolButton').enabled = validRegistration
    self.get('RegisterSaveToolButton').enabled = validRegistration
    return validRegistration

  def initialize( self ):
    # Create output if necessary
    if not self.createRegisterOutputConnected:
      self.get('RegisterFixedNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createRegisterOutput)
      self.get('RegisterMovingNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createRegisterOutput)
      self.createRegisterOutputConnected = True
    self.createRegisterOutput()

  def setFixedNode( self, node ):
    self.get('RegisterFixedNodeComboBox').setCurrentNode(node)

  def getFixedNode( self ):
    return self.get('RegisterFixedNodeComboBox').currentNode()

  def setMovingNode( self, node ):
    self.get('RegisterMovingNodeComboBox').setCurrentNode(node)

  def getMovingNode( self ):
    return self.get('RegisterMovingNodeComboBox').currentNode()

  def getOutputNode( self ):
    return self.get('RegisterOutputNodeComboBox').currentNode()

  def getRegisterValidCallBack( self ):
    return self.ResamplingValidCallBack

  def setRegisterValidCallBack( self, callback ):
    self.RegisterValidCallBack = callback

  def collapse( self, collapsed = True, disable = False ):
    self.get('RegisterCollapsibleGroupBox').setChecked(not collapsed)
    self.get('RegisterCollapsibleGroupBox').enabled = not disable

  def saveRegisteredImage( self ):
    self.WorkflowStep.saveFile(
      'Registered Image', 'VolumeFile', '.mha', self.get('RegisterOutputNodeComboBox'))

  def createRegisterOutput( self ):
    fixedNode = self.get('RegisterFixedNodeComboBox').currentNode()
    movingNode = self.get('RegisterMovingNodeComboBox').currentNode()
    if fixedNode is None or movingNode is None:
      return

    self.WorkflowStep.createOutputIfNeeded(
      movingNode, 'reg', self.get('RegisterOutputNodeComboBox') )

  def registerImageWorkflowParameters( self ):
    parameters = self.WorkflowStep.getJsonParameters(slicer.modules.expertautomatedregistration)
    parameters['fixedImage'] = self.getFixedNode()
    parameters['movingImage'] = self.getMovingNode()
    parameters['resampledImage'] = self.getOutputNode()

    registrationType = self.get('RegisterTypeComboBox').currentText
    if registrationType == 'Initial':
      parameters['registration'] = registrationType
    else:
      parameters['registration'] = 'Pipeline' + registrationType

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.WorkflowStep.getCLINode(slicer.modules.expertautomatedregistration)
    self.get('RegisterFixedNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('fixedImage'))
    self.get('RegisterMovingNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('movingImage'))
    self.get('RegisterOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('resampledImage'))

    index = self.get('RegisterTypeComboBox').findText(cliNode.GetParameterAsString('registration'))
    if index != -1:
      self.get('RegisterTypeComboBox').setCurrentIndex(index)

  def runRegistration( self, run ):
    if run:
      cliNode = self.WorkflowStep.getCLINode(slicer.modules.expertautomatedregistration)
      parameters = self.registerImageWorkflowParameters()
      self.get('RegisterApplyPushButton').setChecked(True)
      self.WorkflowStep.observeCLINode(cliNode, self.onRegistrationCLIModified)
      cliNode = slicer.cli.run(slicer.modules.expertautomatedregistration, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.WorkflowStep.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onRegistrationCLIModified)
      self.get('RegisterApplyPushButton').enabled = False
      cliNode.Cancel()

  def onRegistrationCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.WorkflowStep.updateViews()

      if self.isRegistrationValid() and self.RegisterValidCallBack:
        self.RegisterValidCallBack()

    if not cliNode.IsBusy():
      self.get('RegisterApplyPushButton').setChecked(False)
      self.get('RegisterApplyPushButton').enabled = True
      print 'Expert Automated Registration %s' % cliNode.GetStatusString()
      self.WorkflowStep.removeObservers(self.onRegistrationCLIModified)

  def openRegisterImageModule( self ):
    self.WorkflowStep.openModule('ExpertAutomatedRegistration')

    cliNode = self.WorkflowStep.getCLINode(slicer.modules.expertautomatedregistration)
    parameters = self.registerImageWorkflowParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def updateNames( self, volumeName, title ):
    self.get('RegisterCollapsibleGroupBox').setTitle(title)
    self.get('RegisterOutputNodeLabel').setText('Registered %s' %volumeName.lower())
