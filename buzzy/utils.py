# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2012, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

import os
import os.path
import re
import subprocess
import sys

import buzzy.config
from buzzy.errors import BuzzyError
from buzzy.log import log


def _if_run(pretty_name, cmd, **kw):
    """
    Run a command in a subprocess and return whether it exits successfully.
    Ignore any output.
    """

    log(1, "$ %s" % " ".join(cmd))

    try:
        subprocess.check_output(cmd, stderr=subprocess.STDOUT, **kw)
        return True
    except OSError:
        raise BuzzyError("Error running %s" % pretty_name)
    except subprocess.CalledProcessError:
        return False

def if_run(cmd, **kw):
    return _if_run(cmd[0], cmd, **kw)

def if_sudo(cmd, **kw):
    cmd.insert(0, "sudo")
    return _if_run(cmd[1], cmd, **kw)


def _run(pretty_name, cmd, **kw):
    """
    Run a command in a subprocess, capturing the output.  If the process
    succeeds, don't print anything out.  If it fails, print out the output and
    raise an error.
    """

    log(1, "$ %s" % " ".join(cmd))

    try:
        if buzzy.config.verbosity >= 1:
            subprocess.check_call(cmd, **kw)
        else:
            subprocess.check_output(cmd, stderr=subprocess.STDOUT, **kw)
    except OSError:
        raise BuzzyError("Error running %s" % pretty_name)
    except subprocess.CalledProcessError as e:
        raise BuzzyError("Error running %s" % pretty_name, e.output)

def run(cmd, **kw):
    return _run(cmd[0], cmd, **kw)

def sudo(cmd, **kw):
    cmd.insert(0, "sudo")
    return _run(cmd[1], cmd, **kw)


def extract_attrs(obj, names=None):
    if names is None:
        names = [ name for name in dir(obj) if not name.startswith("_") ]
    return { name: getattr(obj, name) for name in names }


def get_arch_from_uname():
    return subprocess.check_output(["uname", "-m"]).decode("ascii").rstrip()


def makedirs(path):
    if not os.path.isdir(path):
        os.makedirs(path)


TARBALL_EXTS = re.compile(r"""
    (
        ((\.tar)?(\.bz2|\.gz|\.xz|\.Z)?)
      | (\.zip)
    )$
""", re.VERBOSE)

def tarball_basename(path):
    return TARBALL_EXTS.sub("", os.path.basename(path))


def trim(docstring):
    """From PEP 257"""
    if not docstring:
        return ''
    # Convert tabs to spaces (following the normal Python rules)
    # and split into a list of lines:
    lines = docstring.expandtabs().splitlines()
    # Determine minimum indentation (first line doesn't count):
    indent = None
    for line in lines[1:]:
        stripped = line.lstrip()
        if stripped:
            this_indent = len(line) - len(stripped)
            if indent is None:
                indent = this_indent
            else:
                indent = min(indent, this_indent)
    # Remove indentation (first line is special):
    trimmed = [lines[0].strip()]
    if indent is not None:
        for line in lines[1:]:
            trimmed.append(line[indent:].rstrip())
    # Strip off trailing and leading blank lines:
    while trimmed and not trimmed[-1]:
        trimmed.pop()
    while trimmed and not trimmed[0]:
        trimmed.pop(0)
    # Return a single string:
    return '\n'.join(trimmed)