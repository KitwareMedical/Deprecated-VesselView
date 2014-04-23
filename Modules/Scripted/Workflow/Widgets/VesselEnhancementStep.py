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

class VesselEnhancementStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(VesselEnhancementStep, self).__init__()

    self.initialize( 'VesselEnhancementStep' )
    self.setName( 'Enhance vessels' )
    self.setDescription('Enhance the vessels of the input image')

    self.createExtractVesselOutputConnected = False

  def setupUi( self ):
    self.loadUi('VesselEnhancementStep.ui')
    self.step('SegmentationStep').get('SegmentMasterNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                            self.get('VesselEnhancementInputNode1ComboBox').setCurrentNode)
    self.step('SegmentationStep').get('SegmentMergeNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                                           self.get('VesselEnhancementMaskNodeComboBox').setCurrentNode)
    self.get('VesselEnhancementMaskNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)
    self.get('VesselEnhancementMaskNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                          self.setMaskColorNode)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('VesselEnhancementOutputSaveToolButton').icon = saveIcon
    self.get('VesselEnhancementSaveToolButton').icon = saveIcon
    self.get('VesselEnhancementSaveToolButton').connect('clicked()', self.saveVesselEnhancementImage)

    self.get('VesselEnhancementApplyPushButton').connect('clicked(bool)', self.runVesselEnhancement)
    self.get('VesselEnhancementGoToModulePushButton').connect('clicked()', self.openVesselEnhancementModule)

  def validate( self, desiredBranchId = None ):
    validEnhancement = True
    #cliNode = self.getCLINode(slicer.modules.enhanceusingnjetdiscriminantanalysis)
    #validEnhancement = (cliNode.GetStatusString() == 'Completed')
    #self.get('VesselEnhancementOutputSaveToolButton').enabled = validEnhancement
    #self.get('VesselEnhancementSaveToolButton').enabled = validEnhancement

    self.validateStep(validEnhancement, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    super(WorkflowStep, self).onEntry(comingFrom, transitionType)

    # Create output if necessary
    if not self.createExtractVesselOutputConnected:
      self.get('VesselEnhancementInputNode1ComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createVesselEnhancementOutput)
      self.createExtractVesselOutputConnected = True
    self.createVesselEnhancementOutput()

    # Get the parameters from the segmentation step
    self.get('VesselEnhancementInputNode2ComboBox').setCurrentNode(
      self.step('SegmentationStep').getVolume2())

    self.get('VesselEnhancementObjectIDLabelComboBox').setCurrentColor(
      self.step('SegmentationStep').getMaskImageObjectId())

  def saveVesselEnhancementImage( self ):
    self.saveFile('Vessel Image', 'VolumeFile', '.mha', self.get('VesselEnhancementOutputNodeComboBox'))

  def createVesselEnhancementOutput( self ):
    self.createOutputIfNeeded( self.get('VesselEnhancementInputNode1ComboBox').currentNode(),
                               'ves',
                               self.get('VesselEnhancementOutputNodeComboBox') )

  def vesselEnhancementParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.enhanceusingnjetdiscriminantanalysis)
    parameters['inputVolumesString'] = self.getInputFilenames()
    parameters['labelmap'] = self.get('VesselEnhancementMaskNodeComboBox').currentNode()
    parameters['outputBase'] = self.get('VesselEnhancementOutputNodeComboBox').currentNode()
    parameters['objectIdList'] = str(self.get('VesselEnhancementObjectIDLabelComboBox').currentColor)

    return parameters

  def updateFromCLIParameters( self ):
    pass
    #cliNode = self.WorkflowStep.getCLINode(slicer.modules.enhanceusingnjetdiscriminantanalysis)
    # TO DO When CLI is here

  def runVesselEnhancement( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.enhanceusingnjetdiscriminantanalysis)
      parameters = self.vesselEnhancementParameters()
      self.get('VesselEnhancementApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.enhanceusingnjetdiscriminantanalysis)
      cliNode = slicer.cli.run(slicer.modules.enhanceusingnjetdiscriminantanalysis, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onVesselEnhancementCLIModified)
      self.get('VesselEnhancementApplyPushButton').enabled = False
      cliNode.Cancel()

  def onVesselEnhancementCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()
      #self.setViews(self.get('VesselEnhancementOutputNodeComboBox').currentNode())

    if not cliNode.IsBusy():
      self.get('VesselEnhancementApplyPushButton').setChecked(False)
      self.get('VesselEnhancementApplyPushButton').enabled = True
      print 'Enhance Using NJet Discriminant Analysis %s' % cliNode.GetStatusString()
      self.removeObservers(self.onVesselEnhancementCLIModified)

  def openVesselEnhancementModule( self ):
    self.openModule('EnhanceUsingNJetDiscriminantAnalysis')

    cliNode = self.getCLINode(slicer.modules.enhanceusingnjetdiscriminantanalysis)
    parameters = self.vesselEnhancementParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def getInputFilenames( self ):
    inputVolume1 = self.get('VesselEnhancementInputNode1ComboBox').currentNode()
    inputVolume2 = self.get('VesselEnhancementInputNode2ComboBox').currentNode()

    return self.getFilenameFromVolume(inputVolume1) + ', ' + self.getFilenameFromVolume(inputVolume1)

  def getFilenameFromVolume( self, volume ):
    if not volume:
      return ''

    if volume.GetNumberOfStorageNodes() < 1:
      # Save it in temp dir
      tempPath = slicer.app.temporaryPath
      volumeName = tempPath + '/' + volume.GetName() + '.nrrd'
      slicer.util.saveNode(volume, volumeName)

    storageNode = volume.GetNthStorageNode(0)
    return storageNode.GetFileName()

  def updateConfiguration( self, config ):
    self.get('VesselEnhancementInput1Label').setText(config['Volume1Name'])
    self.get('VesselEnhancementInput2Label').setText(config['Volume2Name'])
    self.get('VesselEnhancementInputMaskLabel').setText(config['Organ'] + ' mask')
    self.get('VesselEnhancementObjectIDLabel').setText(config['Organ'] + ' label')

  def setMaskColorNode( self, node ):
    if not node or not node.GetLabelMap():
      return

    displayNode = node.GetDisplayNode()
    if displayNode:
      self.get('VesselEnhancementObjectIDLabelComboBox').setMRMLColorNode(displayNode.GetColorNode())
