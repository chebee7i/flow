/*******************************************************************************
 FieldViewer: Dynamics Toolset application class.
 Copyright (c) 2006-2008 Jordan Van Aalsburg

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#include <algorithm>
#include <iostream>
#include <string>

#include <IO/OpenFile.h>

// ToolBox includes
//
#include "ToolBox/ToolBox"
#include "ToolBox/Extensions/ToolRotator.h"

#include "FieldViewer.h"
#include "Directory.h"

//DynamicsFactory Factory;

/** Returns the base directory for resource files.
 *
 * Returns RESOURCEDIR if it exists or
 * returns "." to search the current directory instead.
 */
std::string getResourceDir()
{
    std::string dir(RESOURCEDIR);

    try {
        IO::DirectoryPtr dirPtr = IO::openDirectory(dir.c_str());
    }
    catch (IO::Directory::OpenError e)
    {
        // This means you must run mycelia must exist in the CWD.
        dir = ".";
    }
    return dir;
}


Viewer::Viewer(int &argc, char** argv, char** appDefaults) :
   Vrui::Application(argc, argv, appDefaults)
{

    // load ToolBox
    ToolBox::ToolBoxFactory::instance();

    // load dynamics plugins
    try 
    {
        model_names = loadPlugins();
    }
    catch (std::runtime_error& e) 
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    catch (...) 
    {
        std::cerr << "UNDEFINED ERROR." << std::endl;
    }

    // alphabetize the names list
    std::sort(model_names.begin(), model_names.end());
    /*
    // initialize the model dynamics (first one alphabetically)
    //model=Factory[model_names[0]]();
    // load lorenz
    std::string name = "Lorenz Attractor";
    model = Factory[name]();
    */

    // center the display
    //resetNavigationCallback(0);
}

Viewer::~Viewer()
{
    // close all dynamic libs (plugins)
    for (DLList::iterator lib=dl_list.begin(); lib != dl_list.end(); ++lib)
    {
        dlclose(*lib);
    }
}

//
// Internal methods
//


std::vector<std::string> Viewer::loadPlugins() throw(std::runtime_error)
{
    std::string directory( getResourceDir() );
    directory += "/plugins";

    std::cout << "Loading plugins from: " << directory << std::endl;

    Directory dir;
    dir.addExtensionFilter("so");
    dir.read(directory);

    void *dlib;
    std::vector<std::string>::const_iterator lib;
    for (lib=dir.contents().begin(); lib != dir.contents().end(); ++lib)
    {
        std::cout << "\topening " << *lib << "..." << std::endl;

        // prepend directory name to library file name
        std::string file=directory + "/" + *lib;

        // open the library
        dlib=dlopen(file.c_str(), RTLD_NOW);
        //dlib=dlopen(file.c_str(), RTLD_LAZY|RTLD_GLOBAL);

        if (dlib == NULL)
        {
            throw std::runtime_error(dlerror());
        }

        // add the handle to our list
        dl_list.insert(dl_list.end(), dlib);
    }

    // create an array of model names
    std::vector<std::string> model_names;
    /*
    for (DynamicsFactory::iterator itr=Factory.begin(); itr != Factory.end(); ++itr)
    {
        model_names.insert(model_names.end(), itr->first);
    }
    */
    // return the name array
    return model_names;
}


//
// Interface
//

void Viewer::initContext(GLContextData& contextData) const
{
}

void Viewer::display(GLContextData& contextData) const
{
}

void Viewer::frame()
{
}


