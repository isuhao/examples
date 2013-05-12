//--------------------------------------------------------------------------------------
// File: ShadowMap.cpp
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include <exception>
#include <atlbase.h>
#include <cstdint>
#include "DXUTcamera.h"
#include "DXUTsettingsdlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"

class exception : public std::exception
{

};

void v(HRESULT hr)
{
    if (hr != S_OK)
    {
        throw exception();
    }
}

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

float g_Light_Space_Far_Z = 350.0f;
float g_Light_Space_Near_Z = 0.01f;

#define SHADOWMAP_SIZE 512

#define HELPTEXTCOLOR D3DXCOLOR( 0.0f, 1.0f, 0.3f, 1.0f )

LPCWSTR g_aszMeshFile[] =
{
    L"room.x",
    L"airplane\\airplane 2.x",
    L"misc\\car.x",
    L"misc\\sphere.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"UI\\arrow.x",
    L"ring.x",
    L"ring.x",
};

#define NUM_OBJ (sizeof(g_aszMeshFile)/sizeof(g_aszMeshFile[0]))

D3DXMATRIXA16 g_amInitObjWorld[NUM_OBJ] =
{
    D3DXMATRIXA16( 3.5f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.5f, 0.0f, 0.0f, 0.0f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 0.43301f, 0.25f, 0.0f, 0.0f, -0.25f, 0.43301f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 5.0f,
                   1.33975f, 0.0f, 1.0f ),
    D3DXMATRIXA16( 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, -14.5f, -7.1f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, -7.0f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 5.0f, 0.2f, 5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 5.0f, 0.2f, -5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -5.0f, 0.2f, 5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -5.0f, 0.2f, -5.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 14.0f, 0.2f, 14.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, 14.0f, 0.2f, -14.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -14.0f, 0.2f, 14.0f,
                   1.0f ),
    D3DXMATRIXA16( 5.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.5f, 0.0f, 0.0f, -9.0f, 0.0f, 0.0f, -14.0f, 0.2f, -14.0f,
                   1.0f ),
    D3DXMATRIXA16( 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, -14.5f, -9.0f, 0.0f,
                   1.0f ),
    D3DXMATRIXA16( 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f, 0.9f, 0.0f, 14.5f, -9.0f, 0.0f,
                   1.0f ),
};


D3DVERTEXELEMENT9 g_aVertDecl[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};


//-----------------------------------------------------------------------------
// Name: class CObj
// Desc: Encapsulates a mesh object in the scene by grouping its world matrix
//       with the mesh.
//-----------------------------------------------------------------------------
#pragma warning( disable : 4324 )
struct CObj
{
    CDXUTXFileMesh m_Mesh;
    D3DXMATRIXA16 m_mWorld;
	D3DXMATRIXA16* m_pWorld;
};




//-----------------------------------------------------------------------------
// Name: class CViewCamera
// Desc: A camera class derived from CFirstPersonCamera.  The arrow keys and
//       numpad keys are disabled for this type of camera.
//-----------------------------------------------------------------------------
class CViewCamera : public CFirstPersonCamera
{
protected:
    virtual D3DUtil_CameraKeys MapKey( UINT nKey )
    {
        // Provide custom mapping here.
        // Same as default mapping but disable arrow keys.
        switch( nKey )
        {
            case 'A':
                return CAM_STRAFE_LEFT;
            case 'D':
                return CAM_STRAFE_RIGHT;
            case 'W':
                return CAM_MOVE_FORWARD;
            case 'S':
                return CAM_MOVE_BACKWARD;
            case 'Q':
                return CAM_MOVE_DOWN;
            case 'E':
                return CAM_MOVE_UP;

            case VK_HOME:
                return CAM_RESET;
        }

        return CAM_UNKNOWN;
    }
};




//-----------------------------------------------------------------------------
// Name: class CLightCamera
// Desc: A camera class derived from CFirstPersonCamera.  The letter keys
//       are disabled for this type of camera.  This class is intended for use
//       by the spot light.
//-----------------------------------------------------------------------------
class CLightCamera : public CFirstPersonCamera
{
protected:
    virtual D3DUtil_CameraKeys MapKey( UINT nKey )
    {
        // Provide custom mapping here.
        // Same as default mapping but disable arrow keys.
        switch( nKey )
        {
            case VK_LEFT:
                return CAM_STRAFE_LEFT;
            case VK_RIGHT:
                return CAM_STRAFE_RIGHT;
            case VK_UP:
                return CAM_MOVE_FORWARD;
            case VK_DOWN:
                return CAM_MOVE_BACKWARD;
            case VK_PRIOR:
                return CAM_MOVE_UP;        // pgup
            case VK_NEXT:
                return CAM_MOVE_DOWN;      // pgdn

            case VK_NUMPAD4:
                return CAM_STRAFE_LEFT;
            case VK_NUMPAD6:
                return CAM_STRAFE_RIGHT;
            case VK_NUMPAD8:
                return CAM_MOVE_FORWARD;
            case VK_NUMPAD2:
                return CAM_MOVE_BACKWARD;
            case VK_NUMPAD9:
                return CAM_MOVE_UP;
            case VK_NUMPAD3:
                return CAM_MOVE_DOWN;

            case VK_HOME:
                return CAM_RESET;
        }

        return CAM_UNKNOWN;
    }
};



