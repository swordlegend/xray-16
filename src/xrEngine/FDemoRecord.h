#if !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "IInputReceiver.h"
#include "Effector.h"

class ENGINE_API CDemoRecord : public CEffectorCam, public IInputReceiver, public pureRender
{
private:
    enum movement_speed
    {
        speed_0,
        speed_1,
        speed_2,
        speed_3,
    };
    movement_speed m_speed;
    movement_speed m_angle_speed;

    static struct force_position
    {
        bool set_position;
        Fvector p;
    } g_position;
    CGameFont m_Font;
    int iCount;
    IWriter* file;
    Fvector m_HPB;
    Fvector m_Position;
    Fmatrix m_Camera;
    u32 m_Stage;

    Fvector m_vT;
    Fvector m_vR;
    Fvector m_vVelocity;
    Fvector m_vAngularVelocity;

    bool m_bMakeCubeMap;
    bool m_bMakeScreenshot;
    int m_iLMScreenshotFragment;
    bool m_bMakeLevelMap;
    shared_str m_CurrentWeatherCycle;

    float m_fSpeed0;
    float m_fSpeed1;
    float m_fSpeed2;
    float m_fSpeed3;
    float m_fAngSpeed0;
    float m_fAngSpeed1;
    float m_fAngSpeed2;
    float m_fAngSpeed3;

    void MakeCubeMapFace(Fvector& D, Fvector& N);
    void MakeLevelMapProcess();
    void MakeScreenshotFace();
    void RecordKey();
    void MakeCubemap();
    void MakeScreenshot();
    void MakeLevelMapScreenshot(bool bHQ);

public:
    CDemoRecord(const char* name, float life_time = 60 * 60 * 1000);
    virtual ~CDemoRecord();

    void OnAxisMove(float x, float y, float scaleX, float scaleY, bool invertX, bool invertY);
    virtual void IR_OnMouseMove(int dx, int dy);
    virtual void IR_OnMouseHold(int btn);

    virtual void IR_OnKeyboardPress(int dik);
    virtual void IR_OnKeyboardHold(int dik);
    virtual void IR_OnKeyboardRelease(int dik);

    void IR_OnControllerPress(int key, float x, float y) override;
    void IR_OnControllerHold(int key, float x, float y) override;
    void IR_OnControllerRelease(int key, float x, float y) override;

    void IR_OnControllerAttitudeChange(Fvector change) override;

    virtual bool ProcessCam(SCamEffectorInfo& info);
    static void SetGlobalPosition(const Fvector& p) { g_position.p.set(p), g_position.set_position = true; }
    static void GetGlobalPosition(Fvector& p) { p.set(g_position.p); }
    bool m_b_redirect_input_to_level;
    virtual void OnRender();
};

#endif // !defined(AFX_FDEMORECORD_H__D7638760_FB61_11D3_B4E3_4854E82A090D__INCLUDED_)
