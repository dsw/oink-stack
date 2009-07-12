
PROJ_NAME = 'oink-stack'

REPO_BASE = 'https://svn.cubewano.org/repos/%(PROJ_NAME)s'%locals()

REPO_TRUNK = '%(REPO_BASE)s/trunk/%(PROJ_NAME)s'%locals()
REPO_TAG_DIR = '%(REPO_BASE)s/tags'%locals()

RELEASES_DIR = '/home/quarl/proj/%(PROJ_NAME)s/www/releases'%locals()

TAG_EXTERNALS = 1