//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*                      g_pFont = NULL;         // Font for drawing text
ID3DXFont*                      g_pFontSmall = NULL;    // Font for drawing text
ID3DXSprite*                    g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXEffect*                    g_pEffect = NULL;       // D3DX effect interface
bool                            g_bShowHelp = true;     // If true, it renders the UI control text
CDXUTDialogResourceManager      g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg                 g_SettingsDlg;          // Device settings dialog
CDXUTDialog                     g_HUD;                  // dialog for standard controls
CFirstPersonCamera              g_VCamera[6];           // View camera
CFirstPersonCamera              g_LCamera[6];           // Camera obj to help adjust light
CObj g_Obj[NUM_OBJ];         // Scene object meshes
LPDIRECT3DVERTEXDECLARATION9    g_pVertDecl = NULL;// Vertex decl for the sample
LPDIRECT3DTEXTURE9              g_pTexDef = NULL;       // Default texture for objects
D3DLIGHT9                       g_Light;                // The spot light in the scene
CDXUTXFileMesh                  g_LightMesh;
LPDIRECT3DTEXTURE9              g_pShadowMap = NULL;    // Texture to which the shadow map is rendered
LPDIRECT3DSURFACE9              g_pDSShadow = NULL;     // Depth-stencil buffer for rendering to shadow map
float                           g_fLightFov;            // FOV of the spot light (in radian)
D3DXMATRIXA16                   g_mShadowProj;          // Projection matrix for shadow map

bool                            g_bRightMouseDown = false;		// Indicates whether right mouse button is held
bool                            g_bCameraPerspective = true;	// Indicates whether we should render view from the camera's or the light's perspective
bool                            g_bFreeLight = true;			// Whether the light is freely moveable.

uint32_t						g_FrameStep = 0;

//Animation History for 3 animated objects
D3DXMATRIXA16					g_Animation_Obj_1[6];
D3DXMATRIXA16					g_Animation_Obj_2[6];
D3DXMATRIXA16					g_Animation_Obj_3[6];

//Animation History for 3 animated objects
D3DXMATRIXA16					g_Animation_Time_Obj_1[6];
D3DXMATRIXA16					g_Animation_Time_Obj_2[6];
D3DXMATRIXA16					g_Animation_Time_Obj_3[6];

CFirstPersonCamera              g_Time_VCamera[6];	// View camera
CFirstPersonCamera              g_Time_LCamera[6];  // Camera obj to help adjust light

IDirect3DTexture9 *				g_iframe_back_buffer[2];
IDirect3DTexture9 *				g_iframe_depth_buffer[2];

IDirect3DSurface9 *				g_iframe_back_buffer_surface[2];
IDirect3DSurface9 *				g_iframe_depthbuffer_surface[2];


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN 1
#define IDC_TOGGLEREF        3
#define IDC_CHANGEDEVICE     4
#define IDC_CHECKBOX         5
#define IDC_LIGHTPERSPECTIVE 6
#define IDC_ATTACHLIGHTTOCAR 7



//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void InitializeDialogs();
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed,
                                  void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                 void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void RenderText();
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
                         bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnLostDevice( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );

void RenderScene(	IDirect3DDevice9* pd3dDevice, 
					bool bRenderShadow, 
					float fElapsedTime, 
					const D3DXMATRIX* pmView,
					const D3DXMATRIX* pmProj,
					const CFirstPersonCamera*	  pLightCamera
				  );

class ScopedAnimations
{
	public:

	explicit ScopedAnimations(uint32_t frame_step)
	{
		old_matrix[0] = g_Obj[1].m_pWorld;
		old_matrix[1] = g_Obj[2].m_pWorld;
		old_matrix[2] = g_Obj[3].m_pWorld;

		g_Obj[1].m_pWorld =  &g_Animation_Obj_1[frame_step];
		g_Obj[2].m_pWorld =  &g_Animation_Obj_2[frame_step];
		g_Obj[3].m_pWorld =  &g_Animation_Obj_3[frame_step];

	}

	~ScopedAnimations()
	{
		g_Obj[1].m_pWorld = old_matrix[0];
		g_Obj[2].m_pWorld = old_matrix[1];
		g_Obj[3].m_pWorld = old_matrix[2];
	}

	private:

	D3DXMATRIXA16* old_matrix[3];
};

class ScopedAnimationsTime
{
	public:

	explicit ScopedAnimationsTime(uint32_t frame_step)
	{
		old_matrix[0] = g_Obj[1].m_pWorld;
		old_matrix[1] = g_Obj[2].m_pWorld;
		old_matrix[2] = g_Obj[3].m_pWorld;

		g_Obj[1].m_pWorld =  &g_Animation_Time_Obj_1[frame_step];
		g_Obj[2].m_pWorld =  &g_Animation_Time_Obj_2[frame_step];
		g_Obj[3].m_pWorld =  &g_Animation_Time_Obj_3[frame_step];

	}

	~ScopedAnimationsTime()
	{
		g_Obj[1].m_pWorld = old_matrix[0];
		g_Obj[2].m_pWorld = old_matrix[1];
		g_Obj[3].m_pWorld = old_matrix[2];
	}

	private:

