import os
import unittest
from __main__ import vtk, qt, ctk, slicer
import urllib

#
# SlicerTestRecentFilesTests
#

class SlicerTestRecentFilesTests:
  def __init__(self, parent):
    parent.title = "SlicerTestRecentFilesTests"
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
    slicer.selfTests['SlicerTestRecentFilesTests'] = self.runTest

  def runTest(self):
    tester = SlicerTestRecentFilesTestsTest()
    tester.runTests()

#
# qSlicerTestRecentFilesTestsTest
#

class SlicerTestRecentFilesTestsTest(unittest.TestCase):

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
    slicer.mrmlScene.Clear(0)
    deleteHistoryAction = self.deleteRecentFilesHistoryAction()
    deleteHistoryAction.trigger()
    self.recentFilesChangedSignalTriggered = 0

  def tearDown(self):
    pass

  def runTests(self):
    """Run as few or as many tests as needed here.
    """
    for methodName in self.getTestMethodNames():
      self.runTest(methodName)

  def runTest(self, method):
    self.setUp()
    getattr(self, method)()
    self.tearDown()

  def deleteRecentFilesHistoryAction(self):
    mainWindow = slicer.util.mainWindow()
    self.assertTrue(mainWindow)

    recentlyLoadedMenu = slicer.util.findChildren(mainWindow, 'RecentlyLoadedMenu')[0]
    self.assertTrue(recentlyLoadedMenu)

    for action in recentlyLoadedMenu.actions():
      if action.text == 'Clear History':
         return action
    return None

  def openVolume(self, downloads):
    # perform the downloads if needed, then load
    for url,name,loader in downloads:
      filePath = slicer.app.temporaryPath + '/' + name
      if not os.path.exists(filePath) or os.stat(filePath).st_size == 0:
        self.delayDisplay('Requesting download %s from %s...\n' % (name, url))
        urllib.urlretrieve(url, filePath)
      if loader:
        self.delayDisplay('Loading %s...\n' % (name,))
        loader(filePath)
    self.delayDisplay('Finished with download and loading\n')

  def onRecentFilesChanged(self):
    self.recentFilesChangedSignalTriggered = self.recentFilesChangedSignalTriggered + 1

  def test_LoadData(self):
    self.delayDisplay('test_TestReturnToWelcome')

    mainWindow = slicer.util.mainWindow()
    self.assertTrue(mainWindow)
    self.assertEqual(len(mainWindow.recentlyLoadedPaths()), 0)
    
    mainWindow.connect('recentlyLoadedFilesChanged()', self.onRecentFilesChanged)

    downloads = (
        ('http://slicer.kitware.com/midas3/download?items=5767', 'FA.nrrd', slicer.util.loadVolume),
        )
    self.openVolume(downloads)

    self.assertEqual(self.recentFilesChangedSignalTriggered, 1)
    self.assertEqual(len(mainWindow.recentlyLoadedPaths()), 1)
    path = mainWindow.recentlyLoadedPaths()[0]
    self.assertTrue(path.find('FA.nrrd') > 0)

    self.delayDisplay('Test passed!')

  def test_ClearHistory(self):
    self.delayDisplay('test_ClearHistory')

    self.test_LoadData()
    deleteHistoryAction = self.deleteRecentFilesHistoryAction()
    deleteHistoryAction.trigger()

    self.assertEqual(self.recentFilesChangedSignalTriggered, 2)
    self.assertEqual(len(slicer.util.mainWindow().recentlyLoadedPaths()), 0)

    self.delayDisplay('Test passed!')

#
# qSlicerTestRecentFilesTestsWidget
#

class SlicerTestRecentFilesTestsWidget():
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

    self.moduleName = 'SlicerTestRecentFilesTests'
    self.tester = SlicerTestRecentFilesTestsTest()

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
