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

class InteractiveSegmentTubes:
  def __init__(self, parent):
    import string
    parent.title = "Interactive Segment Tubes"
    parent.categories = ["TubeTK"]
    parent.contributors = ["Johan Andruejol (Kitware)"]
    parent.helpText = string.Template("""""")
    parent.acknowledgementText = """"""
    #parent.icon = qt.QIcon('')
    self.parent = parent

#
# Abstract Interactive Segment Tubes
#

class AbstractInteractiveSegmentTubes:
  def __init__(self):
    self._Observations = []

  def removeObservers(self, method, nodeType = ''):
    for o, e, m, g, t in self._Observations:
      if nodeType != '' and o.GetClassName() != nodeType:
        continue

      if method == m:
        o.RemoveObserver(t)
        self._Observations.remove([o, e, m, g, t])

  def addObserver(self, object, event, method, group = 'none'):
    if object is None or self.hasObserver(object, event, method):
      return
    tag = object.AddObserver(event, method)
    self._Observations.append([object, event, method, group, tag])

  def hasObserver(self, object, event, method):
    for o, e, m, g, t in self._Observations:
      if o == object and e == event and m == method:
        return True
    return False

  def observer(self, event, method):
    for o, e, m, g, t in self._Observations:
      if e == event and m == method:
        return o
    return None

#
# Interactive Segment Tubes widget
#

