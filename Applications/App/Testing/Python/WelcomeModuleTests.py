import os
import unittest
from __main__ import vtk, qt, ctk, slicer

#
# WelcomeModuleTests
#

class WelcomeModuleTests:
  def __init__(self, parent):
    parent.title = "WelcomeModuleTests" # TODO make this more human readable by adding spaces
    parent.categories = ["Testing.TestCases"]
    parent.dependencies = []
    parent.contributors = ["Johan Andruejol (Kitware)"] # replace with "Firstname Lastname (Org)"
    parent.helpText = """
    """
    parent.acknowledgementText = """TODO""" # replace with organization, grant and thanks.
    self.parent = parent

    # Add this test to the SelfTest module's list for discovery when the module
    # is created.  Since this module may be discovered before SelfTests itself,
    # create the list if it doesn't already exist.
    try:
      slicer.selfTests
    except AttributeError:
      slicer.selfTests = {}
    slicer.selfTests['WelcomeModuleTests'] = self.runTest

  def runTest(self):
    tester = WelcomeModuleTestsTest()
    tester.runTests()

#
# qWelcomeModuleTestsTest
#

class WelcomeModuleTestsTest(unittest.TestCase):

  def delayDisplay(self,message,msec=1000):
    """This utility method displays a small dialog and waits.
    This does two things: 1) it lets the event loop catch up
    to the state of the test so that rendering and widget updates
    have all taken place before the test continues and 2) it
    shows the user/developer/tester the state of the test
    so that we'll know when it breaks.
    """
    print(message)
    self.info = qt.QDialog()
    self.infoLayout = qt.QVBoxLayout()
    self.info.setLayout(self.infoLayout)
    self.label = qt.QLabel(message,self.info)
    self.infoLayout.addWidget(self.label)
    qt.QTimer.singleShot(msec, self.info.close)
    self.info.exec_()

  def getTestMethodNames(self):
    methods = []
    for method in dir(self):
      if (callable(getattr(self, method)) and method.find('test_') != -1):
        methods.append(method)
    return methods

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.util.selectModule('Welcome')
    slicer.mrmlScene.Clear(0)

  def tearDown(self):
    slicer.util.selectModule('WelcomeModuleTests')

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

  def runTests(self):
    """Run as few or as many tests as needed here.
    """
    for methodName in self.getTestMethodNames():
      self.runTest(methodName)

  def runTest(self, method):
    self.setUp()
    getattr(self, method)()
    self.tearDown()

  def test_TestReturnToWelcome(self):
    self.delayDisplay('test_TestReturnToWelcome')

    # Find the return to welcome screen button
    mainWindow = slicer.util.mainWindow()
    returnToWelcomScreenPushButton = self.findWidget(mainWindow, 'ReturnToWelcomeScreenButton')

    slicer.util.selectModule(slicer.modules.volumes)
    returnToWelcomScreenPushButton.click()
    self.assertEqual(slicer.util.selectedModule(), 'Welcome')

    slicer.util.selectModule(slicer.modules.welcome)
    returnToWelcomScreenPushButton.click()
    self.assertEqual(slicer.util.selectedModule(), 'Welcome')

    self.delayDisplay('Test passed!')

  def test_TestHiddenWidget(self):
    self.delayDisplay('test_TestHiddenWidget')
    mainWindow = slicer.util.mainWindow()
    # Test hidden widgets
    self.assertFalse(mainWindow.moduleSelector().isVisible())
    hiddenWidgetNames = ['CaptureToolBar',
                         'DialogToolBar',
                         'MainToolBar',
                         'ModuleToolBar',
                         'MouseModeToolBar',
                         'ViewersToolBar',
                         'ViewToolBar'
                         ]
    for hiddenWidgetName in hiddenWidgetNames:
       w = self.findWidget(mainWindow, hiddenWidgetName)
       self.assertFalse(w.isVisible())

    self.delayDisplay('Test passed!')

  def test_TestLayout(self):
    self.delayDisplay('test_TestLayout')

    self.assertEquals(slicer.app.layoutManager().layout, slicer.vtkMRMLLayoutNode.SlicerLayoutUserView)

    self.delayDisplay('Test passed!')

#
# qWelcomeModuleTestWidget
#

class WelcomeModuleTestsWidget():
  def __init__(self, parent = None):
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

    self.moduleName = 'WelcomeModuleTests'
    self.tester = WelcomeModuleTestsTest()

  def setup(self):
    # Instantiate and connect widgets ...

    # reload button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "Tests Reload"
    self.layout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)

    # reload and test button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadAndTestButton = qt.QPushButton("Reload and Test")
    self.reloadAndTestButton.toolTip = "Reload this module and then run the self tests."
    self.layout.addWidget(self.reloadAndTestButton)
    self.reloadAndTestButton.connect('clicked()', self.onReloadAndTest)

    self.testButton = qt.QPushButton('Run Tests')
    self.layout.addWidget(self.testButton)
    self.testButton.connect('clicked(bool)', self.tester.runTests)

    # Add vertical spacer
    self.layout.addStretch(1)

  def onReload(self):
    """Generic reload method for any scripted module.
    ModuleWizard will subsitute correct default.
    """
    globals()[self.moduleName] = slicer.util.reloadScriptedModule(self.moduleName)

  def onReloadAndTest(self):
    self.onReload()
    self.tester.runTests()
