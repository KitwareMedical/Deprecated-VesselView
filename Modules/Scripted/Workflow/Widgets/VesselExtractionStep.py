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
    self.setName( 'Extract the vessels skeletons' )
    self.setDescription('Extract the shape of the vessels of the input image')

    self.createExtractVesselOutputConnected = False

  def setupUi( self ):
    self.loadUi('VesselExtractionStep.ui')
    self.step('ExtractSkeletonStep').get('ExtractSkeletonInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                                     self.get('VesselExtractionInputNodeComboBox').setCurrentNode)
    self.step('ExtractSkeletonStep').get('ExtractSkeletonOutputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                                      self.get('VesselExtractionSkeletonMaskNodeComboBox').setCurrentNode)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('VesselExtractionOutputSaveToolButton').icon = saveIcon
    self.get('VesselExtractionSaveToolButton').icon = saveIcon
    self.get('VesselExtractionSaveToolButton').connect('clicked()', self.saveVesselExtractionImage)

    self.get('VesselExtractionApplyPushButton').connect('clicked(bool)', self.runVesselExtraction)
    self.get('VesselExtractionGoToModulePushButton').connect('clicked()', self.openVesselExtractionModule)

  def validate( self, desiredBranchId = None ):
    validExtraction = True
    #cliNode = self.getCLINode(slicer.modules.segmenttubes)
    #validExtraction = (cliNode.GetStatusString() == 'Completed')
    #self.get('VesselExtractionOutputSaveToolButton').enabled = validExtraction
    #self.get('VesselExtractionSaveToolButton').enabled = validExtraction

    self.validateStep(validExtraction, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(WorkflowStep, self).onEntry(comingFrom, transitionType)

    # Create output if necessary
    if not self.createExtractVesselOutputConnected:
      self.get('VesselExtractionInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createVesselExtractionOutput)
      self.createExtractVesselOutputConnected = True
    self.createVesselExtractionOutput()

  def saveVesselExtractionImage( self ):
    self.saveFile('Tube objects', 'SpatialObject', '.tre', self.get('VesselExtractionOutputNodeComboBox'))

  def createVesselExtractionOutput( self ):
    self.createOutputIfNeeded( self.get('VesselExtractionInputNodeComboBox').currentNode(),
                               'tubes',
                               self.get('VesselExtractionOutputNodeComboBox') )

  def vesselExtractionParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.segmenttubes)
    parameters['inputVolume'] = self.get('VesselExtractionInputNodeComboBox').currentNode()
    parameters['outputTubeFile'] = self.get('VesselExtractionOutputNodeComboBox').currentNode()
    parameters['seedMaskVolume'] = self.get('VesselExtractionMaskNodeComboBox').currentNode()

    return parameters

  def runVesselExtraction( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.segmenttubes)
      parameters = self.vesselExtractionParameters()
      self.get('VesselExtractionApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.segmenttubes)
      cliNode = slicer.cli.run(slicer.modules.segmenttubes, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onVesselExtractionCLIModified)
      self.get('VesselExtractionApplyPushButton').enabled = False
      cliNode.Cancel()

  def onVesselExtractionCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()

    if not cliNode.IsBusy():
      self.get('VesselExtractionApplyPushButton').setChecked(False)
      self.get('VesselExtractionApplyPushButton').enabled = True
      print 'Segment Tubes %s' % cliNode.GetStatusString()
      self.removeObservers(self.onVesselExtractionCLIModified)

  def openVesselExtractionModule( self ):
    self.openModule('SegmentTubes')

    cliNode = self.getCLINode(slicer.modules.segmenttubes)
    parameters = self.VesselExtractionParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def updateConfiguration( self, config ):
    self.get('VesselExtractionInputLabel').setText('Enhanced ' + config['Workflow']['Organ'] + ' image')
    self.get('VesselExtractiontSkeletonMaskLabel').setText(config['Workflow']['Organ'] + ' vessel skeleton mask')
