import sys
sys.path.append('lib/PUtils/python/')
import PUtils

if __name__ == '__main__':
    node = WebserverNode()
    node.initialize()
    node.run()
