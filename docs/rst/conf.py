# ##############################################################################
# Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# ##############################################################################

# Configuration file for the Sphinx documentation builder.

import os
# -- Project information -----------------------------------------------------
project = 'AOCL-Compression'
copyright = '2020-2024, Advanced Micro Devices, Inc'
author = 'Advanced Micro Devices, Inc'
version = '5.0.0'
release = '5.0.0'

extensions = ['breathe', 'myst_parser']
pwd = os.path.dirname(os.path.abspath(__file__))
pwd = os.path.join(pwd, '..')
pwd = os.path.join(pwd,'xml')
breathe_projects = {"aocl-compression": pwd}
breathe_default_project = "aocl-compression"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_template']

# tells the myst_parser to generate labels for heading anchors for h1, h2, and h3 level headings (corresponding to #, ##, and ### in markdown)
myst_heading_anchors = 3

# -- Options for HTML output -------------------------------------------------

html_theme = 'rocm_docs_theme'
html_theme_options = {
    "link_main_doc": False,
    "flavor": "local",
    "repository_provider" : None,
}
