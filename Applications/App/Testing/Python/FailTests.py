import os
import unittest
from __main__ import vtk, qt, ctk, slicer

#
# FailTests
#

class FailTests:
  def __init__(self, parent):
    parent.title = "FailTests" # TODO make this more human readable by adding spaces
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
    slicer.selfTests['FailTests'] = self.runTest

  def runTest(self):
    tester = FailTestsTest()
    tester.runTests()

#
# FailTestsTest
#

class FailTestsTest(unittest.TestCase):
  @unittest.expectedFailure
  def test_TestAssert(self):
    self.assertTrue(False)

  @unittest.expectedFailure
  def test_TestException(self):
    raise Exception("This should fail")

  @unittest.expectedFailure
  def test_TestFail(self):
    self.fail("This should fail")

#
# FailTestsWidget
#

class FailTestsWidget():
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

    self.moduleName = 'FailTests'
    self.tester = FailTestsTest()

  def setup(self):
    pass
