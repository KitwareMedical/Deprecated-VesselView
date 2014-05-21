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
    self.WorkflowConfigData = {}
    self.PresetButtons = []

  def setupUi( self ):
    self.loadUi('InitialStep.ui')

    moduleName = 'Workflow'
    scriptedModulesPath = eval('slicer.modules.%s.path' % moduleName.lower())
    scriptedModulesPath = os.path.dirname(scriptedModulesPath)
    iconsPath = os.path.join(scriptedModulesPath, 'Widgets', 'Resources', 'Icons')

    buttonGroup = qt.QButtonGroup(self.get('InitialCollapsibleGroupBox'))

    logic = slicer.modules.workflow.logic()
    resourceDir = qt.QDir(logic.GetModuleShareDirectory() + '/Resources')
    for dir in resourceDir.entryList(resourceDir.Dirs | resourceDir.NoDotAndDotDot):
      pushButton = qt.QPushButton(self.get('InitialCollapsibleGroupBox'))
      buttonGroup.addButton(pushButton)
      pushButton.text = dir.replace('_', ' ')
      pushButton.setProperty('Path', resourceDir.absolutePath() + '/' + dir)
      pushButton.checkable = True
      pushButton.connect('clicked()', self.onPresetSelected)
      pushButton.setIcon(qt.QIcon(os.path.join(iconsPath, dir)))
      pushButton.setIconSize(qt.QSize(75, 75))

      self.PresetButtons.append(pushButton)
      self.get('InitialCollapsibleGroupBox').layout().addWidget(pushButton)

    # No init, it is done by the workflow once everything is set up

  def validate( self, desiredBranchId = None ):
    validInitializaton = self.getSelectedButton() is not None
    self.validateStep(validInitializaton, desiredBranchId)

  def getSelectedButton( self ):
    for p in self.PresetButtons:
      if p.isChecked():
        return p
    return None

  def onPresetSelected( self ):
    clickedButton = self.getSelectedButton()
    if clickedButton is None:
      self.validate()
      return

    path = clickedButton.property('Path')
    presetFiles = qt.QDir(path)

    self.Presets = {}
    self.WorkflowConfigData = {}
    for filename in presetFiles.entryList(['*'], presetFiles.Files):
      absolutePath = '%s/%s' % (presetFiles.absolutePath(), filename)
      if filename.endswith('.json'):
        self.Presets[filename[:-len('.json')]] = absolutePath
      elif filename.endswith('.dict'):
        file = open(absolutePath)
        self.WorkflowConfigData[filename[:-len('.dict')]] = eval(file.read())

    self.Workflow.updateConfiguration()
    self.validate()

  def getPresets( self ):
    return self.Presets

  def getConfigurationData( self ):
    return self.WorkflowConfigData
