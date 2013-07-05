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
import ResampleWidget

class ResampleStep( WorkflowStep ) :

  # \todo Revise tooltips in GUI

  def __init__( self ):
    super(ResampleStep, self).__init__()

    self.initialize( 'ResampleStep' )
    self.setName( 'Resample image' )
    self.setDescription('Resample the image')

    self.createResamplingOutputConnected = False

  def setupUi( self ):
    self.loadUi('ResampleStep.ui')

    self.ResampleVolume1 = ResampleWidget.ResampleWidget(self)
    self.ResampleVolume1.setMRMLScene(slicer.mrmlScene)
    self.get('ResampleWidgetsLayout').addWidget(self.ResampleVolume1)
    self.ResampleVolume1.setResamplingValidCallBack(self.onResampleVolume1Valid)
    self.ResampleVolume1.setTitle('A) Resample volume 1')

    self.ResampleVolume2 = ResampleWidget.ResampleWidget(self)
    self.ResampleVolume2.setMRMLScene(slicer.mrmlScene)
    self.ResampleVolume2.setTitle('B) Resample volume 2')
    self.ResampleVolume2.collapse(True)
    self.ResampleVolume2.setResamplingValidCallBack(self.validate)
    self.get('ResampleWidgetsLayout').addWidget(self.ResampleVolume2)

  def validate( self, desiredBranchId = None ):
    validResampling = (self.ResampleVolume1.isResamplingValid()
                       and self.ResampleVolume2.isResamplingValid())

    self.validateStep(validResampling, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):

    self.ResampleVolume1.setInputNode(
      self.step('RegisterStep').get('RegisterFixedNodeComboBox').currentNode())
    self.ResampleVolume1.initialize()

    # Superclass call done last because it calls validate()
    super(ResampleStep, self).onEntry(comingFrom, transitionType)

  def updateFromCLIParameters( self ):
    self.ResampleVolume1.updateFromCLIParameters()
    self.ResampleVolume2.updateFromCLIParameters()

  def onResampleVolume1Valid( self ):
    self.ResampleVolume1.collapse( True )
    self.ResampleVolume2.setInputNode(
      self.step('RegisterStep').get('RegisterOutputNodeComboBox').currentNode())
    self.ResampleVolume2.initialize()
    self.ResampleVolume2.setSpacing( self.ResampleVolume1.getOutputNode().GetSpacing() )
    self.ResampleVolume2.setMakeIsotropic( False )
    self.ResampleVolume2.collapse( False, False )

  def getResampledVolume1( self ):
	return self.ResampleVolume1.getOutputNode()
