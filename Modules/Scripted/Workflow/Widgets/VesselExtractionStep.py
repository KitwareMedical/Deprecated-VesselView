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
    self.createExtractVesselSeed = False

  def setupUi( self ):
    self.loadUi('VesselExtractionStep.ui')

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('VesselExtractionSavePushButton').setVisible(False)
    self.get('VesselExtractionSavePushButton').icon = saveIcon
    self.get('VesselExtractionSavePushButton').connect('clicked()', self.saveVesselExtractionImage)

    self.get('VesselExtractionApplyPushButton').connect('clicked(bool)', self.runVesselExtraction)
    self.get('VesselExtractionGoToModulePushButton').connect('clicked()', self.openVesselExtractionModule)

  def validate( self, desiredBranchId = None ):
    validExtraction = True
    cliNode = self.getCLINode(slicer.modules.segmenttubes)
    validExtraction = (cliNode.GetStatusString() == 'Completed')
    self.get('VesselExtractionSavePushButton').setVisible(validExtraction)
    self.get('VesselExtractionSavePushButton').enabled = validExtraction

    self.validateStep(validExtraction, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    self.Workflow.updateLayout(self.Workflow.maximumNumberOfInput + 1)

    self.get('VesselExtractionInputNodeComboBox').setCurrentNode(
      self.step('ExtractSeedsStep').get('ExtractSeedsOutputNodeComboBox').currentNode())

    # Create output if necessary
    if not self.createExtractVesselOutputConnected:
      self.get('VesselExtractionInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createVesselExtractionOutput)
      self.createExtractVesselOutputConnected = True
    self.createVesselExtractionOutput()

    if not self.createExtractVesselSeed:
      self.get('VesselExtractionSeedPointNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createVesselExtractSeed)
      self.createExtractVesselSeed = True
    self.createVesselExtractSeed()

    self.updateViews()
    super(VesselExtractionStep, self).onEntry(comingFrom, transitionType)

  def saveVesselExtractionImage( self ):
    self.saveFile('Tube objects', 'SpatialObject', '.tre', self.get('VesselExtractionOutputNodeComboBox'))

  def createVesselExtractionOutput( self ):
    self.createOutputIfNeeded( self.get('VesselExtractionInputNodeComboBox').currentNode(),
                               'tubes',
                               self.get('VesselExtractionOutputNodeComboBox') )

  def createVesselExtractSeed( self ):
    nodeName = 'VesselExtractionSeed'
    node = self.getFirstNodeByNameAndClass(nodeName, 'vtkMRMLMarkupsFiducialNode')
    if node == None:
      nodeID = slicer.modules.markups.logic().AddNewFiducialNode(nodeName)
      node = slicer.mrmlScene.GetNodeByID(nodeID)

      imageCenter = self.getImageCenter(
        self.get('VesselExtractionInputNodeComboBox').currentNode())
      slicer.modules.markups.logic().AddFiducial(imageCenter[0], imageCenter[1], imageCenter[2])
      node.SetNthMarkupLabel(0, '')

    self.get('VesselExtractionSeedPointNodeComboBox').setCurrentNode(node)

  def vesselExtractionParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.segmenttubes)
    parameters['inputVolume'] = self.get('VesselExtractionInputNodeComboBox').currentNode()
    parameters['outputTubeFile'] = self.get('VesselExtractionOutputNodeComboBox').currentNode()
    parameters['seedPhysicalPoint'] = self.get('VesselExtractionSeedPointNodeComboBox').currentNode()

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.getCLINode(slicer.modules.segmenttubes)
    self.get('VesselExtractionInputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('inputVolume'))
    self.get('VesselExtractionOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('outputTubeFile'))
    self.get('VesselExtractionSeedPointNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('seedPhysicalPoint'))

  def runVesselExtraction( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.segmenttubes)
      parameters = self.vesselExtractionParameters()
      self.get('VesselExtractionApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.onVesselExtractionCLIModified)
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
    self.get('VesselExtractiontSeedPointLabel').setText(config['Workflow']['Organ'] + ' vessel seed')

  def getHelp( self ):
    return '''Extract the vessels from the vessely image based on the seed
      position and the seed image.
      '''

  def updateViews( self ):
    viewDictionnary = {}

    subDictionnary = {}
    backgroundNode =  self.get('VesselExtractionInputNodeComboBox').currentNode()
    subDictionnary['Background'] = backgroundNode.GetID() if backgroundNode is not None else ''
    subDictionnary['Foreground'] = ''
    subDictionnary['Label'] = ''
    viewDictionnary['Input1'] = subDictionnary
    self.setViews(viewDictionnary)

  def getImageCenter( self, volumeNode ):
    if not volumeNode:
      return [0.0, 0.0, 0.0]

    dims = range(6)
    volumeNode.GetRASBounds(dims)
    origin = []
    for i in range(0, 6, 2):
      origin.append( dims[i] + (dims[i + 1] - dims[i])*0.5 )
    return origin