	D3DXMATRIXA16* old_matrix[3];
};


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	for (uint32_t i = 0; i < 6; ++i)
	{
		// Initialize the camera
		g_VCamera[i].SetScalers( 0.01f, 15.0f );
		g_LCamera[i].SetScalers( 0.01f, 8.0f );
		g_VCamera[i].SetRotateButtons( true, false, false );
		g_LCamera[i].SetRotateButtons( false, false, true );

		g_Time_VCamera[i].SetScalers( 0.01f, 15.0f );
		g_Time_LCamera[i].SetScalers( 0.01f, 8.0f );
		g_Time_VCamera[i].SetRotateButtons( true, false, false );
		g_Time_LCamera[i].SetRotateButtons( false, false, true );

		// Set up the view parameters for the camera
		D3DXVECTOR3 vFromPt = D3DXVECTOR3( 0.0f, 5.0f, -18.0f );
		D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
		g_VCamera[i].SetViewParams( &vFromPt, &vLookatPt );
		g_Time_VCamera[i].SetViewParams( &vFromPt, &vLookatPt );

		vFromPt = D3DXVECTOR3( 0.0f, 0.0f, -12.0f );
		vLookatPt = D3DXVECTOR3( 0.0f, -2.0f, 1.0f );

		g_LCamera[i].SetViewParams( &vFromPt, &vLookatPt );
		g_Time_LCamera[i].SetViewParams( &vFromPt, &vLookatPt );
	}

    // Initialize the spot light
    g_fLightFov = D3DX_PI / 2.0f;

    g_Light.Diffuse.r = 1.0f;
    g_Light.Diffuse.g = 1.0f;
    g_Light.Diffuse.b = 1.0f;
    g_Light.Diffuse.a = 1.0f;
    g_Light.Position = D3DXVECTOR3( -8.0f, -8.0f, 0.0f );
    g_Light.Direction = D3DXVECTOR3( 1.0f, -1.0f, 0.0f );
    D3DXVec3Normalize( ( D3DXVECTOR3* )&g_Light.Direction, ( D3DXVECTOR3* )&g_Light.Direction );
    g_Light.Range = 10.0f;
    g_Light.Theta = g_fLightFov / 2.0f;
    g_Light.Phi = g_fLightFov / 2.0f;

    // Set the callback functions. These functions allow DXUT to notify
    // the application about device changes, user input, and windows messages.  The 
    // callbacks are optional so you need only set callbacks for events you're interested 
    // in. However, if you don't handle the device reset/lost callbacks then the sample 
    // framework won't be able to reset your device since the application must first 
    // release all device resources before resetting.  Likewise, if you don't handle the 
    // device created/destroyed callbacks then DXUT won't be able to 
    // recreate your device resources.
    DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackMouse( MouseProc );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    InitializeDialogs();

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    // Initialize DXUT and create the desired Win32 window and Direct3D 
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the defaul hotkeys
    DXUTCreateWindow( L"ShadowMap" );
    DXUTCreateDevice( true, 640, 480 );

    // Pass control to DXUT for handling the message pump and 
    // dispatching render calls. DXUT will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Sets up the dialogs
//--------------------------------------------------------------------------------------
void InitializeDialogs()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
    g_HUD.AddCheckBox( IDC_CHECKBOX, L"Display help text", 35, iY += 24, 125, 22, true, VK_F1 );
    g_HUD.AddCheckBox( IDC_LIGHTPERSPECTIVE, L"View from light's perspective", 0, iY += 24, 160, 22, false, L'V' );
    g_HUD.AddCheckBox( IDC_ATTACHLIGHTTOCAR, L"Attach light to car", 0, iY += 24, 160, 22, false, L'F' );
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Must support pixel shader 2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
        return false;

    // need to support D3DFMT_R32F render target
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_CUBETEXTURE, D3DFMT_R32F ) ) )
        return false;

    // need to support D3DFMT_A8R8G8B8 render target
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_CUBETEXTURE, D3DFMT_A8R8G8B8 ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the 
// application to modify the device settings. The supplied pDeviceSettings parameter 
// contains the settings that the framework has selected for the new device, and the 
// application can make any desired changes directly to this structure.  Note however that 
// DXUT will not correct invalid device settings so care must be taken 
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.  
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );

    IDirect3D9* pD3D = DXUTGetD3D9Object();
    D3DCAPS9 caps;

    v( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
                            pDeviceSettings->d3d9.DeviceType,
                            &caps ) );

    // Turn vsync off
    pDeviceSettings->d3d9.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    g_SettingsDlg.GetDialogControl()->GetComboBox( DXUTSETTINGSDLG_PRESENT_INTERVAL )->SetEnabled( false );

    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                 void* pUserContext )
{
    HRESULT hr;


    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &g_pFont ) );
    V_RETURN( D3DXCreateFont( pd3dDevice, 12, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &g_pFontSmall ) );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
    // shader debugger. Debugging vertex shaders requires either REF or software vertex 
    // processing, and debugging pixel shaders requires REF.  The 
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile 
    // against the next higher available software target, which ensures that the 
    // unoptimized shaders do not exceed the shader model limitations.  Setting these 
    // flags will cause slower rendering since the shaders will be unoptimized and 
    // forced into software.  See the DirectX documentation for more information about 
    // using the shader debugger.
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif

#ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
#ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"ShadowMap.fx" ) );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags,
                                        NULL, &g_pEffect, NULL ) );

    // Create vertex declaration
    V_RETURN( pd3dDevice->CreateVertexDeclaration( g_aVertDecl, &g_pVertDecl ) );

    // Initialize the meshes
    for( int i = 0; i < NUM_OBJ; ++i )
    {
        V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, g_aszMeshFile[i] ) );
        if( FAILED( g_Obj[i].m_Mesh.Create( pd3dDevice, str ) ) )
            return DXUTERR_MEDIANOTFOUND;
        V_RETURN( g_Obj[i].m_Mesh.SetVertexDecl( pd3dDevice, g_aVertDecl ) );
        g_Obj[i].m_mWorld = g_amInitObjWorld[i];
		g_Obj[i].m_pWorld = &g_Obj[i].m_mWorld;
    }

	//initialize animation matrices
	for (uint32_t i = 0; i < 6; ++i)
	{
		g_Animation_Obj_1[i] = g_Obj[1].m_mWorld;
		g_Animation_Time_Obj_1[i] = g_Obj[1].m_mWorld;
	}

	for (uint32_t i = 0; i < 6; ++i)
	{
		g_Animation_Obj_2[i] = g_Obj[2].m_mWorld;
		g_Animation_Time_Obj_2[i] = g_Obj[2].m_mWorld;
	}

	for (uint32_t i = 0; i < 6; ++i)
	{
		g_Animation_Obj_1[3] = g_Obj[3].m_mWorld;
		g_Animation_Time_Obj_1[3] = g_Obj[3].m_mWorld;
	}

	// Initialize the light mesh
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"spotlight.x" ) );
    if( FAILED( g_LightMesh.Create( pd3dDevice, str ) ) )
        return DXUTERR_MEDIANOTFOUND;
    V_RETURN( g_LightMesh.SetVertexDecl( pd3dDevice, g_aVertDecl ) );

    // World transform to identity
    D3DXMATRIXA16 mIdent;
    D3DXMatrixIdentity( &mIdent );
    V_RETURN( pd3dDevice->SetTransform( D3DTS_WORLD, &mIdent ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
    if( g_pFontSmall )
        V_RETURN( g_pFontSmall->OnResetDevice() );
    if( g_pEffect )
        V_RETURN( g_pEffect->OnResetDevice() );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;

	for (uint32_t i = 0; i<6;++i)
	{
	    g_VCamera[i].SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 100.0f );
	    g_LCamera[i].SetProjParams( D3DX_PI / 4, fAspectRatio, g_Light_Space_Near_Z, g_Light_Space_Far_Z );

		g_Time_VCamera[i].SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 100.0f );
	    g_Time_LCamera[i].SetProjParams( D3DX_PI / 4, fAspectRatio, g_Light_Space_Near_Z, g_Light_Space_Far_Z );
	}

    // Create the default texture (used when a triangle does not use a texture)
    V_RETURN( pd3dDevice->CreateTexture( 1, 1, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexDef,
                                         NULL ) );
    D3DLOCKED_RECT lr;
    V_RETURN( g_pTexDef->LockRect( 0, &lr, NULL, 0 ) );
    *( LPDWORD )lr.pBits = D3DCOLOR_RGBA( 255, 255, 255, 255 );
    V_RETURN( g_pTexDef->UnlockRect( 0 ) );

    // Restore the scene objects
    for( int i = 0; i < NUM_OBJ; ++i )
        V_RETURN( g_Obj[i].m_Mesh.RestoreDeviceObjects( pd3dDevice ) );
    V_RETURN( g_LightMesh.RestoreDeviceObjects( pd3dDevice ) );

    // Restore the effect variables
    V_RETURN( g_pEffect->SetVector( "g_vLightDiffuse", ( D3DXVECTOR4* )&g_Light.Diffuse ) );
    V_RETURN( g_pEffect->SetFloat( "g_fCosTheta", cosf( g_Light.Theta ) ) );

    // Create the shadow map texture
    V_RETURN( pd3dDevice->CreateTexture( SHADOWMAP_SIZE, SHADOWMAP_SIZE,
                                         1, D3DUSAGE_RENDERTARGET,
                                         D3DFMT_R32F,
                                         D3DPOOL_DEFAULT,
                                         &g_pShadowMap,
                                         NULL ) );

    // Create the depth-stencil buffer to be used with the shadow map
    // We do this to ensure that the depth-stencil buffer is large
    // enough and has correct multisample type/quality when rendering
    // the shadow map.  The default depth-stencil buffer created during
    // device creation will not be large enough if the user resizes the
    // window to a very small size.  Furthermore, if the device is created
    // with multisampling, the default depth-stencil buffer will not
    // work with the shadow map texture because texture render targets
    // do not support multisample.
    DXUTDeviceSettings d3dSettings = DXUTGetDeviceSettings();
    V_RETURN( pd3dDevice->CreateDepthStencilSurface( SHADOWMAP_SIZE,
                                                     SHADOWMAP_SIZE,
                                                     d3dSettings.d3d9.pp.AutoDepthStencilFormat,
                                                     D3DMULTISAMPLE_NONE,
                                                     0,
                                                     TRUE,
                                                     &g_pDSShadow,
                                                     NULL ) );

    // Initialize the shadow projection matrix
    D3DXMatrixPerspectiveFovLH( &g_mShadowProj, g_fLightFov, 1, g_Light_Space_Near_Z, g_Light_Space_Far_Z );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, pBackBufferSurfaceDesc->Height );
    CDXUTControl* pControl = g_HUD.GetControl( IDC_LIGHTPERSPECTIVE );
    if( pControl )
        pControl->SetLocation( 0, pBackBufferSurfaceDesc->Height - 50 );
    pControl = g_HUD.GetControl( IDC_ATTACHLIGHTTOCAR );
    if( pControl )
        pControl->SetLocation( 0, pBackBufferSurfaceDesc->Height - 25 );
    return S_OK;
}

