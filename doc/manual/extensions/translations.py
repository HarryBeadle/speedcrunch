# -*- coding: utf-8 -*-
import os

from sphinx import locale

# translation stuff
_CATALOG = 'extra-doc-strings'


def _(message):
    return locale.translators[_CATALOG].gettext(message)


def l_(string):
    """Lazy gettext, puts off resolving the string for as long as possible."""
    return locale._TranslationProxy(_, string)


def init(locale_dirs, language):
    """Load translations."""
    # (try to) load string translations
    if language and locale_dirs:
        locale.init(locale_dirs, language, _CATALOG)


def load_translations(env):
    # (try to) load string translations
    if env.config.locale_dirs:
        locale_dirs = [os.path.join(env.srcdir, x)
                       for x in env.config.locale_dirs]
        init(locale_dirs, env.config.language)
