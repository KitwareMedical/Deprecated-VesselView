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

import imp, sys, os, unittest

from __main__ import vtk, qt, ctk, slicer

import Widgets

class Workflow:
  def __init__(self, parent):
    parent.title = "Prometheus"
    parent.categories = ["", "TubeTK"]
    parent.dependencies = []
    parent.contributors = ["Julien Finet (Kitware), Johan Andruejol (Kitware)"]
    parent.helpText = """
    Step by step workflow to monitor RFA of lesions. See <a href=\"http://public.kitware.com/Wiki/TubeTK\"</a> for more information.
    """
    parent.acknowledgementText = """
    This work is supported by the National Institute of Health
    """
    self.parent = parent

#
# Workflow widget
class WorkflowWidget:
  def __init__(self, parent = None):
    self.moduleName = 'Workflow'
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
    self.layout = self.parent.layout()
    if not parent:
      self.setup()
      self.parent.show()

  def setup(self):

    self.level = 0
    self.workflow = ctk.ctkWorkflow()

    #workflowWidget = ctk.ctkWorkflowStackedWidget()
    workflowWidget = ctk.ctkWorkflowWidget()

    workflowWidget.setWorkflow( self.workflow )
    self.workflowWidget = workflowWidget

    workflowWidget.buttonBoxWidget().hideInvalidButtons = True
    workflowWidget.buttonBoxWidget().hideGoToButtons = True
    workflowWidget.buttonBoxWidget().backButtonFormat = '[<-]{back:#}"/"{!#}") "{back:name}(back:description)'
    workflowWidget.buttonBoxWidget().nextButtonFormat = '{next:#}"/"{!#}") "{next:name}(next:description)[->]'
    workflowWidget.workflowGroupBox().titleFormat = '[current:icon]{#}"/"{!#}") "{current:name}'
    workflowWidget.workflowGroupBox().hideWidgetsOfNonCurrentSteps = True

    #Creating each step of the workflow
    loadDataStep = Widgets.LoadDataStep()
    registrationStep = Widgets.RegisterStep()

    self.steps = []
    self.steps.append(loadDataStep)
    self.steps.append(registrationStep)
    for step in self.steps:
      # \todo: b) steps should be able to access the workflow widget automatically
      step.Workflow = self
      # \todo: f) have an option to setup all the gui at startup
      step.createUserInterface()

    #Connecting the created steps of the workflow
    self.workflow.addTransition(loadDataStep, registrationStep)

    self.layout.addWidget(workflowWidget)

    # Advanced settings
    self.advancedSettings = self.loadUi('AdvancedSettingsPanel.ui')
    levelComboBox = self.findWidget(self.advancedSettings, 'WorkflowLevelComboBox')
    levelComboBox.connect('currentIndexChanged(int)', self.setWorkflowLevel)
    self.layout.addWidget(self.advancedSettings)
    self.setWorkflowLevel(levelComboBox.currentIndex)

    # Reload
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "%s Reload" % self.moduleName
    self.layout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.reloadModule)

    # Starting and showing the module in layout
    self.workflow.start()

  def step(self, stepid):
    return [s for s in self.steps if s.stepid == stepid][0]

  def reloadModule(self,moduleName=None):
    """Generic reload method for any scripted module.
    ModuleWizard will subsitute correct default moduleName.
    """
    import imp, sys, os, slicer, qt

    if moduleName == None:
      moduleName = self.moduleName
    widgetName = moduleName + "Widget"

    # reload the source code
    # - set source file path
    # - load the module to the global space
    filePath = eval('slicer.modules.%s.path' % moduleName.lower())
    p = os.path.dirname(filePath)
    if not sys.path.__contains__(p):
      sys.path.insert(0,p)
    fp = open(filePath, "r")
    globals()[moduleName] = imp.load_module(
        moduleName, fp, filePath, ('.py', 'r', imp.PY_SOURCE))
    fp.close()

    # rebuild the widget
    # - find and hide the existing widget
    # - create a new widget in the existing parent
    parent = slicer.util.findChildren(name='%s Reload' % moduleName)[0].parent()
    for child in parent.children():
      try:
        child.hide()
      except AttributeError:
        pass

    self.layout.removeWidget(self.workflowWidget)
    self.workflowWidget.deleteLater()
    self.workflowWidget = None

    # Remove spacer items
    item = parent.layout().itemAt(0)
    while item:
      parent.layout().removeItem(item)
      item = parent.layout().itemAt(0)
    # create new widget inside existing parent
    globals()[widgetName.lower()] = eval(
        'globals()["%s"].%s(parent)' % (moduleName, widgetName))
    globals()[widgetName.lower()].setup()

  def loadUi(self, uiFileName):
    loader = qt.QUiLoader()
    moduleName = 'Workflow'
    scriptedModulesPath = eval('slicer.modules.%s.path' % moduleName.lower())
    scriptedModulesPath = os.path.dirname(scriptedModulesPath)
    path = os.path.join(scriptedModulesPath, 'Widgets', 'Resources', 'UI', uiFileName)

    qfile = qt.QFile(path)
    qfile.open(qt.QFile.ReadOnly)
    widget = loader.load(qfile)
    widget.setAutoFillBackground(False)

    return widget

  def findWidget(self, widget, objectName):
    if widget.objectName == objectName:
        return widget
    else:
        children = []
        for w in widget.children():
            resulting_widget = self.findWidget(w, objectName)
            if resulting_widget:
                return resulting_widget
        return None

  def setWorkflowLevel(self, level):
    self.level = level
    for step in self.steps:
      step.setWorkflowLevel(level)