CFirstPersonCamera* GetLightCamera( uint32_t frame_step )
{
	return &g_LCamera[ frame_step ];
}

CFirstPersonCamera* GetTimeLightCamera( uint32_t frame_step )
{
	return &g_Time_LCamera[ frame_step ];
}

CFirstPersonCamera* GetViewCamera( uint32_t frame_step )
{
	return &g_VCamera[ frame_step ];
}

CFirstPersonCamera* GetTimeViewCamera( uint32_t frame_step )
{
	return &g_Time_VCamera[ frame_step ];
}

void CopyViewCameras( )
{
	for ( uint32_t i = 0; i<5; ++i)
	{
		g_VCamera[i] = g_VCamera[i+1];
	}
}

void CopyLightCameras( )
{
	for ( uint32_t i = 0; i<5; ++i)
	{
		g_LCamera[i] = g_LCamera[i+1];
	}
}

void CopyAnimations()
{
	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Animation_Obj_1[i] = g_Animation_Obj_1[i+1];
	}

	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Animation_Obj_2[i] = g_Animation_Obj_2[i+1];
	}

	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Animation_Obj_3[i] = g_Animation_Obj_3[i+1];
	}
}

D3DXMATRIXA16* GetAnimation_1( )
{
	return &g_Animation_Obj_1[5];
}

D3DXMATRIXA16* GetAnimation_2( )
{
	return &g_Animation_Obj_2[5];
}

D3DXMATRIXA16* GetAnimation_3(  )
{
	return &g_Animation_Obj_3[5];
}

void CopyTimeViewCameras()
{
	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Time_VCamera[i] = g_Time_VCamera[i+1];
	}
}

void CopyTimeLightCameras()
{
	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Time_LCamera[i] = g_Time_LCamera[i+1];
	}
}

void CopyTimeAnimations()
{
	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Animation_Time_Obj_1[i] = g_Animation_Time_Obj_1[i+1];
	}

	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Animation_Time_Obj_2[i] = g_Animation_Time_Obj_2[i+1];
	}

	for ( uint32_t i = 0; i<5; ++i)
	{
		g_Animation_Time_Obj_3[i] = g_Animation_Time_Obj_3[i+1];
	}
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	CopyViewCameras();
	CopyLightCameras();
	CopyAnimations();

	// Update the camera's position based on user input 
    GetViewCamera( 5 )->FrameMove( fElapsedTime );
    GetLightCamera( 5 )->FrameMove( fElapsedTime );
	
	// Animate the plane, car and sphere meshes
    D3DXMATRIXA16 m;
    D3DXMatrixRotationY( &m, D3DX_PI * fElapsedTime / 4.0f );
    D3DXMatrixMultiply( GetAnimation_1(), GetAnimation_1(), &m );

    D3DXMatrixRotationY( &m, -D3DX_PI * fElapsedTime / 4.0f );
    D3DXMatrixMultiply( GetAnimation_2(), GetAnimation_2(), &m );

    D3DXVECTOR3 vR( 0.1f, 1.0f, -0.2f );
    D3DXMatrixRotationAxis( &m, &vR, -D3DX_PI * fElapsedTime / 6.0f );
    D3DXMatrixMultiply( GetAnimation_3(), &m, GetAnimation_3() );

	if (g_FrameStep % 4 == 0 )
	{
		g_Time_VCamera[5] = g_VCamera[5];
		g_Time_LCamera[5] = g_LCamera[5];

		g_Animation_Time_Obj_1[5] = g_Animation_Obj_1[5];
		g_Animation_Time_Obj_2[5] = g_Animation_Obj_2[5];
		g_Animation_Time_Obj_3[5] = g_Animation_Obj_2[5];

		CopyTimeViewCameras();
		CopyTimeLightCameras();
		CopyTimeAnimations();
	}

}

