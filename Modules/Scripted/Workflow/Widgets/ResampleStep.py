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
import ResampleWidget

class ResampleStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(ResampleStep, self).__init__()

    self.initialize( 'ResampleStep' )
    self.setName( 'Resample image' )
    self.setDescription('Resample the image')

    self.NumberOfResampleVolumes = 1
    self.ResampleVolumes = []
    self.createResamplingOutputConnected = False

  def setupUi( self ):
    self.loadUi('ResampleStep.ui')

    for i in range(self.NumberOfResampleVolumes):
      self.ResampleVolumes.append(ResampleWidget.ResampleWidget(self))
      self.ResampleVolumes[i].setMRMLScene(slicer.mrmlScene)
      self.get('ResampleWidgetsLayout').addWidget(self.ResampleVolumes[i])
      title = '%s) Resample volume 1' %string.ascii_uppercase[i]
      self.ResampleVolumes[i].setTitle(title)
      self.ResampleVolumes[i].setProperty('VolumeNumber', i)
      self.ResampleVolumes[i].setResamplingValidCallBack(self.onResampleVolumeValid)

    self.setNumberOfInputs(1)

  def validate( self, desiredBranchId = None ):
    validResampling = True
    for widget in self.ResampleVolumes:
      if widget.visible:
        validResampling = widget.isResamplingValid() and validResampling

    self.validateStep(validResampling, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    for i in range(len(self.ResampleVolumes)):
      self.ResampleVolumes[i].setInputNode(self.step('LoadData').getInputNode(i))
      self.ResampleVolumes[i].initialize()

    # Superclass call done last because it calls validate()
    super(ResampleStep, self).onEntry(comingFrom, transitionType)
    self.updateViews()

  def updateFromCLIParameters( self ):
    for i in range(len(self.ResampleVolumes)):
      self.ResampleVolumes[i].updateFromCLIParameters()

  def onResampleVolumeValid( self, index ):
    if index not in range(len(self.ResampleVolumes)):
      return

    #self.ResampleVolumes[index].collapse(True)
    if index < len(self.ResampleVolumes) - 1:
      #Enable next
      nextWidget = self.ResampleVolumes[index + 1]
      nextWidget.setSpacing( self.ResampleVolumes[index].getOutputNode().GetSpacing() )
      nextWidget.setMakeIsotropic( False )
      nextWidget.collapse( False )
    self.validate()

  def getResampledNode( self, index ):
    '''Return the volume obtained at the end of the resample step.
       If the step does not have a resampled node for the index, it defaults
       to LoadDataStep getInputNode.
       Index should be in [0, 2].'''
    if index not in range(3):
      return

    try:
      return self.ResampleVolumes[index].getOutputNode()
    except IndexError:
      return self.step('LoadData').getInputNode(index)

  def updateConfiguration( self, config ):
    for i in range(len(self.ResampleVolumes)):
      self.ResampleVolumes[i].updateNames(
        config['Workflow']['Volume%iName' %(i+1)],
        '%s) Resample %s' % (string.ascii_uppercase[i], config['Workflow']['Volume%iName' %(i+1)].lower()))

  def setNumberOfInputs( self, numberOfInputs ):
    if numberOfInputs not in range(1,4):
      return

    for i in range(len(self.ResampleVolumes)):
      self.ResampleVolumes[i].visible = (i+1 <= numberOfInputs)

  def updateViews( self ):
    viewDictionnary = {}
    for i, widget in enumerate(self.ResampleVolumes, start=1):
      subDictionnary = {}
      id = widget.getInputNode().GetID() if widget.getInputNode() is not None else ''
      subDictionnary['Background'] = id
      id = widget.getOutputNode().GetID() if widget.getOutputNode() is not None else ''
      subDictionnary['Foreground'] = id
      viewDictionnary['Input%i' %i] = subDictionnary
    self.setViews(viewDictionnary)
