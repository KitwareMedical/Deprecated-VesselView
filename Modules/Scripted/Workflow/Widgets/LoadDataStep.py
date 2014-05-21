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

class LoadDataStep( WorkflowStep ) :

  def __init__( self ):
    super(LoadDataStep, self).__init__()

    self.initialize( 'LoadData' )
    self.setName( 'Load input scans' )
    self.setDescription('Load CT or MR images')
    self._numberOfInputs = 3

  def setupUi( self ):
    self.loadUi('LoadDataStep.ui')
    loadIcon = self.style().standardIcon(qt.QStyle.SP_DialogOpenButton)
    # Volumes
    for i in range(1, self.Workflow.maximumNumberOfInput + 1):
      self.get('Volume%iNodeComboBox' %i).connect('currentNodeChanged(vtkMRMLNode*)', self.validate)
      self.get('Volume%iNodeComboBox' %i).connect('currentNodeChanged(vtkMRMLNode*)', self.onVolumeChanged)
      self.get('Volume%iNodeToolButton' %i).icon = loadIcon
      slot = getattr(self, 'loadVolume%iNode' %i)
      self.get('Volume%iNodeToolButton' %i).connect('clicked()', slot)

    # + and -
    self.get('RemoveVolumeToolButton').connect('clicked()', self.removeOneInput)
    self.get('AddVolumeToolButton').connect('clicked()', self.addOneInput)

    # Go to Volumes
    self.get('GoToVolumesModulePushButton').connect('clicked()', self.goToVolumesModule)

    # Initialize only one input
    self.setNumberOfInputs(1)

  def validate( self, desiredBranchId = None ):
    isValid = True
    for i in range(1, self._numberOfInputs + 1):
      isValid = isValid and self.get('Volume%iNodeComboBox' %i).currentNode() != None

    self.validateStep(isValid, desiredBranchId)

  def onVolumeChanged( self ):
    viewDictionnary = {}
    for i in range(1, self._numberOfInputs + 1):
      subDictionnary = {
        'Background' : self.get('Volume%iNodeComboBox' %i).currentNodeID
        }
      viewDictionnary['Input%i' %i] = subDictionnary
    self.Workflow.setViews(viewDictionnary)

  def loadVolume1Node(self):
    self.loadFile('First Volume', 'VolumeFile', self.get('Volume1NodeComboBox'))

  def loadVolume2Node(self):
    self.loadFile('Second Volume', 'VolumeFile', self.get('Volume2NodeComboBox'))

  def loadVolume3Node(self):
    self.loadFile('Third Volume', 'VolumeFile', self.get('Volume3NodeComboBox'))

  def goToVolumesModule(self):
    self.openModule('Volumes')

  def removeOneInput(self):
    self.setNumberOfInputs(self.getNumberOfInputs() - 1)
    self.validate()

  def addOneInput(self):
    self.setNumberOfInputs(self.getNumberOfInputs() + 1)
    self.validate()

  def setNumberOfInputs(self, newNumberOfInputs):
    if newNumberOfInputs < 1 or newNumberOfInputs > self.Workflow.maximumNumberOfInput:
      return
    if self._numberOfInputs == newNumberOfInputs:
      return

    self._numberOfInputs = newNumberOfInputs
    self.Workflow.updateLayout(self.getNumberOfInputs())
    self.Workflow.updateConfiguration()
    self.onVolumeChanged()
    self.Workflow.onNumberOfInputsChanged(self._numberOfInputs)

  def onNumberOfInputsChanged( self, numberOfInputs ):
    if numberOfInputs not in range(1, self.Workflow.maximumNumberOfInput + 1):
      return

    for i in range(2, self.Workflow.maximumNumberOfInput + 1):
      combobox = self.get('Volume%iNodeComboBox' %i)
      shouldActivate = (i <= self._numberOfInputs)
      if not shouldActivate:
        combobox.setCurrentNodeID('')

      combobox.visible = shouldActivate
      self.get('Volume%iLabel' %i).visible = shouldActivate
      self.get('Volume%iNodeToolButton' %i).visible = shouldActivate

  def getNumberOfInputs(self):
    return self._numberOfInputs

  def updateConfiguration( self, config ):
    for i in range(1, self.Workflow.maximumNumberOfInput + 1):
      self.get('Volume%iLabel' %i).setText(config['Workflow']['Volume%iName' %i])

      sliceNode = slicer.mrmlScene.GetNodeByID('vtkMRMLSliceNodeInput%i' %i)
      if sliceNode:
        sliceNode.SetLayoutLabel(config['Workflow']['Organ'] + (' volume %i' %i))

  def onEntry(self, comingFrom, transitionType):
    super(LoadDataStep, self).onEntry(comingFrom, transitionType)
    self.Workflow.updateLayout(self.getNumberOfInputs())
    self.onVolumeChanged()

  def getInputNode( self, index):
    '''Return the volume chosen by the user. Index should be in [0, 2].'''
    if index not in range(self.Workflow.maximumNumberOfInput):
      return
    return self.get('Volume%iNodeComboBox' %(index+1)).currentNode()

  def getHelp( self ):
    return '''Load the data from which the analysis will be run. One to three
      volumes can be used.
      '''
