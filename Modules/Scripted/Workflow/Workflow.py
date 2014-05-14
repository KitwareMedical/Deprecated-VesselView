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
import json

class Workflow:
  def __init__(self, parent):
    parent.title = "Workflow"
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

    parent.icon = qt.QIcon("%s/DesktopIcon.png" % Widgets.ICON_DIR)

#
# Workflow widget
class WorkflowWidget:
  def __init__(self, parent = None):
    self.moduleName = 'Workflow'
    self._layouts = []
    self.maximumNumberOfInput = 3
    self._CurrentViewID = 1

    self._CurrentViewNodes = {}
    for i in range(1, self.maximumNumberOfInput + 1):
      subDictionnary = {
        'Background' : '',
        'Foreground' : '',
        'Label' : '',
        }
      self._CurrentViewNodes['Input%i' %i] = subDictionnary

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

    self.setupLayouts()

  def setup(self):

    self.level = 0
    self.workflow = ctk.ctkWorkflow(self.parent)

    #workflowWidget = ctk.ctkWorkflowStackedWidget()
    workflowWidget = ctk.ctkWorkflowWidget()

    workflowWidget.setWorkflow( self.workflow )
    self.workflowWidget = workflowWidget
    self.Settings = None

    workflowWidget.buttonBoxWidget().hideInvalidButtons = True
    workflowWidget.buttonBoxWidget().hideGoToButtons = True
    workflowWidget.buttonBoxWidget().backButtonFormat = '[<-]{back:#}"/"{!#}") "{back:name}(back:description)'
    workflowWidget.buttonBoxWidget().nextButtonFormat = '{next:#}"/"{!#}") "{next:name}(next:description)[->]'
    workflowWidget.workflowGroupBox().titleFormat = '[current:icon]{#}"/"{!#}") "{current:name}'
    workflowWidget.workflowGroupBox().hideWidgetsOfNonCurrentSteps = True

    #Creating each step of the workflow
    self.steps = [Widgets.InitialStep(),
                  Widgets.LoadDataStep(),
                  Widgets.ResampleStep(),
                  Widgets.RegisterStep(),
                  Widgets.SegmentationStep(),
                  Widgets.VesselEnhancementStep(),
                  #Widgets.ExtractSkeletonStep(),
                  #Widgets.VesselExtractionStep(),
                 ]
    i = 0
    for step in self.steps:
      # \todo: b) steps should be able to access the workflow widget automatically
      step.Workflow = self
      # \todo: f) have an option to setup all the gui at startup
      step.createUserInterface()

      #Connecting the created steps of the workflow
      if i != 0:
        self.workflow.addTransition(self.steps[i-1], self.steps[i])
      i += 1

    self.layout.addWidget(workflowWidget)

    # Settings
    self.Settings = self.loadUi('WorkflowSettingsPanel.ui')

    # Display settings
    opacitySlider = self.findWidget(self.Settings, 'OpacityRatioDoubleSlider')
    opacitySlider.connect('valueChanged(double)', self.setOpacityRatio)
    self.setOpacityRatio(opacitySlider.value)

    # Hide and disable until step 1 is validated
    self.setDisplaySettingsVisible(False)
    self.setDisplaySettingsEnabled(False)

    # Advanced settings
    levelComboBox = self.findWidget(self.Settings, 'WorkflowLevelComboBox')
    levelComboBox.connect('currentIndexChanged(int)', self.setWorkflowLevel)
    self.setWorkflowLevel(levelComboBox.currentIndex)

    self.reloadButton = self.findWidget(self.Settings, 'ReloadPushButton')
    self.reloadButton.connect('clicked()', self.reloadModule)

    # Add CLI progress bar
    self.CLIProgressBar = slicer.qSlicerCLIProgressBar()
    self.CLIProgressBar.setStatusVisibility(self.CLIProgressBar.VisibleAfterCompletion)
    self.CLIProgressBar.setProgressVisibility(self.CLIProgressBar.HiddenWhenIdle)
    self.Settings.layout().insertWidget(1, self.CLIProgressBar) # insert after spacer

    # Insert settings before workflow's buttons
    collapsibleGroupBox = self.findWidget(self.workflowWidget.workflowGroupBox(), 'CollapsibleButton')
    collapsibleGroupBox.layout().addWidget(self.Settings)

    # Init naming and jsons.
    self.step('Initial').onPresetSelected()

    # Starting and showing the module in layout
    self.workflow.start()

  def step(self, stepid):
    for s in self.steps:
      if s.stepid == stepid:
        return s

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

  def setOpacityRatio(self, ratio):
    # 0 == all background <-> 1 == all foreground
    sliceCompositeNodes = slicer.mrmlScene.GetNodesByClass("vtkMRMLSliceCompositeNode")
    sliceCompositeNodes.SetReferenceCount(sliceCompositeNodes.GetReferenceCount()-1)
    for i in range(0, sliceCompositeNodes.GetNumberOfItems()):
      sliceCompositeNode = sliceCompositeNodes.GetItemAsObject(i)
      sliceCompositeNode.SetForegroundOpacity(ratio)

  def getProgressBar( self ):
    return self.CLIProgressBar

  def enter(self):
    # Collapse DataProbe as it takes screen real estate
    dataProbeCollapsibleWidget = self.findWidget(
      slicer.util.mainWindow(), 'DataProbeCollapsibleWidget')
    dataProbeCollapsibleWidget.checked = False

    self.updateLayout(self._CurrentViewID)

    for s in self.steps:
      s.updateFromCLIParameters()

  def getJsonParameters( self, module ):
    presets = self.step('Initial').getPresets()
    parameters = {}
    try:
      jsonFilePath = presets[module.name]
    except KeyError:
      return parameters

    jsonData = open(jsonFilePath)
    try:
      data = json.load(jsonData)
    except ValueError:
      print 'Could not read JSON file %s. Make sure the file is valid' % jsonFilePath
      return parameters

    # For all the parameters not already there, add the json parameters
    # Try to be as robust as possible
    jsonParametersList = data['ParameterGroups'][1]['Parameters']
    for p in jsonParametersList:
      try:
        parameters[p['Name']] = p['Value']
      except KeyError:
        print 'Could not find value for %s. Passing.' % p['Name']
        continue
    return parameters

  def updateConfiguration(self):
    config = self.step('Initial').getConfigurationData()
    if not config:
      return

    for step in self.steps:
      step.updateConfiguration(config)

  def setDisplaySettingsVisible( self, visible ):
    if not self.Settings:
      return
    displayGroupbox = self.findWidget(self.Settings, 'DisplaySettingsCollapsibleGroupBox')
    displayGroupbox.setVisible(visible)

  def setDisplaySettingsEnabled( self, enabled ):
    if not self.Settings:
      return

    opacitySlider = self.findWidget(self.Settings, 'OpacityRatioDoubleSlider')
    opacitySlider.setEnabled(enabled)

  def setViews( self, nodes ):
    if not nodes:
      return

    showDisplaySettings = False
    for i in range(1, self.maximumNumberOfInput + 1):
      input = 'Input%i' %i
      id = 'vtkMRMLSliceCompositeNode%s' %input
      sliceCompositeNode = slicer.mrmlScene.GetNodeByID(id)
      if not sliceCompositeNode:
        continue

      sliceCompositeNode.SetDoPropagateVolumeSelection(True)
      numberOfVolumeTypeVisible = 0
      for volumeType in ['Background', 'Foreground', 'Label']:
        try:
          self._CurrentViewNodes[input][volumeType] = nodes[input][volumeType]
        except:
          pass

        id = self._CurrentViewNodes[input][volumeType]
        getattr(sliceCompositeNode, 'Set%sVolumeID' % volumeType)(id)
        if id:
          numberOfVolumeTypeVisible = numberOfVolumeTypeVisible + 1
          showDisplaySettings = showDisplaySettings or numberOfVolumeTypeVisible > 1
      sliceCompositeNode.SetDoPropagateVolumeSelection(False)
    slicer.app.applicationLogic().FitSliceToAll()

    self.setDisplaySettingsVisible(showDisplaySettings)
    self.setDisplaySettingsEnabled(showDisplaySettings)

  def onNumberOfInputsChanged( self, numberOfInputs ):
    '''This function calls the 'onNumberOfInputsChanged' on all the steps. This
       should only be called on the workflow by the LoadData module.'''
    for step in self.steps:
      # Make sure that the steps have widgets already
      if hasattr(step, 'widget'):
        step.onNumberOfInputsChanged(numberOfInputs)

  def updateLayout( self, numberOfViews ):
    if numberOfViews not in range(1, self.maximumNumberOfInput + 1):
      print 'This should not happen, the number of inputs should be in [1, %i[' %(self.maximumNumberOfInput + 1)
      return

    layoutNode = slicer.mrmlScene.GetNthNodeByClass(0, "vtkMRMLLayoutNode")
    if layoutNode is None:
      return

    newLayout = slicer.vtkMRMLLayoutNode().SlicerLayoutUserView + numberOfViews
    self._CurrentViewID = numberOfViews
    layoutNode.SetViewArrangement(newLayout)
    slicer.app.applicationLogic().FitSliceToAll()

  def setupLayouts( self ):
    layoutNode = slicer.mrmlScene.GetNthNodeByClass(0, "vtkMRMLLayoutNode")
    if layoutNode is None:
      return

    if not self._layouts:
      tag = 'Input'
      for i in range(1, self.maximumNumberOfInput + 1):
        self._layouts.append(self._inputLayout(tag, i))

      oldView = layoutNode.GetViewArrangement()
      # The slice composite node are created when the layout is used.
      # To be able to manipulate thgem correctly, we'll select the layouts
      # after their creation
      for i, layout in enumerate(self._layouts, start=1):
        view = slicer.vtkMRMLLayoutNode().SlicerLayoutUserView + i
        layoutNode.AddLayoutDescription(view, layout)
        layoutNode.SetViewArrangement(view)

        # Prevent the layout slice composite node to update when a node is added
        sliceCompositeNode = slicer.mrmlScene.GetNodeByID('vtkMRMLSliceCompositeNode' + tag + str(i))
        if sliceCompositeNode:
          sliceCompositeNode.SetDoPropagateVolumeSelection(False)
        else:
          print('Developer error ! There should be a slice composite node here !')

  def _inputLayout( self, tag, numberOfInputs ):
    sliceItems = ''
    for i in range(1, numberOfInputs + 1):
      sliceItems = sliceItems + self._sliceItemLayout(tag + str(i), 'Axial', '#a9a9a9')

    return (
      "<layout type=\"vertical\" split=\"true\" >"
      "<item>"
      "<layout type=\"horizontal\">"
      "%s"
      "</layout>"
      " </item>"
      "</layout>"
      ) % sliceItems

  def _sliceItemLayout( self, tag, axe, color ):
    return (
      "<item>"
      "<view class=\"vtkMRMLSliceNode\" singletontag=\"%s\">"
      "<property name=\"orientation\" action=\"default\">%s</property>"
      "<property name=\"viewlabel\" action=\"default\">%s</property>"
      "<property name=\"viewcolor\" action=\"default\">%s</property>"
      "</view>"
      "</item>"
      ) % (tag, axe, tag, color)
