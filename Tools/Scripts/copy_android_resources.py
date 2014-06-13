#!/usr/bin/python
#
#  copy_android_resources.py
#  Chilli Source
#  Created by Scott Downie on 12/06/2014.
#
#  The MIT License (MIT)
#
#  Copyright (c) 2014 Tag Games Limited
#
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#  THE SOFTWARE.
#

import sys
import file_system_utils
import subprocess
import os

#----------------------------------------------------------------------
# Copies the resource from ProjectResources and AppResources
# into res and assets directories that are required by Android.
#
# Copies the jars into libs as required by Android.
#
# Premulitplies all the PNGs in assets
#
# @author S Downie
#----------------------------------------------------------------------


#----------------------------------------------------------------------
# Copies the resource from ProjectResources and AppResources
# into res and assets directories that are required by Android.
#
# @author S Downie
#
# @param Project directory path
#----------------------------------------------------------------------
def copy_resources(project_dir):
    file_system_utils.delete_directory(os.path.join(project_dir, "assets"))
    file_system_utils.delete_directory(os.path.join(project_dir, "res"))

    app_src_path = os.path.join(project_dir, "AppResources")
    cs_src_path = os.path.join(project_dir, "ChilliSource", "CSResources")
    platform_src_path = os.path.join(project_dir, "ProjectResources", "Android")

    app_dst_path = os.path.join(project_dir, "assets", "AppResources")
    cs_dst_path = os.path.join(project_dir, "assets", "CSResources")
    platform_dst_path = os.path.join(project_dir, "res")

    file_system_utils.overwrite_directory(app_src_path, app_dst_path)
    file_system_utils.overwrite_directory(cs_src_path, cs_dst_path)
    file_system_utils.overwrite_directory(platform_src_path, platform_dst_path)

#----------------------------------------------------------------------
# Copies the jars into libs as required by Android.
#
# @author S Downie
#
# @param Project directory path
#----------------------------------------------------------------------
def copy_jars(project_dir):
    jars_src_path = os.path.join(project_dir, "ChilliSource", "Libraries", "Core", "Libs", "Android", "jars")
    jars_dst_path = os.path.join(project_dir, "libs")

    file_system_utils.copy_directory(jars_src_path, jars_dst_path)

#----------------------------------------------------------------------
# Premulitplies all the PNGs in assets
#
# @author S Downie
#
# @param Project directory path
#----------------------------------------------------------------------
def premultiply_pngs(project_dir):
    jarFile = os.path.join(project_dir, "ChilliSource", "Tools", "PreMultipliedAlphaPNGTool.jar")
    png_files = file_system_utils.get_file_paths_with_extensions(os.path.join(project_dir, "assets"), ["png"])

    for png_file in png_files:
        subprocess.call(["java", "-Djava.awt.headless=true", "-Xmx512m", "-jar", jarFile, "--input", png_file, "--output", png_file]);

#----------------------------------------------------------------------
# The entry point into the script.
# 
# @author S Downie
#
# @param The list of arguments.
#----------------------------------------------------------------------
def main(args):
    if not len(args) is 2:
        print("ERROR: Missing project path")
        return

    project_dir = args[1]
    copy_resources(project_dir)
    copy_jars(project_dir)
    premultiply_pngs(project_dir)
   
if __name__ == "__main__":
    main(sys.argv)