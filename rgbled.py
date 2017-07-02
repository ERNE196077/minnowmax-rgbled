from Xlib.display import Display

def printWindowHierrarchy(window, indent):
    children = window.query_tree().children
    for w in children:
        print(indent, window.get_wm_class())
        printWindowHierrarchy(w, indent+'-')

display = Display()
root = display.screen().root
printWindowHierrarchy(root, '-')