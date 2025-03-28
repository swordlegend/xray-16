#include "pch_script.h"
#include "UIGameSP.h"
#include "Actor.h"
#include "Level.h"
#include "xrEngine/xr_input.h"
#include "xrNetServer/NET_Messages.h"

#ifdef DEBUG
#include "attachable_item.h"
#endif

#include "game_cl_single.h"
#include "xrEngine/xr_level_controller.h"
#include "ActorCondition.h"
#include "xrEngine/XR_IOConsole.h"
#include "Common/object_broker.h"
#include "GametaskManager.h"
#include "GameTask.h"

#include "ui/UIActorMenu.h"
#include "ui/UIPdaWnd.h"
#include "ui/UITalkWnd.h"
#include "xrUICore/MessageBox/UIMessageBox.h"

CUIGameSP::CUIGameSP() : m_game(NULL), m_game_objective(NULL)
{
    TalkMenu = xr_new<CUITalkWnd>();
    UIChangeLevelWnd = xr_new<CChangeLevelWnd>();
}

CUIGameSP::~CUIGameSP()
{
    delete_data(TalkMenu);
    delete_data(UIChangeLevelWnd);
    CloseTimeDilator();
}

void CUIGameSP::HideShownDialogs()
{
    HideActorMenu();
    HidePdaMenu();
    CUIDialogWnd* mir = TopInputReceiver();
    if (mir && mir == TalkMenu)
    {
        mir->HideDialog();
    }
}

void CUIGameSP::ReinitDialogs()
{
    delete_data(TalkMenu);
    TalkMenu = xr_new<CUITalkWnd>();
    delete_data(UIChangeLevelWnd);
    UIChangeLevelWnd = xr_new<CChangeLevelWnd>();
}

void CUIGameSP::SetClGame(game_cl_GameState* g)
{
    inherited::SetClGame(g);
    m_game = smart_cast<game_cl_Single*>(g);
    R_ASSERT(m_game);
}

void CUIGameSP::OnFrame()
{
    inherited::OnFrame();

    if (Device.Paused())
        return;

    if (m_game_objective)
    {
        bool keyPressed = false;
        ForAllActionKeys(kSCORES, [&](size_t /*keyboard_index*/, int key)
        {
            if (pInput->iGetAsyncKeyState(key))
            {
                keyPressed = true;
                return true;
            }
            return false;
        });

        if (!keyPressed)
        {
            RemoveCustomStatic("main_task");
            RemoveCustomStatic("secondary_task");
            m_game_objective = NULL;
        }
    }
}

void CUIGameSP::OnUIReset()
{
    inherited::OnUIReset();
    ReinitDialogs();
}

bool CUIGameSP::IR_UIOnKeyboardPress(int dik)
{
    if (inherited::IR_UIOnKeyboardPress(dik))
        return true;
    if (Device.Paused())
        return false;

    CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
    if (!pInvOwner)
        return false;
    CEntityAlive* EA = smart_cast<CEntityAlive*>(Level().CurrentEntity());
    if (!EA || !EA->g_Alive())
        return false;

    CActor* pActor = smart_cast<CActor*>(pInvOwner);
    if (!pActor)
        return false;

    if (!pActor->g_Alive())
        return false;

    switch (GetBindedAction(dik))
    {
    case kACTIVE_JOBS:
    {
        if (!pActor->inventory_disabled())
            ShowPdaMenu();
        break;
    }
    case kMAP:
    {
        if (!pActor->inventory_disabled())
        {
            PdaMenu->Show_MapWnd(true);
            ShowPdaMenu();
        }
        break;
    }
    case kCONTACTS:
    {
        if (!pActor->inventory_disabled())
        {
            PdaMenu->Show_ContactsWnd(true);
            ShowPdaMenu();
        }
        break;
    }
    case kINVENTORY:
    {
        if (!pActor->inventory_disabled())
            ShowActorMenu();

        break;
    }
    case kSCORES:
    {
        if (!pActor->inventory_disabled())
        {
            m_game_objective = AddCustomStatic("main_task", true);
            CGameTask* t1 = Level().GameTaskManager().ActiveTask(eTaskTypeStoryline);
            CGameTask* t2 = Level().GameTaskManager().ActiveTask(eTaskTypeAdditional);

            if (t1 && t2)
            {
                m_game_objective->m_static->SetTextST(t1->m_Title.c_str());
                StaticDrawableWrapper* sm2 = AddCustomStatic("secondary_task", true);
                sm2->m_static->SetTextST(t2->m_Title.c_str());
            }
            else
            {
                if (t1 || t2)
                {
                    CGameTask* t = (t1) ? t1 : t2;
                    m_game_objective->m_static->SetTextST(t->m_Title.c_str());
                    StaticDrawableWrapper* sm2 = AddCustomStatic("secondary_task", true);
                    sm2->m_static->TextItemControl()->SetTextST(t->m_Description.c_str());
                }
                else
                {
                    m_game_objective->m_static->TextItemControl()->SetTextST("st_no_active_task");
                }
            }
        }
        break;
    }
    } // switch (GetBindedAction(dik))

    return false;
}
#ifdef DEBUG
void CUIGameSP::Render()
{
    inherited::Render();
}
#endif

