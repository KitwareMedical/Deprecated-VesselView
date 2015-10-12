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
from slicer.ScriptedLoadableModule import *

class Welcome(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    parent.title = "Welcome"
    parent.categories = ["TubeTK"]
    parent.contributors = ["Johan Andruejol (Kitware)"]
    parent.helpText = """You should not see this"""
    parent.acknowledgementText = """"""

    self.parent = parent

class WelcomeWidget(ScriptedLoadableModuleWidget):
  def setup(self):
    slicer.util.mainWindow().moduleSelector().connect('moduleSelected(QString)',
      self.onModuleSelected)
    self.onModuleSelected()

    # Collapse the data probe button by default
    dataProbeCollapsibleButton = slicer.util.findChildren(text='Data Probe')
    if dataProbeCollapsibleButton is not None:
        dataProbeCollapsibleButton[0].collapsed = True

  def hideWidget(self, parent, name):
    w = self.findWidget(parent, name)
    if w:
      w.hide()
    else:
      print('Could not find widget named %s' %name)

  def onModuleSelected(self, module = 'Welcome'):
    moduleIsWelcome = (module == 'Welcome')
    mainWindow = slicer.util.mainWindow()
    mainWindow.setPanelDockWidgetVisible(not moduleIsWelcome)

    if not moduleIsWelcome:
      return

    slicer.app.layoutManager().setLayout(
      slicer.vtkMRMLLayoutNode.SlicerLayoutUserView)
    mainWindow.moduleSelector().hide()
    self.hideWidget(mainWindow, 'CaptureToolBar')
    self.hideWidget(mainWindow, 'DialogToolBar')
    self.hideWidget(mainWindow, 'MainToolBar')
    #self.hideWidget(mainWindow, 'menubar')
    self.hideWidget(mainWindow, 'ModuleToolBar')
    self.hideWidget(mainWindow, 'MouseModeToolBar')
    self.hideWidget(mainWindow, 'ViewersToolBar')
    self.hideWidget(mainWindow, 'ViewToolBar')
    self.hideWidget(mainWindow, 'DefaultToolBar')

  ## UTILS ##
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
