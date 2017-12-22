//---------------------------------------------------------------------------

#include <vcl.h>
#include <vfw.h>
#include "IniFiles.hpp"
#pragma hdrstop

#include "programming.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
TForm1 *Form1;
TIniFile *Ini = new TIniFile("options.ini");
HWND hwndPreview;
bool preview = false;
bool messageExit;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
TModalResult CheckMessageDialog(AnsiString Message, AnsiString Caption,
  AnsiString CheckBoxCaption, bool *Checked,
  TMsgDlgType DlgType, TMsgDlgButtons Buttons)
{
   TForm *Dialog=CreateMessageDialog(Message, DlgType, Buttons);
   if (!Caption.IsEmpty()) Dialog->Caption = Caption;

   int LeftEdge=Dialog->ClientWidth;
   int TopEdge =Dialog->ClientHeight;
   if (Dialog->ControlCount>0) {
     for (int i = 1; i < Dialog->ControlCount; i++) {
       TControl *Ctrl=Dialog->Controls[i];
       if (Ctrl->Left < LeftEdge) LeftEdge=Ctrl->Left;
       if (Ctrl->Top  < TopEdge)  TopEdge =Ctrl->Top;
     }
   } else {
     LeftEdge=10;
     TopEdge=10;
   }
 
   TCheckBox * CheckBox = new TCheckBox(Dialog);
   CheckBox->Parent  = Dialog;
   CheckBox->Caption = CheckBoxCaption;
   CheckBox->Checked = *Checked;
   CheckBox->Left    = LeftEdge;
   CheckBox->Top     = Dialog->ClientHeight;
   CheckBox->Width   = Dialog->Canvas->TextWidth(CheckBox->Caption)+30;
 
   Dialog->ClientHeight = Dialog->ClientHeight + CheckBox->Height + TopEdge;
 
   if (CheckBox->Width + LeftEdge*2 > Dialog->ClientWidth)
     Dialog->ClientWidth = CheckBox->Width + LeftEdge*2;
 
   TModalResult Result = Dialog->ShowModal();
   *Checked = CheckBox->Checked;
 
   delete Dialog;
   Dialog=NULL;
   
   return Result;
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    hwndPreview = capCreateCaptureWindow (
    TEXT("My Capture Window"),   
    WS_CHILD | WS_VISIBLE,       
    0, 0,
    Panel1->Width,
    Panel1->Height,
    (HWND) Panel1->Handle, 0);
    capDriverConnect(hwndPreview, 0);
    capPreviewScale(hwndPreview,TRUE);
    capPreviewRate(hwndPreview, 66);
    TrayIcon1->Visible = true;
    messageExit = Ini->ReadBool("Settings", "MessageExit", true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
        TModalResult res = Ini->ReadInteger("Settings", "OptionExit", mrYes);

        if(messageExit){
                bool checkv=true;  //checkbox  value
                res = CheckMessageDialog(
                        "Свернуть программу в трей?",
                        "Вопрос всей жизни",
                        "Не показывать это сообщение снова",
                        &checkv,
                        mtConfirmation,
                        TMsgDlgButtons() << mbYes << mbNo  << mbCancel);
                if(checkv){
                        Ini->WriteBool("Settings", "MessageExit", false);
                        Ini->WriteInteger("Settings", "OptionExit", )
                }
        }
        ShowMessage("Check box " + AnsiString(checkv ? "checked":"not cheked") + "\n dialog result:" + AnsiString(res));

        if(res == mrYes) {
                CanClose = false;
                capPreview(hwndPreview, false);
                ShowWindow(Form1->Handle, SW_HIDE);
        }
        if(res == mrYes)
                CanClose = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrayIcon1Click(TObject *Sender)
{
        //TrayIcon1->Visible = false;
        ShowWindow(Form1->Handle, SW_SHOW);
        capPreview(hwndPreview, true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonSettingsClick(TObject *Sender)
{
        if(preview)
                capPreview(hwndPreview, false);
        capDlgVideoFormat(hwndPreview);
        preview = true;
        capPreview(hwndPreview, true);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonPreviewClick(TObject *Sender)
{
        preview = preview ? false : true;
        capPreview(hwndPreview, preview);
}
//---------------------------------------------------------------------------


