#!/usr/bin/env python3
# This file should work with Python 2.7 and 3.4+
from __future__ import print_function, unicode_literals

import argparse
from io import open
import os
import subprocess
import sys


SOURCE_LANGUAGE = "en"
LANGUAGES = ["de", "en", "es", "fr"]

DOC_DIR = os.path.dirname(os.path.abspath(__file__))


class Tools:
    TOOLS = ["sphinx-build", "qcollectiongenerator"]

    def __init__(self, args):
        self._tools = {}
        for tool in self.TOOLS:
            safename = tool.replace("-", "_")
            self._tools[safename] = getattr(args, "%s_binary" % safename)

    def run_tool(self, toolname, *args):
        cmd = [self._tools[toolname]]
        cmd.extend(args)
        return subprocess.check_call(cmd)

    def __getattr__(self, name):
        return lambda *args: self.run_tool(name, *args)

    @classmethod
    def add_arguments(cls, parser):
        for tool in cls.TOOLS:
            parser.add_argument("--%s-binary" % tool, default=tool,
                                help="%s command" % tool)


def build_docs(tools, source_dir, build_dir, lang=None, builder="html",
               tags=[], extra_config={}):
    config = extra_config.copy()
    if not lang or lang == SOURCE_LANGUAGE:
        lang = SOURCE_LANGUAGE
    else:
        config["language"] = lang
    print("Building docs for '%s'..." % lang)
    args = [source_dir, os.path.join(build_dir, lang), "-b", builder]
    for k, v in config.items():
        args.extend(["-D", "%s=%s" % (k, v)])
    if tags:
        for tag in tags:
            args.extend(["-t", tag])
    return tools.sphinx_build(*args)


def generate_qrc(f, files, prefix="/"):
    f.write("<RCC>\n")
    f.write("  <qresource prefix=\"%s\">\n" % prefix)
    for alias, path in files:
        f.write("    <file alias=\"%s\">%s</file>\n" % (alias, path))
    f.write("  </qresource>\n")
    f.write("</RCC>\n")


# commands
def build_standalone_docs(tools, args):
    for lang in LANGUAGES:
        build_docs(tools, args.source_dir, args.build_dir, lang)


def build_bundled_docs(tools, args):
    resources = []
    for lang in LANGUAGES:
        basename = "manual-%s" % lang
        build_docs(tools, args.source_dir, args.build_dir, lang,
                   builder="qthelp", tags=["sc_bundled_docs"],
                   extra_config={"qthelp_basename": basename})
        tools.qcollectiongenerator(os.path.join(args.build_dir, lang,
                                                "%s.qhcp" % basename))
        resources.append(("%s.qch" % basename, "%s/%s.qch" % (lang, basename)))
        resources.append(("%s.qhp" % basename, "%s/%s.qhp" % (lang, basename)))
    with open(os.path.join(args.build_dir, "manual.qrc"), "w",
              encoding="utf-8") as f:
        generate_qrc(f, resources, prefix="/manual")


# CLI
def build_argument_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--source-dir", "-s", default=DOC_DIR,
        help="Documentation source directory (default: %(default)s)")
    Tools.add_arguments(parser)
    parser.set_defaults(func=lambda *args: parser.print_help())
    subparsers = parser.add_subparsers()

    # build-html-docs
    parser_build_html = subparsers.add_parser(
        "build-standalone-docs",
        help="Build stand-alone HTML docs for all languages")
    parser_build_html.add_argument(
        "--build-dir", "-b", default="_build-html",
        help="Output base directory (default: %(default)s")
    parser_build_html.set_defaults(func=build_standalone_docs)

    # build-bundled-docs
    parser_build_bundled = subparsers.add_parser(
        "build-bundled-docs",
        help="Build Qt Help docs for all languages")
    parser_build_bundled.add_argument(
        "--build-dir", "-b", default="_build-bundled",
        help="Output base directory (default: %(default)s")
    parser_build_bundled.set_defaults(func=build_bundled_docs)

    return parser


def main(argv):
    parser = build_argument_parser()
    args = parser.parse_args(argv[1:])
    tools = Tools(args)
    try:
        args.func(tools, args)
    except Exception as exc:
        print("Error:", exc, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main(sys.argv)