//--------------------------------------------------------------------------------------
// Renders the scene onto the current render target using the current
// technique in the effect.
//--------------------------------------------------------------------------------------
void RenderScene( IDirect3DDevice9* pd3dDevice, bool bRenderShadow, float fElapsedTime, const D3DXMATRIX* pmView,
                  const D3DXMATRIX* pmProj, const CFirstPersonCamera* pLightCamera )
{
    HRESULT hr;

    // Set the projection matrix
    v( g_pEffect->SetMatrix( "g_mProj", pmProj ) );

    // Update the light parameters in the effect
    if( g_bFreeLight )
    {
        // Freely moveable light. Get light parameter
        // from the light camera.
        D3DXVECTOR3 v1 = *pLightCamera->GetEyePt();
        D3DXVECTOR4 v4;
        D3DXVec3Transform( &v4, &v1, pmView );
        v( g_pEffect->SetVector( "g_vLightPos", &v4 ) );
        *( D3DXVECTOR3* )&v4 = *pLightCamera->GetWorldAhead();
        v4.w = 0.0f;  // Set w 0 so that the translation part doesn't come to play
        D3DXVec4Transform( &v4, &v4, pmView );  // Direction in view space
        D3DXVec3Normalize( ( D3DXVECTOR3* )&v4, ( D3DXVECTOR3* )&v4 );
        v( g_pEffect->SetVector( "g_vLightDir", &v4 ) );
    }
    else
    {
        // Light attached to car.  Get the car's world position and direction.
        D3DXMATRIXA16 m = *g_Obj[2].m_pWorld;
        D3DXVECTOR3 v1( m._41, m._42, m._43 );
        D3DXVECTOR4 vPos;
        D3DXVec3Transform( &vPos, &v1, pmView );
        D3DXVECTOR4 v4( 0.0f, 0.0f, -1.0f, 1.0f );  // In object space, car is facing -Z
        m._41 = m._42 = m._43 = 0.0f;  // Remove the translation
        D3DXVec4Transform( &v4, &v4, &m );  // Obtain direction in world space
        v4.w = 0.0f;  // Set w 0 so that the translation part doesn't come to play
        D3DXVec4Transform( &v4, &v4, pmView );  // Direction in view space
        D3DXVec3Normalize( ( D3DXVECTOR3* )&v4, ( D3DXVECTOR3* )&v4 );
        v( g_pEffect->SetVector( "g_vLightDir", &v4 ) );
        vPos += v4 * 4.0f;  // Offset the center by 3 so that it's closer to the headlight.
        v( g_pEffect->SetVector( "g_vLightPos", &vPos ) );
    }

    // Clear the render buffers
    v( pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                          0x000000ff, 1.0f, 0L ) );

    if( bRenderShadow )
        v( g_pEffect->SetTechnique( "RenderShadow" ) );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        if( !bRenderShadow )
            v( g_pEffect->SetTechnique( "RenderScene" ) );

		g_pEffect->SetFloat("g_Light_Space_Far_Z", g_Light_Space_Far_Z);
		g_pEffect->SetFloat("g_Light_Space_Near_Z", g_Light_Space_Near_Z);
		g_pEffect->SetMatrix("g_mShadowProj",&g_mShadowProj);

        // Render the objects
        for( int obj = 0; obj < NUM_OBJ; ++obj )
        {
            D3DXMATRIXA16 mWorldView = *g_Obj[obj].m_pWorld;
            D3DXMatrixMultiply( &mWorldView, &mWorldView, pmView );
            v( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );

            LPD3DXMESH pMesh = g_Obj[obj].m_Mesh.GetMesh();
            UINT cPass;
            v( g_pEffect->Begin( &cPass, 0 ) );
            for( UINT p = 0; p < cPass; ++p )
            {
                v( g_pEffect->BeginPass( p ) );

                for( DWORD i = 0; i < g_Obj[obj].m_Mesh.m_dwNumMaterials; ++i )
                {
                    D3DXVECTOR4 vDif( g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.r,
                                      g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.g,
                                      g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.b,
                                      g_Obj[obj].m_Mesh.m_pMaterials[i].Diffuse.a );
                    v( g_pEffect->SetVector( "g_vMaterial", &vDif ) );
                    if( g_Obj[obj].m_Mesh.m_pTextures[i] )
					{
                        v( g_pEffect->SetTexture( "g_txScene", g_Obj[obj].m_Mesh.m_pTextures[i] ) );
					}
                    else
					{
                        v( g_pEffect->SetTexture( "g_txScene", g_pTexDef ) );
					}
                    v( g_pEffect->CommitChanges() );
                    v( pMesh->DrawSubset( i ) );
                }
                v( g_pEffect->EndPass() );
            }
            v( g_pEffect->End() );
        }

        // Render light
        if( !bRenderShadow )
            v( g_pEffect->SetTechnique( "RenderLight" ) );

        D3DXMATRIXA16 mWorldView = *pLightCamera->GetWorldMatrix();
        D3DXMatrixMultiply( &mWorldView, &mWorldView, pmView );
        v( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );

        UINT cPass;
        LPD3DXMESH pMesh = g_LightMesh.GetMesh();
        v( g_pEffect->Begin( &cPass, 0 ) );
        for( UINT p = 0; p < cPass; ++p )
        {
            v( g_pEffect->BeginPass( p ) );

            for( DWORD i = 0; i < g_LightMesh.m_dwNumMaterials; ++i )
            {
                D3DXVECTOR4 vDif( g_LightMesh.m_pMaterials[i].Diffuse.r,
                                  g_LightMesh.m_pMaterials[i].Diffuse.g,
                                  g_LightMesh.m_pMaterials[i].Diffuse.b,
                                  g_LightMesh.m_pMaterials[i].Diffuse.a );
                v( g_pEffect->SetVector( "g_vMaterial", &vDif ) );
                v( g_pEffect->SetTexture( "g_txScene", g_LightMesh.m_pTextures[i] ) );
                v( g_pEffect->CommitChanges() );
                v( pMesh->DrawSubset( i ) );
            }
            v( g_pEffect->EndPass() );
        }
        v( g_pEffect->End() );

        if( !bRenderShadow )
            // Render stats and help text
            RenderText();

        // Render the UI elements
        if( !bRenderShadow )
            g_HUD.OnRender( fElapsedTime );

        V( pd3dDevice->EndScene() );
    }
}