void CUIGameSP::StartTrade(CInventoryOwner* pActorInv, CInventoryOwner* pOtherOwner)
{
    //.	if( MainInputReceiver() )	return;

    ActorMenu->SetActor(pActorInv);
    ActorMenu->SetPartner(pOtherOwner);

    ActorMenu->SetMenuMode(mmTrade);
    ActorMenu->ShowDialog(true);
}

void CUIGameSP::StartUpgrade(CInventoryOwner* pActorInv, CInventoryOwner* pMech)
{
    //.	if( MainInputReceiver() )	return;

    ActorMenu->SetActor(pActorInv);
    ActorMenu->SetPartner(pMech);

    ActorMenu->SetMenuMode(mmUpgrade);
    ActorMenu->ShowDialog(true);
}

void CUIGameSP::StartTalk(bool disable_break)
{
    RemoveCustomStatic("main_task");
    RemoveCustomStatic("secondary_task");

    TalkMenu->b_disable_break = disable_break;
    TalkMenu->ShowDialog(true);
}

void CUIGameSP::StartCarBody(CInventoryOwner* pActorInv, CInventoryOwner* pOtherOwner) // Deadbody search
{
    if (TopInputReceiver())
        return;

    ActorMenu->SetActor(pActorInv);
    ActorMenu->SetPartner(pOtherOwner);

    ActorMenu->SetMenuMode(mmDeadBodySearch);
    ActorMenu->ShowDialog(true);
}

void CUIGameSP::StartCarBody(CInventoryOwner* pActorInv, CInventoryBox* pBox) // Deadbody search
{
    if (TopInputReceiver())
        return;

    ActorMenu->SetActor(pActorInv);
    ActorMenu->SetInvBox(pBox);
    VERIFY(pBox);

    ActorMenu->SetMenuMode(mmDeadBodySearch);
    ActorMenu->ShowDialog(true);
}

extern ENGINE_API bool bShowPauseString;
void CUIGameSP::ChangeLevel(GameGraph::_GRAPH_ID game_vert_id, u32 level_vert_id, Fvector pos, Fvector ang,
    Fvector pos2, Fvector ang2, bool b_use_position_cancel, const shared_str& message_str, bool b_allow_change_level)
{
    if (TopInputReceiver() != UIChangeLevelWnd)
    {
        UIChangeLevelWnd->m_game_vertex_id = game_vert_id;
        UIChangeLevelWnd->m_level_vertex_id = level_vert_id;
        UIChangeLevelWnd->m_position = pos;
        UIChangeLevelWnd->m_angles = ang;
        UIChangeLevelWnd->m_position_cancel = pos2;
        UIChangeLevelWnd->m_angles_cancel = ang2;
        UIChangeLevelWnd->m_b_position_cancel = b_use_position_cancel;
        UIChangeLevelWnd->m_b_allow_change_level = b_allow_change_level;
        UIChangeLevelWnd->m_message_str = message_str;

        UIChangeLevelWnd->ShowDialog(true);
    }
}

void CUIGameSP::StartDialog(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
    inherited::StartDialog(pDialog, bDoHideIndicators);

    if (pDialog == ActorMenu && ActorMenu->GetMenuMode() == mmInventory)
        TimeDilator()->SetCurrentMode(UITimeDilator::Inventory);
    else if (pDialog == PdaMenu)
        TimeDilator()->SetCurrentMode(UITimeDilator::Pda);
}

void CUIGameSP::StopDialog(CUIDialogWnd* pDialog)
{
    inherited::StopDialog(pDialog);

    if (pDialog == ActorMenu || pDialog == PdaMenu)
        TimeDilator()->SetCurrentMode(UITimeDilator::None);
}

CChangeLevelWnd::CChangeLevelWnd() : CUIDialogWnd(CChangeLevelWnd::GetDebugType())
{
    m_messageBox = xr_new<CUIMessageBox>();
    m_messageBox->SetAutoDelete(true);
    AttachChild(m_messageBox);
}

void CChangeLevelWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (pWnd == m_messageBox)
    {
        if (msg == MESSAGE_BOX_YES_CLICKED)
        {
            OnOk();
        }
        else if (msg == MESSAGE_BOX_NO_CLICKED || msg == MESSAGE_BOX_OK_CLICKED)
        {
            OnCancel();
        }
    }
    else
        inherited::SendMessage(pWnd, msg, pData);
}

