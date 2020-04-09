; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CEngineViewport
LastTemplate=CView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "editor.h"
LastPage=0

ClassCount=7
Class1=CEditorApp
Class2=CEditorDoc
Class3=CEditorView
Class4=CMainFrame

ResourceCount=4
Resource1=IDR_MAINFRAME
Resource2=IDR_EDITORTYPE
Resource3=IDR_EDITORTYPE_CNTR_IP
Class5=CChildFrame
Class6=CAboutDlg
Class7=CEngineViewport
Resource4=IDD_ABOUTBOX

[CLS:CEditorApp]
Type=0
HeaderFile=editor.h
ImplementationFile=editor.cpp
Filter=N

[CLS:CEditorDoc]
Type=0
HeaderFile=editorDoc.h
ImplementationFile=editorDoc.cpp
Filter=N

[CLS:CEditorView]
Type=0
HeaderFile=editorView.h
ImplementationFile=editorView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
BaseClass=CMDIChildWnd
VirtualFilter=mfWC


[CLS:CAboutDlg]
Type=0
HeaderFile=editor.cpp
ImplementationFile=editor.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_APP_ABOUT
CommandCount=7

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_EDITORTYPE]
Type=1
Class=CEditorView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_EDIT_PASTE_SPECIAL
Command13=ID_OLE_EDIT_LINKS
Command14=ID_OLE_VERB_FIRST
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
CommandCount=21

[MNU:IDR_EDITORTYPE_CNTR_IP]
Type=1
Class=CEditorView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_WINDOW_NEW
Command9=ID_WINDOW_CASCADE
Command10=ID_WINDOW_TILE_HORZ
Command11=ID_WINDOW_ARRANGE
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
Command14=ID_CANCEL_EDIT_CNTR
CommandCount=14

[ACL:IDR_EDITORTYPE_CNTR_IP]
Type=1
Class=CEditorView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_NEXT_PANE
Command5=ID_PREV_PANE
Command6=ID_CANCEL_EDIT_CNTR
CommandCount=6

[CLS:CEngineViewport]
Type=0
HeaderFile=EngineViewport.h
ImplementationFile=EngineViewport.cpp
BaseClass=CView
Filter=C
LastObject=CEngineViewport
VirtualFilter=VWC