void RenderIFrameStep1(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext, uint32_t frame_step )
{
	//simulate lower fps
	::Sleep(6);

	//Step 1
    //
    // Compute the view matrix for the light
    // This changes depending on the light mode
    // (free movement or attached)
    //
	
	//put animations for rendering
	ScopedAnimationsTime f(frame_step);

	D3DXMATRIXA16 mLightView;

    if( g_bFreeLight )
        mLightView = *GetTimeLightCamera(frame_step)->GetViewMatrix();
    else
    {
        // Light attached to car.
        mLightView = *g_Obj[2].m_pWorld;
        D3DXVECTOR3 vPos( mLightView._41, mLightView._42, mLightView._43 );  // Offset z by -2 so that it's closer to headlight
        D3DXVECTOR4 vDir = D3DXVECTOR4( 0.0f, 0.0f, -1.0f, 1.0f );  // In object space, car is facing -Z
        mLightView._41 = mLightView._42 = mLightView._43 = 0.0f;  // Remove the translation
        D3DXVec4Transform( &vDir, &vDir, &mLightView );  // Obtain direction in world space
        vDir.w = 0.0f;  // Set w 0 so that the translation part below doesn't come to play
        D3DXVec4Normalize( &vDir, &vDir );
        vPos.x += vDir.x * 4.0f;  // Offset the center by 4 so that it's closer to the headlight
        vPos.y += vDir.y * 4.0f;
        vPos.z += vDir.z * 4.0f;
        vDir.x += vPos.x;  // vDir denotes the look-at point
        vDir.y += vPos.y;
        vDir.z += vPos.z;
        D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
        D3DXMatrixLookAtLH( &mLightView, &vPos, ( D3DXVECTOR3* )&vDir, &vUp );
    }

	//
    // Render the shadow map
    //
    CComPtr<IDirect3DSurface9> pOldRT;
    v( pd3dDevice->GetRenderTarget( 0, &pOldRT ) );

    CComPtr<IDirect3DSurface9> pShadowSurf;
    v ( g_pShadowMap->GetSurfaceLevel( 0, &pShadowSurf ) ) ;
    v ( pd3dDevice->SetRenderTarget( 0, pShadowSurf ) );

	CComPtr<IDirect3DSurface9> pOldDS;
	v( pd3dDevice->GetDepthStencilSurface( &pOldDS ) ) ;
	v( pd3dDevice->SetDepthStencilSurface( g_pDSShadow ) );

    {
        CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Shadow Map" );
        RenderScene( pd3dDevice, true, fElapsedTime, &mLightView, &g_mShadowProj, GetLightCamera(frame_step) );
    }

	v(pd3dDevice->SetDepthStencilSurface( pOldDS ));
    v(pd3dDevice->SetRenderTarget( 0, pOldRT ) );
}

void RenderIFrameStep2(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext, uint32_t frame_step)
{
	//simulate lower fps
	::Sleep(6);

	//Step 1
	//
    // Compute the view matrix for the light
    // This changes depending on the light mode
    // (free movement or attached)
    //

	//put animations for rendering
	ScopedAnimationsTime f(frame_step);
	D3DXMATRIXA16 mLightView;

    if( g_bFreeLight )
        mLightView = *GetTimeLightCamera(frame_step)->GetViewMatrix();
    else
    {
        // Light attached to car.
        mLightView = *g_Obj[2].m_pWorld;
        D3DXVECTOR3 vPos( mLightView._41, mLightView._42, mLightView._43 );  // Offset z by -2 so that it's closer to headlight
        D3DXVECTOR4 vDir = D3DXVECTOR4( 0.0f, 0.0f, -1.0f, 1.0f );  // In object space, car is facing -Z
        mLightView._41 = mLightView._42 = mLightView._43 = 0.0f;  // Remove the translation
        D3DXVec4Transform( &vDir, &vDir, &mLightView );  // Obtain direction in world space
        vDir.w = 0.0f;  // Set w 0 so that the translation part below doesn't come to play
        D3DXVec4Normalize( &vDir, &vDir );
        vPos.x += vDir.x * 4.0f;  // Offset the center by 4 so that it's closer to the headlight
        vPos.y += vDir.y * 4.0f;
        vPos.z += vDir.z * 4.0f;
        vDir.x += vPos.x;  // vDir denotes the look-at point
        vDir.y += vPos.y;
        vDir.z += vPos.z;
        D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
        D3DXMatrixLookAtLH( &mLightView, &vPos, ( D3DXVECTOR3* )&vDir, &vUp );
    }


	//Step 2
    //
    // Now that we have the shadow map, render the scene.
    //

	CFirstPersonCamera* view_camera = GetTimeViewCamera(frame_step);
    const D3DXMATRIX* pmView = g_bCameraPerspective ? view_camera->GetViewMatrix() :
        &mLightView;

    // Initialize required parameter
    v( g_pEffect->SetTexture( "g_txShadow", g_pShadowMap ) );
    // Compute the matrix to transform from view space to
    // light projection space.  This consists of
    // the inverse of view matrix * view matrix of light * light projection matrix
    D3DXMATRIXA16 mViewToLightProj;
    mViewToLightProj = *pmView;
    D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
    D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &mLightView );
    //D3DXMatrixMultiply( &mViewToLightProj, &mViewToLightProj, &g_mShadowProj );
    v( g_pEffect->SetMatrix( "g_mViewToLight", &mViewToLightProj ) );

    {
        CDXUTPerfEventGenerator g( DXUT_PERFEVENTCOLOR, L"Scene" );
        RenderScene( pd3dDevice, false, fElapsedTime, pmView, view_camera->GetProjMatrix(), GetLightCamera(frame_step) );
    }
    g_pEffect->SetTexture( "g_txShadow", NULL );
}

