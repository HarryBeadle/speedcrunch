from docutils.parsers.rst import Directive
from sphinx.builders.qthelp import QtHelpBuilder
import re
from docutils import nodes

class QtKeywordDirective(Directive):
    """
    Directive to pass a keyword to QtHelp project. Syntax:
        .. qtkeyword:: ARGUMENTS
    When building with the custom qthelp2 builder, ARGUMENTS will be copied
    verbatim into a corresponding keyword entry:
        <keyword ARGUMENTS ref=somewhere/>
    """
    has_content = False
    required_arguments = 1
    optional_arguments = 10
    def run(self):
        self.arguments = " ".join(self.arguments)
        env = self.state.document.settings.env
        if env.config.ignore_qtkeywords:
            return []
        
        # create a (resolvable) target  we can link to
        targetid = "kw-%d" % env.new_serialno('kw')
        targetnode = nodes.target('', '', ids=[targetid])
        
        if not hasattr(env, 'all_qtkeywords'):
            env.all_qtkeywords = []
            
        env.all_qtkeywords.append({
            'docname': env.docname,
            'lineno': self.lineno,
            'target': targetnode,
            'designation': self.arguments,
            'refuri': env.app.builder.get_target_uri(env.docname) + '#' + targetid
        })
        return [targetnode]




_idpattern = re.compile(r"((?P<title>.+) )?(\((class in )?(?P<id>[\w\.]+)( (?P<descr>\w+))?\))$")
_func_pattern = re.compile(r"(?P<fname>[\w_]+)\(\) \(built-in function\)$")

class MyQtHelpBuilder(QtHelpBuilder):
    """
    Custom version of the qthelp builder.
    
    This one automatically generates useful id entries from the index, e.g.
    'cos() (built-in function)' will be mapped to 'cos'.
    
    Also, it allows the use of the 'qtkeyword' directive.
    """
    name = "qthelp2"
        
    def init(self):
        self.keywords_completed = False
        QtHelpBuilder.init(self)        

    def keyword_item(self, name, ref):
        function_matchobj = _func_pattern.match(name)
        generic_matchobj = _idpattern.match(name)
        
        if function_matchobj:
            groupdict = function_matchobj.groupdict()
            id = groupdict['fname']
        elif generic_matchobj:
            groupdict = generic_matchobj.groupdict()
            shortname = groupdict['title']
            id = groupdict.get('id')
            name = groupdict.get('title')
            if shortname.endswith('()'):
                shortname = shortname[:-2]
        else:
            id = None

        if id:
            item = ' '*12 + '<keyword name="%s" id="%s" ref="%s"/>' % (
                name, id, ref[1])
        else:
            item = ' '*12 + '<keyword name="%s" ref="%s"/>' % (name, ref[1])
        item.encode('ascii', 'xmlcharrefreplace')
        return item
        
        
        
    def build_keywords(self, title, refs, subitems):
        # call parent method for constructing the keywords from index
        keywords = QtHelpBuilder.build_keywords(self, title, refs, subitems)
        
        # now we add our keywords
        if self.keywords_completed or not hasattr(self.app.env, 'all_qtkeywords'):
            return keywords
        for kw in self.env.all_qtkeywords:          
            keywords.append(' '*12 + '<keyword {} ref="{}"/>'.format(kw["designation"], kw['refuri']))
                                                
        
        self.keywords_completed = True
        return keywords

def purge_keywords(app, env, docname):
    """
    If the document has become out of date, clear all keywords that are
    associated with it. They will be regenerated when the document is parsed.
    This is necessary to make sure that any keywords that are removed from the
    source don't stay in the persistent environment.
    """
    if not hasattr(env, 'all_qtkeywords'):
        return

    env.all_qtkeywords = [kw for kw in env.all_qtkeywords
                          if kw['docname'] != docname]

def process_keywords(app, env, docname):
    if not hasattr(app.env, 'all_qtkeywords'):
        return
    for kw in app.env.all_qtkeywords:
        kw['refuri'] = app.builder.get_target_uri(docname)
        kw['refuri'] += '#' + kw['target']['refid']
    
def setup(app):
    app.add_config_value('ignore_qtkeywords', True, 'html')
    app.add_builder(MyQtHelpBuilder)
    app.add_directive('qtkeyword', QtKeywordDirective)
    app.connect('env-purge-doc', purge_keywords)
    #app.connect('doctree-resolved', process_keywords)

    return {'version': '0.1'}   # identifies the version of our extension


