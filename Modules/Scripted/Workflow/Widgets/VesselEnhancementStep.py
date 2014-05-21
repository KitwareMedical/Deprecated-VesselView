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

    self.get('VesselEnhancementMaskNodeComboBox').addAttribute('vtkMRMLScalarVolumeNode', 'LabelMap', 1)
    self.get('VesselEnhancementMaskNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                          self.setMaskColorNode)

    saveIcon = self.style().standardIcon(qt.QStyle.SP_DialogSaveButton)
    self.get('VesselEnhancementSaveToolButton').setVisible(False)
    self.get('VesselEnhancementSaveToolButton').icon = saveIcon
    self.get('VesselEnhancementSaveToolButton').connect('clicked()', self.saveVesselEnhancementImage)

    self.get('VesselEnhancementApplyPushButton').connect('clicked(bool)', self.runVesselEnhancement)
    self.get('VesselEnhancementGoToModulePushButton').connect('clicked()', self.openVesselEnhancementModule)

  def validate( self, desiredBranchId = None ):
    validEnhancement = True
    cliNode = self.getCLINode(slicer.modules.enhancetubesusingdiscriminantanalysis)
    validEnhancement = (cliNode.GetStatusString() == 'Completed')
    self.get('VesselEnhancementSaveToolButton').setVisible(validEnhancement)
    self.get('VesselEnhancementSaveToolButton').enabled = validEnhancement

    if validEnhancement:
      viewDictionnary = {}
      for i in range(1, self.step('LoadData').getNumberOfInputs() + 1):
        subDictionnary = {}
        vesselyImageNode =  self.get('VesselEnhancementOutputNodeComboBox').currentNode()
        subDictionnary['Background'] = vesselyImageNode.GetID() if vesselyImageNode is not None else ''
        subDictionnary['Foreground'] = ''
        subDictionnary['Label'] = ''
        viewDictionnary['Input%i' %i] = subDictionnary
      self.setViews(viewDictionnary)

      # Switch to one layout view to observe the vessely image
      self.Workflow.updateLayout(1)

    self.validateStep(validEnhancement, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    self.Workflow.updateLayout(self.step('LoadData').getNumberOfInputs())

    # Create output if necessary
    if not self.createExtractVesselOutputConnected:
      self.get('VesselEnhancementInputNode1ComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.createVesselEnhancementOutput)
      self.createExtractVesselOutputConnected = True
    self.createVesselEnhancementOutput()

    # Get the parameters from the segmentation step
    for i in range(0, self.Workflow.maximumNumberOfInput):
      self.get('VesselEnhancementInputNode%iComboBox' %(i+1)).setCurrentNode(
        self.step('RegisterStep').getRegisteredNode(i))

    self.get('VesselEnhancementMaskNodeComboBox').setCurrentNode(
      self.step('SegmentationStep').getMergeNode())

    super(WorkflowStep, self).onEntry(comingFrom, transitionType)

  def saveVesselEnhancementImage( self ):
    self.saveFile('Vessel Image', 'VolumeFile', '.mha', self.get('VesselEnhancementOutputNodeComboBox'))

  def createVesselEnhancementOutput( self ):
    self.createOutputIfNeeded( self.get('VesselEnhancementInputNode1ComboBox').currentNode(),
                               'ves',
                               self.get('VesselEnhancementOutputNodeComboBox') )

  def getTubeColor( self ):
    return self.get('VesselEnhancementObjectIDLabelComboBox').currentColor

  def getVesselNode( self ):
    return self.get('VesselEnhancementOutputNodeComboBox').currentNode()

  def getMaskNode( self ):
    return self.get('VesselEnhancementMaskNodeComboBox').currentNode()

  def vesselEnhancementParameters( self ):
    parameters = self.getJsonParameters(slicer.modules.enhancetubesusingdiscriminantanalysis)
    parameters['inputVolumesString'] = self.getInputFilenames()
    parameters['labelmap'] = self.getMaskNode()
    parameters['outputVolume'] = self.getVesselNode()
    parameters['tubeId'] = self.getTubeColor()
    parameters['unknownId'] = '-1'
    parameters['backgroundId'] = '0' # This should always be 0 since after the PDF segmenter, the background is switched to 0

    return parameters

  def updateFromCLIParameters( self ):
    cliNode = self.getCLINode(slicer.modules.enhancetubesusingdiscriminantanalysis)

    jointFilenames = cliNode.GetParameterAsString('inputVolumesString')
    filenames = jointFilenames.split(',')
    for i in range(0, self.Workflow.maximumNumberOfInput):
      try:
        id = self.getVolumeIDFromFilename(filenames[i].strip())
      except IndexError:
        id = ''
      self.get('VesselEnhancementInputNode%iComboBox' %(i+1)).setCurrentNodeID(id)

    self.get('VesselEnhancementOutputNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('outputVolume'))
    self.get('VesselEnhancementMaskNodeComboBox').setCurrentNodeID(cliNode.GetParameterAsString('labelmap'))
    self.get('VesselEnhancementObjectIDLabelComboBox').setCurrentColor(cliNode.GetParameterAsString('tubeId'))

  def runVesselEnhancement( self, run ):
    if run:
      cliNode = self.getCLINode(slicer.modules.enhancetubesusingdiscriminantanalysis)
      parameters = self.vesselEnhancementParameters()
      self.get('VesselEnhancementApplyPushButton').setChecked(True)
      self.observeCLINode(cliNode, self.onVesselEnhancementCLIModified)
      cliNode = slicer.cli.run(slicer.modules.enhancetubesusingdiscriminantanalysis, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onVesselEnhancementCLIModified)
      self.get('VesselEnhancementApplyPushButton').enabled = False
      cliNode.Cancel()

  def onVesselEnhancementCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      self.validate()

    if not cliNode.IsBusy():
      self.get('VesselEnhancementApplyPushButton').setChecked(False)
      self.get('VesselEnhancementApplyPushButton').enabled = True
      print 'Enhance Using NJet Discriminant Analysis %s' % cliNode.GetStatusString()
      self.removeObservers(self.onVesselEnhancementCLIModified)

  def openVesselEnhancementModule( self ):
    self.openModule('EnhanceTubesUsingDiscriminantAnalysis')

    cliNode = self.getCLINode(slicer.modules.enhancetubesusingdiscriminantanalysis)
    parameters = self.vesselEnhancementParameters()
    slicer.cli.setNodeParameters(cliNode, parameters)

  def getInputFilenames( self ):
    filenames = []
    for i in range(1, self.Workflow.maximumNumberOfInput + 1):
      inputVolume = self.get('VesselEnhancementInputNode%iComboBox' %i).currentNode()
      if inputVolume and inputVolume.GetID():
        filenames.append(self.getFilenameFromVolume(inputVolume))

    return ','.join(filenames)

  def getFilenameFromVolume( self, volume ):
    if not volume:
      return ''

    storageNode = volume.GetNthStorageNode(0)
    if not storageNode or not storageNode.GetFileName():
      # Save it in temp dir
      tempPath = slicer.app.temporaryPath
      volumeName = tempPath + '/' + volume.GetName() + '.nrrd'
      slicer.util.saveNode(volume, volumeName)

    return storageNode.GetFileName()

  def getVolumeIDFromFilename( self, filename ):
    if not filename:
      return ''

    collection = slicer.mrmlScene.GetNodesByClass('vtkMRMLScalarVolumeNode')
    for i in range(collection.GetNumberOfItems()):
      volume = collection.GetItemAsObject(i)
      if volume is None:
        continue

      for j in range(volume.GetNumberOfStorageNodes()):
        storageNode = volume.GetNthStorageNode(j)
        if storageNode and storageNode.GetFileName() == filename:
          return volume.GetID()

    return ''

  def updateConfiguration( self, config ):
    for i in range(1, self.Workflow.maximumNumberOfInput + 1):
      self.get('VesselEnhancementInput%iLabel' %i).setText(config['Workflow']['Volume%iName' %i])
    self.get('VesselEnhancementInputMaskLabel').setText(config['Workflow']['Organ'] + ' mask')
    self.get('VesselEnhancementObjectIDLabel').setText(config['Workflow']['Organ'] + ' label')

  def setMaskColorNode( self, node ):
    if not node or not node.GetLabelMap():
      return

    displayNode = node.GetDisplayNode()
    if displayNode:
      self.get('VesselEnhancementObjectIDLabelComboBox').setMRMLColorNode(displayNode.GetColorNode())

    self.get('VesselEnhancementObjectIDLabelComboBox').setCurrentColor(
      self.step('SegmentationStep').getOrganColor())

  def onNumberOfInputsChanged( self, numberOfInputs ):
    if numberOfInputs not in range(1,4):
      return

    for i in range(1, self.Workflow.maximumNumberOfInput + 1):
      if i > numberOfInputs:
        self.get('VesselEnhancementInput%iLabel' %i).setProperty('workflow', ['NotVisible'])
        self.get('VesselEnhancementInputNode%iComboBox' %i).setProperty('workflow', ['NotVisible'])
      else:
        self.get('VesselEnhancementInput%iLabel' %i).setProperty('workflow', ['2'])
        self.get('VesselEnhancementInputNode%iComboBox' %i).setProperty('workflow', ['2'])
    self.setWorkflowLevel(self.Workflow.level)

  def updateViews( self ):
    viewDictionnary = {}
    for i in range(1, self.step('LoadData').getNumberOfInputs() + 1):
      subDictionnary = {}
      backgroundNode =  self.get('VesselEnhancementInputNode%iComboBox' %i).currentNode()
      subDictionnary['Background'] = backgroundNode.GetID() if backgroundNode is not None else ''
      foregroundNode = self.get('VesselEnhancementOutputNodeComboBox').currentNode()
      subDictionnary['Foreground'] = foregroundNode.GetID() if foregroundNode is not None else ''
      labelNode = self.get('VesselEnhancementMaskNodeComboBox').currentNode()
      subDictionnary['Label'] = labelNode.GetID() if labelNode is not None else ''
      viewDictionnary['Input%i' %i] = subDictionnary
    self.setViews(viewDictionnary)
