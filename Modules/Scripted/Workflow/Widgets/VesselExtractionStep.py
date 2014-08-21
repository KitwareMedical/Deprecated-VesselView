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
from InteractiveSegmentTubesLogic import SegmentTubesLogic

class VesselExtractionStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(VesselExtractionStep, self).__init__()

    self.initialize( 'VesselExtractionStep' )
    self.setName( 'Extract the vessels skeletons' )
    self.setDescription('Extract the shape of the vessels of the input image')

    self.createExtractVesselOutputConnected = False
    self.createExtractVesselSeed = False
    self.logic = SegmentTubesLogic()

  def setupUi( self ):
    self.loadUi('VesselExtractionStep.ui')

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('VesselExtractionSavePushButton').setVisible(False)
    self.get('VesselExtractionSavePushButton').icon = saveIcon
    self.get('VesselExtractionSavePushButton').connect('clicked()', self.saveVesselExtractionImage)

    self.get('VesselExtractionOutputNodeComboBox').connect(
      'nodeAddedByUser(vtkMRMLNode*)', self.logic.addDisplayNodes)

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
      self.step('VesselEnhancementStep').getVesselNode())

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
    slicer.modules.markups.logic().StartPlaceMode(False)
    super(VesselExtractionStep, self).onEntry(comingFrom, transitionType)

  def saveVesselExtractionImage( self ):
    self.saveFile('Tube objects', 'SpatialObject', '.tre', self.get('VesselExtractionOutputNodeComboBox'))

  def createVesselExtractionOutput( self ):
    self.createOutputIfNeeded( self.get('VesselExtractionInputNodeComboBox').currentNode(),
                               'tubes',
                               self.get('VesselExtractionOutputNodeComboBox') )
    self.updateViews()

  def createVesselExtractSeed( self ):
    nodeName = 'VesselExtractionSeed'
    node = self.getFirstNodeByNameAndClass(nodeName, 'vtkMRMLMarkupsFiducialNode')
    if node == None:
      nodeID = slicer.modules.markups.logic().AddNewFiducialNode(nodeName)
      node = slicer.mrmlScene.GetNodeByID(nodeID)
      node.SetMarkupLabelFormat('')

      selectionNodeID = slicer.modules.markups.logic().GetSelectionNodeID()
      selectionNode = slicer.mrmlScene.GetNodeByID(selectionNodeID)
      selectionNode.SetReferenceActivePlaceNodeID(nodeID)

    self.get('VesselExtractionSeedPointNodeComboBox').setCurrentNode(node)

  def vesselExtractionParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.segmenttubes)
    parameters['inputVolume'] = self.get('VesselExtractionInputNodeComboBox').currentNode()
    parameters['OutputNode'] = self.get('VesselExtractionOutputNodeComboBox').currentNode()
    parameters['outputTubeFile'] = self.logic.getFilenameFromNode(parameters['OutputNode'])
    parameters['seedP'] = self.get('VesselExtractionSeedPointNodeComboBox').currentNode()

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.getCLINode(slicer.modules.segmenttubes)
    self.get('VesselExtractionInputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('inputVolume'))
    self.get('VesselExtractionOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('outputTubeFile'))
    self.get('VesselExtractionSeedPointNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('seedPhysicalPoint'))

  def runVesselExtraction( self, run ):
    parameters = {}
    if run:
      parameters = self.vesselExtractionParameters()
      self.get('VesselExtractionApplyPushButton').setChecked(True)
    else:
      self.get('VesselExtractionApplyPushButton').enabled = False
    self.logic.run(run, parameters, self.onVesselExtractionCLIModified)

  def onVesselExtractionCLIModified( self, cliNode, *unused ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()

    if not cliNode.IsBusy():
      self.logic.removeObservers(self.onVesselExtractionCLIModified)
      self.get('VesselExtractionApplyPushButton').setChecked(False)
      self.get('VesselExtractionApplyPushButton').enabled = True
      print 'Segment Tubes %s' % cliNode.GetStatusString()
    self.Workflow.getProgressBar().setCommandLineModuleNode(cliNode)

  def openVesselExtractionModule( self ):
    self.openModule('SegmentTubes')

    cliNode = self.getCLINode(slicer.modules.segmenttubes)
    parameters = self.vesselExtractionParameters()
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
