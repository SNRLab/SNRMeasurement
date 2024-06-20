cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(SNRMeasurement)

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "https://github.com/SNRLab/SNRMeasurement#snrmeasurement")
set(EXTENSION_CATEGORY "IGT")
set(EXTENSION_CONTRIBUTORS "Babak Matinfar (BWH), Junichi Tokuda (BWH)")
set(EXTENSION_DESCRIPTION "3D Slicer CLI to calculate SNR of images.")
set(EXTENSION_ICONURL "https://raw.githubusercontent.com/SNRLab/SNRMeasurement/master/SNRMeasurement.png")
set(EXTENSION_SCREENSHOTURLS "https://raw.githubusercontent.com/SNRLab/SNRMeasurement/master/SNRRegions.png")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(SNRMeasurement)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
