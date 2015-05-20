/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#include "vtkSlicerTortuosityLogic.h"

// ITK includes
#include "itkVesselTubeSpatialObject.h"
#include "itktubeTortuositySpatialObjectFilter.h"
#include "itkVector.h"

// Spatial object includes
#include "vtkMRMLSpatialObjectsNode.h"

// TubeTK includes
#include "tubeTubeMath.h"

// VTK includes
#include "vtkObjectFactory.h"
#include "vtkDelimitedTextWriter.h"
#include "vtkDelimitedTextReader.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTable.h"

#include <math.h>

vtkStandardNewMacro(vtkSlicerTortuosityLogic);

//------------------------------------------------------------------------------
vtkSlicerTortuosityLogic::vtkSlicerTortuosityLogic( void )
{
  this->FlagToArrayNames[DistanceMetric] = "DistanceMetric";
  this->FlagToArrayNames[InflectionCountMetric] = "InflectionCountMetric";
  this->FlagToArrayNames[InflectionPoints] = "InflectionPoints";
  this->FlagToArrayNames[SumOfAnglesMetric] = "SumOfAnglesMetric";
  this->FlagToArrayNames[AllOtherMetrics] = "AllOtherMetrics";

  std::string otherMetricArray[] = {  "TotalCurvature",
                                      "TotalSquaredCurvatureMetric",
                                      "PathLengthMetric",
                                      "ChordLengthMetric",
                                      "Percentile95Metric",
                                      "InflectionCount1Metric",
                                      "InflectionCount2Metric",
                                      "AverageRadiusMetric",
                                      "Tau4Metric",
                                      "CurvatureScalarMetric"};
  otherMetricList = new std::vector<std::string>(
        otherMetricArray, otherMetricArray + 10);
  // For printing, only keep the metrics that are tube specific and not
  // those who are point specific.
  otherPrintableMetricList = new std::vector<std::string>(
        otherMetricArray, otherMetricArray + 9);

}

//------------------------------------------------------------------------------
vtkSlicerTortuosityLogic::~vtkSlicerTortuosityLogic( void )
{
}

