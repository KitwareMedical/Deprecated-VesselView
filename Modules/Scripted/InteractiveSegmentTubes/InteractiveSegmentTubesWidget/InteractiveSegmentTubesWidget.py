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

from InteractiveSegmentTubesLogic import *

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

    # Connect widget internal signals here:
    self.get('SeedsSizeSliderWidget').connect('valueChanged(double)', self.updateMRMLFromWidget)
    self.get('SeedsShowStatusCheckBox').connect('stateChanged(int)', self.updateMRMLFromWidget)

    # Connect widget signals to logic here:
    self.get('OutputNodeComboBox').connect('nodeAddedByUser(vtkMRMLNode*)', self.logic.addDisplayNodes)
    self.logic.setGUICallback(self.onSegmentTubesUpdated)

    self.get('InputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.setInputNode)
    self.get('OutputNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.setOutputNode)

    # Connect signals that needs to be treated by both logic and GUI here:
    self.get('ApplyPushButton').connect('toggled(bool)', self.run)
    self.get('SeedPointNodeComboBox').connect('currentNodeChanged(vtkMRMLNode*)', self.setSeedNode)

  def enter(self):
    slicer.app.layoutManager().setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
    self.updateWidgetFromMRML()

    interactionNode = slicer.app.applicationLogic().GetInteractionNode()
    interactionNode.SetPlaceModePersistence(1)

  def exit(self):
    pass

  def run( self, run ):
    if run:
      self.get('ApplyPushButton').setText('Stop')
    else:
      self.get('ApplyPushButton').setText('Start')
    self.logic.run(run)

  def onSegmentTubesUpdated( self, cliNode, *unused ):
    if not cliNode:
      return

    if not cliNode.IsBusy():
      print 'Segment Tubes %s' % cliNode.GetStatusString()

    self.get('ApplyPushButton').enabled = not self.logic.isInTransitionState()
    self.get('CLIProgressBar').setCommandLineModuleNode(cliNode)

  def setInputNode( self, inputNode ):
    self.get('ApplyPushButton').setChecked(False)
    self.logic.setInputNode(inputNode)

  def setOutputNode( self, outputNode ):
    self.get('ApplyPushButton').setChecked(False)
    self.logic.setOutputNode(outputNode)

  def setSeedNode( self, seedNode ):
    self.get('ApplyPushButton').setChecked(False)

    # Observe seed node to update GUI
    if not self.hasObserver(seedNode, 'ModifiedEvent', self.updateWidgetFromMRML):
      self.removeObservers('ModifiedEvent', 'vtkMRMLMarkupsFiducialNode')
      self.addObserver(seedNode, 'ModifiedEvent', self.updateWidgetFromMRML)

    seedDisplayNode = seedNode.GetMarkupsDisplayNode() if seedNode else None

    # Observe seed display node as well
    if not self.hasObserver(seedDisplayNode, 'ModifiedEvent', self.updateWidgetFromMRML):
      self.removeObservers('ModifiedEvent', 'vtkMRMLMarkupsDisplayNode')
      self.addObserver(seedDisplayNode, 'ModifiedEvent', self.updateWidgetFromMRML)

    self.logic.setSeedNode(seedNode)
    self.updateWidgetFromMRML()

  def updateMRMLFromWidget( self ):
    seedNode = self.get('SeedPointNodeComboBox').currentNode()
    if seedNode:

      seedDisplayNode = seedNode.GetMarkupsDisplayNode() if seedNode else None
      if seedDisplayNode:
        disabledModify = seedDisplayNode.StartModify()
        scale = self.get('SeedsSizeSliderWidget').value
        seedDisplayNode.SetGlyphScale(scale)

        textScale = scale if self.get('SeedsShowStatusCheckBox').isChecked() else 0.0
        seedDisplayNode.SetTextScale(textScale)
        seedDisplayNode.EndModify(disabledModify)

  def updateWidgetFromMRML( self, *unused ):
    seedNode = self.get('SeedPointNodeComboBox').currentNode()
    seedDisplayNode = None

    seedDisplayNode = seedNode.GetMarkupsDisplayNode() if seedNode else None
    if seedDisplayNode:
      self.get('SeedsSizeSliderWidget').value = seedDisplayNode.GetGlyphScale()
      self.get('SeedsShowStatusCheckBox').setChecked(seedDisplayNode.GetTextScale() > 1e-6)

  #-----------------------------------------------------------------------------
  # Utilities functions
  #-----------------------------------------------------------------------------
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
