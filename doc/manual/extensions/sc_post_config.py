# -*- coding: utf-8 -*-

"""
Extension to handle SpeedCrunch-specific config settings.

Mostly, it sets various things based on the `language` setting. The docs
language is specified on the command-line and not available in `conf.py`.
"""

import re

_LANG_RE = re.compile(r'([^._@]+)')


def _parse_lang_code(lang_code):
    """Extract the language part from a full language code.

    `lang_code` may be of this form:
    language[_script][_country][.codeset][@modifier]
    """
    if not lang_code:
        return None
    m = _LANG_RE.match(lang_code)
    if m is None:
        return None
    return m.group(1)


def _post_config(app, env, *args, **kwargs):
    """Fixup config values based on the selected doc language."""
    lang = _parse_lang_code(env.config.language)
    if env.config.language and lang:
        if not env.config.qthelp_basename:
            env.config.qthelp_basename = 'SpeedCrunch-%s' % env.config.language
        if not env.config.html_search_language:
            env.config.html_search_language = lang


def setup(app):
    # this event seems fine for our fixup
    app.connect('env-before-read-docs', _post_config)
