/*=========================================================================

Library:   VesselView

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

import QtQuick 1.0

ListModel {

    ListElement {
        name: "About"
        module: ""
        imageSource: ":/Icons/Medium/VesselViewSplashScreen.svg"
        description: "aboutText"
        layout: -1
        fileTypes: ""
        subTasks:[]
    }
    ListElement {
        name: "Vessel Display"
        module: "SpatialObjects"
        imageSource: ":/Icons/Medium/VesselDisplay.svg"
        description: " "
        layout: -1
        fileTypes: "SpatialObjectFile"
        subTasks:[
            ListElement {
                name: "Vessel Display"
                module: "SpatialObjects"
                imageSource: ":/Icons/Medium/VesselDisplay.svg"
                description:  "
    <html>
    <center>
    <em>Visualize images, vessels, and organs in 3D.</em>
    </center>
    <br><br><br>
    <div text-align=\"left\">
    Loads multiple data files and selectively displays them using a variety of
    basic and advanced visualization formats.
    </div>
    </html>"
                layout: -1
                fileTypes: "SpatialObjectFile"
            }
        ]
    }
    ListElement {
        name: "Vessel Segmentation"
        module: "SpatialObjectFile"
        imageSource: "/Icons/Medium/InteractiveVesselSegmentation.svg"
        description:  ""
        layout: 3
        fileTypes: "SpatialObjectFile"
        subTasks: [
            ListElement {
                name: "Interactive Vessel Segmentation"
                module: "InteractiveSegmentTubes"
                imageSource: ":/Icons/Medium/InteractiveVesselSegmentation.svg"
                description:  "
     <html>
     <center>
     <em>Point-and-click within an image to segment tubes.</em>
     </center>
     <br><br><br>
     <div text-align=\"left\">
     Provides an interactive interface to
     <a href=\"http://www.tubetk.org\">TubeTK</a>'s vessel segmentation algorithms.
     <br><br>
     Pre-defined algorithm parameterizations are given for common tasks, such as:
     <ul>
       <li> Liver vessels in contrast-enhanced CT </li>
       <li> Brain vessels in MRA </li>
       <li> Lung vessels in CT </li>
     </ul>
     </div>
     </html>"
                layout: 3
                fileTypes: "SpatialObjectFile"
            },
            ListElement {
                name: "Automatic Vessel Segmentation"
                module: "Workflow"
                imageSource: ":/Icons/Medium/AutomaticVesselSegmentation.svg"
                description:  "
     <html>
     <center>
     <em>Apply a trained statistical model to initiate vessel segmentations.</em>
     </center>
     <br><br><br>
     <div text-align=\"left\">
     Provides a workflow-style interface to
     <a href=\"http://www.tubetk.org\">TubeTK</a>'s statistical methods for
     identifying points in an image for seeding vessel extractions.
     <br><br>
     Statistical models for seed selection are given for common conditions, such as:
     <ul>
       <li> Liver vessels in contrast-enhanced CT </li>
       <li> Brain vessels in MRA </li>
       <li> Lung vessels in CT </li>
     </ul>
     </div>
     </html>"
                layout: 3
                fileTypes: "VolumeFile"
            }
        ]
    }
    ListElement {
        name: "Compute Vessel Tortuosity"
        module: "Tortuosity"
        imageSource: ":/Icons/Medium/ComputeVesselTortuosity.svg"
        description:  ""
        layout: 24 // ConventionalQuantitative
        fileTypes: "SpatialObjectFile"
        subTasks:[
            ListElement {
                name: "Compute Vessel Tortuosity"
                module: "Tortuosity"
                imageSource: ":/Icons/Medium/ComputeVesselTortuosity.svg"
                description: "
    <html>
    <center>
    <em>Compute tortuosity metrics on vessels.</em>
    </center>
    <br><br><br>
    <div text-align=\"left\">
    Work initiated by Dr. Bullitt has shown that morphological features of vessels
    within and around tumors can be used to assess malignancy and treatment
    efficacy.   Others have associated vessel morphological features with diabetic
    retinopathy, retinopathy of prematurity, and a variety of arterial diseases.
    <br><br>
    VesselView contains a variety of novel tortuosity metrics including:
    <ul>
    <li> Fourier-based methods </li>
    <li> Normalized inflection count </li>
    <li> Mean curvature </li>
    </ul>
    </div>
    </html>"
                layout: 24 // SlicerLayoutDefaultView
                fileTypes: "SpatialObjectFile"
            }
        ]
    }
    ListElement {
        name: "Interactive Organ segmentation"
        module: "Editor"
        imageSource: ":/Icons/Medium/InteractiveOrganSegmentation.svg"
        description: ""
        layout: 3
        fileTypes: "VolumeFile"
        subTasks:[
            ListElement {
                name: "Interactive Organ segmentation"
                module: "Editor"
                imageSource: ":/Icons/Medium/InteractiveOrganSegmentation.svg"
                description: "
    <html>
    <center>
    <em>Semi-automated methods for segmenting organs and regions of interest in images.</em>
    </center>
    <br><br><br>
    <div text-align=\"left\">
    Provides methods from TubeTK and 3D Slicer for creating and editing masks that
    represent organs and/or regions of interest that can be used to constrain
    interactive and automated vessel segmentation algorithms in VesselView.
    </div>
    </html>"
                layout: 3
                fileTypes: "VolumeFile"
            }

        ]
    }
    ListElement {
        name: "Utilities"
        module: "Welcome"
        imageSource: ""
        description: ""
        layout: 4 // SlicerLayoutDefaultView
        fileTypes: "SpatialObjectFile"
        subTasks:[
            ListElement {
                name: "Convert Vessel Files"
                module: "ConvertTRE"
                imageSource: ":/Icons/Medium/ConvertVessels.svg"
                description: "
     <html>
     <center>
     <em>Convert between various tube file formats.</em>
     </center>
     <br><br><br>
     <div text-align=\"left\">
     Convert between ITK's TubeSpatialObject format (provided by MetaIO) and legacy
     formats such as
     <ul>
       <li> Dr. Bullitt's TubeX files </li>
       <li> UNC CADDLab's .tre files (pre-cursor to MetaIO) </li>
     </ul>
     </div>
     </html>"
                layout: 4 // SlicerLayoutDefaultView
                fileTypes: "SpatialObjectFile"
            },
            ListElement {
                name: "Crop Image"
                module: "CropImage"
                imageSource: ""
                description:  "
     <html>
     <center>
     <em>Crop Image</em>
     </center>
     <br><br><br>

     </html>"
                layout: 3
                fileTypes: "ImageFile"
            },
            ListElement {
                name: "Crop Volume"
                module: "CropVolume"
                imageSource: ""
                description:  "
     <html>
     <center>
     <em>Crop Volume</em>
     </center>
     <br><br><br>

     </html>"
                layout: 3
                fileTypes: "VolumeFile"
            }
        ]
    }
}
