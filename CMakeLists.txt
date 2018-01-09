cmake_minimum_required(VERSION 2.8.9)

project(SNRMeasurement)

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/SNRMeasurement")
set(EXTENSION_CATEGORY "IGT")
set(EXTENSION_CONTRIBUTORS "Babak Matinfar (BWH), Junichi Tokuda (BWH)")
set(EXTENSION_DESCRIPTION "3D Slicer CLI to calculate SNR of images.")
set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/SNRMeasurement/SNRMeasurement.png?revision=21745&view=co")
set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/a/ab/Slicer-r19441-SNRMeasurement-screenshot.png http://wiki.slicer.org/slicerWiki/images/1/1e/Slicer-r19441-SNRMeasurement-screenshot-2.png")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(SNRMeasurement)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
