import os
import unittest
from __main__ import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging

#
# VesselDisplay
#

class VesselDisplay(ScriptedLoadableModule):

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Vessel Display" 
    self.parent.categories = ["Examples"]
    self.parent.dependencies = ["SubjectHierarchy"]
    self.parent.contributors = [""] 
    self.parent.helpText = """    """
    self.parent.acknowledgementText = """ """ 
#
# VesselDisplayWidget
#

class VesselDisplayWidget(ScriptedLoadableModuleWidget):

  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    # Instantiate and connect widgets ...
    #Display Region. Obtained from Subject Hierarchy
    displayCollapsibleButton = ctk.ctkCollapsibleButton()
    displayCollapsibleButton.text = "Display"
    self.layout.addWidget(displayCollapsibleButton)

    # Layout within the display collapsible button
    displayFormLayout = qt.QHBoxLayout()
    displayCollapsibleButton.setLayout(displayFormLayout)
    
    self.subjectHierarchyTreeView = slicer.qMRMLSubjectHierarchyTreeView()
    self.subjectHierarchyTreeView.setMRMLScene(slicer.app.mrmlScene())
    self.subjectHierarchyTreeView.setColumnHidden(self.subjectHierarchyTreeView.sceneModel().idColumn,True)
    displayFormLayout.addWidget(self.subjectHierarchyTreeView)
    self.subjectHierarchyTreeView.connect("currentNodeChanged(vtkMRMLNode*)", self.onSubjectHierarchyNodeSelect)

    #Properties Region
    self.displayPropertiesCollapsibleButton = ctk.ctkCollapsibleButton()
    self.displayPropertiesCollapsibleButton.text = "Display Properties"
    self.layout.addWidget(self.displayPropertiesCollapsibleButton)
    self.displayPropertiesCollapsibleButton.enabled = False

    # Layout within the display-properties collapsible button
    displayPropertiesFormLayout = qt.QHBoxLayout()
    self.displayPropertiesCollapsibleButton.setLayout(displayPropertiesFormLayout)

    # Volume display properties
    self.volumeDisplayWidget = slicer.qSlicerVolumeDisplayWidget()
    displayPropertiesFormLayout.addWidget(self.volumeDisplayWidget)
    self.volumeDisplayWidget.hide()

    #Spacial Objects display properties
    self.spacialObjectsWidget = slicer.qSlicerSpatialObjectsModuleWidget()
    displayPropertiesFormLayout.addWidget(self.spacialObjectsWidget)
    self.spacialObjectsWidget.hide()

  def onSubjectHierarchyNodeSelect(self):
    self.displayPropertiesCollapsibleButton.enabled = True
    #get current node from subject hierarchy
    currentInstance = slicer.qSlicerSubjectHierarchyPluginHandler().instance()
    currentNode = currentInstance.currentNode()
    if currentNode != None:
    #current node is subject hierarchy node
      currentAssociatedNode = currentNode.GetAssociatedNode()      
      if currentAssociatedNode !=None:
        currentNodetype = currentAssociatedNode.GetNodeTagName()
        print currentNodetype
        if 'Volume' in currentNodetype :
          self.volumeDisplayWidget.show()
          self.spacialObjectsWidget.hide()
          self.volumeDisplayWidget.setMRMLVolumeNode(currentAssociatedNode)
          slicer.app.layoutManager().setLayout(3)
          return
        elif 'Spatial' in currentNodetype :
          self.volumeDisplayWidget.hide()
          self.spacialObjectsWidget.show()
          self.spacialObjectsWidget.setSpatialObjectsNode(currentAssociatedNode)
          slicer.app.layoutManager().setLayout(4)
          return
    self.displayPropertiesCollapsibleButton.enabled = False
#
# VesselDisplayLogic
#

class VesselDisplayLogic(ScriptedLoadableModuleLogic):

  def hasImageData(self,volumeNode):
    """This is an example logic method that
    returns true if the passed in volume
    node has valid image data
    """
    if not volumeNode:
      logging.debug('hasImageData failed: no volume node')
      return False
    if volumeNode.GetImageData() == None:
      logging.debug('hasImageData failed: no image data in volume node')
      return False
    return True
#
# VesselDisplayTest
#

class VesselDisplayTest(ScriptedLoadableModuleTest):

  def runTest(self):
    self.test_VesselDisplay1()

  def test_VesselDisplay1(self):

    self.delayDisplay("Starting the test")
    #
    # first, get some data
    #
    import urllib
    downloads = (
        ('http://slicer.kitware.com/midas3/download?items=5767', 'FA.nrrd', slicer.util.loadVolume),
        )

    for url,name,loader in downloads:
      filePath = slicer.app.temporaryPath + '/' + name
      if not os.path.exists(filePath) or os.stat(filePath).st_size == 0:
        logging.info('Requesting download %s from %s...\n' % (name, url))
        urllib.urlretrieve(url, filePath)
      if loader:
        logging.info('Loading %s...' % (name,))
        loader(filePath)
    self.delayDisplay('Finished with download and loading')

    volumeNode = slicer.util.getNode(pattern="FA")
    logic = VesselDisplayLogic()
    logic.hasImageData(volumeNode)
    self.delayDisplay('Test passed!')
