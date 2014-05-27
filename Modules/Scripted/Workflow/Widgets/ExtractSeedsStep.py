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

class ExtractSeedsStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(ExtractSeedsStep, self).__init__()

    self.initialize( 'ExtractSeedsStep' )
    self.setName( 'Extract the vessels seeds' )
    self.setDescription('Extract seeds points from the vessely input image')

    self.createExtractSeedsOutputConnected = False

  def setupUi( self ):
    self.loadUi('ExtractSeedsStep.ui')
    self.step('VesselEnhancementStep').get('VesselEnhancementMaskNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                                        self.setMaskColorNode)
    self.get('ExtractSeedsMaskNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('ExtractSeedsSavePushButton').setVisible(False)
    self.get('ExtractSeedsSavePushButton').icon = saveIcon
    self.get('ExtractSeedsSavePushButton').connect('clicked()', self.saveExtractSeedsImage)

    self.get('ExtractSeedsApplyPushButton').connect('clicked(bool)', self.runExtractSeeds)
    self.get('ExtractSeedsGoToModulePushButton').connect('clicked()', self.openExtractSeedsModule)

  def validate( self, desiredBranchId = None ):
    validExtraction = True
    cliNode = self.getCLINode(slicer.modules.segmenttubeseeds)
    validExtraction = (cliNode.GetStatusString() == 'Completed')
    self.get('ExtractSeedsSavePushButton').setVisible(validExtraction)
    self.get('ExtractSeedsSavePushButton').enabled = validExtraction

    self.validateStep(validExtraction, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    self.Workflow.updateLayout(1)
    self.updateViews()

    # Create output if necessary
    if not self.createExtractSeedsOutputConnected:
      self.get('ExtractSeedsInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createExtractSeedsOutput)
      self.createExtractSeedsOutputConnected = True
    self.createExtractSeedsOutput()

    self.get('ExtractSeedsInputNodeComboBox').setCurrentNode(
      self.step('VesselEnhancementStep').getVesselNode())
    self.get('ExtractSeedsMaskNodeComboBox').setCurrentNode(
      self.step('VesselEnhancementStep').getMaskNode())

    super(ExtractSeedsStep, self).onEntry(comingFrom, transitionType)

  def setMaskColorNode( self, node ):
    if not node or not node.GetLabelMap():
      return

    displayNode = node.GetDisplayNode()
    if displayNode:
      self.get('ExtractSeedsObjectIDLabelComboBox').setMRMLColorNode(displayNode.GetColorNode())

    self.get('ExtractSeedsObjectIDLabelComboBox').setCurrentColor(
      self.step('VesselEnhancementStep').getTubeColor())

  def saveExtractSeedsImage( self ):
    self.saveFile('Seeds Image', 'VolumeFile', '.mha', self.get('ExtractSeedsOutputNodeComboBox'))

  def createExtractSeedsOutput( self ):
    self.createOutputIfNeeded( self.get('ExtractSeedsInputNodeComboBox').currentNode(),
                               'seed',
                               self.get('ExtractSeedsOutputNodeComboBox') )

  def ExtractSeedsParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.segmenttubeseeds)
    parameters['inputImage'] = self.get('ExtractSeedsInputNodeComboBox').currentNode()
    parameters['labelMap'] = self.get('ExtractSeedsMaskNodeComboBox').currentNode()
    parameters['tubeId'] = self.get('ExtractSeedsObjectIDLabelComboBox').currentColor
    parameters['unknownId'] = '-1'
    parameters['backgroundId'] = '0' # This should always be 0 since after the PDF segmenter, the background is switched to 0
    parameters['outputSeedImage'] = self.get('ExtractSeedsOutputNodeComboBox').currentNode()

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.getCLINode(slicer.modules.segmenttubeseeds)

    self.get('ExtractSeedsInputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('inputImage'))
    self.get('ExtractSeedsMaskNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('labelmap'))
    self.get('ExtractSeedsObjectIDLabelComboBox').setCurrentColor(cliNode.GetParameterAsString('tubeId'))
    self.get('ExtractSeedsOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('outputSeedImage'))

  def runExtractSeeds( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.segmenttubeseeds)
      parameters = self.ExtractSeedsParameters()
      self.get('ExtractSeedsApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.onExtractSeedsCLIModified)
      cliNode = slicer.cli.run(slicer.modules.segmenttubeseeds, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onExtractSeedsCLIModified)
      self.get('ExtractSeedsApplyPushButton').enabled = False
      cliNode.Cancel()

  def onExtractSeedsCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()

      self.updateViews()

    if not cliNode.IsBusy():
      self.get('ExtractSeedsApplyPushButton').setChecked(False)
      self.get('ExtractSeedsApplyPushButton').enabled = True
      print 'Segment Tube Seeds %s' % cliNode.GetStatusString()
      self.removeObservers(self.onExtractSeedsCLIModified)

  def openExtractSeedsModule( self ):
    self.openModule('SegmentTubeSeeds')

    cliNode = self.getCLINode(slicer.modules.segmenttubeseeds)
    parameters = self.ExtractSeedsParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def updateConfiguration( self, config ):
    organ = config['Workflow']['Organ']
    self.get('ExtractSeedsInputLabel').setText('Enhanced ' + organ + ' image')
    self.get('ExtractSeedsMaskLabel').setText(organ + ' mask')
    self.get('ExtractSeedsObjectIDLabel').setText(organ + ' label')

  def updateViews( self ):
    viewDictionnary = {}
    for i in range(1, self.step('LoadData').getNumberOfInputs() + 1):
      subDictionnary = {
        'Background' : self.get('ExtractSeedsInputNodeComboBox').currentNodeID,
        'Foreground' : self.get('ExtractSeedsOutputNodeComboBox').currentNodeID,
        'Label' : '',
        }
      viewDictionnary['Input%i' %i] = subDictionnary
    self.setViews(viewDictionnary)

  def getHelp( self ):
    return '''Extract a seed image from the vessely image. These seeds will be
      used to generate the vessel in the next step.
      '''
