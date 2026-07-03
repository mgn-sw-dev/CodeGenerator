# UNDER CONSTRUCTION! DO NOT USE!

# CodeGenerator
This repository contains the code for the firmware code generator. 

# Scope
- Gen2
- Gen2.9
- Entry

# Target
- Firmware files are to be generated from a configuration file, which can then be integrated.
-  The generator should be able to assess the utilisation of the cores and distribute the load accordingly.
-  Corresponding files, based on templates, are to be generated for the individual configuration elements. The generated files describe elements such as memory size or object elements that are specific to the project.
-  Parsers are available for loading the configuration
-  Integration with fleet management to load the configuration without storing the necessary files locally

# Structure
Generator is divided into three levels:
- View: No Buissnes Logic, only for GUI Interface 
- Model: Connection between View and Core
- Core: Buisness and Parsing logic

- At the Model and Core levels, only standard C/C++ libraries or libraries that are licence-free are permitted.
- QT must only be used at the view level

# Interface
- GUI
- Commandline interface
