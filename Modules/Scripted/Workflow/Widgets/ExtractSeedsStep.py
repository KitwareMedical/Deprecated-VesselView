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
    self.setName( 'Extract the vessels skeletons' )
    self.setDescription('Extract the shape of the vessels of the input image')

    self.createExtractVesselOutputConnected = False

  def setupUi( self ):
    self.loadUi('ExtractSeedsStep.ui')
    self.step('VesselEnhancementStep').get('VesselEnhancementOutputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                                          self.get('ExtractSkeletonInputNodeComboBox').setCurrentNode)
    self.step('VesselEnhancementStep').get('VesselEnhancementMaskNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                                        self.get('ExtractSkeletonMaskNodeComboBox').setCurrentNode)
    self.get('ExtractSkeletonMaskNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('ExtractSkeletonOutputSaveToolButton').icon = saveIcon
    self.get('ExtractSkeletonSaveToolButton').icon = saveIcon
    self.get('ExtractSkeletonSaveToolButton').connect('clicked()', self.saveExtractSkeletonImage)

    self.get('ExtractSkeletonApplyPushButton').connect('clicked(bool)', self.runExtractSkeleton)
    self.get('ExtractSkeletonGoToModulePushButton').connect('clicked()', self.openExtractSkeletonModule)

  def validate( self, desiredBranchId = None ):
    validExtraction = True
    #cliNode = self.getCLINode(slicer.modules.enhanceusingnjetdiscriminantanalysis)
    #validExtraction = (cliNode.GetStatusString() == 'Completed')
    #self.get('ExtractSkeletonOutputSaveToolButton').enabled = validExtraction
    #self.get('ExtractSkeletonSaveToolButton').enabled = validExtraction

    self.validateStep(validExtraction, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(WorkflowStep, self).onEntry(comingFrom, transitionType)

    # Create output if necessary
    if not self.createExtractVesselOutputConnected:
      self.get('ExtractSkeletonInputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createExtractSkeletonOutput)
      self.createExtractVesselOutputConnected = True
    self.createExtractSkeletonOutput()

  def saveExtractSkeletonImage( self ):
    self.saveFile('Skeleton Image', 'VolumeFile', '.mha', self.get('ExtractSkeletonOutputNodeComboBox'))

  def createExtractSkeletonOutput( self ):
    self.createOutputIfNeeded( self.get('ExtractSkeletonInputNodeComboBox').currentNode(),
                               'ske',
                               self.get('ExtractSkeletonOutputNodeComboBox') )

  def extractSkeletonParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.segmenttubeseeds)
    parameters['inputImage'] = self.get('ExtractSkeletonInputNodeComboBox').currentNode()
    parameters['outputSeedImage'] = self.get('ExtractSkeletonMaskNodeComboBox').currentNode()
    parameters['labelmap'] = self.get('ExtractSkeletonMaskNodeComboBox').currentNode()

    return parameters

  def runExtractSkeleton( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.segmenttubeseeds)
      parameters = self.extractSkeletonParameters()
      self.get('ExtractSkeletonApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.segmenttubeseeds)
      cliNode = slicer.cli.run(slicer.modules.segmenttubeseeds, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onExtractSkeletonCLIModified)
      self.get('ExtractSkeletonApplyPushButton').enabled = False
      cliNode.Cancel()

  def onExtractSkeletonCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()
      #self.setViews(self.get('ExtractSkeletonOutputNodeComboBox').currentNode())

    if not cliNode.IsBusy():
      self.get('ExtractSkeletonApplyPushButton').setChecked(False)
      self.get('ExtractSkeletonApplyPushButton').enabled = True
      print 'Segment Tube Seeds %s' % cliNode.GetStatusString()
      self.removeObservers(self.onExtractSkeletonCLIModified)

  def openExtractSkeletonModule( self ):
    self.openModule('SegmentTubeSeeds')

    cliNode = self.getCLINode(slicer.modules.segmenttubeseeds)
    parameters = self.extractSkeletonParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def getInputFilenames( self ):
    inputVolume1 = self.get('ExtractSkeletonInputNode1ComboBox').currentNode()
    inputVolume2 = self.get('ExtractSkeletonInputNode2ComboBox').currentNode()

    return self.getFilenameFromVolume(inputVolume1) + ', ' + self.getFilenameFromVolume(inputVolume1)

  def updateConfiguration( self, config ):
    self.get('ExtractSkeletonInputLabel').setText('Enhanced ' + config['Organ'] + ' image')
    self.get('ExtractSkeletonMaskLabel').setText(config['Organ'] + ' mask')
