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

class InitialStep( WorkflowStep ) :

  def __init__( self ):
    super(InitialStep, self).__init__()

    self.initialize( 'Initial' )
    self.setName( 'Tasks types' )
    self.setDescription('Choose what tasks will be performed')

    self.Presets = {}
    self.WorkflowConfigData = None

  def setupUi( self ):
    self.loadUi('InitialStep.ui')

    logic = slicer.modules.workflow.logic()
    resourceDir = qt.QDir(logic.GetModuleShareDirectory() + '/Resources')
    for dir in resourceDir.entryList(resourceDir.Dirs | resourceDir.NoDotAndDotDot):
      self.get('InitialAnalysisTypeComboBox').addItem(dir.replace('_', ' '), resourceDir.absolutePath() + '/' + dir)

    self.get('InitialAnalysisTypeComboBox').connect('currentIndexChanged(int)', self.onPresetSelected)
    # No init, it is done by the workflow once everything is set up

  def validate( self, desiredBranchId = None ):
    validInitializaton = True
    self.validateStep(validInitializaton, desiredBranchId)

  def onPresetSelected( self, index = None ):
    if not index:
      index = self.get('InitialAnalysisTypeComboBox').currentIndex

    path = self.get('InitialAnalysisTypeComboBox').itemData(index)
    presetFiles = qt.QDir(path)

    self.Presets = {}
    nameFilters = ['*.json']
    for file in presetFiles.entryList(nameFilters, presetFiles.Files):
      filename = file.strip('.json')
      absolutePath = '%s/%s' % (presetFiles.absolutePath(), file)

      if filename != self.Workflow.moduleName:
        self.Presets[filename] = absolutePath
      else:
        jsonData = open(absolutePath)
        try :
          self.WorkflowConfigData = json.load(jsonData)
        except ValueError:
          print 'Could not read json file %s. No config loaded' % absolutePath
          self.WorkflowConfigData = None

    self.Workflow.updateConfiguration()

  def getPresets( self ):
    return self.Presets

  def getConfigurationData( self ):
    return self.WorkflowConfigData