//------------------------------------------------------------------------------
void vtkSlicerTortuosityLogic::PrintSelf(ostream& os, vtkIndent indent)
{
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic::UniqueMeasure(int flag)
{
  return flag == vtkSlicerTortuosityLogic::DistanceMetric ||
    flag == vtkSlicerTortuosityLogic::InflectionCountMetric ||
    flag == vtkSlicerTortuosityLogic::InflectionPoints ||
    flag == vtkSlicerTortuosityLogic::SumOfAnglesMetric;
}

//------------------------------------------------------------------------------
vtkDoubleArray* vtkSlicerTortuosityLogic
::GetDistanceMetricArray(vtkMRMLSpatialObjectsNode* node, int flag)
{
  return this->GetOrCreateArray(
    node, flag & vtkSlicerTortuosityLogic::DistanceMetric);
}

//------------------------------------------------------------------------------
vtkDoubleArray* vtkSlicerTortuosityLogic
::GetInflectionCountMetricArray(vtkMRMLSpatialObjectsNode* node, int flag)
{
  return this->GetOrCreateArray(
    node, flag & vtkSlicerTortuosityLogic::InflectionCountMetric);
}

//------------------------------------------------------------------------------
vtkDoubleArray* vtkSlicerTortuosityLogic
::GetInflectionPointsArray(vtkMRMLSpatialObjectsNode* node, int flag)
{
  return this->GetOrCreateArray(
    node, flag & vtkSlicerTortuosityLogic::InflectionPoints);
}

//------------------------------------------------------------------------------
vtkDoubleArray* vtkSlicerTortuosityLogic
::GetSumOfAnglesMetricArray(vtkMRMLSpatialObjectsNode* node, int flag)
{
  return this->GetOrCreateArray(
    node, flag & vtkSlicerTortuosityLogic::SumOfAnglesMetric);
}

//------------------------------------------------------------------------------
std::vector<vtkDoubleArray*> vtkSlicerTortuosityLogic
::GetAllOtherMetricArrays(vtkMRMLSpatialObjectsNode* node)
{
  std::vector<vtkDoubleArray*> metricArraysArray;

  for(int i=0 ; i<otherMetricList->size() ; i++)
    {
//    std::cout<<"Creating "<<otherMetricList->at(i)<<" Array"<<std::endl;
    vtkDoubleArray* metricArray =
        this->GetOrCreateArray<vtkDoubleArray>(node, otherMetricList->at(i).c_str());
    if (!metricArray)
      {
      std::cerr<<"The array "<<otherMetricList->at(i)<<" couldn't be created or fetched"<<std::endl;
      }

    metricArray->Initialize();

    // If it's new, make it the correct size
    vtkDoubleArray* ids = this->GetArray<vtkDoubleArray>(node, "TubeIDs");
    assert(ids);
    if (metricArray->GetSize() != ids->GetSize());
      {
      metricArray->SetNumberOfValues(ids->GetSize());
      }

    metricArraysArray.push_back(metricArray);
    }
  return metricArraysArray;
}

//------------------------------------------------------------------------------
vtkDoubleArray* vtkSlicerTortuosityLogic
::GetOrCreateArray(vtkMRMLSpatialObjectsNode* node, int flag)
{
  if (!flag || !this->UniqueMeasure(flag))
    {
    return NULL;
    }

  std::string name = this->FlagToArrayNames[flag];
  vtkDoubleArray* metricArray =
    this->GetOrCreateArray<vtkDoubleArray>(node, name.c_str());
  if (!metricArray)
    {
    return NULL;
    }

  // If it's new, make it the correct size
  vtkDoubleArray* ids = this->GetArray<vtkDoubleArray>(node, "TubeIDs");
  assert(ids);
  if (metricArray->GetSize() != ids->GetSize());
    {
    metricArray->SetNumberOfValues(ids->GetSize());
    }
  return metricArray;
}

//------------------------------------------------------------------------------
template<typename T> T* vtkSlicerTortuosityLogic
::GetArray(vtkMRMLSpatialObjectsNode* node, const char* name)
{
  vtkPolyData* polydata = node->GetPolyData();
  if (!polydata)
    {
    return NULL;
    }
  vtkPointData* pointData = polydata->GetPointData();
  if (!pointData)
    {
    return NULL;
    }

  return T::SafeDownCast(pointData->GetArray(name));
}

//------------------------------------------------------------------------------
template<typename T> T* vtkSlicerTortuosityLogic
::GetOrCreateArray(vtkMRMLSpatialObjectsNode* node, const char* name)
{
  T* metricArray = this->GetArray<T>(node, name);
  if (!metricArray)
    {
    vtkPolyData* polydata = node->GetPolyData();
    if (!polydata)
      {
      return NULL;
      }
    vtkPointData* pointData = polydata->GetPointData();
    if (!pointData)
      {
      return NULL;
      }

    vtkNew<T> newMetricArray;
    newMetricArray->SetName(name);
    pointData->AddArray(newMetricArray.GetPointer());
    return newMetricArray.GetPointer();
    }
  return metricArray;
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic
::RunDistanceMetric(vtkMRMLSpatialObjectsNode* node)
{
  return this->RunMetrics(
    node, vtkSlicerTortuosityLogic::DistanceMetric);
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic
::RunInflectionCountMetric(vtkMRMLSpatialObjectsNode* node)
{
  return this->RunMetrics(
    node, vtkSlicerTortuosityLogic::InflectionCountMetric);
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic
::RunInflectionPoints(vtkMRMLSpatialObjectsNode* node)
{
  return this->RunMetrics(
    node, vtkSlicerTortuosityLogic::InflectionPoints);
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic
::RunSumOfAnglesMetric(vtkMRMLSpatialObjectsNode* node)
{
  return this->RunMetrics(
    node, vtkSlicerTortuosityLogic::SumOfAnglesMetric);
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic
::RunMetrics(vtkMRMLSpatialObjectsNode* node, int flag,
             tube::SmoothTubeFunctionEnum smoothingMethod, double smoothingScale,
             int subsampling)
{
  if (!node)
    {
    return false;
    }

  // typdefs
  typedef vtkMRMLSpatialObjectsNode::TubeNetType                    TubeNetType;
  typedef itk::VesselTubeSpatialObject<3>                           VesselTubeType;
  typedef itk::tube::TortuositySpatialObjectFilter<VesselTubeType>  FilterType;

  TubeNetType* spatialObject = node->GetSpatialObject();

  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  // 1 - Get the metric arrays
  vtkDoubleArray* dm = this->GetDistanceMetricArray(node, flag);
  vtkDoubleArray* icm = this->GetInflectionCountMetricArray(node, flag);
  vtkDoubleArray* ip = this->GetInflectionPointsArray(node, flag);
  vtkDoubleArray* soam = this->GetSumOfAnglesMetricArray(node, flag);

  // Histogram parameters
  int numberOfBins = 20;
  double histMin = 0.0;
  double histMax = 1.0;
  double histStep = (histMax - histMin)/numberOfBins;


  std::vector<vtkDoubleArray*> others;
  if( flag & vtkSlicerTortuosityLogic::AllOtherMetrics )
    {
    others = this->GetAllOtherMetricArrays(node);
    if(others.empty())
      {
      std::cerr<<"Other metrics cannot be computed"<<std::endl;
      }

    // Create the data arrays for the histogram
    for(int i = 0 ; i < numberOfBins ; i++)
      {
      vtkSmartPointer<vtkIntArray> histArray;
      std::ostringstream oss;
      oss << "Hist-Bin#"
          << i << ": "
          << i*histStep <<" - "
          << (i+1)*histStep;
      std::string arrayName =oss.str();

      if(histogramArrays.size() < numberOfBins)
        {
        histArray = vtkSmartPointer<vtkIntArray>::New();
        histArray->SetName(arrayName.c_str());
        histArray->SetNumberOfValues(tubeList->size());
        histogramArrays.push_back(histArray);
        }
      else
        {
        histArray = histogramArrays[i];
        histArray->Initialize();
        histArray->SetName(arrayName.c_str());
        histArray->SetNumberOfValues(tubeList->size());
        }
      }
    }

  if (!dm && !icm && !ip && !soam && others.empty())
    {
    std::cerr<<"Tortuosity flag mode unknown."<<std::endl;
    return false;
    }

  // Rewrite number of points array everytime
  vtkIntArray* nop = this->GetOrCreateArray<vtkIntArray>(node, "NumberOfPoints");
  nop->Initialize();


  // 2 - Fill the metric arrays

  int tubeNumber = 0;
  int totalNumberOfPointsAdded = 0;
  for(TubeNetType::ChildrenListType::iterator tubeIt = tubeList->begin();
        tubeIt != tubeList->end(); ++tubeIt)
    {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube)
      {
      continue;
      }

    if (currTube->GetNumberOfPoints() < 2)
      {
      std::cerr<<"Error, vessel #"<<currTube->GetId()
        <<" has less than 2 points !"<<std::endl
        <<"Skipping the vessel."<<std::endl;
      continue;
      }

    if(subsampling != 1)
      {
      std::cout<<"WARNING: the subsampling has not been implemented yet."
               <<"The entered subsampling value will have no effect "
               <<"on the computation."<<std::endl;
      }

    // Update filter

    FilterType::Pointer filter = FilterType::New();
    filter->SetMeasureFlag(flag);
    filter->SetSmoothingMethod(smoothingMethod);
    filter->SetSmoothingScale(smoothingScale);
    filter->SetNumberOfBins(numberOfBins);
    filter->SetHistMin(histMin);
    filter->SetHistMax(histMax);
    filter->SetInput(currTube);
    filter->Update();

    if (filter->GetOutput()->GetId() != currTube->GetId())
      {
      std::cerr<<"Error while running filter on tube."<<std::endl;
      return false;
      }

    // Fill the arrays
    // tubeIndex: index of points in the whole tube data set (same across all tubes)
    // filterIndex: index of points in a specific tube (reset to 0 when changing tube)
    int numberOfPoints = currTube->GetPoints().size();
//    std::cout<<"currTube size: "<<numberOfPoints<<std::endl;
    for(int filterIndex = 0, tubeIndex = totalNumberOfPointsAdded;
      filterIndex < numberOfPoints; ++filterIndex, ++tubeIndex)
      {
//      std::cout<<"filterIndex = "<<filterIndex<<std::endl;
      if (dm)
        {
        dm->SetValue(tubeIndex, filter->GetDistanceMetric());
        }
      if (icm)
        {
        icm->SetValue(tubeIndex, filter->GetInflectionCountMetric());
        }
      if (soam)
        {
        soam->SetValue(tubeIndex, filter->GetSumOfAnglesMetric());
        }
      if (ip)
        {
        ip->SetValue(tubeIndex, filter->GetInflectionPointValue(filterIndex));
        }
      if(!others.empty())
        {
        if(others[0]!= NULL)
          {
          others[0]->SetValue(tubeIndex, filter->GetTotalCurvatureMetric());
          }
        if(others[1]!= NULL)
          {
          others[1]->SetValue(tubeIndex, filter->GetTotalSquaredCurvatureMetric());
          }
        if(others[2]!= NULL)
          {
          others[2]->SetValue(tubeIndex, filter->GetPathLengthMetric());
          }
        if(others[3]!= NULL)
          {
          others[3]->SetValue(tubeIndex, filter->GetChordLengthMetric());
          }
        if(others[4]!= NULL)
          {
          others[4]->SetValue(tubeIndex, filter->GetPercentile95Metric());
          }
        if(others[5]!= NULL)
          {
          others[5]->SetValue(tubeIndex, filter->GetInflectionCount1Metric());
          }
        if(others[6]!= NULL)
          {
          others[6]->SetValue(tubeIndex, filter->GetInflectionCount2Metric());
          }
        if(others[7]!= NULL)
          {
          others[7]->SetValue(tubeIndex, filter->GetAverageRadiusMetric());
          }
        if(others[8]!= NULL)
          {
          others[8]->SetValue(tubeIndex, filter->GetTotalCurvatureMetric() / filter->GetPathLengthMetric());
          }
        if(others[9]!= NULL)
          {
          others[9]->SetValue(tubeIndex, filter->GetCurvatureScalarMetric(filterIndex));
          }
        }
      nop->InsertNextValue(numberOfPoints);
      }
    if(!others.empty())
      {
//      std::cout<<"vessel #"<<currTube->GetId()<<" curvature (every 100 points):"<<std::endl;
//      for(int i=0 ; i < numberOfPoints ; i=i+100)
//        {
//        std::cout<<others[7]->GetValue(i)<<" ";
//        }
//      std::cout<<std::endl;

      // Get the histogram features
      for(int i = 0 ; i < numberOfBins ; i++)
        {
//        std::cout<<"Tube#"<<tubeNumber<<" : Bin#"<<i<<" Value = "<<filter->GetHistogramMetric(i)<<std::endl;
        histogramArrays[i]->SetValue(tubeNumber, filter->GetHistogramMetric(i));
        }

      }

    tubeNumber++;
    totalNumberOfPointsAdded += numberOfPoints;
    }

  // Update the arrays to recompute the range
  dm->Modified();
  icm->Modified();
  ip->Modified();
  soam->Modified();
  for(int i = 0 ; i < others.size() ; i++)
    {
    others[i]->Modified();
    }
  for(int i = 0 ; i < numberOfBins ; i++)
    {
    histogramArrays[i]->Modified();
    }

  std::cout<<"Metric computation done."<<std::endl;
  return true;
}

//------------------------------------------------------------------------------
std::vector<std::string> vtkSlicerTortuosityLogic::GetNamesFromFlag(int flag)
{
  std::vector<std::string> names;
  names.push_back("TubeIDs");
  names.push_back("NumberOfPoints");
  for (int compareFlag = vtkSlicerTortuosityLogic::DistanceMetric;
    compareFlag < vtkSlicerTortuosityLogic::All;
    compareFlag = compareFlag << 1)
    {
    if (flag & compareFlag)
      {
      if(compareFlag != vtkSlicerTortuosityLogic::AllOtherMetrics)
        {
        names.push_back(this->FlagToArrayNames[compareFlag]);
        }
      else
        {
        // Add the other metrics
        for (std::vector<std::string>::iterator it = otherPrintableMetricList->begin();
          it != otherPrintableMetricList->end(); ++it)
          {
          names.push_back(*it);
          }
        }
      }
    }
  return names;
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic
::SaveAsCSV(vtkMRMLSpatialObjectsNode* node, const char* filename, int flag)
{
  if (!node || !filename)
    {
    return false;
    }

//  std::cout<<"Flag = "<<flag<<std::endl;
  // Get the metric arrays
  std::vector<vtkDataArray*> metricArrays;
  std::vector<std::string> names = this->GetNamesFromFlag(flag);
  for (std::vector<std::string>::iterator it = names.begin();
    it != names.end(); ++it)
    {
//    std::cout<<"names["<<it-names.begin()<<"] = "<<it->c_str()<<std::endl;
    vtkDataArray* metricArray =
      this->GetArray<vtkDataArray>(node, it->c_str());
    if (metricArray)
      {
      metricArrays.push_back(metricArray);
      }
    }

    for (std::vector<vtkDataArray*>::iterator it = metricArrays.begin();
      it != metricArrays.end(); ++it)
      {
//      std::cout<<"metricArrays["<<it-metricArrays.begin()<<"] = "<<(*it)->GetName()<<std::endl;
      }

  // Make sure we have everything we need for export
  if (metricArrays.size() <= 0)
    {
    std::cout<<"No array found for given flag: "<<flag<<std::endl;
    return false;
    }

  vtkIntArray* numberOfPointsArray =
    this->GetArray<vtkIntArray>(node, "NumberOfPoints");
  if (!numberOfPointsArray)
    {
    std::cerr<<"Expected ''NumberOfPoints'' array on the node point data."
      <<std::endl<<"Cannot proceed."<<std::endl;
    return false;
    }

  // Create  the table. Each column has only one value per vessel
  // instead of one value per each point of the vessel.
  vtkNew<vtkTable> table;
  for(std::vector<vtkDataArray*>::iterator it = metricArrays.begin();
    it != metricArrays.end(); ++it)
    {
    vtkNew<vtkDoubleArray> newArray;
    newArray->SetName((*it)->GetName());

    for (int j = 0; j < numberOfPointsArray->GetNumberOfTuples(); j += numberOfPointsArray->GetValue(j))
      {
      newArray->InsertNextTuple((*it)->GetTuple(j));
      }

    table->AddColumn(newArray.GetPointer());
    }


  // Add the histogram features to the table
  if(flag & vtkSlicerTortuosityLogic::AllOtherMetrics)
    {
    for(std::vector< vtkSmartPointer<vtkIntArray> >::iterator it = histogramArrays.begin();
      it != histogramArrays.end(); ++it)
      {
      table->AddColumn((*it));
      }
    }

  // Write out the table to file
  vtkNew<vtkDelimitedTextWriter> writer;
  writer->SetFileName(filename);

#if (VTK_MAJOR_VERSION < 6)
  writer->SetInput(table.GetPointer());
#else
  writer->SetInputData(table.GetPointer());
#endif

  return writer->Write();
}

//------------------------------------------------------------------------------
bool vtkSlicerTortuosityLogic::LoadColorsFromCSV(
  vtkMRMLSpatialObjectsNode *node, const char* filename)
{
  if (!node || !filename)
    {
    return false;
    }

  typedef vtkMRMLSpatialObjectsNode::TubeNetType  TubeNetType;
  typedef itk::VesselTubeSpatialObject<3>         VesselTubeType;

  // Load the table from file
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(filename);
  reader->SetFieldDelimiterCharacters(",");
  reader->SetHaveHeaders(true);
  reader->SetDetectNumericColumns(true);
  reader->Update();
  vtkTable* colorTable = reader->GetOutput();
  if (!colorTable)
    {
    std::cerr<<"Error in reading CSV file"<<std::endl;
    return false;
    }

  // Check if table is valid
  if (colorTable->GetNumberOfColumns() != 2)
    {
    std::cerr<<"Expected 2 columns in CSV file."
      <<std::endl<<"Cannot proceed."<<std::endl;
    return false;
    }

  // Get the tube list of the spatial object
  TubeNetType* spatialObject = node->GetSpatialObject();
  char childName[] = "Tube";
  TubeNetType::ChildrenListType* tubeList =
    spatialObject->GetChildren(spatialObject->GetMaximumDepth(), childName);

  // Create a new data array in the node
  double defaultValue = 0.0;
  vtkDoubleArray* customColorScaleArray =
    this->GetOrCreateArray<vtkDoubleArray>(node, "CustomColorScale");

  // Set the size of the array
  vtkDoubleArray* ids = this->GetArray<vtkDoubleArray>(node, "TubeIDs");
  assert(ids);
  if (customColorScaleArray->GetNumberOfTuples() != ids->GetNumberOfTuples())
    {
    customColorScaleArray->SetNumberOfTuples(ids->GetNumberOfTuples());
    }

  // Initialize the array with the default value
  customColorScaleArray->FillComponent(0, defaultValue);

  // Iterate through tubeList
  size_t totalNumberOfPoints = 0;
  TubeNetType::ChildrenListType::iterator tubeIt;
  for (tubeIt = tubeList->begin(); tubeIt != tubeList->end(); ++tubeIt)
    {
    VesselTubeType* currTube =
      dynamic_cast<VesselTubeType*>((*tubeIt).GetPointer());
    if (!currTube)
      {
      continue;
      }
    if (currTube->GetNumberOfPoints() < 2)
      {
      std::cerr<<"Error, vessel #"<<currTube->GetId()
        <<" has less than 2 points !"<<std::endl
        <<"Skipping the vessel."<<std::endl;
      continue;
      }

    // Get the current tube ID
    int tubeId = currTube->GetId();
    vtkDebugMacro(<<"Tube ID "<<tubeId);

    // Look for the ID in the table and get the corresponding value
    double valueToAssign = 0.0; //Default value for not specified tubes
    int tubeIndex = -1;
    for (size_t i = 0; i < colorTable->GetNumberOfRows(); i++)
      {
      if (colorTable->GetValue(i, 0).ToInt() == tubeId)
        {
        tubeIndex = i;
        valueToAssign = colorTable->GetValue(tubeIndex, 1).ToDouble();
        vtkDebugMacro(<<" found in CSV : value = "<<valueToAssign);
        break;
        }
      }
    if (tubeIndex == -1)
      {
      vtkDebugMacro(<<" not found in the CSV file");
      totalNumberOfPoints += currTube->GetPoints().size();
      continue;
      }

    // Fill the array of that tube
    size_t numberOfPoints = currTube->GetPoints().size();
    for (size_t j = totalNumberOfPoints; j < totalNumberOfPoints + numberOfPoints; j++)
      {
      customColorScaleArray->SetValue(j, valueToAssign);
      }
    totalNumberOfPoints += numberOfPoints;
    }

    // Notify the array of the changes
    customColorScaleArray->Modified();

  return true;
}
