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

import os, string
from __main__ import qt, ctk, slicer
from WorkflowStep import *
import RegisterWidget

class RegisterStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(RegisterStep, self).__init__()

    self.initialize( 'RegisterStep' )
    self.setName( 'Register images' )
    self.setDescription('Align the scans for a perfect overlay ')

    self.RegisterWidgets = []
    self.createRegisterOutputConnected = False

  def setupUi( self ):
    self.loadUi('RegisterStep.ui')

    # 2 register steps
    self.RegisterWidgets.append(RegisterWidget.RegisterWidget(self))
    self.RegisterWidgets[0].setMRMLScene(slicer.mrmlScene)
    self.get('RegisterWidgetsLayout').addWidget(self.RegisterWidgets[0])
    self.RegisterWidgets[0].setRegisterValidCallBack(self.onRegisterVolume2Valid)
    self.RegisterWidgets[0].setTitle('A) Register volume 2')

    self.RegisterWidgets.append(RegisterWidget.RegisterWidget(self))
    self.RegisterWidgets[1].setMRMLScene(slicer.mrmlScene)
    self.get('RegisterWidgetsLayout').addWidget(self.RegisterWidgets[1])
    self.RegisterWidgets[1].setRegisterValidCallBack(self.onRegisterVolume3Valid)
    self.RegisterWidgets[1].setTitle('B) Register volume 3')
    self.RegisterWidgets[1].collapse(True)

    self.onNumberOfInputsChanged(self.step('LoadData').getNumberOfInputs())

  def onNumberOfInputsChanged( self, numberOfInputs ):
    if numberOfInputs not in range(1,4):
      return

    self.get('RegisterOneInputLabel').visible = (numberOfInputs == 1)

    for i in range(len(self.RegisterWidgets)):
      self.RegisterWidgets[i].visible = (i+2 <= numberOfInputs)

  def validate( self, desiredBranchId = None ):
    validRegistration = True
    for i in range(len(self.RegisterWidgets)):
      if self.RegisterWidgets[i].visible:
        validRegistration = self.RegisterWidgets[i].isRegistrationValid() and validRegistration

    self.validateStep(validRegistration, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    for i in range(len(self.RegisterWidgets)):
      self.RegisterWidgets[i].setFixedNode(
        self.step('ResampleStep').getResampledNode(0))
      self.RegisterWidgets[i].setMovingNode(
        self.step('ResampleStep').getResampledNode(i+1))
      self.RegisterWidgets[i].initialize()

    # Superclass call done last because it calls validate()
    super(RegisterStep, self).onEntry(comingFrom, transitionType)

  def updateFromCLIParameters( self ):
    for widget in self.RegisterWidgets:
      widget.updateFromCLIParameters()

  def onRegisterVolume2Valid( self ):
    if not self.RegisterWidgets[1].visible:
      self.validate()
    else:
      self.RegisterWidgets[0].collapse(True)
      self.RegisterWidgets[1].collapse(False)
    self.updateViews()

  def onRegisterVolume3Valid( self ):
    self.validate()
    self.updateViews()

  def updateViews( self ):
    viewDictionnary = {}
    for i, widget in enumerate(self.RegisterWidgets, start=1):
      if widget.visible:
        subDictionnary = {}
        id = widget.getFixedNode().GetID() if widget.getFixedNode() is not None else ''
        subDictionnary['Background'] = id
        id = widget.getOutputNode().GetID() if widget.getOutputNode() is not None else ''
        subDictionnary['Foreground'] = id

        viewDictionnary['Input%i' %i] = subDictionnary
    self.setViews(viewDictionnary)

  def updateConfiguration( self, config ):
    for i in range(len(self.RegisterWidgets)):
      self.RegisterWidgets[i].updateNames(
        config['Workflow']['Volume%iName' %(i+2)],
        '%s) Register %s' % (string.ascii_uppercase[i], config['Workflow']['Volume%iName' %(i+2)].lower()))

  def getRegisteredNode( self, index):
    '''Return the volume obtained at the end of the registration step.
       Index should be in [0, 2].'''
    if index not in range(0, 3):
      return

    if index == 0:
      return self.RegisterWidgets[0].getFixedNode()
    else:
      return self.RegisterWidgets[index-1].getOutputNode()
