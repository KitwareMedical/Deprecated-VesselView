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

class VesselExtractionStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(VesselExtractionStep, self).__init__()

    self.initialize( 'VesselExtractionStep' )
    self.setName( 'Extract vessels' )
    self.setDescription('Extract the vessels of the input image')

    self.createExtractVesselOutputConnected = False

  def setupUi( self ):
    self.loadUi('VesselExtractionStep.ui')
    self.step('SegmentationStep').get('SegmentOutputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                            self.get('VesselExtractionInputNodeComboBox').setCurrentNode)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('VesselExtractionOutputSaveToolButton').icon = saveIcon
    self.get('VesselExtractionSaveToolButton').icon = saveIcon
    self.get('VesselExtractionSaveToolButton').connect('clicked()', self.saveVesselExtractionImage)

    self.get('VesselExtractionApplyPushButton').connect('clicked(bool)', self.runVesselExtraction)
    self.get('VesselExtractionGoToModulePushButton').connect('clicked()', self.openVesselExtractionModule)

  def validate( self, desiredBranchId = None ):
    validExtraction = False

    #cliNode = self.getCLINode(slicer.modules.brainsfit)
    #validRegistration = (cliNode.GetStatusString() == 'Completed')
    #self.get('RegisterOutputSaveToolButton').enabled = validRegistration
    #self.get('RegisterSaveToolButton').enabled = validRegistration

    self.validateStep(validExtraction, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(WorkflowStep, self).onEntry(comingFrom, transitionType)

    # Create output if necessary
    if not self.createExtractVesselOutputConnected:
      self.get('VesselExtractionInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createVesselExtractionOutput)
      self.createExtractVesselOutputConnected = True
    self.createVesselExtractionOutput()

  def saveVesselExtractionImage( self ):
    self.saveFile('Vessel Image', 'VolumeFile', '.mha', self.get('VesselExtractionOutputNodeComboBox'))

  def createVesselExtractionOutput( self ):
    self.createOutputIfNeeded( self.get('VesselExtractionInputNodeComboBox').currentNode(),
                               'ves',
                               self.get('VesselExtractionOutputNodeComboBox') )

  def vesselExtractionParameters( self ):
    parameters = {}
    #parameters['fixedVolume'] = self.get('RegisterFixedNodeComboBox').currentNode()
    #parameters['movingVolume'] = self.get('RegisterMovingNodeComboBox').currentNode()
    #parameters['outputVolume'] = self.get('RegisterOutputNodeComboBox').currentNode()

    return parameters

  def runVesselExtraction( self, run ):
    pass
    #if run:
    #  cliNode = self.getCLINode(slicer.modules.brainsfit)
    #  parameters = self.registerImageParameters()
    #  self.get('RegisterApplyPushButton').setChecked(True)
    #  self.observeCLINode(cliNode, self.onRegistrationCLIModified)
    #  cliNode = slicer.cli.run(slicer.modules.brainsfit, cliNode, parameters, wait_for_completion = False)
    #else:
    #  cliNode = self.observer(
    #    slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
    #    self.onRegistrationCLIModified)
    #  self.get('RegisterApplyPushButton').enabled = False
    #  cliNode.Cancel()

  def onVesselExtractionCLIModified( self, cliNode, event ):
    pass
    #if cliNode.GetStatusString() == 'Completed':
    #  self.validate()

    #if not cliNode.IsBusy():
    #  self.get('RegisterApplyPushButton').setChecked(False)
    #  self.get('RegisterApplyPushButton').enabled = True
    #  print 'BRAINSFIT Registration %s' % cliNode.GetStatusString()
    #  self.removeObservers(self.onRegistrationCLIModified)

  def openVesselExtractionModule( self ):
    pass
    #self.openModule('BRAINSFit')

    #cliNode = self.getCLINode(slicer.modules.brainsfit)
    #parameters = self.registerImageParameters()
    #slicer.cli.setNodeParameters(cliNode, parameters)
