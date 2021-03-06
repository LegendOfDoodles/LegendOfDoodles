// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>


// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3d12sdklayers.h>
#include <initguid.h>
#include <dxgidebug.h>
#include <math.h>

#include <memory>

#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <fstream>
////////////////////////////////////////////////////////////
#include <sstream>
#include <regex>

#include <fstream>
#include <string>

//////////////////////////////////////////////////////////

#if _DEBUG
#include <iostream>
#endif

// 네임 스페이스 처리
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;
//using namespace FMOD;

using Microsoft::WRL::ComPtr;
using std::shared_ptr;

// 라이브러리 추가
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "dxguid.lib")

//#pragma comment(lib, "./library/lib/fmod_vc.lib")		// x32
#pragma comment(lib, "./library/lib/fmod64_vc.lib")		// x64

// 사용자 정의 헤더
#include "00.Global/01.Utility/Macros.h"
#include "00.Global/01.Utility/Vectors.h"
#include "00.Global/01.Utility/Enumerations.h"
#include "00.Global/01.Utility/03.SafeFunc/SafeFunc.h"
#include "00.Global/01.Utility/01.Exception/ExptProcess.h"
#include "05.Objects/98.Prefabs/Prefabs.h"
#include "07.Network/protocol.h"