void CChangeLevelWnd::OnOk()
{
    HideDialog();
    NET_Packet p;
    p.w_begin(M_CHANGE_LEVEL);
    p.w(&m_game_vertex_id, sizeof(m_game_vertex_id));
    p.w(&m_level_vertex_id, sizeof(m_level_vertex_id));
    p.w_vec3(m_position);
    p.w_vec3(m_angles);

    Level().Send(p, net_flags(TRUE));
}

bool CUIGameSP::FillDebugTree(const CUIDebugState& debugState)
{
#ifndef MASTER_GOLD
    if (!CUIGameCustom::FillDebugTree(debugState))
        return false;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
    if (debugState.selected == this)
        flags |= ImGuiTreeNodeFlags_Selected;

    const bool open = ImGui::TreeNodeEx(this, flags, "Game UI (%s)", CUIGameSP::GetDebugType());
    if (ImGui::IsItemClicked())
        debugState.select(this);

    if (open)
    {
        TalkMenu->FillDebugTree(debugState);
        UIChangeLevelWnd->FillDebugTree(debugState);
        if (m_game_objective)
            m_game_objective->wnd()->FillDebugTree(debugState);
        ImGui::TreePop();
    }

    return open;
#else
    UNUSED(debugState);
    return false;
#endif
}

void CUIGameSP::FillDebugInfo()
{
#ifndef MASTER_GOLD
    CUIGameCustom::FillDebugInfo();

    if (ImGui::CollapsingHeader(CUIGameSP::GetDebugType()))
    {

    }
#endif
}

void CChangeLevelWnd::OnCancel()
{
    HideDialog();
    if (m_b_position_cancel)
        Actor()->MoveActor(m_position_cancel, m_angles_cancel);
}

bool CChangeLevelWnd::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
    if (keyboard_action == WINDOW_KEY_PRESSED)
    {
        if (IsBinded(kQUIT, dik))
            OnCancel();
        return true;
    }
    return inherited::OnKeyboardAction(dik, keyboard_action);
}

bool g_block_pause = false;

// Morrey: Не инициализировалась форма, поскольку виртуальная функция отличалась набором аргуметов
void CChangeLevelWnd::Show(bool status)
{
    inherited::Show(status);
    if (status)
    {
        m_messageBox->InitMessageBox(
            m_b_allow_change_level ? "message_box_change_level" : "message_box_change_level_disabled");
        SetWndPos(m_messageBox->GetWndPos());
        m_messageBox->SetWndPos(Fvector2().set(0.0f, 0.0f));
        SetWndSize(m_messageBox->GetWndSize());

        m_messageBox->SetText(m_message_str.c_str());

        g_block_pause = true;
        Device.Pause(TRUE, TRUE, TRUE, "CChangeLevelWnd_show");
        bShowPauseString = false;
    }
    else
    {
        g_block_pause = false;
        Device.Pause(FALSE, TRUE, TRUE, "CChangeLevelWnd_hide");
    }
}

void CChangeLevelWnd::ShowDialog(bool bDoHideIndicators)
{
    m_messageBox->InitMessageBox(m_b_allow_change_level
        ? "message_box_change_level"
        : "message_box_change_level_disabled");

    SetWndPos(m_messageBox->GetWndPos());
    m_messageBox->SetWndPos(Fvector2().set(0.0f, 0.0f));
    SetWndSize(m_messageBox->GetWndSize());

    m_messageBox->SetText(m_message_str.c_str());

    g_block_pause = true;
    Device.Pause(TRUE, TRUE, TRUE, "CChangeLevelWnd_show");
    bShowPauseString = false;
    inherited::ShowDialog(bDoHideIndicators);
}

void CChangeLevelWnd::HideDialog()
{
    g_block_pause = false;
    Device.Pause(FALSE, TRUE, TRUE, "CChangeLevelWnd_hide");
    inherited::HideDialog();
}

void CChangeLevelWnd::FillDebugInfo()
{
#ifndef MASTER_GOLD
    CUIDialogWnd::FillDebugInfo();

    if (ImGui::CollapsingHeader(CChangeLevelWnd::GetDebugType()))
    {
        ImGui::Checkbox("Level change allowed", &m_b_allow_change_level);
        ImGui::DragScalar("Game vertex ID", ImGuiDataType_U16, &m_game_vertex_id);
        ImGui::DragScalar("Level vertex ID", ImGuiDataType_U32, &m_level_vertex_id);
        ImGui::DragFloat3("Position", (float*)&m_position);
        ImGui::DragFloat3("Angles", (float*)&m_angles);
        ImGui::Separator();
        ImGui::Checkbox("Teleport Actor on cancel", &m_b_position_cancel);
        ImGui::DragFloat3("Position on cancel", (float*)&m_position_cancel);
        ImGui::DragFloat3("Angles on cancel", (float*)&m_angles_cancel);
    }
#endif
}
