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


def load_translations(env):
    # (try to) load string translations
    locale_dirs = [os.path.join(env.srcdir, x)
                   for x in env.config.locale_dirs]
    locale.init(locale_dirs, env.config.language, _CATALOG)
