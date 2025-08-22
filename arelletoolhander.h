#ifndef ARELLETOOLHANDLER_H
#define ARELLETOOLHANDLER_H

#include <vector>
using namespace std;

#include "arellepanelview.h"

class CArelleToolHander
{
public:
    CArelleToolHander();
    ~CArelleToolHander();

   static FS_LPSTR              Tool_GetName(FS_LPVOID clientData);
   static FS_LPCWSTR            Tool_GetTitle(FS_LPVOID clientData);
   static void                  Tool_InitNewView(FS_LPVOID clientData, FPD_Document pDocument, FS_HWND hParent);
   static void                  Tool_CloseView(FS_LPVOID clientData, FS_HWND window, FPD_Document doc);
   static void                  Tool_SetActiveView(FS_LPVOID clientData, FPD_Document doc, FS_HWND window);
   static void                  Tool_GetButtonTip(FS_LPVOID clientData, FS_WideString* csOutTip);
   static FS_DIBitmap           Tool_PanelViewGetButtonIconUnion(FS_LPVOID clientData,FS_APPMODE mode,FS_BTNSTATE state, FS_ICONSIZE size);
   static FS_HWND               Tool_PanelViewOnGetHwnd(FS_LPVOID clientData, FPD_Document doc, FS_HWND parent);
   static FS_HWND               Tool_PanelViewOnPanelActive(FS_LPVOID clientData, FPD_Document doc, FS_HWND parent);

   static CArellePanelView*        Tool_GetView(FPD_Document pDocument, FS_HWND hParent);

   static FS_DIBitmap      m_pPanelIcon;

   static std::vector<CArellePanelView*> m_PanelViewArray;
};

#endif // ARELLETOOLHANDER_H
