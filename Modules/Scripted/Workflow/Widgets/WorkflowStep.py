import imp, sys, os
from __main__ import qt, ctk, vtk, slicer

class WorkflowStep( ctk.ctkWorkflowWidgetStep ) :

  def __init__( self ):
    pass

  def createUserInterface( self ):
    # if the user interface has already be created, quit
    # \todo: f) have an option to setup all the gui at startup
    if hasattr(self, 'widget'):
      return
    self.setupUi()
    self.widget.setMRMLScene(slicer.mrmlScene)
    self.setWorkflowLevel(self.Workflow.level)

  def setupUi( self ):
    '''\todo automatically retrieve the ui filename.'''
    #self.loadUI('LoadDataStep.ui')
    pass

  def loadUi(self, uiFileName):
    widget = self.Workflow.loadUi(uiFileName)
    layout = qt.QVBoxLayout(self)
    layout.setContentsMargins(0,0,0,0)
    layout.addWidget(widget)
    self.setLayout(layout)
    self.widget = widget
    return widget

  def get(self, objectName):
    return self.findWidget(self.widget, objectName)

  def step(self, stepid):
    return self.Workflow.step(stepid)

  def getChildren(self, object):
    '''Return the list of the children and grand children of a Qt object'''
    children = object.children()
    allChildren = list(children)
    for child in children:
      allChildren.extend( self.getChildren(child) )
    return allChildren

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

  def removeObservers(self, method):
    for o, e, m, g, t in self.Observations:
      if method == m:
        o.RemoveObserver(t)
        self.Observations.remove([o, e, m, g, t])

  def addObserver(self, object, event, method, group = 'none'):
    if self.hasObserver(object, event, method):
      print 'already has observer'
      return
    tag = object.AddObserver(event, method)
    self.Observations.append([object, event, method, group, tag])

  def hasObserver(self, object, event, method):
    for o, e, m, g, t in self.Observations:
      if o == object and e == event and m == method:
        return True
    return False

  def observer(self, event, method):
    for o, e, m, g, t in self.Observations:
      if e == event and m == method:
        return o
    return None

  def getCLINode(self, cliModule):
    """ Return the cli node to use for a given CLI module. Create the node in
    scene if needed. Return None in the case of scripted module.
    """
    cliNode = slicer.mrmlScene.GetFirstNodeByName(cliModule.title)
    # Also check path to make sure the CLI isn't a scripted module
    if (cliNode == None) and ('qt-scripted-modules' not in cliModule.path):
      cliNode = slicer.cli.createNode(cliModule)
      cliNode.SetName(cliModule.title)
    return cliNode

  def loadLabelmapFile(self, title, fileType, nodeComboBox):
    volumeNode = self.loadFile(title, fileType, nodeComboBox)
    if volumeNode != None:
      volumesLogic = slicer.modules.volumes.logic()
      volumesLogic.SetVolumeAsLabelMap(volumeNode, 1)
      nodeComboBox.setCurrentNode(volumeNode)

  def loadFile(self, title, fileType, nodeComboBox):
    manager = slicer.app.ioManager()
    loadedNodes = vtk.vtkCollection()
    properties = {}
    res = manager.openDialog(fileType, slicer.qSlicerFileDialog.Read, properties, loadedNodes)
    loadedNode = loadedNodes.GetItemAsObject(0)
    if res == True:
      nodeComboBox.setCurrentNode(loadedNode)
    self.reset3DViews()
    return loadedNode

  def saveFile(self, title, fileType, fileSuffix, nodeComboBox):
    self.saveNode(title, fileType, fileSuffix, nodeComboBox.currentNode())

  def saveNode(self, title, fileType, fileSuffix, node):
    manager = slicer.app.ioManager()
    properties = {}
    properties['nodeID'] = node.GetID()
    properties['defaultFileName'] = node.GetName() + fileSuffix
    manager.openDialog(fileType, slicer.qSlicerFileDialog.Write, properties)

  def reset3DViews(self):
    # Reset focal view around volumes
    manager = slicer.app.layoutManager()
    for i in range(0, manager.threeDViewCount):
      manager.threeDWidget(i).threeDView().resetFocalPoint()
      rendererCollection = manager.threeDWidget(i).threeDView().renderWindow().GetRenderers()
      for i in range(0, rendererCollection.GetNumberOfItems()):
        rendererCollection.GetItemAsObject(i).ResetCamera()

  def resetSliceViews(self):
    # Reset focal view around volumes
    manager = slicer.app.layoutManager()
    for i in manager.sliceViewNames():
      manager.sliceWidget(i).sliceController().fitSliceToBackground()

  def openModule(self, moduleName):
    slicer.util.selectModule(moduleName)

  def getFirstNodeByNameAndClass(self, name, className):
    nodes = slicer.mrmlScene.GetNodesByClass(className)
    nodes.UnRegister(nodes)
    for i in range(0, nodes.GetNumberOfItems()):
      node = nodes.GetItemAsObject(i)
      if node.GetName() == name:
        return node
    return None

  def setWorkflowLevel(self, level):
    widgets = self.getChildren(self.widget)
    for widget in widgets:
      workflow = widget.property('workflow')
      if workflow != None:
        widget.setVisible( str(level) in workflow )
    # Potentially enable/disable next button
    self.validate(None)

  def validate( self, desiredBranchId = None ):
    '''Check whether the step is valid or not. A valid step means the user can
    go forward to the next step.
    To be reimplemented by the step.'''
    self.validateStep(False, desiredBranchId)

  def validateStep(self, valid, desiredBranchId):
    valid = valid or (self.Workflow.level > 0)
    # If desiredBranchId is valid, it means the validation comes from ctk.
    # It should then be passed through to go to the next step.
    if desiredBranchId != None:
      super( WorkflowStep, self ).validate(valid, desiredBranchId)
    # Enable/Disable next step button.
    # \todo: c) make it automatic in ctkWorkflow
    if (self.Workflow.workflow.currentStep() ==
        self.Workflow.workflow.step(self.stepid)):
      self.Workflow.workflowWidget.buttonBoxWidget().nextButton().enabled = valid

  def onEntry(self, comingFrom, transitionType):
    '''Can be reimplemented by the step'''
    comingFromId = "None"
    if comingFrom: comingFromId = comingFrom.id()
    super( WorkflowStep, self ).onEntry(comingFrom, transitionType)
    # Don't pass a valid step as it would automatically jump to the step.
    self.validate(None)

  def onExit(self, goingTo, transitionType):
    '''Can be reimplemented by the step'''
    goingToId = "None"
    if goingTo: goingToId = goingTo.id()
    super( WorkflowStep, self ).onExit(goingTo, transitionType)