void RenderIFrameStep3(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext, uint32_t time_step)
{
	//simulate lower fps
	::Sleep(6);
}

void RenderIFrameStep4(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext, uint32_t time_step)
{
	//simulate lower fps
	::Sleep(6);
}

void RenderBFrame(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext, uint32_t frame_step, uint32_t time_step_1, uint32_t time_step_2 )
{

}

void DisplayIFrame(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext, uint32_t time_step )
{

}

//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, DXUT will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

	if ( g_FrameStep % 4 == 0 )
	{
		RenderIFrameStep1( pd3dDevice, fTime, fElapsedTime, pUserContext, 5);	// t
		RenderBFrame( pd3dDevice, fTime, fElapsedTime, pUserContext, 5, 3, 4 );	// ( t-2, t-1, 1 / n )
	}

	if ( g_FrameStep % 4 == 1 )
	{
		RenderIFrameStep2( pd3dDevice, fTime, fElapsedTime, pUserContext, 5);	// t
		RenderBFrame( pd3dDevice, fTime, fElapsedTime, pUserContext, 5, 3, 4 );	// ( t-2, t-1, 2 / n )
	}

	if ( g_FrameStep % 4 == 2 )
	{
		RenderIFrameStep3( pd3dDevice, fTime, fElapsedTime, pUserContext, 5);	// t
		DisplayIFrame( pd3dDevice, fTime, fElapsedTime, pUserContext, 4);		// ( t-1 )
	}

	if ( g_FrameStep % 4 == 3 )
	{
		RenderIFrameStep4( pd3dDevice, fTime, fElapsedTime, pUserContext, 5);	// t
		RenderBFrame( pd3dDevice, fTime, fElapsedTime, pUserContext, 5, 4, 5 );	// ( t-1, t, 1 / n)
	}

	g_FrameStep++;
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) ); // Show FPS
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

    // Draw help
    if( g_bShowHelp )
    {
        const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
        txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height - 15 * 10 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Controls:" );

        txtHelper.SetInsertionPos( 15, pd3dsdBackBuffer->Height - 15 * 9 );
        WCHAR text[512];
        swprintf_s(text,L"Rotate camera\nMove camera\n"
            L"Rotate light\nMove light\n"
            L"Change light mode (Current: %s)\nChange view reference (Current: %s)\n"
            L"Hidehelp\nQuit",
            g_bFreeLight ? L"Free" : L"Car-attached",
            g_bCameraPerspective ? L"Camera" : L"Light" );
        txtHelper.DrawTextLine(text);

            
        txtHelper.SetInsertionPos( 265, pd3dsdBackBuffer->Height - 15 * 9 );
        txtHelper.DrawTextLine(
            L"Left drag mouse\nW,S,A,D,Q,E\n"
            L"Right drag mouse\nW,S,A,D,Q,E while holding right mouse\n"
            L"F\nV\nF1\nESC" );
    }
    else
    {
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Press F1 for help" );
    }
    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all windows messages to camera and dialogs so they can respond to user input
    if( WM_KEYDOWN != uMsg || g_bRightMouseDown )
        GetLightCamera( 5 )->HandleMessages( hWnd, uMsg, wParam, lParam );

    if( WM_KEYDOWN != uMsg || !g_bRightMouseDown )
    {
        if( g_bCameraPerspective )
            GetViewCamera(5)->HandleMessages( hWnd, uMsg, wParam, lParam );
        else
            GetLightCamera(5)->HandleMessages( hWnd, uMsg, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke 
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}


void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down,
                         bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext )
{
    g_bRightMouseDown = bRightButtonDown;
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
        case IDC_CHECKBOX:
        {
            CDXUTCheckBox* pCheck = ( CDXUTCheckBox* )pControl;
            g_bShowHelp = pCheck->GetChecked();
            break;
        }
        case IDC_LIGHTPERSPECTIVE:
        {
            CDXUTCheckBox* pCheck = ( CDXUTCheckBox* )pControl;
            g_bCameraPerspective = !pCheck->GetChecked();
            if( g_bCameraPerspective )
            {
                GetViewCamera(5)->SetRotateButtons( true, false, false );
                GetLightCamera(5)->SetRotateButtons( false, false, true );
            }
            else
            {
                GetViewCamera(5)->SetRotateButtons( false, false, false );
                GetLightCamera(5)->SetRotateButtons( true, false, true );
            }

            break;
        }
        case IDC_ATTACHLIGHTTOCAR:
        {
            CDXUTCheckBox* pCheck = ( CDXUTCheckBox* )pControl;
            g_bFreeLight = !pCheck->GetChecked();
            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all 
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for 
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pFontSmall )
        g_pFontSmall->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );

    SAFE_RELEASE( g_pDSShadow );
    SAFE_RELEASE( g_pShadowMap );
    SAFE_RELEASE( g_pTexDef );

    for( int i = 0; i < NUM_OBJ; ++i )
        g_Obj[i].m_Mesh.InvalidateDeviceObjects();
    g_LightMesh.InvalidateDeviceObjects();
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE( g_pEffect );
    SAFE_RELEASE( g_pFont );
    SAFE_RELEASE( g_pFontSmall );
    SAFE_RELEASE( g_pVertDecl );

    SAFE_RELEASE( g_pEffect );

    for( int i = 0; i < NUM_OBJ; ++i )
        g_Obj[i].m_Mesh.Destroy();
    g_LightMesh.Destroy();
}
