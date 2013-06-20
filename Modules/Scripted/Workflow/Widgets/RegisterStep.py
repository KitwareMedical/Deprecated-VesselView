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

class RegisterStep( WorkflowStep ) :

  def __init__( self ):
    self.initialize( 'Register' )
    self.setName( 'Register images' )
    self.setDescription('Align the scans for a perfect overlay ')

  def setupUi( self ):
    self.loadUi('RegisterStep.ui')
    self.step('LoadData').get('Volume1NodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                             self.get('RegisterFixedNodeComboBox').setCurrentNode)
    self.step('LoadData').get('Volume2NodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)',
                                                             self.get('RegisterMovingNodeComboBox').setCurrentNode)

  def validate( self, desiredBranchId ):
    self.validateStep(True, desiredBranchId)