class InteractiveSegmentTubesWidget(AbstractInteractiveSegmentTubes):

  def __init__(self, parent=None):
    AbstractInteractiveSegmentTubes.__init__(self)

    self.widget = None
    self.moduleName = 'Interactive Segment Tubes'
    self.logic = InteractiveSegmentTubesLogic()

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
    self.loadUi('InteractiveSegmentTubes.ui')
    self.widget.setMRMLScene(slicer.mrmlScene)

    self.get('ApplyPushButton').connect('clicked(bool)', self.run)
    self.get('CancelPushButton').connect('clicked(bool)', self.cancel)

    self.get('OutputNodeComboBox').connect('nodeAddedByUser(vtkMRMLNode*)', self.logic.addDisplayNodes)
    self.get('SeedPointNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.updateWidgetFromMRML)

    self.get('SeedsNameLineEdit').connect('editingFinished()', self.updateMRMLFromWidget)
    self.get('SeedsSizeSliderWidget').connect('valueChanged(double)', self.updateMRMLFromWidget)
    self.get('SeedsNoNameCheckBox').connect('stateChanged(int)', self.enableSeedsName)

  def enter(self):
    slicer.app.layoutManager().setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
    self.updateMRMLFromWidget()

  def exit(self):
    pass

  def loadUi(self, uiFileName):
    loader = qt.QUiLoader()
    scriptedModulesPath = eval('slicer.modules.%s.path' % self.moduleName.replace(" ", "").lower())
    scriptedModulesPath = os.path.dirname(scriptedModulesPath)
    path = os.path.join(scriptedModulesPath, 'Resources', 'UI', uiFileName)

    qfile = qt.QFile(path)
    qfile.open(qt.QFile.ReadOnly)
    uiWidget = loader.load(qfile)
    uiWidget.setAutoFillBackground(False)
    uiWidget.setPalette(slicer.app.palette())

    self.layout.setContentsMargins(0,0,0,0)
    self.layout.addWidget(uiWidget)
    self.widget = uiWidget

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

  def get(self, objectName):
    return self.findWidget(self.widget, objectName)

  def segmentTubesParameters( self ):
    parameters = {}
    parameters['inputVolume'] = self.get('InputNodeComboBox').currentNode()
    parameters['OutputNode'] = self.get('OutputNodeComboBox').currentNode()
    parameters['outputTubeFile'] = self.logic.getFilenameFromNode(parameters['OutputNode'], '.tre')
    parameters['seedP'] = self.get('SeedPointNodeComboBox').currentNode()

    return parameters

  def run( self, run ):
    parameters = self.segmentTubesParameters()
    self.get('ApplyPushButton').setChecked(run)
    self.logic.run(run, parameters, self.onSegmentTubesUpdated)

  def cancel( self, run ):
    self.run(False)

  def onSegmentTubesUpdated( self, cliNode ):
    if not cliNode:
      return

    self.get('ApplyPushButton').setChecked(cliNode.IsBusy())
    self.get('ApplyPushButton').enabled = not (cliNode.GetStatusString() == 'Cancelling')
    self.get('CLIProgressBar').setCommandLineModuleNode(cliNode)

  def enableSeedsName( self, checked ):
    self.get('SeedsNameLineEdit').enabled = not checked
    self.updateMRMLFromWidget()

  def updateWidgetFromMRML( self, *unused):
    seedNode = self.get('SeedPointNodeComboBox').currentNode()
    seedDisplayNode = None

    # Seed
    #
    if not self.hasObserver(seedNode, 'ModifiedEvent', self.updateWidgetFromMRML):
      self.removeObservers('ModifiedEvent', 'vtkMRMLMarkupsFiducialNode')
      self.addObserver(seedNode, 'ModifiedEvent', self.updateWidgetFromMRML)

    markupLogic = slicer.modules.markups.logic()
    markupLogic.SetActiveListID(seedNode)

    self.get('SeedsNameLineEdit').enabled = seedNode is not None
    if seedNode:
      # Seed label
      self.get('SeedsNameLineEdit').text = seedNode.GetMarkupLabelFormat()

      # Seed scale
      seedDisplayNode = seedNode.GetMarkupsDisplayNode()

    # Seed display
    #
    if not self.hasObserver(seedDisplayNode, 'ModifiedEvent', self.updateWidgetFromMRML):
      self.removeObservers('ModifiedEvent', 'vtkMRMLMarkupsDisplayNode')
      self.addObserver(seedDisplayNode, 'ModifiedEvent', self.updateWidgetFromMRML)

    if seedDisplayNode:
      self.get('SeedsSizeSliderWidget').value = seedDisplayNode.GetGlyphScale()

  def updateMRMLFromWidget( self ):
    seedNode = self.get('SeedPointNodeComboBox').currentNode()
    if seedNode:
      # Seed label
      seedLabel = ''
      if self.get('SeedsNameLineEdit').enabled:
        seedLabel = self.get('SeedsNameLineEdit').text
      seedNode.SetMarkupLabelFormat(seedLabel)

      markupLogic = slicer.modules.markups.logic()
      markupLogic.RenameAllMarkupsFromCurrentFormat(seedNode)

      # Seed scale
      scale = self.get('SeedsSizeSliderWidget').value
      displayNode = seedNode.GetMarkupsDisplayNode()
      if displayNode:
        disabledModify = displayNode.StartModify()
        displayNode.SetGlyphScale(scale)
        displayNode.SetTextScale(scale)
        displayNode.EndModify(disabledModify)

#
# Interactive Segment Tubes logic
#

class InteractiveSegmentTubesLogic(AbstractInteractiveSegmentTubes):
  """TODO"""
  def __init__(self):
    AbstractInteractiveSegmentTubes.__init__(self)

    self._CurrentOutputNode = None

    self.SegmentTubesUpdatedCallback = None

  def addDisplayNodes( self, node ):
    if node:
      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.AddDisplayNodes(node)

  def getFilenameFromNode(self, node):
    if not node:
      return ''

    storageNode = node.GetNthStorageNode(0)
    if not storageNode or not storageNode.GetFileName():
      # Save it in temp dir
      tempPath = slicer.app.temporaryPath
      nodeName = os.path.join(tempPath, node.GetName() + 'tre')
      if os.path.isfile(nodeName):
        os.remove(nodeName)

      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.SaveSpatialObject(nodeName, node)
      return nodeName

    return storageNode.GetFileName()

  def getCLINode(self, module, nodeName = None):
    if not nodeName:
      nodeName = module.title

    cliNode = slicer.mrmlScene.GetFirstNodeByName(nodeName)
    # Also check path to make sure the CLI isn't a scripted module
    if (cliNode == None) and ('qt-scripted-modules' not in module.path):
      cliNode = slicer.cli.createNode(module)
      cliNode.SetName(nodeName)
    return cliNode

  def run( self, run, parameters, callback ):
    self.SegmentTubesUpdatedCallback = callback

    if run:
      cliNode = self.getCLINode(slicer.modules.segmenttubes)
      self.observeCLINode(cliNode, self.onCLIModified)
      self._CurrentOutputNode = parameters['OutputNode']
      cliNode = slicer.cli.run(slicer.modules.segmenttubes, cliNode, parameters, wait_for_completion = False)
    else:
      cliNode = self.observer(
        slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
        self.onCLIModified)
      cliNode.Cancel()

  def onCLIModified( self, cliNode, event ):
    if cliNode.GetStatusString() == 'Completed':
      # Automatically load the output tube file
      spatialObjectLogic = slicer.modules.spatialobjects.logic()
      spatialObjectLogic.SetSpatialObject(self._CurrentOutputNode, cliNode.GetParameterAsString('outputTubeFile'))

    if not cliNode.IsBusy():
      print 'Segment Tubes %s' % cliNode.GetStatusString()
      self.removeObservers(self.onCLIModified)

    if self.SegmentTubesUpdatedCallback:
      self.SegmentTubesUpdatedCallback(cliNode)

  def observeCLINode(self, cliNode, onCLINodeModified = None):
    if cliNode != None and onCLINodeModified != None:
      self.addObserver(cliNode,
                       slicer.vtkMRMLCommandLineModuleNode().StatusModifiedEvent,
                       onCLINodeModified)
