#! /usr/bin/python3
try:
    from tkinter import *
    from tkinter import ttk
    from tkinter import filedialog
except:
    print( "sudo apt-get install python3-tk" )

import os
import runpy
import importlib
import importlib.util
import logging
import threading
import datetime
import time

from od_logging import ConsoleUi 


logger = logging.getLogger('console')

def donothing():
    filewin = Toplevel(root)
    button = Button(filewin, text="Do nothing button")
    button.pack()

def displayHelp():
    help=('This is a OD test script framework\n'
    'version 0.01\n')
    filewin = Toplevel(root)
    button = Button(filewin, text=help)
    button.pack()

def runTestTask(project, test):
    logger.info("\n\n********** Running "+test+"************")
    file = project+'/'+test

    spec = importlib.util.spec_from_file_location("", file )
    foo = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(foo)
        
    logger.info("\n********** Finished "+test+"***********")

#
# initGui
#
class GuiMain():
    def __init__(self):
        self.tk = Tk()

        self.initGuiMenu(self.tk)
        self.tk.title("Optimal Software Test Framework")

        tabControl = ttk.Notebook(self.tk)
        tab1 = ttk.Frame(tabControl)
        tab2 = ttk.Frame(tabControl)

#====
        tabControl.add(tab1, text ='Selection') 
        tabControl.add(tab2, text ='Console') 
        tabControl.pack(expand = 1, fill ="both") 

        mainframe = ttk.Frame(tab1, padding="3 3 12 12")
        mainframe.grid(column=0, row=0, sticky=(N, W, E, S))
        tab1.columnconfigure(0, weight=1)
        tab1.rowconfigure(0, weight=1)

#       path
        self.pathStr = StringVar(value=os.getcwd())
        ttk.Label(mainframe, text="base path:").grid(column=1, row=1, sticky=E)
        path = ttk.Entry(mainframe, width=40, textvariable=self.pathStr)
        path.grid(column=2, row=1, sticky=(W, E))

#       project
        ttk.Label(mainframe, text="project:").grid(column=1, row=2, sticky=E)
        self.projectList.clear()
        for x in os.listdir():
            if x.endswith(".proj"):
                print("project: ", x)
                self.projectList.append(x)

        self.projects = ttk.Combobox(mainframe,
            values=self.projectList,
        )
        if self.projectList:
            self.projects.current(0)
        self.projects.grid(column=2, row=2, sticky=(W,E))
        self.projects.bind("<<ComboboxSelected>>", self.rebuildTestList);

#       test
        ttk.Label(mainframe, text="test:").grid(column=1, row=3, sticky=E)
        self.testList.clear()
        print("list test in ", self.projects.get())
        self.tests = ttk.Combobox(mainframe,
            values=self.testList,
        )
        self.rebuildTestList("")
        self.tests.grid(column=2, row=3, sticky=(W,E))

        ttk.Button(mainframe, text="Run Test", command=self.runTest).grid(column=3, row=4, sticky=W)

        for child in mainframe.winfo_children(): 
            child.grid_configure(padx=5, pady=5)

#       console tab
        console_frame = ttk.Labelframe(tab2, text="Console")
        console_frame.grid(column=0, row=0, sticky=(N, W, E, S))
        console_frame.columnconfigure('all', weight=1)
        console_frame.rowconfigure('all', weight=1)
        self.console = ConsoleUi(console_frame)

        for child in console_frame.winfo_children(): 
            child.grid_configure(padx=5, pady=5)

#====

    #
    # Rebuild Test List
    #
    def rebuildTestList(self, event):
        self.testList.clear()
        for x in os.listdir(self.projects.get()):
            if x.endswith(".py"):
                self.testList.append(x)
        self.tests.config(values=self.testList)
        if self.testList:
            self.tests.current(0)
        else:
            self.tests.set('')

    #
    # Create Gui Menu bar
    #
    def initGuiMenu(self, root):
        menubar = Menu(root)
        filemenu = Menu(menubar, tearoff=0)
        filemenu.add_command(label="New", command=donothing)
        filemenu.add_command(label="Open", command=donothing)
        filemenu.add_command(label="Save", command=donothing)
        filemenu.add_command(label="Save as...", command=donothing)
        filemenu.add_command(label="Close", command=donothing)
        filemenu.add_separator()
        filemenu.add_command(label="Exit", command=root.quit)
        menubar.add_cascade(label="File", menu=filemenu)

        helpmenu = Menu(menubar, tearoff=0)
        helpmenu.add_command(label="Help Index", command=donothing)
        helpmenu.add_command(label="About...", command=displayHelp)
        menubar.add_cascade(label="Help", menu=helpmenu, compound = RIGHT)

        root.config(menu=menubar)

    #
    # Run the selected python script
    #
    def runTest(self):
        x = threading.Thread(target=runTestTask, args=(self.projects.get(),self.tests.get()))
        x.start()
#        runTestTask( self.projects.get(), self.tests.get())

    # some varables in the object
    projectList = []
    testList = []

class Clock(threading.Thread):
    """Class to display the time every seconds

    Every 5 seconds, the time is displayed using the logging.ERROR level
    to show that different colors are associated to the log levels
    """

    def __init__(self):
        super().__init__()
        self._stop_event = threading.Event()

    def run(self):
        logger.debug('Clock started')
        previous = -1
        while not self._stop_event.is_set():
            now = datetime.datetime.now()
            if previous != now.second:
                previous = now.second
                if now.second % 5 == 0:
                    level = logging.ERROR
                else:
                    level = logging.INFO
                logger.log(level, now)
            time.sleep(0.2)

    def stop(self):
        self._stop_event.set()



#
# M A I N
#

root = GuiMain()
logging.basicConfig(level=logging.DEBUG)
logger.debug('Start main')

clock = Clock()
# clock.start()

root.tk.mainloop()

