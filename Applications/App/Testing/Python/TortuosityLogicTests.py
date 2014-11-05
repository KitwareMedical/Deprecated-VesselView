import os
import unittest
from __main__ import vtk, qt, ctk, slicer

#
# TortuosityLogicTests
#

class TortuosityLogicTests:
  def __init__(self, parent):
    parent.title = "TortuosityLogicTests" # TODO make this more human readable by adding spaces
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
    slicer.selfTests['TortuosityLogicTests'] = self.runTest

  def runTest(self):
    tester = TortuosityLogicTestsTest()
    tester.runTests()

#
# qTortuosityLogicTestsTest
#

class TortuosityLogicTestsTest(unittest.TestCase):

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

  def tearDown(self):
    pass

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

  def runAndCheckMetrics(self, nameTemplate, expectedValues):
    for i in range(len(expectedValues)):

      self.delayDisplay('testing %s ' %(nameTemplate %i))
      node = slicer.util.getFirstNodeByClassByName('vtkMRMLSpatialObjectsNode', nameTemplate %i)
      self.assertTrue(node, 'loading node failed')

      logic = slicer.modules.tortuosity.logic()
      self.assertTrue(logic.RunMetrics(node, logic.All), 'RunMetrics failed')

      dm = logic.GetDistanceMetricArray(node)
      self.assertTrue(dm, 'No distance metric array')
      icm = logic.GetInflectionCountMetricArray(node)
      self.assertTrue(icm, 'No inflection count array')
      soam = logic.GetSumOfAnglesMetricArray(node)
      self.assertTrue(soam, 'No sum of angles array')
      
      for index in range(dm.GetNumberOfTuples()):
        dmValue = dm.GetValue(index)
        icmValue = icm.GetValue(index)
        soamValue = soam.GetValue(index)
        msg = '%s value look up failed. Expected: %s Got: %s (Case #%s)'

        self.assertAlmostEqual(dmValue, expectedValues[i]['DM'], 4, msg %('DM', expectedValues[i]['DM'], dmValue, i))
        self.assertAlmostEqual(icmValue, expectedValues[i]['ICM'], 4, msg %('ICM', expectedValues[i]['ICM'], icmValue, i) )
        self.assertAlmostEqual(soamValue, expectedValues[i]['SOAM'], 4, msg %('SOAM', expectedValues[i]['SOAM'], soamValue, i) )

  def test_TestStraightVessels(self):
    self.delayDisplay('test_TestStraightVessels')

    nameTemplate = 'StraightTube_test%s'
    expectedValues = [
        {
        'DM': 1.0,
        'ICM': 1.0,
        'SOAM:': 0.0,
        },
        {
        'DM': 1.0,
        'ICM': 1.0,
        'SOAM:': 0.0,
        },
        {
        'DM': 1.0,
        'ICM': 1.0,
        'SOAM:': 0.0,
        },
      ]

    self.runAndCheckMetrics(nameTemplate, expectedValues)
    self.delayDisplay('Test passed!')

  def test_TestSinusVessels(self):
    self.delayDisplay('test_TestSinusVessels')

    nameTemplate = 'SinusTube_test%s'
    expectedValues = [
        {
        'DM': 1.21581,
        'ICM': 1.21581 * 2.0,
        'SOAM:': 0.411187,
        },
        {
        'DM': 1.21581,
        'ICM': 1.21581 * 4.0,
        'SOAM:': 0.411187,
        },
        {
        'DM': 5.87042,
        'ICM': 5.87042 * 2.0,
        'SOAM:': 0.158497,
        },
        {
        'DM': 3.40308,
        'ICM': 3.40308 * 2.0,
        'SOAM:': 1.28584,
        },
      ]

    self.runAndCheckMetrics(nameTemplate, expectedValues)
    self.delayDisplay('Test passed!')

#
# qWelcomeModuleTestWidget
#

class TortuosityLogicTestsWidget():
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

    self.moduleName = 'TortuosityLogicTests'
    self.tester = TortuosityLogicTestsTest()

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
